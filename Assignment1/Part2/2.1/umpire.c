#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <sys/stat.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2

#define ROCK 0
#define PAPER 1
#define SCISSORS 2

int calc(int x, int y)
{
	// Given player's move(x) and opponent's move(y) calc(x,y) returns score of player for this game.
	if (x == y)
		return 0;
	if (x == ROCK)
	{
		return (y == SCISSORS);
	}
	if (x == PAPER)
	{
		return (y == ROCK);
	}
	if (x == SCISSORS)
	{
		return (y == PAPER);
	}
}

int ask(int input, int output)
{
	// communicating with player

	if (write(output, "GO\n", 3) != 3)
	{
		exit(-1);
	}
	char ret;
	if (read(input, &ret, 1) != 1)
	{
		exit(-1);
	}
	return (ret - '0');
}

int main(int argc, char *argv[])
{

	if (argc < 3)
	{
		return -1;
	}
	char *player1 = argv[1];
	char *player2 = argv[2];

	// Two pipes one for reading and the other for writing
	int fd1_in[2], fd1_out[2];
	if (pipe(fd1_in) < 0 || pipe(fd1_out) < 0)
	{
		exit(-1);
	}

	pid_t player1_fork = fork();
	if (player1_fork < 0)
	{
		exit(-1);
	}

	if (!player1_fork)
	{
		// Setting up input and output
		dup2(fd1_in[0], STDIN);
		dup2(fd1_out[1], STDOUT);
		close(fd1_in[1]);
		close(fd1_out[0]);

		char *s[2];
		s[0] = player1;
		s[1] = NULL;
		if (execv(player1, s))
		{
			exit(-1);
		}
		exit(-1);
	}

	int fd2_in[2], fd2_out[2];
	if (pipe(fd2_in) < 0 || pipe(fd2_out) < 0)
	{
		exit(-1);
	}

	pid_t player2_fork = fork();
	if (player2_fork < 0)
	{
		exit(-1);
	}

	if (!player2_fork)
	{
		// Setting up input and output
		dup2(fd2_in[0], STDIN);
		dup2(fd2_out[1], STDOUT);
		close(fd2_in[1]);
		close(fd2_out[0]);
		char *s[2];
		s[0] = player2;
		s[1] = NULL;
		execv(player2, s);
		exit(-1);
	}
	else
	{
		close(fd2_out[1]);
		close(fd2_in[0]);
		close(fd1_in[0]);
		close(fd1_out[1]);
		int score1 = 0, score2 = 0;
		for (int r = 0; r < 10; r++)
		{
			int move1 = ask(fd1_out[0], fd1_in[1]);
			int move2 = ask(fd2_out[0], fd2_in[1]);
			score1 += calc(move1, move2);
			score2 += calc(move2, move1);
		}
		printf("%d %d", score1, score2);
	}

	return 0;
}