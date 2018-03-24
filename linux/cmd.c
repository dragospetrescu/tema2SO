/**
 * Operating Systems 2013-2017 - Assignment 2
 *
 * TODO Name, Group
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include <fcntl.h>
#include <unistd.h>
#include <printf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmd.h"
#include "utils.h"
#include "parser.h"

#define READ        0
#define WRITE        1

/**
 * Internal change-directory command.
 */
static bool shell_cd(word_t *dir) {
	/* TODO execute cd */

	if (setenv("PWD", dir->string, 1) == 0)
		printf("AM AJUNS AICI %s vs %s\n",dir->string, getenv("PWD"));
	return true;
}

/**
 * Internal exit/quit command.
 */
static int shell_exit(void) {
	/* TODO execute exit/quit */

	exit(0); /* TODO replace with actual exit code */
}

/**
 * Parse a simple command (internal, environment variable assignment,
 * external command).
 */
static int parse_simple(simple_command_t *s, int level, command_t *father) {
	/* TODO sanity checks */

	/* TODO if builtin command, execute the command */
	if (strcmp(s->verb->string, "cd") == 0) {
		if (s->params != NULL) {
			shell_cd(s->params);
		}
	}
	if (strcmp(s->verb->string, "exit") == 0 || strcmp(s->verb->string, "quit") == 0) {
		shell_exit();
	}

	/* TODO if variable assignment, execute the assignment and return
	 * the exit status
	 */

	/* TODO if external command:
	 *   1. fork new process
	 *     2c. perform redirections in child
	 *     3c. load executable in child
	 *   2. wait for child
	 *   3. return exit status
	 */


	printf("%d\n ", s->io_flags);
	if(s->in != NULL) {
		printf("in: %s\n", s->in->string);
	}

	if(s->out != NULL) {
		printf("out: %s\n", s->out->string);
	}

	if(s->err != NULL) {
		printf("err: %s\n", s->err->string);
	}
	pid_t pid, wait_ret;
	int status;
	char **params;
	word_t *params_parser;
	int no_args = 0;

	params = malloc(sizeof(char *) * 50);
	params_parser = s->params;

	pid = fork();
	switch (pid) {
		case -1:    /* error */
			perror("fork");
		case 0:        /* child process */
			chdir(getenv("PWD"));
			params[no_args] = malloc(
					sizeof(char) * (strlen(s->verb->string) + 1));
			strcpy(params[no_args], s->verb->string);
			no_args++;

			while (params_parser != NULL) {
				params[no_args] = malloc(
						sizeof(char) * (strlen(params_parser->string) + 1));
				strcpy(params[no_args], params_parser->string);
				params_parser = params_parser->next_part;
				no_args++;
			}
			params[no_args] = (char *) NULL;
			execvp(s->verb->string, params);

			fflush(stdout);
		default:    /* parent process */
			wait_ret = waitpid(pid, &status, 0);


//			if (WIFEXITED(status))
//				printf("Child process (pid %d) terminated normally, "
//							   "with exit code %d\n",
//					   pid, WEXITSTATUS(status));
	}


	return 0; /* TODO replace with actual exit status */
}

/**
 * Process two commands in parallel, by creating two children.
 */
static bool do_in_parallel(command_t *cmd1, command_t *cmd2, int level,
						   command_t *father) {
	/* TODO execute cmd1 and cmd2 simultaneously */

	return true; /* TODO replace with actual exit status */
}

/**
 * Run commands by creating an anonymous pipe (cmd1 | cmd2)
 */
static bool do_on_pipe(command_t *cmd1, command_t *cmd2, int level,
					   command_t *father) {
	/* TODO redirect the output of cmd1 to the input of cmd2 */

	return true; /* TODO replace with actual exit status */
}

/**
 * Parse and execute a command.
 */
int parse_command(command_t *c, int level, command_t *father) {
	/* TODO sanity checks */




	if (c->op == OP_NONE) {
		/* TODO execute a simple command */
//		fprintf(stdout, "%s %s", c->scmd->verb->string, c->scmd->params->string);
		parse_simple(c->scmd, level, father);
		return 0; /* TODO replace with actual exit code of command */
	}

	switch (c->op) {
		case OP_SEQUENTIAL:
			/* TODO execute the commands one after the other */
			break;

		case OP_PARALLEL:
			/* TODO execute the commands simultaneously */
			break;

		case OP_CONDITIONAL_NZERO:
			/* TODO execute the second command only if the first one
			 * returns non zero
			 */
			break;

		case OP_CONDITIONAL_ZERO:
			/* TODO execute the second command only if the first one
			 * returns zero
			 */
			break;

		case OP_PIPE:
			/* TODO redirect the output of the first command to the
			 * input of the second
			 */
			break;

		default:
			return SHELL_EXIT;
	}

	return 0; /* TODO replace with actual exit code of command */
}
