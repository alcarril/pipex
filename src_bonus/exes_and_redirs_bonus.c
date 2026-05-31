/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exes_and_redirs_bonus.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/07 02:56:26 by alex              #+#    #+#             */
/*   Updated: 2026/05/31 15:40:06 by alejandro        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_bonus.h"

int	tunel_in_file(char *file)
{
	int	fd_trgt;

	if (access(file, F_OK | R_OK) == -1)
		return (-1);
	fd_trgt = open(file, O_RDONLY, 0664);
	if (fd_trgt < 0)
		return (-1);
	if (dup2(fd_trgt, STDIN_FILENO) == -1)
	{
		close(fd_trgt);
		return (-1);
	}
	close(fd_trgt);
	return (fd_trgt);
}

int	tunel_out_file(char *file)
{
	int	fd_trgt;
	fd_trgt = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0664);
	if (fd_trgt < 0)
		return (-1);
	if (dup2(fd_trgt, STDOUT_FILENO) == -1)
	{
		close(fd_trgt);
		return (-1);
	}
	close(fd_trgt);
	return (fd_trgt);
}

void	pipe_forward(int *pipe_reference, int pipe_port, int fd)
{
	int		other_fd;

	if (pipe_port == 0)
		other_fd = 1;
	else
		other_fd = 0;
	close(pipe_reference[other_fd]);
	if (dup2(pipe_reference[pipe_port], fd) == -1)
	{
		close(pipe_reference[pipe_port]);
		ft_error(NULL, NULL, NULL, NULL);
	}
	close(pipe_reference[pipe_port]);
}

void	close_fds(int *pipe_ports)
{
	if (pipe_ports)
	{
		close(pipe_ports[0]);
		close(pipe_ports[1]);
	}
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
}

void	search_and_exec(t_control *c, int position)
{
	char	**orders_list;
	char	*comand;
	char	*x_file;

	orders_list = ft_split((const char *)(c->args[position]), ' ');
	if (!orders_list || !orders_list[0])
	{
		free(c->src_file);
		if (orders_list || !orders_list[0])
			ft_free_table(orders_list);
		ft_putstr_fd("comand not found\n", 2);
		exit(1);
	}
	comand = orders_list[0];
	x_file = check_exe(comand, c->env);
	if (x_file && (access(x_file, F_OK | X_OK) == -1))
	{
		ft_error(orders_list, c->src_file, x_file, strerror(errno));
	}
	else if (!x_file || execve(x_file, orders_list, c->env) == -1)
	{
		ft_error(orders_list, c->src_file, comand, "command not found");
	}
}
