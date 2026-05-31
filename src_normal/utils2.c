/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/27 17:42:02 by alcarril          #+#    #+#             */
/*   Updated: 2026/05/31 21:36:47 by alejandro        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

/**
 * @brief  Lightweight error routing. Frees the provided string table 
 * if it exists and immediately terminates execution via exit(1).
 *
 * @param  str  Optional null-terminated string array matrix to free.
 * @return void
 */
void	ft_error2(char **str)
{
	if (str)
		ft_free_table(str);
	exit (1);
}
