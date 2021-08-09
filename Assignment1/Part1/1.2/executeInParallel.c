#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <sys/stat.h>
#include <fcntl.h>

int len(char *s)
{
	int i = 0;
	while (s[i] != '\0')
		i++;
	return i;
}

int try_this_directory(char **cmd, char *directory)
{
	int N = len(directory) + 10 + len(cmd[0]);
	char command[N + 1];

	// flag = 1 if the command contains slashes

	int flag = 0;
	for (int i = 0; i < len(cmd[0]); i++)
	{
		if (cmd[0][i] == '/')
		{
			flag = 1;
		}
	}

	if (flag == 0)
	{
		// if the command doesnot contain slash append it to the name of directory
		command[0] = '\0';
		if (len(directory) == 0)
			strcat(command, "./");
		strcat(command, directory);
		if (len(directory) != 0)
			strcat(command, "/");
		strcat(command, cmd[0]);
	}
	else
	{
		// if the command containes slash directly execute it
		command[0] = '\0';
		strcat(command, cmd[0]);
	}
	execv(command, &cmd[0]);
	return -1;
}

int executeCommand(char **cmd)
{
	char *path = getenv("CS330_PATH");

	// flag = 1 if current directory is in path 0 otherwise

	int flag = 0;
	int cur = 0;
	for (int i = 0; i < len(path); i++)
	{
		if (path[i] == ':')
		{
			if (cur == 0)
			{
				flag = 1;
			}
			cur = 0;
		}
		else
			cur++;
	}
	if (cur == 0)
		flag = 1;

	char *directory = strtok(path, ":");

	// Trying each directory in the path one by one

	while (directory != NULL)
	{
		try_this_directory(cmd, directory);
		directory = strtok(NULL, ":");
	}

	if (flag)
	{
		try_this_directory(cmd, "");
	}

	printf("UNABLE TO EXECUTE\n");
	return -1;
}

void parse(char *s, char **ans)
{
	char *token = strtok(s, " ");
	int i = 0;
	while (token)
	{
		int n = len(token) + 1;
		ans[i] = malloc(n * sizeof(char));
		for (int j = 0; j < n; j++)
		{
			ans[i][j] = token[j];
		}
		ans[i][n - 1] = '\0';
		token = strtok(NULL, " ");
		i++;
	}
}

int readline(int fd, char *des)
{
	char s[100];
	int n;
	int i = 0;
	while ((n = read(fd, s, 1)) != -1)
	{
		if (n == 0 || s[0] == '\n')
		{
			break;
		}
		des[i] = s[0];
		i++;
	}
	if (i == 0)
		return 0;
	des[i] = '\0';
	return i;
}

int execute_in_parallel(char *infile, char *outfile)
{
	int input = open(infile, O_RDONLY);
	char buff[1025];
	int fd[100][2];	 // pipes to communicate outputs
	int err[100][2]; // pipes to communicate errors
	int tot = 0;
	while (readline(input, buff))
	{
		if (pipe(fd[tot]) < 0 || pipe(err[tot]) < 0)
		{
			return -1;
		}
		pid_t pid = fork();
		if (pid < 0)
		{
			return -1;
		}
		if (!pid)
		{
			close(fd[tot][0]);
			close(err[tot][0]);
			dup2(fd[tot][1], 1);
			int n = len(buff);
			int cnt = 1;
			for (int i = 0; i < n; i++)
			{
				if (buff[i] == ' ')
					cnt++;
			}
			char *cmd[cnt + 1];
			parse(buff, cmd);
			cmd[cnt] = NULL;
			int ret = executeCommand(cmd);
			// to make sure the parent knows i exited with -1 status
			write(err[tot][1], "1", 1);
			exit(-1);
		}
		close(fd[tot][1]);
		close(err[tot][1]);
		tot++;
	}

	int ret = 0;
	int output = open(outfile, O_RDWR | O_CREAT, 0644);
	dup2(output, 1);
	for (int i = 0; i < tot; i++)
	{
		char buf[2];
		while (read(fd[i][0], buf, 1) == 1)
		{
			printf("%c", buf[0]);
		}
		if (read(err[i][0], buf, 1) == 1)
		{
			if (buf[0] == '1')
			{
				ret = -1;
			}
		}
	}
	return ret;
}

int main(int argc, char *argv[])
{
	return execute_in_parallel(argv[1], argv[2]);
}