void handle_keydown(game_state_t *game_state, renderer_t *renderer, font_t **fonts,
                    SDL_Window **window, SDL_Event event)
{
    // ESC key
    if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
    {
        if (game_state->type == PAUSE_MODE)
        {
            game_state->type = GAME_MODE;
        }
        else if (game_state->type == GAME_MODE)
        {
            game_state->pause->selected_option = OPTION_START;
            game_state->type = PAUSE_MODE;
        }
        else if (game_state->type == MENU_MODE)
        {
            game_state->running = false;
        }
    }

    // other keys in MENU
    else if (game_state->type == MENU_MODE)
    {
        // up
        if (event.key.keysym.scancode == SDL_SCANCODE_UP)
        {
            game_state->menu->selected_option--;
        }
        // down
        else if (event.key.keysym.scancode == SDL_SCANCODE_DOWN)
        {
            game_state->menu->selected_option++;
        }
        // left
        else if (event.key.keysym.scancode == SDL_SCANCODE_LEFT)
        {
            if (game_state->menu->current_menu == SETTINGS_MENU)
            {
                switch (game_state->menu->selected_option)
                {
                    case SETTINGS_OPTION_VSYNC:
                        if (game_state->vsync)
                        {
                            int flags = SDL_RENDERER_ACCELERATED;
                            create_renderer(renderer, fonts, window, flags);
                            game_state->vsync = false;
                        }
                        else
                        {
                            int flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
                            create_renderer(renderer, fonts, window, flags);
                            game_state->vsync = true;
                        }
                        break;
                    case SETTINGS_OPTION_AA:
                        if (game_state->anti_aliasing)
                        {
                            int flags = SDL_RENDERER_ACCELERATED;
                            if (game_state->vsync) flags |= SDL_RENDERER_PRESENTVSYNC;
                            disable_antialiasing(renderer, fonts, window, flags);
                            game_state->anti_aliasing = false;
                        }
                        else
                        {
                            int flags = SDL_RENDERER_ACCELERATED;
                            if (game_state->vsync) flags |= SDL_RENDERER_PRESENTVSYNC;
                            enable_antialiasing(renderer, fonts, window, flags);
                            game_state->anti_aliasing = true;
                        }
                        break;
                    case SETTINGS_OPTION_VOLUME:
                        game_state->volume -= 5;
                        if (game_state->volume < 0) game_state->volume = 0;
                        break;
                    default: break;
                }
            }
        }
        // right
        else if (event.key.keysym.scancode == SDL_SCANCODE_RIGHT)
        {
            if (game_state->menu->current_menu == SETTINGS_MENU)
            {
                switch (game_state->menu->selected_option)
                {
                    case SETTINGS_OPTION_VSYNC:
                        if (game_state->vsync)
                        {
                            int flags = SDL_RENDERER_ACCELERATED;
                            create_renderer(renderer, fonts, window, flags);
                            game_state->vsync = false;
                        }
                        else
                        {
                            int flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
                            create_renderer(renderer, fonts, window, flags);
                            game_state->vsync = true;
                        }
                        break;
                    case SETTINGS_OPTION_AA:
                        if (game_state->anti_aliasing)
                        {
                            int flags = SDL_RENDERER_ACCELERATED;
                            if (game_state->vsync) flags |= SDL_RENDERER_PRESENTVSYNC;
                            disable_antialiasing(renderer, fonts, window, flags);
                            game_state->anti_aliasing = false;
                        }
                        else
                        {
                            int flags = SDL_RENDERER_ACCELERATED;
                            if (game_state->vsync) flags |= SDL_RENDERER_PRESENTVSYNC;
                            enable_antialiasing(renderer, fonts, window, flags);
                            game_state->anti_aliasing = true;
                        }
                        break;
                    case SETTINGS_OPTION_VOLUME:
                        game_state->volume += 5;
                        if (game_state->volume > 100) game_state->volume = 100;
                        break;
                    default: break;
                }
            }
        }
        // return
        else if (event.key.keysym.scancode == SDL_SCANCODE_RETURN)
        {
            if (game_state->menu->current_menu == MAIN_MENU)
            {
                switch (game_state->menu->selected_option)
                {
                    case OPTION_START:
                        game_state->type = GAME_MODE;
                        break;
                    case OPTION_SETTINGS:
                        enter_settings_menu(game_state);
                        break;
                    case OPTION_EXIT:
                        game_state->running = false;
                        break;
                    default: break;
                }
            }
            else if (game_state->menu->current_menu == SETTINGS_MENU)
            {
                switch (game_state->menu->selected_option)
                {
                    case SETTINGS_OPTION_VSYNC:
                        break;
                    case SETTINGS_OPTION_AA:
                        break;
                    case SETTINGS_OPTION_BACK:
                        game_state->menu->selected_option = OPTION_SETTINGS;
                        game_state->menu->current_menu = MAIN_MENU;
                        break;
                    default: break;
                }
            }
        }

        // wrap at bottom/top
        if (game_state->menu->current_menu == MAIN_MENU)
        {
            if (game_state->menu->selected_option == -1)
            {
                game_state->menu->selected_option = OPTION_COUNT - 1;
            }
            if (game_state->menu->selected_option == OPTION_COUNT)
            {
                game_state->menu->selected_option = 0;
            }
        }
        else if (game_state->menu->current_menu == SETTINGS_MENU)
        {
            if (game_state->menu->selected_option == -1)
            {
                game_state->menu->selected_option = SETTINGS_OPTION_COUNT - 1;
            }
            if (game_state->menu->selected_option == SETTINGS_OPTION_COUNT)
            {
                game_state->menu->selected_option = 0;
            }
        }
    }

    // other keys in PAUSE
    else if (game_state->type == PAUSE_MODE)
    {
        if (event.key.keysym.scancode == SDL_SCANCODE_UP ||
                event.key.keysym.scancode == SDL_SCANCODE_DOWN)
        {
            if (game_state->pause->selected_option == OPTION_START)
            {
                game_state->pause->selected_option = OPTION_EXIT;
            }
            else
            {
                game_state->pause->selected_option = OPTION_START;
            }
        }
        else if (event.key.keysym.scancode == SDL_SCANCODE_RETURN)
        {
            switch (game_state->pause->selected_option)
            {
                case OPTION_START:
                    game_state->type = GAME_MODE;
                    break;
                case OPTION_EXIT:
                    game_state->game = reset_game(game_state->game);
                    game_state->menu->selected_option = OPTION_START;
                    game_state->menu->current_menu = MAIN_MENU;
                    game_state->type = MENU_MODE;
                    break;
                default:
                    assert(0);
                    break;
            }
        }
    }
}

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

        // make selected option glow
        {
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

        // spawn particles
        // TODO: stop using hardcoded values here
        if (menu->last_angle > 200000) menu->last_angle = 0;
        if (menu->time_since_last_color >= 0.1)
        {
            menu->time_since_last_color = menu->time_since_last_color - 0.1 + dt;

            menu->particles_color.x = rand() % 255;
            menu->particles_color.y = rand() % 255;
            menu->particles_color.z = rand() % 255;
        }
        menu->time_since_last_color += dt;
        if (menu->time_since_last_spawn >= 0.05)
        {
            menu->time_since_last_spawn = menu->time_since_last_spawn - 0.05 + dt;

            double x, y;
            for (int i = 0; i < 24; i++)
            {
                double radians = (2 * PI / 24) * i + (menu->last_angle += 0.001);
                x = cos(radians);
                y = sin(radians);

                v2 pos = V2(DEFAULT_SCREEN_WIDTH / 2, DEFAULT_SCREEN_HEIGHT / 4);
                particle_t *p = spawn_particle_towards(pos, V2(x, y), 0, 200, V2(0, 0),
                                                       BALL_IMG_PATH, 30, 30,
                                                       menu->particles_color, 0);
                menu->particles->push_back(p);
            }
        }
        menu->time_since_last_spawn += dt;

        // update particles
        std::list<particle_t *>::iterator it, end;
        for (it = menu->particles->begin(), end = menu->particles->end(); it != end;)
        {
            particle_t *particle = *it;

            update_particle_position(particle, dt);

            // TODO: use current instead of default (we don't have current yet :()
            if (particle->pos.x < -DEFAULT_SCREEN_WIDTH  - 40||
                particle->pos.x > DEFAULT_SCREEN_WIDTH + 40 ||
                particle->pos.y < -DEFAULT_SCREEN_HEIGHT  - 40||
                particle->pos.y > DEFAULT_SCREEN_HEIGHT + 40)
            {
                it = menu->particles->erase(it);
                free(particle);
                continue;
            }

            ++it;
        }
    }
    else if (game_state->type == PAUSE_MODE)
    {
        assert(game_state->pause);

        pause_mode_t *pause = game_state->pause;

        if (pause->alpha_increasing)
        {
            pause->alpha_selected_option += (int) (MENU_FONT_GLOW_RATE * dt);
        }
        else
        {
            pause->alpha_selected_option -= (int) (MENU_FONT_GLOW_RATE * dt);
        }

        if (pause->alpha_selected_option > 255 - MENU_FONT_GLOW_FIXED)
        {
            pause->alpha_selected_option = 255 - MENU_FONT_GLOW_FIXED;
            pause->alpha_increasing = false;
        }
        else if (pause->alpha_selected_option < 0)
        {
            pause->alpha_selected_option = 0;
            pause->alpha_increasing = true;
        }
    }
}

void game_state_render(game_state_t *game_state, font_t **fonts, renderer_t *renderer)
{
    assert(game_state);
    assert(fonts);
    assert(*fonts);
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
        render_entity(renderer, game->player);

        // render enemy
        rect.x = (int) round(game->enemy.pos.x - game->enemy.w / 2);
        rect.y = (int) round(game->enemy.pos.y - game->enemy.h / 2);
        rect.w = (int) round(game->enemy.w);
        rect.h = (int) round(game->enemy.h);
        display_image(renderer, game->enemy.image_path, &rect, 0, V3(255, 255, 255), 255, 0, 0);

        // render particles
        std::list<particle_t *>::iterator it, end;
        for (it = game->particles->begin(), end = game->particles->end(); it != end; ++it)
        {
            particle_t *particle = *it;

            rect.x = (int) round(particle->pos.x - particle->w / 2);
            rect.y = (int) round(particle->pos.y - particle->h / 2);
            rect.w = (int) round(particle->w);
            rect.h = (int) round(particle->h);

            display_image(renderer, particle->image_path, &rect, 0, particle->color, 255, 0, 0);
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
                display_image(renderer, HP_IMG_PATH, &rect, 0, V3(255, 255, 255), 255, 0, 0);
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
                display_image(renderer, HP_IMG_PATH, &rect, &source, V3(255, 255, 255), 255, 0, 0);
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
            display_image(renderer, HP_IMG_PATH, &rect, 0, V3(255, 0, 0), 255, 0, 0);
        }

        if (game_state->type == PAUSE_MODE)
        {
            assert(game_state->pause);

            pause_mode_t *pause = game_state->pause;

            // render options box
            {
                SDL_Rect box = {
                    PAUSE_BOX_START_X,
                    PAUSE_BOX_START_Y,
                    PAUSE_BOX_END_X - PAUSE_BOX_START_X,
                    PAUSE_BOX_END_Y - PAUSE_BOX_START_Y
                };

                display_image(renderer, MENU_BOX_PATH, &box, 0, V3(255, 255, 255),
                        PAUSE_BOX_ALPHA, 0, 0);
            }

            // render options
            {
                for (int i = 0; i < OPTION_COUNT; i++)
                {
                    if (i != OPTION_START && i != OPTION_EXIT) continue;

                    const char *text;
                    int y;
                    switch (i)
                    {
                        case OPTION_START:
                            text = "Resume";
                            y = PAUSE_START_Y;
                            break;
                        case OPTION_EXIT:
                            text = "Exit";
                            y = PAUSE_START_Y + 4 * MENU_SPACING / 3;
                            break;
                        default:
                            assert(0);
                            break;
                    }

                    v3 color;
                    if (pause->selected_option == i) color = {255, 255, 0};
                    else color = {255, 255, 255};

                    u8 alpha;
                    if (pause->selected_option == i)
                    {
                        alpha = (u8) (MENU_FONT_GLOW_FIXED + pause->alpha_selected_option);
                    }
                    else alpha = 255;

                    draw_centralized_text(text, renderer, fonts[2], PAUSE_START_X,
                                          y, PAUSE_END_X, color, alpha);
                }
            }
        }
    }
    else if (game_state->type == MENU_MODE)
    {
        menu_mode_t *menu = game_state->menu;
        assert(menu);

        // render background
        // TODO: stop using hardcoded values here
        SDL_SetRenderDrawColor(renderer->sdl, 110, 0, 0, 255);
        SDL_RenderClear(renderer->sdl);

        // render particles
        {
            SDL_Rect rect;
            std::list<particle_t *>::iterator it, end;
            for (it = menu->particles->begin(), end = menu->particles->end(); it != end; ++it)
            {
                particle_t *particle = *it;

                rect.x = (int) round(particle->pos.x - particle->w / 2);
                rect.y = (int) round(particle->pos.y - particle->h / 2);
                rect.w = (int) round(particle->w);
                rect.h = (int) round(particle->h);

                display_image(renderer, particle->image_path, &rect, 0, particle->color, 255, 0, 0);
            }
        }

        // render options box
        {
            SDL_Rect box = {
                MENU_BOX_START_X,
                MENU_BOX_START_Y,
                MENU_BOX_END_X - MENU_BOX_START_X,
                MENU_BOX_END_Y - MENU_BOX_START_Y
            };

            display_image(renderer, MENU_BOX_PATH, &box, 0, V3(255, 255, 255),
                          MENU_BOX_ALPHA, 0, 0);
        }

        // render options
        {
            // main menu options
            if (menu->current_menu == MAIN_MENU)
            {
                for (int i = 0; i < OPTION_COUNT; i++)
                {
                    const char *text;
                    bool spaced = false;
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
                            spaced = true;
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

                    int space = 0;
                    if (spaced) space = MENU_SPACING;

                    draw_centralized_text(text, renderer, fonts[2], MENU_START_X,
                                          MENU_START_Y + i * MENU_SPACING + space,
                                          MENU_END_X, color, alpha);
                }
            }
            // settings menu options
            else if (menu->current_menu == SETTINGS_MENU)
            {
                for (int i = 0; i < SETTINGS_OPTION_COUNT; i++)
                {
                    const char *text, *value_text;
                    bool spaced = false;

                    v3 color;
                    if (menu->selected_option == i) color = {255, 255, 0};
                    else color = {255, 255, 255};

                    u8 alpha;
                    if (menu->selected_option == i) alpha = (u8) (MENU_FONT_GLOW_FIXED +
                        menu->alpha_selected_option);
                    else alpha = 255;

                    switch (i)
                    {
                        case SETTINGS_OPTION_VSYNC:
                            text = "Vertical sync";
                            if (game_state->vsync) value_text = "ON";
                            else value_text = "OFF";
                            draw_centralized_text(value_text, renderer, fonts[1], MENU_END_X - 30,
                                                  MENU_START_Y - 70 + i * MENU_SPACING,
                                                  MENU_END_X + 30, color, alpha);
                            break;
                        case SETTINGS_OPTION_AA:
                            text = "Anti-aliasing";
                            if (game_state->anti_aliasing) value_text = "ON";
                            else value_text = "OFF";
                            draw_centralized_text(value_text, renderer, fonts[1], MENU_END_X - 30,
                                                  MENU_START_Y - 70 + i * MENU_SPACING,
                                                  MENU_END_X + 30, color, alpha);
                            break;
                        case SETTINGS_OPTION_VOLUME:
                            text = "Volume";
                            char volume_text[5];
                            sprintf(&volume_text[0], "%d%%", game_state->volume);
                            draw_centralized_text(volume_text, renderer, fonts[1], MENU_END_X - 30,
                                                  MENU_START_Y - 70 + i * MENU_SPACING,
                                                  MENU_END_X + 30, color, alpha);
                            break;
                        case SETTINGS_OPTION_BACK:
                            text = "Back";
                            spaced = true;
                            break;
                        default:
                            text = "";
                            break;
                    }

                    int space = 0;
                    if (spaced) space = MENU_SPACING * 4;

                    draw_text(text, renderer, fonts[1], MENU_START_X - 36,
                              MENU_START_Y - 70 + i * MENU_SPACING + space, color, alpha);
                }
            }
        }
    }
}

