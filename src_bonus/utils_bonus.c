/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 22:55:05 by alex              #+#    #+#             */
/*   Updated: 2026/05/31 21:33:46 by alejandro        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_bonus.h"

/**
 * @brief  Resolves a command string into a valid binary path. 
 * Skips if prefixed with "./". Queries $PATH env string,
 * splits it, and evaluates candidate paths.
 *
 * @param  x_file  The input command name or string pointer.
 * @param  env     The environment variables matrix (envp).
 * @return char* Absolute binary path on success, x_file if 
 * found locally via access(), or NULL if invalid.
 */
char	*check_exe(char *x_file, char **env)
{
	char	**absolute_paths_table;
	char	*absolute_paths;
	char	*path;

	if (!ft_strncmp(x_file, "./", 2))
		return (x_file);
	absolute_paths = get_env_value("PATH", env);
	if (!absolute_paths)
	{
		if (access(x_file, F_OK) != -1)
			return (x_file);
		return (NULL);
	}
	absolute_paths_table = ft_split(absolute_paths, ':');
	if (!absolute_paths_table || !absolute_paths[0])
		return (NULL);
	path = find_exe_file(absolute_paths_table, x_file);
	if (!path)
	{
		if (access(x_file, F_OK) != -1)
			return (ft_free_table(absolute_paths_table), x_file);
		return (ft_free_table(absolute_paths_table), NULL);
	}
	ft_free_table(absolute_paths_table);
	return (path);
}

/**
 * @brief  Searches the environment array for a specific variable
 * key and returns a pointer to its value segment.
 *
 * @param  key_value  The environment variable key name (e.g., "PATH").
 * @param  env        The environment array matrix (envp).
 * @return char* Pointer to the value substring within envp matrix,
 * or NULL if the key is not located.
 */
char	*get_env_value(const char *key_value, char **env)
{
	int		i;
	size_t	key_len;
	char	*new_value;

	if (!key_value)
		return (NULL);
	key_len = ft_strlen(key_value);
	if (key_len == 0 || env == NULL)
		return (NULL);
	new_value = ft_strjoin(key_value, "=");
	if (new_value == NULL)
		return (NULL);
	i = 0;
	while (env[i] != NULL)
	{
		if ((ft_strncmp(new_value, env[i], key_len + 1)) == 0)
		{
			free(new_value);
			return ((env[i] + key_len + 1));
		}
		i++;
	}
	free(new_value);
	return (NULL);
}

/**
 * @brief  Iterates through possible directory tokens, builds absolute
 * paths, and tests execution rights via access().
 *
 * @param  posible_paths  Split directory path strings array matrix.
 * @param  x_file         Target command binary name to locate.
 * @return char* Allocated verified absolute path on success,
 * or NULL if command is not found.
 */
char	*find_exe_file(char **posible_paths, char *x_file)
{
	char	*relative_path;
	char	*aux;
	int		i;

	if (!posible_paths || !x_file)
		return (NULL);
	i = 0;
	while (posible_paths[i])
	{
		aux = ft_strjoin(posible_paths[i], "/");
		if (!aux)
			return (NULL);
		relative_path = ft_strjoin(aux, x_file);
		if (!relative_path)
		{
			free(aux);
			return (NULL);
		}
		if (access(relative_path, F_OK | X_OK) != -1)
			return (free(aux), relative_path);
		i++;
		free(aux);
		free(relative_path);
	}
	return (NULL);
}

/**
 * @brief  Standard error workflow. Prints diagnostics to STDERR, 
 * manages custom parameter deallocations safely, clears 
 * descriptors, and forces termination via exit(1).
 *
 * @param  table1   Optional command table array pointer to free.
 * @param  ptr1     Optional dedicated dynamic string tracker to free.
 * @param  message  Custom error context message string.
 * @param  error    System error translation message (strerror).
 * @return void
 */
void	ft_error(char **table1, char *ptr1, char *message, char *error)
{
	if (message)
	{
		ft_putstr_fd(message, 2);
		ft_putstr_fd(": ", 2);
		ft_putstr_fd(error, 2);
		ft_putstr_fd("\n", 2);
	}
	else
		perror("");
	if ((message && table1) && (message == table1[0]))
		message = NULL;
	if ((ptr1 && table1) && (ptr1 != table1[0]))
	{
		free(ptr1);
		ptr1 = NULL;
	}
	if (table1)
		ft_free_table(table1);
	if (message)
	{
		free(message);
		message = NULL;
	}
	close_fds(NULL);
	exit(1);
}

/**
 * @brief  Deallocates a null-terminated pointer array matrix 
 * row by row before freeing the master pointer block.
 *
 * @param  ptr  Pointer to the double char matrix array to clear.
 * @return void
 */
void	ft_free_table(char **ptr)
{
	int	i;

	if (ptr == NULL)
		return ;
	i = 0;
	while (ptr[i] != NULL)
	{
		free(ptr[i]);
		ptr[i] = NULL;
		i++;
	}
	free(ptr);
	ptr = NULL;
}
