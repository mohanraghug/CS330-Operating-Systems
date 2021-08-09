#include <types.h>
#include <mmap.h>

// Helper function to create a new vm_area
struct vm_area *create_vm_area(u64 start_addr, u64 end_addr, u32 flags, u32 mapping_type)
{
    struct vm_area *new_vm_area = alloc_vm_area();
    if (new_vm_area == NULL)
        return new_vm_area;
    new_vm_area->vm_start = start_addr;
    new_vm_area->vm_end = end_addr;
    new_vm_area->access_flags = flags;
    new_vm_area->mapping_type = mapping_type;
    return new_vm_area;
}

int vm_map_phy_memory(struct exec_context *current, u64 address, u32 access_flags, u32 mapping_type, int already_allocated, u64 arg_pfn)
{

    void *os_addr = osmap(current->pgd);
    u64 pfn;
    unsigned long *ptep = (unsigned long *)os_addr + ((address & PGD_MASK) >> PGD_SHIFT);
    if (!(*ptep & 0x1))
    {
        pfn = os_pfn_alloc(OS_PT_REG);
        *ptep = (pfn << PAGE_SHIFT) | 0x7;
        os_addr = osmap(pfn);
        bzero((char *)os_addr, PAGE_SIZE);
    }
    else
    {
        os_addr = (void *)((*ptep) & FLAG_MASK);
    }
    ptep = (unsigned long *)os_addr + ((address & PUD_MASK) >> PUD_SHIFT);
    if (!(*ptep & 0x1))
    {
        pfn = os_pfn_alloc(OS_PT_REG);
        *ptep = (pfn << PAGE_SHIFT) | 0x7;
        os_addr = osmap(pfn);
        bzero((char *)os_addr, PAGE_SIZE);
    }
    else
    {
        os_addr = (void *)((*ptep) & FLAG_MASK);
    }
    ptep = (unsigned long *)os_addr + ((address & PMD_MASK) >> PMD_SHIFT);

    if (mapping_type == HUGE_PAGE_MAPPING)
    {
        if (already_allocated)
        {
            *ptep = (arg_pfn << HUGEPAGE_SHIFT) | 0x5;
            if (access_flags & MM_WR)
                *ptep |= 0x2;
            *ptep |= (1 << 7);
            return 0;
        }
        else
        {
            void *naddr = os_hugepage_alloc();
            pfn = get_hugepage_pfn(naddr);
            *ptep = (pfn << HUGEPAGE_SHIFT) | 0x5;
            if (access_flags & MM_WR)
                *ptep |= 0x2;
            *ptep |= (1 << 7);
            return 0;
        }
    }

    if (!(*ptep & 0x1))
    {
        pfn = os_pfn_alloc(OS_PT_REG);
        *ptep = (pfn << PAGE_SHIFT) | 0x7;
        os_addr = osmap(pfn);
        bzero((char *)os_addr, PAGE_SIZE);
    }
    else
    {
        os_addr = (void *)((*ptep) & FLAG_MASK);
    }
    ptep = (unsigned long *)os_addr + ((address & PTE_MASK) >> PTE_SHIFT);

    // We know that the page was unallocated
    if (!already_allocated)
    {
        pfn = os_pfn_alloc(USER_REG);
        *ptep = (pfn << PAGE_SHIFT) | 0x5;
        if (access_flags & MM_WR)
            *ptep |= 0x2;
    }
    else
    {
        *ptep = (arg_pfn << PAGE_SHIFT) | 0x5;
        if (access_flags & MM_WR)
            *ptep |= 0x2;
    }

    return 0;
}

/**
 * Function will invoked whenever there is page fault. (Lazy allocation)
 * 
 * For valid access. Map the physical page 
 * Return 1
 * 
 * For invalid access, i.e Access which is not matching the vm_area access rights (Writing on ReadOnly pages)
 * return -1. 
 */
int vm_area_pagefault(struct exec_context *current, u64 addr, int error_code)
{
    if (current == NULL)
        return -1;

    struct vm_area *head = current->vm_area;

    while (head)
    {
        if (addr >= head->vm_start && addr < head->vm_end)
        {
            // addr belongs to this segment.
            if ((error_code & 0x2) && !(head->access_flags & MM_WR))
                return -1;
            if (head->mapping_type == NORMAL_PAGE_MAPPING)
                vm_map_phy_memory(current, addr, error_code, NORMAL_PAGE_MAPPING, 0, 0);
            else
            {
                vm_map_phy_memory(current, addr, error_code, HUGE_PAGE_MAPPING, 0, 0);
            }
            return 1;
        }
        head = head->vm_next;
    }

    return -1;
}

/* Returns the pte coresponding to a user address. 
Return NULL if mapping is not present or mapped
in ring-0 */

u64 *vm_area_get_user_pte(struct exec_context *ctx, u64 addr, int dump, int *flag)
{
    u64 *vaddr_base = (u64 *)osmap(ctx->pgd);
    u64 *entry;
    u32 phy_addr;

    entry = vaddr_base + ((addr & PGD_MASK) >> PGD_SHIFT);
    phy_addr = (*entry >> PTE_SHIFT) & 0xFFFFFFFF;
    vaddr_base = (u64 *)osmap(phy_addr);

    /* Address should be mapped as un-priviledged in PGD*/
    if ((*entry & 0x1) == 0 || (*entry & 0x4) == 0)
        return NULL;

    entry = vaddr_base + ((addr & PUD_MASK) >> PUD_SHIFT);
    phy_addr = (*entry >> PTE_SHIFT) & 0xFFFFFFFF;
    vaddr_base = (u64 *)osmap(phy_addr);

    /* Address should be mapped as un-priviledged in PUD*/
    if ((*entry & 0x1) == 0 || (*entry & 0x4) == 0)
        return NULL;

    entry = vaddr_base + ((addr & PMD_MASK) >> PMD_SHIFT);
    phy_addr = (*entry >> PTE_SHIFT) & 0xFFFFFFFF;
    vaddr_base = (u64 *)osmap(phy_addr);

    /* 
        Address should be mapped as un-priviledged in PMD 
         Huge page mapping not allowed
      */
    if ((*entry & 0x1) == 0 || (*entry & 0x4) == 0)
        return NULL;

    if ((*entry & 0x80) != 0)
    {
        // HUGE_PAGE_ENTRY
        *flag = 1;
        return entry;
    }

    entry = vaddr_base + ((addr & PTE_MASK) >> PTE_SHIFT);

    /* Address should be mapped as un-priviledged in PTE*/
    if ((*entry & 0x1) == 0 || (*entry & 0x4) == 0)
        return NULL;

    return entry;
}

/* Returns 0 if successfully mmaped else return -1 (if not found)*/
int unmap_phy_memory(struct exec_context *ctx, u64 addr)
{
    int flag = 0;
    u64 *pte_entry = vm_area_get_user_pte(ctx, addr, 0, &flag);
    if (!pte_entry)
        return -1;

    if (flag == 1)
    {
        u64 foo = (*pte_entry >> HUGEPAGE_SHIFT) * HUGE_PAGE_SIZE;
        os_hugepage_free((void *)foo);
        *pte_entry = 0;
        asm volatile("invlpg (%0);" ::"r"(addr)
                     : "memory"); // Flush TLB
        return 0;
    }

    os_pfn_free(USER_REG, (*pte_entry >> PTE_SHIFT) & 0xFFFFFFFF);
    *pte_entry = 0; // Clear the PTE

    asm volatile("invlpg (%0);" ::"r"(addr)
                 : "memory"); // Flush TLB
    return 0;
}

/*
 * Returns the number of vm_areas currently present
*/

int get_cnt(struct exec_context *current)
{
    struct vm_area *head = current->vm_area;

    int ret = 0;
    while (head)
    {
        ret++;
        head = head->vm_next;
    }

    return ret;
}

/**
 * mmap system call implementation.
 */
long vm_area_map(struct exec_context *current, u64 addr, int length, int prot, int flags)
{

    if (current == NULL)
        return -1;

    if (addr != 0 && (addr < MMAP_AREA_START || addr >= MMAP_AREA_END))
        return -1;

    if (length <= 0 || length > MMAP_AREA_END - addr)
        return -1;

    if ((prot | (PROT_READ | PROT_WRITE)) != (PROT_READ | PROT_WRITE))
        return -1;

    if (flags != MAP_FIXED && flags != 0)
        return -1;

    length = (length + PAGE_SIZE - 1) / PAGE_SIZE;
    length *= PAGE_SIZE;

    if (current->vm_area == NULL)
    {
        // dummy node.
        current->vm_area = create_vm_area(MMAP_AREA_START, MMAP_AREA_START + PAGE_SIZE, 0x4, NORMAL_PAGE_MAPPING);
        if (current->vm_area == NULL) // allocation failed :(
            return -1;
        current->vm_area->vm_next = NULL;
    }

    if (addr != 0)
    {
        addr = (addr + PAGE_SIZE - 1) / PAGE_SIZE;
        addr *= PAGE_SIZE;

        struct vm_area *head = current->vm_area;

        while (head)
        {
            // check if the new allocated area can be placed after this node.

            struct vm_area *next = head->vm_next;

            int ok = (addr >= head->vm_end && addr + length <= MMAP_AREA_END);

            if (next != NULL)
            {
                ok &= (addr + length <= next->vm_start);
            }

            if (ok == 0)
            {
                head = next;
                continue;
            }

            if (next != NULL && next->vm_start == addr + length && head->vm_end == addr && head->access_flags == prot && head->mapping_type == NORMAL_PAGE_MAPPING && next->mapping_type == NORMAL_PAGE_MAPPING && next->access_flags == prot)
            {
                // requested segment exactly fits b/w head and next
                head->vm_end = next->vm_end;
                head->vm_next = next->vm_next;
                dealloc_vm_area(next);
                return addr;
            }

            if (addr == head->vm_end && head->access_flags == prot && head->mapping_type == NORMAL_PAGE_MAPPING)
            {
                // req. segment can be merged with head.
                head->vm_end += length;
                return addr;
            }

            if (addr + length == next->vm_start && next->access_flags == prot && next->mapping_type == NORMAL_PAGE_MAPPING)
            {
                // req.segment can be merged with next.
                next->vm_start = addr;
                return addr;
            }

            struct vm_area *new_vm_area = create_vm_area(addr, addr + length, prot, NORMAL_PAGE_MAPPING);

            if (new_vm_area == NULL)
                return -1;

            new_vm_area->vm_next = next;
            head->vm_next = new_vm_area;
            return addr;
        }

        if (flags == MAP_FIXED)
            return -1;
    }

    struct vm_area *head = current->vm_area;
    while (head)
    {
        int ok = (head->vm_end + length <= MMAP_AREA_END);

        struct vm_area *next = head->vm_next;

        if (next != NULL)
        {
            ok &= (next->vm_start >= head->vm_end + length);
        }

        u64 addr = head->vm_end;

        if (ok == 0)
        {
            head = next;
            continue;
        }

        if (next != NULL && next->vm_start == addr + length && head->vm_end == addr && head->access_flags == prot && head->mapping_type == NORMAL_PAGE_MAPPING && next->mapping_type == NORMAL_PAGE_MAPPING && next->access_flags == prot)
        {
            head->vm_end = next->vm_end;
            head->vm_next = next->vm_next;
            dealloc_vm_area(next);
            return addr;
        }

        if (addr == head->vm_end && head->access_flags == prot && head->mapping_type == NORMAL_PAGE_MAPPING)
        {
            head->vm_end += length;
            return addr;
        }

        if (addr + length == next->vm_start && next->access_flags == prot && next->mapping_type == NORMAL_PAGE_MAPPING)
        {
            next->vm_start = addr;
            return addr;
        }

        if (get_cnt(current) >= 128)
            return -1;

        struct vm_area *new_vm_area = create_vm_area(addr, addr + length, prot, NORMAL_PAGE_MAPPING);

        if (new_vm_area == NULL)
            return -1;

        new_vm_area->vm_next = next;
        head->vm_next = new_vm_area;

        return addr;
    }

    return -1;
}

u64 max(u64 a, u64 b)
{
    if (a > b)
        return a;
    return b;
}

u64 min(u64 a, u64 b)
{
    if (a < b)
        return a;
    return b;
}

int get(int x)
{
    if (x == -EINVAL)
        return 5;
    if (x == -ENOMAPPING)
        return 4;
    if (x == -EVMAOCCUPIED)
        return 3;
    if (x == -EDIFFPROT)
        return 2;
    if (x == -1)
        return 1;
    if (x == 0)
        return 0;
}

int upd_error(int a, int b)
{
    int x = get(a), y = get(b);
    if (x > y)
        return a;
    return b;
}

/**
 * munmap system call implemenations
 */
int vm_area_unmap(struct exec_context *current, u64 addr, int length)
{

    if (current == NULL)
        return -EINVAL;

    if (addr != 0 && (addr < MMAP_AREA_START || addr > MMAP_AREA_END))
        return -EINVAL;

    if (length <= 0 || length > MMAP_AREA_END - addr)
        return -EINVAL;

    if (addr % PAGE_SIZE)
        return -EINVAL;

    struct vm_area *head = current->vm_area;
    struct vm_area *prev = NULL;

    u64 from = addr, to = addr + length;

    while (head)
    {

        if (to <= head->vm_start || from >= to)
            break;

        from = max(from, head->vm_start);

        if (from >= head->vm_start && from < head->vm_end)
        {
            if (head->mapping_type == HUGE_PAGE_MAPPING && from % HUGE_PAGE_SIZE != 0)
            {
                from = (from / HUGE_PAGE_SIZE) * HUGE_PAGE_SIZE;
            }

            u64 tmp = from;
            while (from < head->vm_end && from < to)
            {
                unmap_phy_memory(current, from);
                from += (head->mapping_type == HUGE_PAGE_MAPPING ? HUGE_PAGE_SIZE : PAGE_SIZE);
            }

            if (tmp == head->vm_start)
            {
                if (from == head->vm_end)
                {
                    prev->vm_next = head->vm_next;
                    dealloc_vm_area(head);
                    head = prev;
                }
                else
                {
                    head->vm_start = from;
                }
            }
            else
            {
                if (from == head->vm_end)
                {
                    head->vm_end = tmp;
                }
                else
                {
                    struct vm_area *new_vm_area = create_vm_area(from, head->vm_end, head->access_flags, head->mapping_type);
                    if (new_vm_area == NULL)
                    {
                        return -1;
                    }
                    new_vm_area->vm_next = head->vm_next;
                    head->vm_next = new_vm_area;
                    head->vm_end = tmp;
                }
            }
        }

        prev = head;
        head = head->vm_next;
    }

    return 0;
}

/**
 * make_hugepage system call implemenation
 */
long vm_area_make_hugepage(struct exec_context *current, void *addr, u32 length, u32 prot, u32 force_prot)
{

    if (current == NULL)
        return -EINVAL;

    if (addr == NULL)
        return -EINVAL;

    if ((prot | (PROT_READ | PROT_WRITE)) != (PROT_READ | PROT_WRITE))
        return -EINVAL;

    {
        // error checks
        struct vm_area *head = current->vm_area;
        u64 from = (u64)addr, to = from + length;

        int err = 0;

        while (head)
        {
            if (from >= to)
                break;

            if (from < head->vm_start)
            {
                err = upd_error(err, -ENOMAPPING);
            }

            if (from >= head->vm_start && from < head->vm_end)
            {
                if (head->mapping_type == HUGE_PAGE_MAPPING)
                {
                    err = upd_error(err, -EVMAOCCUPIED);
                }

                if (head->access_flags != prot && force_prot == 0)
                {
                    err = upd_error(err, -EDIFFPROT);
                }

                from = min(to, head->vm_end);
            }
            head = head->vm_next;
        }

        if (from < to)
        {
            err = upd_error(err, -ENOMAPPING);
        }

        if (err < 0)
            return err;
    }

    u64 from = (u64)addr;
    u64 to = (u64)addr + length;

    to = (to / HUGE_PAGE_SIZE) * HUGE_PAGE_SIZE;
    from = (from + HUGE_PAGE_SIZE - 1) / HUGE_PAGE_SIZE * HUGE_PAGE_SIZE;
    length = to - from;

    if (length <= 0)
        return 0;

    struct vm_area *head = current->vm_area;
    struct vm_area *prev = NULL;

    struct vm_area *new_vm_area = create_vm_area(from, to, prot, HUGE_PAGE_MAPPING);

    if (new_vm_area == NULL)
        return -1;

    while (head)
    {
        if (from >= head->vm_start && from < head->vm_end)
        {
            struct vm_area *before = head;
            u64 start = from;

            while (from < to)
            {
                u64 old = from;
                while (from < head->vm_end && from < to)
                {
                    u64 tmp = from;
                    from += HUGE_PAGE_SIZE;
                    int can_lazy = 1;
                    for (u64 addr = tmp; addr < from; addr += PAGE_SIZE)
                    {
                        int flag = 0;
                        u64 *pte = vm_area_get_user_pte(current, addr, 0, &flag);
                        if (pte != NULL)
                        {
                            can_lazy = 0;
                            break;
                        }
                    }

                    if (!can_lazy)
                    {
                        void *huge_addr = os_hugepage_alloc();
                        for (u64 addr = tmp; addr < from; addr += PAGE_SIZE)
                        {
                            int flag = 0;
                            u64 *pte = vm_area_get_user_pte(current, addr, 0, &flag);
                            if (pte != NULL)
                            {
                                void *dest = (void *)(huge_addr + (addr - tmp));
                                void *src = (void *)addr;
                                memcpy(dest, src, PAGE_SIZE);
                                unmap_phy_memory(current, addr);
                            }
                        }
                        vm_map_phy_memory(current, tmp, prot, HUGE_PAGE_MAPPING, 1, get_hugepage_pfn(huge_addr));
                    }
                }
                if (from >= head->vm_end)
                {
                    head = head->vm_next;
                }
            }

            while (head && from >= head->vm_end)
            {
                head = head->vm_next;
            }

            struct vm_area *after = head;

            if (before == after)
            {
                if (start == before->vm_start)
                {
                    after->vm_start = from;
                    prev->vm_next = new_vm_area;
                    new_vm_area->vm_next = after;
                }
                else
                {
                    struct vm_area *new_vm_area2 = create_vm_area(from, before->vm_end, before->access_flags, before->mapping_type);
                    if (new_vm_area2 == NULL)
                        return -1;
                    new_vm_area2->vm_next = before->vm_next;
                    before->vm_next = new_vm_area;
                    new_vm_area->vm_next = new_vm_area2;
                    before->vm_end = start;
                }
            }
            else
            {
                if (start == before->vm_start)
                {
                    prev->vm_next = new_vm_area;
                    new_vm_area->vm_next = after;
                    while (before != after)
                    {
                        struct vm_area *tmp = before->vm_next;
                        dealloc_vm_area(before);
                        before = tmp;
                    }
                    if (after != NULL)
                    {
                        after->vm_start = max(from, after->vm_start);
                    }
                }
                else
                {
                    struct vm_area *store = before;
                    before = before->vm_next;
                    while (before != after)
                    {
                        struct vm_area *tmp = before->vm_next;
                        dealloc_vm_area(before);
                        before = tmp;
                    }
                    store->vm_end = start;
                    store->vm_next = new_vm_area;
                    new_vm_area->vm_next = after;
                    if (after != NULL)
                    {
                        after->vm_start = max(from,after->vm_start);
                    }
                }
            }

            {
                struct vm_area *head = current->vm_area;
                struct vm_area *prev = NULL;
                while (head)
                {
                    if (prev && prev->vm_end == head->vm_start && prev->mapping_type == head->mapping_type && prev->access_flags == head->access_flags)
                    {
                        prev->vm_next = head->vm_next;
                        prev->vm_end = head->vm_end;
                        dealloc_vm_area(head);
                        head = prev;
                    }
                    prev = head;
                    head = head->vm_next;
                }
            }

            return start;
        }

        prev = head;
        head = head->vm_next;
    }

    return 0;
}

/**
 * break_system call implemenation
 */
int vm_area_break_hugepage(struct exec_context *current, void *addr, u32 length)
{
    if (current == NULL)
        return -EINVAL;

    if ((u64)addr % (HUGE_PAGE_SIZE))
        return -EINVAL;

    if (length % (HUGE_PAGE_SIZE))
        return -EINVAL;

    u64 from = (u64)addr, to = from + length;

    struct vm_area *head = current->vm_area;
    struct vm_area *prev = NULL;

    while (head)
    {
        from = max(from, head->vm_start);
        if (from >= to)
            break;
        if (from >= head->vm_start && from < head->vm_end && head->mapping_type == HUGE_PAGE_MAPPING)
        {
            u64 start = from;
            while (from < to && from < head->vm_end)
            {
                int flag = 0;
                u64 *pte = vm_area_get_user_pte(current, from, 0, &flag);
                if (pte != NULL && (*pte & 0x1))
                {
                    u64 foo = (*pte >> HUGEPAGE_SHIFT) * HUGE_PAGE_SIZE;
                    *pte = 0;
                    asm volatile("invlpg (%0);" ::"r"(from)
                                 : "memory"); // Flush TLB
                    for (u64 addr = from; addr < from + HUGE_PAGE_SIZE; addr += PAGE_SIZE)
                    {
                        u64 pfn = os_pfn_alloc(USER_REG);
                        void *dest = osmap(pfn);
                        void *src = (void *)(foo + (addr - from));
                        memcpy(dest, src, PAGE_SIZE);
                        vm_map_phy_memory(current, addr, head->access_flags, NORMAL_PAGE_MAPPING, 1, pfn);
                    }
                    os_hugepage_free((void *)foo);
                }

                from += HUGE_PAGE_SIZE;
            }
            struct vm_area *new_vm_area = create_vm_area(start, from, head->access_flags, NORMAL_PAGE_MAPPING);
            if (start == head->vm_start)
            {
                if (from == head->vm_end)
                {
                    prev->vm_next = new_vm_area;
                    new_vm_area->vm_next = head->vm_next;
                    dealloc_vm_area(head);
                    head = new_vm_area;
                }
                else
                {
                    prev->vm_next = new_vm_area;
                    new_vm_area->vm_next = head;
                    head->vm_start = from;
                }
            }
            else
            {
                if (from == head->vm_end)
                {
                    head->vm_end = start;
                    new_vm_area->vm_next = head->vm_next;
                    head->vm_next = new_vm_area;
                    head = new_vm_area;
                }
                else
                {
                    struct vm_area *new_vm_area2 = create_vm_area(from, head->vm_end, head->access_flags, head->mapping_type);
                    new_vm_area2->vm_next = head->vm_next;
                    new_vm_area->vm_next = new_vm_area2;
                    head->vm_next = new_vm_area;
                    head->vm_end = start;
                    head = new_vm_area;
                }
            }
        }
        prev = head;
        head = head->vm_next;
    }

    prev = NULL;
    head = current->vm_area;

    while (head)
    {
        if (prev && prev->vm_end == head->vm_start && prev->mapping_type == head->mapping_type && prev->access_flags == head->access_flags)
        {
            prev->vm_next = head->vm_next;
            prev->vm_end = head->vm_end;
            dealloc_vm_area(head);
            head = prev;
        }
        prev = head;
        head = head->vm_next;
    }

    return 0;
}
