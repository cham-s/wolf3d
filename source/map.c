/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   map.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cattouma <cattouma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/05/14 20:12:11 by cattouma          #+#    #+#             */
/*   Updated: 2016/05/18 20:37:27 by cattouma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "wolf3d.h"

void	generate_texture(Uint32	texture[8][TEX_H * TEX_W])
{
	int x;
	int y;

	x = 0;
	y = 0;
	while (x < TEX_W)
	{
		while (y < TEX_H)
		{
			int xorcolor = (x * 256 / TEX_W) ^ (y * 256 / TEX_H);
			int ycolor = y * 256 / TEX_H;
			int xycolor = y * 128 / TEX_H + x * 128 / TEX_W;
			texture[0][TEX_W * y + x] = 65536 * 254 * (x != y && x != TEX_W - y);
			texture[1][TEX_W * y + x] = xycolor + 256 * xycolor + 65536 * xycolor;
			texture[2][TEX_W * y + x] = 256 * xycolor + 65536 * xycolor;
			texture[3][TEX_W * y + x] = xorcolor + 256 * xorcolor + 65536 * xorcolor;
			texture[4][TEX_W * y + x] = 256 * xorcolor;
			texture[5][TEX_W * y + x] = 65536 * 192 * (x % 16 && y % 16);
			texture[6][TEX_W * y + x] = 65536 * ycolor;;
			texture[7][TEX_W * y + x] = 128 + 256 * 128 + 65536 * 128;
			y++;
		}
		x++;
		y = 0;
	}
}

void	get_coord_text(t_winfo *w, t_map_info *mi, t_ray_info *ri)
{

	mi->tex_num = w->map[mi->map_x][mi->map_y] - 1; 
	// calculate value of wall x
	if (mi->side == 0)
		mi->wall_x = ri->ray_pos_y + mi->perp_wall_dist * ri->ray_dir_y;
	else
		mi->wall_x = ri->ray_pos_x + mi->perp_wall_dist * ri->ray_dir_x;
	mi->wall_x -= floor(mi->wall_x);
	// x coordinate on the texture
	mi->tex_x = (int)(mi->wall_x * (double)TEX_W);
	if (mi->side == 0 && ri->ray_dir_x > 0)
		mi->tex_x = TEX_W - mi->tex_x - 1;
	if (mi->side == 1 && ri->ray_dir_y < 0)
		mi->tex_x = TEX_W - mi->tex_x - 1;
}

void	extract_color_from_text(t_map_info *mi, Uint32 buffer[HEIGHT][WIDTH], Uint32 texture[8][TEX_H * TEX_W])
{
	int		y;
	int		d;
	int		tex_y;
	Uint32	color;

	y = mi->draw_start;
	while (y < mi->draw_end)
	{
		d = y * 256 - HEIGHT * 128 + mi->line_height * 128;
		tex_y = ((d * TEX_H) / mi->line_height) / 256;
		color = texture[mi->tex_num][TEX_H * tex_y + mi->tex_x];
		if (mi->side == 1)
			color = (color >> 1) & 8355711;
		buffer[y][mi->x] = color;
		y++;
	}
}

void	draw_map(t_winfo *w, t_map_info *mi, t_ray_info *ri)
{
	Uint32	buffer[HEIGHT][WIDTH];
	Uint32	texture[8][TEX_H * TEX_W];

	generate_texture(texture);
	clear_screen(w);
	mi->x = 0;
	while (mi->x < WIDTH)
	{
		calculate_ray_pos(ri, mi);
		perform_dda(mi, w);
		if (mi->side == 0)
			mi->perp_wall_dist = (mi->map_x - ri->ray_pos_x +
					(1 - mi->step_x) / 2) / ri->ray_dir_x;
		else
			mi->perp_wall_dist = (mi->map_y - ri->ray_pos_y +
					(1 - mi->step_y) / 2) / ri->ray_dir_y;
		mi->line_height = (int)(HEIGHT / mi->perp_wall_dist);
		mi->draw_start = -mi->line_height / 2 + HEIGHT / 2;
		if (mi->draw_start < 0)
			mi->draw_start = 0;
		mi->draw_end = mi->line_height / 2 + HEIGHT / 2;
		if (mi->draw_end >= HEIGHT)
			mi->draw_end = HEIGHT - 1;
		//draw_ceiling_wall_floor(w, mi, ri);
		get_coord_text(w, mi, ri);
		extract_color_from_text(mi, buffer, texture);
		draw_white_black(w, mi, buffer);
		//draw_w(w, mi->x, mi->draw_start, mi->draw_end, buffer);
		mi->x++;
	}
	w->first = 0;
	draw_mini_map(w, mi);
	SDL_RenderPresent(w->renderer);
}

static void	other_keys(t_winfo *w, SDL_Event *event)
{
	if (event->key.keysym.sym == SDLK_ESCAPE && w->first)
		w->running = 0;
	else if (event->key.keysym.sym == SDLK_ESCAPE && !w->show_menu)
	{
		Mix_PauseMusic();
		Mix_PlayChannel(-1, w->escape, 0);
		w->show_menu = 1;
	}
	else if (event->key.keysym.sym == SDLK_ESCAPE && w->show_menu)
	{
		Mix_ResumeMusic();
		w->show_menu = 0;
		w->index = 0;
	}
}

void	directions_key(t_winfo *w, t_time_info *ti, t_map_info *mi, SDL_Event *event)
{
	other_keys(w, event);
	if (event->key.keysym.sym == SDLK_UP)
	{
		if (w->show_menu)
		{
			w->index = w->index == 0 ? 1 : 0;
			Mix_PlayChannel(-1, w->move, 0);
		}
		else
			move_forward(mi, ti, w);
	}
	else if (event->key.keysym.sym == SDLK_DOWN)
	{
		if (w->show_menu)
		{
			w->index = w->index == 0 ? 1 : 0;
			Mix_PlayChannel(-1, w->move, 0);
		}
		else
			move_backward(mi, ti, w);
	}
	else if (event->key.keysym.sym == SDLK_RIGHT && !w->show_menu)
		turn_right(mi);
	else if (event->key.keysym.sym == SDLK_LEFT && !w->show_menu)
		turn_left(mi);
}

void	enter_key(t_winfo *w, SDL_Event *event)
{
	if (event->key.keysym.sym == SDLK_RETURN &&
				w->index == 1 && w->show_menu)
		w->running = 0;
	else if (event->key.keysym.sym == SDLK_RETURN &&
				w->index == 0 && w->show_menu)
	{
		Mix_PlayChannel(-1, w->start, 0);
		w->index = 0;
		w->show_menu = 0;
	}
}

void	toggle_music(t_winfo *w)
{
	if (Mix_PlayingMusic() == 0)
		Mix_PlayMusic(w->music, -1);
	else
	{
		if (Mix_PausedMusic() == 1)
			Mix_ResumeMusic();
		else
			Mix_PauseMusic();
	}
}

void	reg_key(t_winfo *w, t_map_info *mi, SDL_Event *event)
{
	if (event->key.keysym.sym == SDLK_p && !w->show_menu)
		toggle_music(w);
	else if (event->key.keysym.sym == SDLK_r)
		init_map_info(mi, w);
}

void	keydown(t_winfo *w, t_map_info *mi, t_time_info *ti, SDL_Event *event)
{
	directions_key(w, ti, mi, event);
	reg_key(w, mi, event);
	enter_key(w, event);
}

/* void	text(t_winfo * w) */
/* { */
/* 	int w = 0, h = 0; */
/* 	Uint32 r, g, b, a; */

/* 	if (SDL_BYTEORDER == SDL_BIG_ENDIAN) */ 
/* 	{ */
/* 		r = 0xff000000; */
/* 		g = 0x00ff0000; */
/* 		b = 0x0000ff00; */
/* 		a = 0x000000ff; */
/* 	} */
/* 	else */
/* 	{ */
/* 		r = 0x000000ff; */
/* 		g = 0x0000ff00; */
/* 		b = 0x00ff0000; */
/* 		a = 0xff000000; */
/* 	} */

/* 	SDL_Surface; */
/* 	surface = SDL_CreateRGBSurface(0, WIDTH, HEIGHT, 32, r, g, b, a); */
/* 	//create map; */
/* 	float map[WIDTH * HEIGHT + 2]; */
/* 	ft_memset(map, 0, WIDTH * HEIGHY + 2); */

/* 	map[]; */

/* 	SDL_RenderPresent(w->renderer); */
/* } */

void	draw(t_winfo *w)
{
	//malloc instead?
	t_ray_info	ri;
	t_map_info	mi;
	t_time_info	ti;
	SDL_Event	event;

	//
	(void)ri;
	init_map_info(&mi, w);
	ti.time = 0;
	ti.oldtime = 0;
	while (w->running)
	{
		change_time_values(&ti);
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				w->running = 0;
			else if (event.key.type == SDL_KEYDOWN)
				keydown(w, &mi, &ti, &event);
		}
		if (w->show_menu)
			render_menu(w);
		else
			draw_map(w, &mi, &ri);
	}
}
