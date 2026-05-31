/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/05 14:39:57 by alex              #+#    #+#             */
/*   Updated: 2026/05/31 21:31:30 by alejandro        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_bonus.h"

/**
 * @brief  Evaluates arguments to initialize input. If "here_doc" is detected, 
 * it provisions a unique temp file in `/tmp/` and reads from STDIN via GNL 
 * until the specified limiter is matched, expanding environment variables on the fly.
 * @param  arg      The execution arguments array (argv).
 * @param  argz     Total number of execution arguments (argc).
 * @param  control  Pointer to the command tracking index (shifts to 4 if here_doc).
 * @param  env      The execution environment matrix (envp).
 * @return char* Dynamically allocated string containing the input filename 
 * (original infile or the temporary here_doc path).
 */
char	*parse_prompt(char **arg, int argz, int *control, char **env)
{
	static char		*l;
	char			*src_name;
	int				fd_inter;

	if (!(ft_strnstr(arg[1], "here_doc", 8) && ft_strlen(arg[1]) == 8))
	{
		src_name = ft_strdup(arg[1]);
		return (src_name);
	}
	*control = 4;
	src_name = new_file_name("/tmp/");
	fd_inter = open(src_name, O_CREAT | O_TRUNC | O_APPEND | O_RDWR, 0664);
	if (fd_inter < 0 || argz < 6)
		return (free(src_name), NULL);
	write(1, ">", 1);
	l = get_next_line(0);
	while (ft_strncmp(l, arg[2], ft_strlen(arg[2])) != 0
		|| (ft_strlen(l) - 1) != ft_strlen(arg[2]))
	{
		write_line_in_heredoc(l, fd_inter, env);
		free(l);
		write(1, ">", 1);
		l = get_next_line(0);
	}
	return (free(l), close (fd_inter), src_name);
}

/**
 * @brief  Iterates through a string line character by character to write it to the 
 * temp file. Automatically intercepts the `$` token to trigger environment variable 
 * expansion.
 * @param  line      The current raw string line read from STDIN.
 * @param  fd_inter  File descriptor of the temporary here_doc file.
 * @param  env       The environment variables matrix (envp).
 * @return void
 */
void	write_line_in_heredoc(char *line, int fd_inter, char **env)
{
	unsigned int	i;

	if (!line)
		return ;
	i = 0;
	while (line[i] != '\0')
	{
		if (line[i] == '$')
		{
			if ((line[i + 1] != '(' && line[i + 1] != ' '
					&& line[i + 1] != '\n' && line[i + 1] != '$'))
			{
				i = write_env(&line[i], fd_inter, i, env);
			}
			else
				ft_putchar_fd(line[i], fd_inter);
		}
		else
			ft_putchar_fd(line[i], fd_inter);
		i++;
	}
}

/**
 * @brief  Extracts a clean substring by slicing out characters bounded by subshell 
 * parenthetical tokens. (Helper tool for nested/token formatting).
 * @param  line  The target string sequence to be trimmed.
 * @return char* Dynamically allocated trimmed string, or NULL on allocation failure.
 */
char	*trim_line(char *line)
{
	int		i;

	i = 0;
	while (line[i] != ')')
		i++;
	line = ft_substr(line, 2, i - 2);
	if (!line)
		return (NULL);
	return (line);
}

/**
 * @brief  Parses out an environment variable key name following a `$` symbol, 
 * queries its value from envp, and writes the expanded content directly to the file descriptor.
 * @param  line   Pointer to the starting position of the variable key inside the string buffer.
 * @param  fd     The target file descriptor where the expanded variable string is written.
 * @param  count  The current absolute index position within the string reader loop.
 * @param  env    The system environment variable matrix (envp).
 * @return unsigned int  The updated absolute index position pointing past the expanded variable key.
 */
unsigned int	write_env(char *line, int fd, unsigned int count, char **env)
{
	char		*env_value;
	char		*env_name;
	int			i;

	i = 0;
	while (line[i] != ' ' && line[i] != '\n')
		i++;
	count = count + i;
	env_name = ft_substr((const char *)line, 1, i -1);
	if (!env_name)
		return (0);

	env_value = get_env_value(env_name, env);
	if (!env_value)
	{
		free(env_name);
		return (count - 1);
	}
	ft_putstr_fd(env_value, fd);
	return (free(env_name), count - 1);
}

/**
 * @brief  Generates an isolated, non-colliding unique file path inside a target directory 
 * by checking file existence sequentially using incremented numerical suffixes and `access()`.
 * @param  path   The target directory prefix (e.g., "/tmp/").
 * @return char* Dynamically allocated absolute string path of the guaranteed unique file.
 */
char	*new_file_name(char *path)
{
	int		name;
	char	*file_name;
	char	*name_str;

	name = 0;
	while (1)
	{
		file_name = ft_itoa(name);
		name_str = ft_strjoin(path, file_name);
		if (!name_str)
			return (NULL);
		if (access(name_str, F_OK))
			break ;
		name++;
		free(file_name);
		free(name_str);
	}
	free(file_name);
	return (name_str);
}
