#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <sys/stat.h>
#include <assert.h>

int try_this_directory(char **cmd, char *directory)
{
	int len = strlen(directory) + 10 + strlen(cmd[0]);
	char command[len + 1];

	// flag = 1 if the command contains slashes

	int flag = 0;
	for (int i = 0; i < strlen(cmd[0]); i++)
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
		if (strlen(directory) == 0)
			strcat(command, "./");
		strcat(command, directory);
		if (strlen(directory) != 0)
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

void parse(char *s, char **ans)
{
	// parses command into a form suitable for execv
	char *token = strtok(s, " ");
	int i = 0;
	while (token)
	{
		int n = strlen(token) + 1;
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

int executeCommand(char *cmd)
{

	int n = strlen(cmd);

	int cnt = 1;
	for (int i = 0; i < n; i++)
	{
		if (cmd[i] == ' ')
			cnt++;
	}

	char *cmds[cnt + 1];
	cmds[cnt] = NULL;
	parse(cmd, cmds);

	pid_t pid, cpid;
	int status;
	pid = fork();

	if (pid < 0)
	{
		exit(-1);
	}

	if (!pid)
	{

		char *path = getenv("CS330_PATH");
		// flag = 1 if current directory is in path 0 otherwise
		int flag = 0;
		int cur = 0;
		for (int i = 0; i < strlen(path); i++)
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
			try_this_directory(cmds, directory);
			directory = strtok(NULL, ":");
		}

		if (flag)
		{
			try_this_directory(cmds, "");
		}

		printf("UNABLE TO EXECUTE\n");
		exit(-1);
	}

	cpid = wait(&status);
	return WEXITSTATUS(status);
}

int main(int argc, char *argv[])
{
	return executeCommand(argv[1]);
}
