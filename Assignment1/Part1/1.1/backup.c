#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <sys/stat.h>
#include <assert.h>

int try_this(char **cmd, char *directory, int *success)
{
	struct stat fileStat;

	int len = strlen(directory) + 1 + strlen(cmd[0]);
	char command[len + 1];
	command[0] = '\0';
	strcat(command, directory);
	strcat(command, "/");
	strcat(command, cmd[0]);
	command[len] = '\0';

	if (stat(command, &fileStat) < 0)
		return 0;

	if (stat(command, &fileStat) == 0 && (fileStat.st_mode & S_IXUSR))
	{
		fprintf(stderr, "Here\n");
		fprintf(stderr, "%s %s %s\n", command, cmd[0], cmd[1]);
		int i, status;
		pid_t pid, cpid;
		pid = fork();
		if (pid < 0)
		{
			exit(0);
		}
		if (!pid)
		{
			if (execv(command, &cmd[0]))
			{
			}
			exit(200);
		}
		cpid = wait(&status);
		if (WEXITSTATUS(status) != 200)
		{
			*success = 1;
			return WEXITSTATUS(status);
		}
		return -1;
	}
	else
	{
		return 0;
	}
}

void doit(char *s, char **ans)
{

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

int executeCommand(char **cmd)
{
	char *pwd = getenv("PWD");
	char *path = getenv("CS330_PATH");
	char *relative_path = strtok(path, ":");

	while (relative_path != NULL)
	{

		// This is if the given path is relative path
		int len = strlen(pwd) + 1 + strlen(relative_path);
		char absolute_path[len + 1];
		absolute_path[0] = '\0';
		strcat(absolute_path, pwd);
		strcat(absolute_path, "/");
		strcat(absolute_path, relative_path);
		absolute_path[len] = '\0';

		int success = 0;
		int status = try_this(cmd, relative_path, &success);
		if (success)
			return status;

		relative_path = strtok(NULL, ":");
	}

	printf("UNABLE TO EXECUTE\n");
	return -1;
}

int main(int argc, char *argv[])
{
	assert(argc >= 2);
	int n = strlen(argv[1]);
	int cnt = 1;
	for (int i = 0; i < n; i++)
	{
		if (argv[1][i] == ' ')
			cnt++;
	}
	char *cmd[cnt + 1];
	cmd[cnt] = argv[2];
	doit(argv[1], cmd);
	int ret = executeCommand(cmd);
	for (int i = 0; i < cnt; i++)
	{
		free(cmd[i]);
	}
	return ret;
}
