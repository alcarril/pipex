/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alex <alex@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 22:55:05 by alex              #+#    #+#             */
/*   Updated: 2025/03/26 05:25:12 by alex             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

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
