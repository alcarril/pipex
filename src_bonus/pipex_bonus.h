/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_bonus.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alex <alex@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/27 03:03:16 by alex              #+#    #+#             */
/*   Updated: 2025/03/22 15:02:18 by alex             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_BONUS_H
# define PIPEX_BONUS_H

# include "../Include/libft/libft.h"
# include "../Include/ft_printf/ft_printf.h"
# include <unistd.h>
# include <fcntl.h>
# include <stdlib.h>
# include <errno.h>
# include <limits.h>
# include <stdio.h>
# include <sys/wait.h>

# define ERROR "Arguments ERROR: Need more arguments\n"
# define ERROR_HEREDOC "HERE_DOC arguments ERROR: Need more arguments\n"

typedef struct s_control
{
	int		control;
	int		start;
	char	**args;
	char	**env;
	int		num_args;
	char	*src_file;
}	t_control;

void			set_control(t_control *control, char **argv, int argz,
					char **env);
unsigned int	write_env(char *line, int fd, unsigned int count,
					char **env);
void			imput_process(t_control *control, int *pipe_ports);
void			link_pipes(t_control *control, int *first_pipe);
void			output_process(t_control *control, int *pipe_ports);
char			*check_exe(char *x_file, char **env);
char			*get_env_value(const char *key_value, char **env);
char			*find_exe_file(char **posible_paths, char *x_file);
void			ft_error(char **table1, char *ptr1, char *ptr2, char *message);
void			ft_free_table(char **ptr);
int				tunel_in_file(char *file);
int				tunel_out_file(char *file);
void			pipe_forward(int *pipe_reference, int pipe_port, int fd);
void			search_and_exec(t_control *control, int position);
char			*parse_prompt(char **arg, int argz, int *control, char **env);
void			write_line_in_heredoc(char *line, int fd_inter, char **env);
char			*trim_line(char *line);
char			*new_file_name(char *path);
void			close_fds(int *pipe_ports);

#endif