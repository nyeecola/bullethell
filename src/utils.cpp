void force_quit(const char *str)
{
    puts(str);
    printf("Error: %s\n", SDL_GetError());
    SDL_Quit();
    exit(1);
}

input_t *initialize_input()
{
    input_t *input = (input_t *) malloc(sizeof(*input));

    input->keys_pressed = SDL_GetKeyboardState(0);
    input->mouse.x = 0;
    input->mouse.y = 0;

    return input;
}

// verifies if image was already renderer
// if not, render and display it
// if yes, just display it
// TODO: check if this is too slow (because of the conversion to std::string every call
void display_image(renderer_t *renderer, const char *char_path, SDL_Rect *dest, SDL_Rect *source,
                   v3 color, int alpha, bool flip_x, bool flip_y)
{
    std::string path(char_path);
    if (!renderer->images.count(path))
    {
        SDL_Texture *tex = IMG_LoadTexture(renderer->sdl, char_path);
        renderer->images[path] = tex;
    }

    SDL_RendererFlip flip = (SDL_RendererFlip) 0;
    if (flip_x) flip = (SDL_RendererFlip) (flip | SDL_FLIP_HORIZONTAL);
    if (flip_y) flip = (SDL_RendererFlip) (flip | SDL_FLIP_VERTICAL);

    SDL_SetTextureColorMod(renderer->images[char_path], (u8) color.x, (u8) color.y, (u8) color.z);
    SDL_SetTextureAlphaMod(renderer->images[char_path], (u8) alpha);
    SDL_RenderCopyEx(renderer->sdl, renderer->images[char_path], source, dest, 0, 0, flip);
}

inline bool test_point_in_circle(v2 point, v2 center, double radius)
{
    if (pow(center.x - point.x, 2) + pow(center.y - point.y, 2) < pow(radius, 2))
    {
        return true;
    }

    return false;
}

inline double get_current_player_hit_circle_radius(entity_t entity)
{
    assert(entity.type == ENTITY_PLAYER);

    return (PARTICLE_DESTROY_ON_HIT_DURATION *
            PARTICLE_DESTROY_ON_HIT_DELTA_RADIUS - 
            PARTICLE_DESTROY_ON_HIT_DELTA_RADIUS *
            entity.player_data.time_since_hit);
}

void create_renderer(renderer_t *renderer, font_t **fonts, SDL_Window **window, int flags)
{
    if (renderer->sdl) SDL_DestroyRenderer(renderer->sdl);
    SDL_DestroyWindow(*window);
    *window = SDL_CreateWindow("Bullet Hell",
                               SDL_WINDOWPOS_UNDEFINED,
                               SDL_WINDOWPOS_UNDEFINED,
                               DEFAULT_SCREEN_WIDTH,
                               DEFAULT_SCREEN_HEIGHT,
                               SDL_WINDOW_SHOWN);

    renderer->sdl = SDL_CreateRenderer(*window, -1, flags);
    if (!renderer->sdl)
    {
        SDL_DestroyWindow(*window);
        force_quit("Failed to create renderer.\n");
    }

    renderer->images = std::map<std::string, SDL_Texture *>();

    fonts[0] = initialize_font(renderer, FONT_PATH, 16);
    fonts[1] = initialize_font(renderer, FONT_PATH, 20);
    fonts[2] = initialize_font(renderer, FONT_PATH, 32);
}

void enable_antialiasing(renderer_t *renderer, font_t **fonts, SDL_Window **window, int flags)
{
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 2);
    glEnable(GL_MULTISAMPLE_ARB);

    create_renderer(renderer, fonts, window, flags);
}

void disable_antialiasing(renderer_t *renderer, font_t **fonts, SDL_Window **window, int flags)
{
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);
    glDisable(GL_MULTISAMPLE_ARB);

    create_renderer(renderer, fonts, window, flags);
}
