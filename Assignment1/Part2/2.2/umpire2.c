#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#define ROCK 0
#define PAPER 1
#define SCISSORS 2

#define STDIN 0
#define STDOUT 1
#define STDERR 2

#include "gameUtils.h"

int getWalkOver(int numPlayers); // Returns a number between [1, numPlayers]

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
	des[i] = '\0';
	return i;
}

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

	int N = 10;
	char *file_name = argv[1];
	if (argc == 4)
	{
		N = atoi(argv[2]);
		file_name = argv[3];
	}
	int inp = open(file_name, O_RDONLY);

	char buff[105];

	readline(inp, buff);

	int P = atoi(buff);

	if (P < 2)
	{
		exit(-1);
	}

	char players[P][105];
	for (int i = 0; i < P; i++)
	{
		if (readline(inp, players[i]) <= 0)
		{
			exit(-1);
		}
	}

	int fd_in[P][2], fd_out[P][2]; // Two pipes for each player one for read and other for write

	for (int i = 0; i < P; i++)
	{
		if (pipe(fd_in[i]) < 0 || pipe(fd_out[i]) < 0)
		{
			exit(-1);
		}
	}

	for (int i = 0; i < P; i++)
	{
		pid_t pid = fork();
		if (pid < 0)
		{
			exit(-1);
		}
		if (!pid)
		{
			dup2(fd_in[i][0], STDIN);
			dup2(fd_out[i][1], STDOUT);
			close(fd_in[i][1]);
			close(fd_out[i][0]);
			char *s[2];
			s[0] = players[i];
			s[1] = NULL;
			if (execv(players[i], s))
			{
				exit(-1);
			}
			exit(-1);
		}
		close(fd_in[i][0]);
		close(fd_out[i][1]);
	}

	int active_players = P;
	int active_list[P];

	for (int i = 0; i < P; i++)
	{
		active_list[i] = i;
	}

	while (active_players > 1)
	{
		for (int i = 0; i < active_players; i++)
		{
			printf("p%d", active_list[i]);
			if (i == active_players - 1)
			{
				printf("\n");
			}
			else
				printf(" ");
		}

		int walked_over = -1;
		if (active_players % 2 == 1)
		{
			walked_over = getWalkOver(active_players);
		}
		int score[active_players]; // Score of each player
		for (int i = 0; i < active_players; i++)
		{
			score[i] = 0;
		}
		int Opponent[active_players]; // Stores the opponent of each player

		for (int r = 0; r < N; r++)
		{
			for (int i = 0; i < active_players; i++)
			{
				if (walked_over == i + 1)
					continue;
				int me = active_list[i];
				int opponent_ind = i + 1;
				if (walked_over == opponent_ind + 1)
				{
					opponent_ind++;
				}
				int opponent = active_list[opponent_ind];
				int move1 = ask(fd_out[me][0], fd_in[me][1]);
				int move2 = ask(fd_out[opponent][0], fd_in[opponent][1]);

				score[i] += calc(move1, move2);
				score[opponent_ind] += calc(move2, move1);

				Opponent[i] = opponent_ind;
				Opponent[opponent_ind] = i;

				i = opponent_ind;
			}
		}

		int new_active_list[P]; // To update the active list of players
		int j = 0;
		for (int i = 0; i < active_players; i++)
		{
			if (walked_over == i + 1)
			{
				new_active_list[j++] = active_list[i];
				continue;
			}
			int opponent = Opponent[i];
			if (score[i] > score[opponent] || (score[i] == score[opponent] && i < opponent))
			{
				new_active_list[j++] = active_list[i];
			}
		}

		active_players = (active_players + 1) / 2;
		for (int i = 0; i < active_players; i++)
		{
			active_list[i] = new_active_list[i];
		}
	}

	printf("p%d", active_list[0]);

	return 0;
}
