#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

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
#include "pause_mode.hpp"
#include "game_state.hpp"
#include "text.hpp"

#include "bin_heap.cpp"
#include "math.cpp"
#include "text.cpp"
#include "utils.cpp"
#include "menu_mode.cpp"
#include "pause_mode.cpp"
#include "game_initialization.cpp"
#include "game_mode.cpp"
#include "game_state.cpp"

int main(int, char *[])
{
    // initialize video
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
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

    // initialize mixer
    audio_t mixer = {};
    if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 1024) >= 0)
    {
        // TODO: remove hardcoded value
        mixer.shot = Mix_LoadWAV("assets/shot.wav");
    }
    else
    {
        printf("Failed to initialize audio mixer.\n");
    }

    // disable multisampling by default
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);
    glDisable(GL_MULTISAMPLE_ARB);

    // create renderer
#if 1
    int flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
#else
    int flags = SDL_RENDERER_ACCELERATED;
#endif
    font_t **fonts = (font_t **) malloc(3 * sizeof(*fonts));
    renderer_t renderer = {};
    create_renderer(&renderer, fonts, &window, flags);

    // initialization
    // TODO: set seed for RNG
    input_t *input = initialize_input();
    game_state_t *game_state = (game_state_t *) calloc(1, sizeof(*game_state));
    assert(game_state);
    game_state->game = initialize_game_mode();
    game_state->menu = initialize_menu_mode();
    game_state->pause = initialize_pause_mode();
    game_state->type = MENU_MODE;
    game_state->running = true;
    game_state->volume = 100;
    game_state->vsync = true;
    game_state->anti_aliasing = false;

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
    u64 current_counter = SDL_GetPerformanceCounter();
    u64 last_counter = 0;
    double delta_time = 0;
    while (game_state->running)
    {
        // handle events
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    game_state->running = false;
                    break;
                case SDL_MOUSEMOTION:
                    input->mouse.x = event.motion.x;
                    input->mouse.y = event.motion.y;
                    break;
                case SDL_KEYDOWN: // TODO: find a better way to handle keydowns and keyups
                    handle_keydown(game_state, &renderer, fonts, &window, event);
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
        game_state_update(game_state, input, mixer, delta_time);

        // render frame
        game_state_render(game_state, fonts, &renderer);
        draw_fps(delta_time, &renderer, fonts[0]);
        SDL_RenderPresent(renderer.sdl);
    }

    // clean up
    SDL_DestroyRenderer(renderer.sdl);
    SDL_DestroyWindow(window);
    Mix_CloseAudio();
    SDL_Quit();

    return 0;
}
