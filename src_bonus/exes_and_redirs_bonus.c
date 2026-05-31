/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exes_and_redirs_bonus.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/07 02:56:26 by alex              #+#    #+#             */
/*   Updated: 2026/05/31 21:29:07 by alejandro        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_bonus.h"

/**
 * @brief  Configures the input file by redirecting its content to STDIN.
 * Verifies file existence and read permissions before opening.
 * @param  file    Path to the input file (infile).
 * @return int     The generated file descriptor (fd_trgt) on success; 
 * -1 if the file doesn't exist, lacks permissions, or dup2 fails.
 */
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

/**
 * @brief  Configures the output file (outfile) by truncating it to 0 bytes
 * and redirecting STDOUT to it. Creates the file if it does not exist.
 * @param  file    Path to the output file (outfile).
 * @return int     The generated file descriptor (fd_trgt) on success;
 * -1 if opening the file or the dup2 redirection fails.
 */
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

/**
 * @brief  Redirects a specific end of a pipe to a standard descriptor (STDIN/STDOUT),
 * ensuring the immediate closure of the opposite, inactive end.
 * @param  pipe_reference  Integer array containing the two pipe ends (fd[2]).
 * @param  pipe_port       The end to duplicate (0 for reading, 1 for writing).
 * @param  fd              The target standard descriptor (STDIN_FILENO / STDOUT_FILENO).
 * @return void
 */
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

/**
 * @brief  Performs a total cleanup by safely closing the given pipe 
 * descriptors (if they exist) along with STDIN, STDOUT, and STDERR.
 * @param  pipe_ports      Array containing the pipe descriptors to close 
 * (can be NULL).
 * @return void
 */
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

/**
 * @brief  Parses a command string at a given index, locates its absolute binary 
 * using the envp ($PATH) directories, and executes it via execve.
 * Handles memory freeing and error routines if the command is invalid.
 * @param  c         Pointer to the main control structure holding environment and args.
 * @param  position  Index of the target command within the arguments matrix (argv).
 * @return void      (Does not return on success; exits via exit/ft_error on failure).
 */
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
