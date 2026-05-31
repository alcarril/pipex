/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/03 17:47:53 by alcarril          #+#    #+#             */
/*   Updated: 2026/05/31 15:40:42 by alejandro        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_bonus.h"

void	imput_process(t_control *c, int *pipe_p)
{
	pid_t		id;

	if (pipe(pipe_p) == -1)
		ft_error(NULL, NULL, NULL, NULL);
	if (tunel_in_file(c->src_file) != -1)
	{
		id = fork();
		if (id < 0)
			ft_error(NULL, c->src_file, NULL, NULL);
		if (id == 0)
		{
			pipe_forward(pipe_p, 1, STDOUT_FILENO);
			search_and_exec(c, c->control - 1);
		}
	}
	else
		perror(c->src_file);
	if (c->control == 4)
		if (unlink(c->src_file) < 0)
			ft_error(NULL, NULL, c->src_file, strerror(errno));
	free(c->src_file);
	c->src_file = NULL;
}

void	link_pipes(t_control *c, int *first_pipe)
{
	static int	iterations_count;
	int			middle_pipes[2];
	int			id;

	if (iterations_count == 0)
		pipe_forward(first_pipe, 0, STDIN_FILENO);
	if (pipe(middle_pipes) == -1)
		ft_error(NULL, NULL, NULL, NULL);
	iterations_count++;
	id = fork();
	if (id < 0)
		ft_error(NULL, NULL, NULL, NULL);
	if (id == 0)
	{
		pipe_forward(middle_pipes, 1, STDOUT_FILENO);
		search_and_exec(c, c->control);
	}
	pipe_forward(middle_pipes, 0, STDIN_FILENO);
	close (middle_pipes[0]);
	close (middle_pipes[1]);
}

void	output_process(t_control *c, int *first_pipe_fd)
{
	pid_t	id;
	int		fd_out;
	int		status;

	if (c->start == c->num_args - 2)
		pipe_forward(first_pipe_fd, 0, STDIN_FILENO);
	id = fork();
	if (id < 0)
		ft_error(NULL, NULL, NULL, NULL);
	if (id == 0)
	{
		fd_out = tunel_out_file(c->args[c->num_args - 1]);
		if (fd_out < 0)
		{
			perror(c->args[c->num_args - 1]);
			close_fds(NULL);
			exit (1);
		}
		search_and_exec(c, c->num_args - 2);
	}
	id = waitpid(-1, &status, 0);
	while (id > 0)
		id = waitpid(-1, &status, 0);
	close_fds(NULL);
}

void	set_control(t_control *c, char **argv, int argz, char **env)
{
	if (!c)
		return ;
	c->control = 3;
	c->args = argv;
	c->num_args = argz;
	c->env = env;
	c->src_file = NULL;
}

int	main(int argz, char **argv, char **env)
{
	t_control	c;
	int			pipe_ports[2];

	set_control(&c, argv, argz, env);
	if (argz < 5)
		return (ft_putstr_fd(ERROR, 2), 1);
	c.src_file = parse_prompt(c.args, c.num_args, &(c.control), c.env);
	if (!c.src_file)
	{
		if (c.control == 4 && c.num_args < 6)
			return (ft_putstr_fd(ERROR_HEREDOC, 2), 1);
		return (1);
	}
	c.start = c.control;
	imput_process(&c, pipe_ports);
	while (c.control < argz - 2)
	{
		link_pipes(&c, pipe_ports);
		c.control++;
	}
	output_process(&c, pipe_ports);
	return (0);
}
