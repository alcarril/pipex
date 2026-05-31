/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alcarril <alcarril@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/03 17:47:53 by alcarril          #+#    #+#             */
/*   Updated: 2025/03/27 18:10:55 by alcarril         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

void	imput_process(char **argv, int *pipe_ports, char **env)
{
	char			**comands;
	char			*x_f;
	pid_t			id;

	if (pipe(pipe_ports) == -1)
		ft_error(NULL, NULL, NULL, NULL);
	id = fork();
	if (id < 0)
		ft_error(NULL, NULL, NULL, NULL);
	if (id == 0)
	{
		pipe_forward(pipe_ports, 1, STDOUT_FILENO);
		if (tunel_file(argv[1], 0) < 0)
			exit(1);
		comands = ft_split(argv[2], ' ');
		if (!comands || !comands[0])
			ft_error2(comands);
		x_f = comands[0];
		x_f = check_exe(x_f, env);
		if (!x_f || execve(x_f, comands, NULL) == -1)
			ft_error(comands, NULL, x_f, NULL);
	}
}

void	output_process(char **argv, int *pipe_ports, char **env)
{
	char	**comands;
	char	*x_file;
	pid_t	id;
	int		status;

	pipe_forward(pipe_ports, 0, STDIN_FILENO);
	id = fork();
	if (id < 0)
		ft_error(NULL, NULL, NULL, NULL);
	if (id == 0)
	{
		if (tunel_file(argv[4], 1) < 0)
			ft_error(NULL, NULL, NULL, NULL);
		comands = ft_split(argv[3], ' ');
		if (!comands || !comands[0])
			ft_error2(comands);
		x_file = comands[0];
		x_file = check_exe(x_file, env);
		if (!x_file || execve(x_file, comands, NULL) == -1)
			ft_error(comands, NULL, x_file, NULL);
	}
	id = waitpid(-1, &status, 0);
	while (id > 0)
		id = waitpid(-1, &status, 0);
}

int	tunel_file(char *file, char flag)
{
	int		fd_target;

	if (flag == 0)
	{
		fd_target = open(file, O_RDONLY);
		if (fd_target < 0)
			return (-1);
		if (dup2(fd_target, STDIN_FILENO) == -1)
			return (close(fd_target), -1);
	}
	else
	{
		fd_target = open(file, O_CREAT | O_TRUNC | O_WRONLY, 0664);
		if (fd_target < 0)
			return (-1);
		if (dup2(fd_target, STDOUT_FILENO) == -1)
			return (close(fd_target), -1);
	}
	close(fd_target);
	return (fd_target);
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

int	main(int argz, char **argv, char **env)
{
	int		pipe_ports[2];

	if (argz == 5)
	{
		imput_process(argv, pipe_ports, env);
		output_process(argv, pipe_ports, env);
		return (0);
	}
	return (ft_putstr_fd(ERROR, 2), 1);
}
