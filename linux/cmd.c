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

static int *redirect(simple_command_t *);

static void set_variable(simple_command_t *);

static void reset_redirect(int *redirects);

static int shell_cd(simple_command_t *s)
{
	int fid;
	int stdin_des = -1;
	int stdout_des = -1;
	int stderr_des = -1;

	if (s->in != NULL) {
		fid = open(s->in->string, O_RDONLY);
		stdin_des = dup(STDIN_FILENO);
		dup2(fid, STDIN_FILENO);
	}

	if (s->out != NULL) {
		stdout_des = dup(STDOUT_FILENO);
		fid = -1;
		if (s->io_flags == IO_REGULAR)
			fid = open(s->out->string,
					   O_RDWR | O_CREAT | O_TRUNC, 0644);
		else if (s->io_flags == IO_OUT_APPEND)
			fid = open(s->out->string,
					   O_APPEND | O_RDWR | O_CREAT, 0644);
		else
			fprintf(stderr, "RAGAT2\n");
		if (fid < 0)
			fprintf(stderr, "RAGAT\n");

		dup2(fid, STDOUT_FILENO);
		if (s->err != NULL) {
			stderr_des = dup(STDERR_FILENO);
			dup2(fid, STDERR_FILENO);
		}
	}

	if (s->err != NULL) {
		fid = -1;
		stderr_des = dup(STDERR_FILENO);
		if (s->io_flags == IO_REGULAR)
			fid = open(s->err->string,
					   O_RDWR | O_CREAT | O_TRUNC, 0644);
		else if (s->io_flags == IO_ERR_APPEND)
			fid = open(s->err->string,
					   O_APPEND | O_RDWR | O_CREAT, 0644);
		else
			fprintf(stderr, "RAGAT4\n");
		if (fid < 0)
			fprintf(stderr, "RAGAT5\n");
		dup2(fid, STDERR_FILENO);
	}

	int result = chdir(s->params->string);

	if (stdin_des != -1)
		dup2(stdin_des, STDIN_FILENO);

	if (stdout_des != -1)
		dup2(stdout_des, STDOUT_FILENO);

	if (stderr_des != -1)
		dup2(stderr_des, STDERR_FILENO);

	return result;
}

static int shell_exit(void)
{
	exit(0);
}

static int parse_simple(simple_command_t *s, int level, command_t *father)
{

	pid_t pid, wait_ret;
	int status;
	int size;
	char **params;
	int *redirects;

	if (strcmp(s->verb->string, "true") == 0)
		return 0;

	if (strcmp(s->verb->string, "false") == 0)
		return 1;

	if (strcmp(s->verb->string, "cd") == 0)
		if (s->params != NULL)
			return shell_cd(s);

	if (strcmp(s->verb->string, "exit") == 0 ||
		strcmp(s->verb->string, "quit") == 0)
		shell_exit();


	if (s->verb != NULL && s->verb->next_part != NULL &&
		strcmp(s->verb->next_part->string, "=") == 0) {
		set_variable(s);
		return 0;
	}

	pid = fork();
	switch (pid) {
	case -1:
		perror("fork");
	case 0:
		redirects = redirect(s);
		params = get_argv(s, &size);
		int result = execvp(s->verb->string, params);

		fflush(stdout);
		reset_redirect(redirects);
		for (int i = 0; i < size; ++i)
			free(params[i]);
		free(params);
		free(redirects);
		return result;

	default:
		wait_ret = waitpid(pid, &status, 0);
		if (status != 0 || wait_ret == -1)
			return -1;
		return 0;
	}
}

static void reset_redirect(int *redirects)
{

	if (redirects[0] != -1)
		close(redirects[0]);

	if (redirects[1] != -1)
		close(redirects[1]);

	if (redirects[2] != -1)
		close(redirects[2]);
}

static int *redirect(simple_command_t *s)
{
	char *filename;
	int fid = -1;
	int *redirects = malloc(sizeof(int) * 3);

	if (s->in != NULL) {
		filename = get_word(s->in);
		fid = open(filename, O_RDONLY);
		redirects[0] = fid;
		dup2(fid, STDIN_FILENO);
		free(filename);
	}

	if (s->out != NULL) {
		filename = get_word(s->out);
		if (s->io_flags == IO_REGULAR)
			fid = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0644);
		else if (s->io_flags == IO_OUT_APPEND)
			fid = open(filename, O_APPEND | O_RDWR | O_CREAT, 0644);
		else
			fprintf(stderr, "IO flags error\n");
		if (fid < 0)
			fprintf(stderr, "Error opening file\n");

		redirects[1] = fid;
		dup2(fid, STDOUT_FILENO);
		if (s->err != NULL &&
			strcmp(s->out->string, s->err->string) == 0) {
			dup2(fid, STDERR_FILENO);
			redirects[2] = -1;
			return redirects;
		}
		free(filename);
	}

	if (s->err != NULL) {
		filename = get_word(s->err);
		if (s->io_flags == IO_REGULAR)
			fid = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0644);
		else if (s->io_flags == IO_ERR_APPEND)
			fid = open(filename, O_APPEND | O_RDWR | O_CREAT, 0644);
		else
			fprintf(stderr, "RAGAT4\n");
		if (fid < 0)
			fprintf(stderr, "RAGAT5\n");
		dup2(fid, STDERR_FILENO);
		redirects[2] = fid;
	}
	return redirects;
}

static void set_variable(simple_command_t *s)
{
	char *name;
	char *value;

	name = strdup(s->verb->string);
	value = get_word(s->verb->next_part->next_part);
	setenv(name, value, 1);

}

static bool do_in_parallel(command_t *cmd1, command_t *cmd2, int level,
						   command_t *father)
{
	pid_t pid1, pid2;
	int status = -1;
	int ret_code;

	pid1 = fork();

	switch (pid1) {
	case -1:
		fprintf(stderr, "ERROR");
		break;

	case 0:
		ret_code = parse_command(cmd1, level + 1, father);
		exit(ret_code);

	default:
		pid2 = fork();
		switch (pid2) {
		case -1:
			fprintf(stderr, "ERROR");
			break;
		case 0:
			ret_code = parse_command(cmd2, level + 1, father);
			exit(ret_code);
		default:
			waitpid(pid1, &status, 0);
			waitpid(pid2, &status, 0);
		}
	}

	if (status == 0)
		return true;
	return false;
}

static bool do_on_pipe(command_t *cmd1, command_t *cmd2, int level,
					   command_t *father)
{
	int fid[2];
	pid_t pid1, pid2;
	int status = -1;
	int ret_code;

	pipe(fid);
	pid1 = fork();

	switch (pid1) {
	case -1:
		fprintf(stderr, "ERROR");
		break;

	case 0:
		close(fid[0]);
		dup2(fid[1], STDOUT_FILENO);
		ret_code = parse_command(cmd1, level + 1, father);
		close(fid[1]);
		exit(ret_code);

	default:
		pid2 = fork();
		switch (pid2) {
		case -1:
			fprintf(stderr, "ERROR");
			break;
		case 0:
			close(fid[1]);
			dup2(fid[0], STDIN_FILENO);
			ret_code = parse_command(cmd2, level + 1, father);
			close(fid[0]);
			exit(ret_code);
		default:
			waitpid(pid1, &status, 0);
			close(fid[1]);
			waitpid(pid2, &status, 0);
			close(fid[0]);
		}
	}

	if (status == 0)
		return true;
	return false;
}

int parse_command(command_t *c, int level, command_t *father)
{

	if (c->op == OP_NONE)
		return parse_simple(c->scmd, level, father);

	switch (c->op) {
	case OP_SEQUENTIAL:
		parse_command(c->cmd1, level + 1, c);
		return parse_command(c->cmd2, level + 1, c);

	case OP_PARALLEL:
		if (do_in_parallel(c->cmd1, c->cmd2, level, c))
			return 0;
		return 1;

	case OP_CONDITIONAL_NZERO:
		if (parse_command(c->cmd1, level + 1, c) != 0)
			return parse_command(c->cmd2, level + 1, c);
		return 0;

	case OP_CONDITIONAL_ZERO:
		if (parse_command(c->cmd1, level + 1, c) == 0)
			parse_command(c->cmd2, level + 1, c);
		return 1;

	case OP_PIPE:
		if (do_on_pipe(c->cmd1, c->cmd2, level, c))
			return 0;
		return 1;

	default:
		return SHELL_EXIT;
	}

}
