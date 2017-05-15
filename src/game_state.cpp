void game_state_update(game_state_t *game_state, input_t *input, double dt)
{
    assert(game_state);
    assert(input);
    assert(dt > 0);

    if (game_state->type == GAME_MODE)
    {
        game_mode_t *game = game_state->game;
        assert(game);

        // player's actions movement
        // TODO: remove this check since we'll handle death differently
        if (game->player.health > 0)
        {
            do_players_actions(game, input, dt);
        }

        // perform enemy's actions
        ai_do_actions(game, dt);

        // update particles
        std::list<particle_t *>::iterator it, end;
        for (it = game->particles->begin(), end = game->particles->end(); it != end;)
        {
            particle_t *particle = *it;

            update_particle_position(particle, dt);

            // TODO: use current instead of default (we don't have current yet :()
            if (particle->pos.x < -DEFAULT_SCREEN_WIDTH / 2 ||
                particle->pos.x > DEFAULT_SCREEN_WIDTH + DEFAULT_SCREEN_WIDTH / 2 ||
                particle->pos.y < -DEFAULT_SCREEN_HEIGHT / 2 ||
                particle->pos.y > DEFAULT_SCREEN_HEIGHT + DEFAULT_SCREEN_HEIGHT / 2)
            {
                it = game->particles->erase(it);
                free(particle);
                continue;
            }

            if (particle->timed)
            {
                particle->time_to_live -= dt;
                if (particle->time_to_live <= 0)
                {
                    it = game->particles->erase(it);
                    free(particle);
                    continue;
                }
            }

            if (detect_particle_collision(game, particle))
            {
                it = game->particles->erase(it);
                free(particle);
                continue;
            }

            ++it;
        }

        // handle player hit
#if 1
        {
            if (game->player.player_data.hit)
            {
                game->player.player_data.hit = false;
                game->player.player_data.time_since_hit = PARTICLE_DESTROY_ON_HIT_DURATION;

                // TODO: think about it
                // game->player.pos = V2(300, 700);

                if (game->player.health <= 0)
                {
                    game_state->game = reset_game(game_state->game);
                    return;
                }
            }

            if (game->player.player_data.time_since_hit > 0)
            {
                game->player.player_data.time_since_hit -= dt * PARTICLE_DESTROY_ON_HIT_DELTA_SPEED;

                std::list<particle_t *>::iterator it, end;
                for (it = game->particles->begin(), end = game->particles->end(); it != end;)
                {
                    particle_t *particle = *it;

                    if (test_point_in_circle(particle->pos, game->player.pos,
                                             get_current_player_hit_circle_radius(game->player)) &&
                        particle->owner != ENTITY_PLAYER)
                    {
                        it = game->particles->erase(it);
                        free(particle);
                        continue;
                    }

                    ++it;
                }
            }
        }
#endif

        // update background
        {
            double multiplier = 40;
            static bool blue_decreasing = false;
            if (blue_decreasing)
            {
                game->background_color.blue -= multiplier * dt;
                if (game->background_color.blue < 120) blue_decreasing = false;
            }
            else
            {
                game->background_color.blue += multiplier * dt;
                if (game->background_color.blue > 250) blue_decreasing = true;
            }
        }
    }
    else if (game_state->type == MENU_MODE)
    {
        assert(game_state->menu);

        menu_mode_t *menu = game_state->menu;

        if (menu->alpha_increasing)
        {
            menu->alpha_selected_option += (int) (MENU_FONT_GLOW_RATE * dt);
        }
        else
        {
            menu->alpha_selected_option -= (int) (MENU_FONT_GLOW_RATE * dt);
        }

        if (menu->alpha_selected_option > 255 - MENU_FONT_GLOW_FIXED)
        {
            menu->alpha_selected_option = 255 - MENU_FONT_GLOW_FIXED;
            menu->alpha_increasing = false;
        }
        else if (menu->alpha_selected_option < 0)
        {
            menu->alpha_selected_option = 0;
            menu->alpha_increasing = true;
        }
    }
    else if (game_state->type == PAUSE_MODE)
    {
        assert(game_state->menu);
    }
}

void game_state_render(game_state_t *game_state, font_t **fonts, renderer_t *renderer)
{
    assert(game_state);
    assert(renderer);
    assert(renderer->sdl);

    // render game (even if it is currently paused)
    if (game_state->type == GAME_MODE || game_state->type == PAUSE_MODE)
    {
        assert(game_state->game);
        game_mode_t *game = game_state->game;

        // render background
        SDL_SetRenderDrawColor(renderer->sdl,
                               (u8) round(game->background_color.red),
                               (u8) round(game->background_color.green),
                               (u8) round(game->background_color.blue),
                               (u8) round(game->background_color.alpha));
        SDL_RenderClear(renderer->sdl);

        // declare rect that will store the render destination (on screen)
        SDL_Rect rect;

        // render player
        rect.x = (int) round(game->player.pos.x - game->player.w / 2);
        rect.y = (int) round(game->player.pos.y - game->player.h / 2);
        rect.w = (int) round(game->player.w);
        rect.h = (int) round(game->player.h);
        display_image(renderer, game->player.image_path, &rect, 0, V3(255, 255, 255));

        // render enemy
        rect.x = (int) round(game->enemy.pos.x - game->enemy.w / 2);
        rect.y = (int) round(game->enemy.pos.y - game->enemy.h / 2);
        rect.w = (int) round(game->enemy.w);
        rect.h = (int) round(game->enemy.h);
        display_image(renderer, game->enemy.image_path, &rect, 0, V3(255, 255, 255));

        // render particles
        std::list<particle_t *>::iterator it, end;
        for (it = game->particles->begin(), end = game->particles->end(); it != end; ++it)
        {
            particle_t *particle = *it;

            rect.x = (int) round(particle->pos.x - particle->w / 2);
            rect.y = (int) round(particle->pos.y - particle->h / 2);
            rect.w = (int) round(particle->w);
            rect.h = (int) round(particle->h);

            display_image(renderer, particle->image_path, &rect, 0, particle->color);
        }

        // render player special attacks
        // TODO: figure out if there is a better way to do this (put it somewhere else?)
        if (game->player.player_data.time_since_hit > 0)
        {
            int num_vertices = 80;
            int px = (int) game->player.pos.x;
            int py = (int) game->player.pos.y;
            for (int i = 1; i <= num_vertices; i++)
            {
                double radius = get_current_player_hit_circle_radius(game->player);

                double radians1 = (2 * PI / num_vertices) * (i - 1);

                int x1 = (int) (cos(radians1) * radius);
                int y1 = (int) (sin(radians1) * radius);

                double radians2 = (2 * PI / num_vertices) * i;

                int x2 = (int) (cos(radians2) * radius);
                int y2 = (int) (sin(radians2) * radius);

                SDL_SetRenderDrawColor(renderer->sdl, 255, 80, 80, 255);
                SDL_RenderDrawLine(renderer->sdl, x1 + px, y1 + py, x2 + px, y2 + py);
            }
        }

        // render enemy health
        int enemy_hearts = (int) ceil((double) game->enemy.health / HP_PER_HEART);
        for (int i = 1; i <= enemy_hearts; i++)
        {
            // destination rect
            rect.w = HP_UNIT_SIZE;
            rect.h = HP_UNIT_SIZE;
            // TODO: change to current screen width
            rect.x = DEFAULT_SCREEN_WIDTH - HP_UNIT_SIZE * i;
            rect.y = ENEMY_HP_BAR_Y;

            // render only part of the last heart if current hp is not a multiple of 100
            int hp_in_heart = game->enemy.health - HP_PER_HEART * (i - 1);
            if (hp_in_heart > HP_PER_HEART)
            {
                display_image(renderer, HP_IMG_PATH, &rect, 0, V3(255, 255, 255));
            }
            else
            {
                SDL_Rect source = {};
                double percentage_to_fill = ((double) hp_in_heart / HP_PER_HEART) * HP_IMG_SIZE;
                source.w = (int) percentage_to_fill;
                source.h = (int) HP_IMG_SIZE;
                source.x = HP_IMG_SIZE - source.w;
                rect.x += HP_IMG_SIZE - source.w;
                rect.w = source.w;
                display_image(renderer, HP_IMG_PATH, &rect, &source, V3(255, 255, 255));
            }
        }

        // render player health
        for (int i = 1; i <= game->player.health; i++)
        {
            rect.w = HP_UNIT_SIZE;
            rect.h = HP_UNIT_SIZE;
            // TODO: change to current screen width
            rect.x = DEFAULT_SCREEN_WIDTH - HP_UNIT_SIZE * i;
            rect.y = PLAYER_HP_BAR_Y;
            display_image(renderer, HP_IMG_PATH, &rect, 0, V3(255, 0, 0));
        }

        if (game_state->type == PAUSE_MODE)
        {
            //assert(game_state->pause);
            // TODO
        }
    }
    else if (game_state->type == MENU_MODE)
    {
        assert(game_state->menu);

        menu_mode_t *menu = game_state->menu;

        // render background
        SDL_SetRenderDrawColor(renderer->sdl, 170, 20, 20, 255);
        SDL_RenderClear(renderer->sdl);

        // render options box
        {
            SDL_Rect box = {
                MENU_BOX_START_X,
                MENU_BOX_START_Y,
                MENU_BOX_END_X - MENU_BOX_START_X,
                MENU_BOX_END_Y - MENU_BOX_START_Y
            };

            SDL_SetRenderDrawBlendMode(renderer->sdl, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer->sdl, 0, 0, 0, MENU_BOX_ALPHA);
            SDL_RenderFillRect(renderer->sdl, &box);
            SDL_SetRenderDrawBlendMode(renderer->sdl, SDL_BLENDMODE_NONE);
        }

        // render options
        {
            for (int i = 0; i < OPTION_COUNT; i++)
            {
                const char *text;
                switch (i)
                {
                    case OPTION_START:
                        text = "Start";
                        break;
                    case OPTION_SETTINGS:
                        text = "Settings";
                        break;
                    case OPTION_EXIT:
                        text = "Exit";
                        break;
                    default:
                        text = "";
                        break;
                }

                v3 color;
                if (menu->selected_option == i) color = {255, 255, 0};
                else color = {255, 255, 255};

                u8 alpha;
                if (menu->selected_option == i) alpha = (u8) (MENU_FONT_GLOW_FIXED +
                                                              menu->alpha_selected_option);
                else alpha = 255;

                draw_centralized_text(text, renderer, fonts[2], MENU_START_X,
                                      MENU_START_Y + i * MENU_SPACING, MENU_END_X,
                                      color, alpha);
            }
        }
    }
}

