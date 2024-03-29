/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cattouma <cattouma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/05/15 13:06:17 by cattouma          #+#    #+#             */
/*   Updated: 2016/05/20 00:18:04 by cattouma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "wolf3d.h"

static void	check_map(char *map)
{
	int			fd;
	struct stat	buf;

	if (stat(map, &buf) < 0)
	{
		ft_putstr_fd(strerror(errno), 2);
		ft_putstr_fd(": ", 2);
		ft_putendl_fd(map, 2);
		exit(EXIT_FAILURE);
	}
	if (!S_ISREG(buf.st_mode))
	{
		ft_putstr_fd("Not a regular file: ", 2);
		ft_putendl_fd(map, 2);
		exit(EXIT_FAILURE);
	}
	if ((fd = open(map, O_RDONLY)) < 0)
	{
		perror("Error ");
		exit(EXIT_FAILURE);
	}
}

int			check_args(int ac, char *map)
{
	char *ext;

	if (ac != 2)
	{
		ft_putstr_fd("usage : ", 2);
		ft_putstr_fd(map, 2);
		ft_putendl_fd("./wolf3d <map>", 2);
		exit(EXIT_FAILURE);
	}
	if (!(ext = ft_strrchr(map, '.')))
	{
		ft_putendl_fd("Error: wrong file extension use .w3d", 2);
		exit(EXIT_FAILURE);
	}
	if (ft_strcmp("w3d", ext + 1))
	{
		ft_putendl_fd("Error: wrong file extension use .w3d", 2);
		exit(EXIT_FAILURE);
	}
	check_map(map);
	return (EXIT_SUCCESS);
}

void		check_error(int *fd, int x, int y)
{
	char *line;

	if (get_next_line(*fd, &line) < 0)
	{
		ft_putendl_fd("Not a valid file", 2);
		exit(EXIT_FAILURE);
	}
	if (x == 0 && y == 0)
	{
		ft_putendl_fd("Empty map", 2);
		exit(EXIT_FAILURE);
	}
	if (x < 3 || y < 3)
	{
		ft_putendl_fd("Map too small", 2);
		exit(EXIT_FAILURE);
	}
	free(line);
}

void		check_len_map(int x_len, t_winfo *w)
{
	if (x_len != w->total_li)
	{
		ft_putendl_fd("Map not well formatted", 2);
		exit(EXIT_FAILURE);
	}
}
