#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <GL/gl.h>

#include <string>
#include <list>
#include <map>

#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <cassert>

#include "main.hpp"
#include "bin_heap.hpp"
#include "constants.hpp"
#include "math.hpp"
#include "game_mode.hpp"
#include "menu_mode.hpp"
#include "game_state.hpp"
#include "text.hpp"

#include "bin_heap.cpp"
#include "math.cpp"
#include "utils.cpp"
#include "text.cpp"
#include "menu_mode.cpp"
#include "game_initialization.cpp"
#include "game_mode.cpp"
#include "game_state.cpp"

int main(int, char *[])
{
    // initialize video
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        force_quit("Failed to initialize SDL.\n");
    }

    // create window
    SDL_Window* window = SDL_CreateWindow("Bullet Hell",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          DEFAULT_SCREEN_WIDTH,
                                          DEFAULT_SCREEN_HEIGHT,
                                          SDL_WINDOW_SHOWN);
    if (!window)
    {
        force_quit("Failed to create window.\n");
    }

    // TODO: handle error (should just print a message, not crash the program)
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 2);
    glEnable(GL_MULTISAMPLE_ARB);

    // create renderer
#if 0
    SDL_Renderer* sdl_renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
#else
    SDL_Renderer* sdl_renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
#endif
    if (!sdl_renderer)
    {
        SDL_DestroyWindow(window);
        force_quit("Failed to create renderer.\n");
    }
    renderer_t renderer = {};
    renderer.sdl = sdl_renderer;

    // initialization
    // TODO: set seed for RNG
    font_t *fonts[] =
    {
        initialize_font(&renderer, FONT_PATH, 20),
        initialize_font(&renderer, FONT_PATH, 26),
        initialize_font(&renderer, FONT_PATH, 32),
    };
    // TODO: error check font initialization
    input_t *input = initialize_input();
    game_state_t *game_state = (game_state_t *) calloc(1, sizeof(*game_state));
    assert(game_state);
    game_state->game = initialize_game_mode();
    game_state->menu = initialize_menu_mode();
    //game_state->pause = initialize_pause_mode();
    game_state->type = MENU_MODE;

    // DEBUG
    // TODO: maybe make this hero's special atk?
    /*
    particle_t *p = spawn_particle_orbiting(&game_state->game->player.pos, 50, 0, 0,
                                            ENTITY_PLAYER, 6, BALL_IMG_PATH, 80, 80,
                                            V3(255, 255, 255));
    game_state->game->particles->push_back(p);
    p = spawn_particle_orbiting(&game_state->game->player.pos, 50, 0, PI/3, ENTITY_PLAYER, 6,
                                BALL_IMG_PATH, 80, 80, V3(255, 255, 255));
    game_state->game->particles->push_back(p);
    p = spawn_particle_orbiting(&game_state->game->player.pos, 50, 0, 2 * PI/3, ENTITY_PLAYER, 6,
                                BALL_IMG_PATH, 80, 80, V3(255, 255, 255));
    game_state->game->particles->push_back(p);
    p = spawn_particle_orbiting(&game_state->game->player.pos, 50, 0, 3 * PI/3, ENTITY_PLAYER, 6,
                                BALL_IMG_PATH, 80, 80, V3(255, 255, 255));
    game_state->game->particles->push_back(p);
    p = spawn_particle_orbiting(&game_state->game->player.pos, 50, 0, 4 * PI/3, ENTITY_PLAYER, 6,
                                BALL_IMG_PATH, 80, 80, V3(255, 255, 255));
    game_state->game->particles->push_back(p);
    p = spawn_particle_orbiting(&game_state->game->player.pos, 50, 0, 5 * PI/3, ENTITY_PLAYER, 6,
                                BALL_IMG_PATH, 80, 80, V3(255, 255, 255));
    game_state->game->particles->push_back(p);
    */

    // main loop
    bool running = true;
    u64 current_counter = SDL_GetPerformanceCounter();
    u64 last_counter = 0;
    double delta_time = 0;
    while (running)
    {
        // handle events
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_MOUSEMOTION:
                    input->mouse.x = event.motion.x;
                    input->mouse.y = event.motion.y;
                    break;
                case SDL_KEYDOWN: // TODO: find a better way to handle keydowns and keyups
                    if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
                    {
                        if (game_state->type == PAUSE_MODE)
                        {
                            game_state->type = GAME_MODE;
                        }
                        else if (game_state->type == GAME_MODE)
                        {
                            game_state->type = PAUSE_MODE;
                        }
                        else if (game_state->type == MENU_MODE)
                        {
                            running = false;
                        }
                    }

                    else if (game_state->type == MENU_MODE)
                    {
                        if (event.key.keysym.scancode == SDL_SCANCODE_UP)
                        {
                            game_state->menu->selected_option--;
                            if (game_state->menu->selected_option == OPTION_BLANK)
                            {
                                game_state->menu->selected_option--;
                            }
                            if (game_state->menu->selected_option == -1)
                            {
                                game_state->menu->selected_option = OPTION_COUNT - 1;
                            }
                        }
                        else if (event.key.keysym.scancode == SDL_SCANCODE_DOWN)
                        {
                            game_state->menu->selected_option++;
                            if (game_state->menu->selected_option == OPTION_BLANK)
                            {
                                game_state->menu->selected_option++;
                            }
                            if (game_state->menu->selected_option == OPTION_COUNT)
                            {
                                game_state->menu->selected_option = 0;
                            }
                        }
                        else if (event.key.keysym.scancode == SDL_SCANCODE_RETURN)
                        {
                            switch (game_state->menu->selected_option)
                            {
                                case OPTION_START:
                                    game_state->type = GAME_MODE;
                                    break;
                                case OPTION_SETTINGS:
                                    // TODO
                                    break;
                                case OPTION_EXIT:
                                    running = false;
                                    break;
                                default: break;
                            }
                        }
                    }
                    break;
                default: break;
            }
        }

        // calculate time elapsed since last frame
        last_counter = current_counter;
        current_counter = SDL_GetPerformanceCounter();
        delta_time = (double) ((abs(current_counter - last_counter)) /
                               (double) SDL_GetPerformanceFrequency());

        // update world for this frame
        game_state_update(game_state, input, delta_time);

        // render frame
        game_state_render(game_state, fonts, &renderer);
        draw_fps(delta_time, &renderer, fonts[0]);
        SDL_RenderPresent(renderer.sdl);
    }

    SDL_DestroyRenderer(renderer.sdl);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
