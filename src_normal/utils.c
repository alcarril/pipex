/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 22:55:05 by alex              #+#    #+#             */
/*   Updated: 2026/05/31 21:36:24 by alejandro        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

/**
 * @brief  Resolves a command string into a valid binary path by
 * querying $PATH, splitting it, and evaluating directory items.
 *
 * @param  x_file  The input command name or path pointer string.
 * @param  env     The environment variables matrix (envp).
 * @return char* Verified path on success, x_file if found locally
 * via access(), or NULL if validation fails.
 */
char	*check_exe(char *x_file, char **env)
{
	char	**posible_paths;
	char	*absolute_paths;
	char	*relative_path;

	absolute_paths = get_env_value("PATH", env);
	if (!absolute_paths)
	{
		if (access(x_file, F_OK | X_OK) != -1)
			return (x_file);
		return (NULL);
	}
	posible_paths = ft_split(absolute_paths, ':');
	if (!posible_paths || !absolute_paths[0])
		return (NULL);
	relative_path = search_relative_path(posible_paths, x_file);
	if (!relative_path)
	{
		if (access(x_file, F_OK | X_OK) != -1)
			return (ft_free_table(posible_paths), x_file);
		return (ft_free_table(posible_paths), NULL);
	}
	ft_free_table(posible_paths);
	return (relative_path);
}

/**
 * @brief  Searches the environment array for a matching variable
 * key name and returns a pointer to its value block.
 *
 * @param  key_value  The environment variable key name (e.g., "PATH").
 * @param  env        The environment array matrix (envp).
 * @return char* Pointer to the value segment within the envp array,
 * or NULL if the target key is not located.
 */
char	*get_env_value(const char *key_value, char **env)
{
	char	*new_value;
	size_t	len;
	int		i;

	if (!key_value)
		return (NULL);
	len = ft_strlen(key_value);
	if (len == 0 || env == NULL)
		return (NULL);
	new_value = ft_strjoin(key_value, "=");
	if (new_value == NULL)
		return (NULL);
	i = 0;
	while (env[i] != NULL)
	{
		if ((ft_strncmp(new_value, env[i], len + 1)) == 0)
		{
			free(new_value);
			return ((env[i] + len + 1));
		}
		i++;
	}
	free(new_value);
	return (NULL);
}

/**
 * @brief  Loops through available path tokens, appends the binary name,
 * and checks execution rights via access().
 *
 * @param  posible_paths  Split directory path strings array matrix.
 * @param  x_file         Target command binary name to locate.
 * @return char* Allocated absolute path string on success,
 * or NULL if the command cannot be located.
 */
char	*search_relative_path(char **posible_paths, char *x_file)
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
 * @brief  Standard error deallocation loop. Prints contextual errors via
 * perror(), clears designated resource pointers safely, closes
 * standard stream descriptors, and triggers exit(1).
 *
 * @param  table1  Optional string table matrix array to clear.
 * @param  table2  Secondary optional string table array to clear.
 * @param  str1    First optional standalone string buffer to free.
 * @param  str2    Second optional standalone string buffer to free.
 * @return void
 */
void	ft_error(char **table1, char **table2, char *str1, char *str2)
{
	if (str1 && str1 != table1[0])
		free(str1);
	if (str2 && str2 != table1[0])
		free(str2);
	if (table1)
		ft_free_table(table1);
	if (table2)
		ft_free_table(table2);
	perror(NULL);
	close (STDIN_FILENO);
	close (STDOUT_FILENO);
	close (STDERR_FILENO);
	exit(1);
}

/**
 * @brief  Iterates through a null-terminated pointer table row by
 * row to free elements before deallocating the master pointer.
 *
 * @param  ptr  Pointer to the double char matrix array to clear.
 * @return void
 */
void	ft_free_table(char **ptr)
{
	int		i;

	if (ptr == NULL)
		return ;
	i = 0;
	while (ptr[i] != NULL)
	{
		free(ptr[i]);
		i++;
	}
	free(ptr);
}
