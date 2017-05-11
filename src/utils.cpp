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
// TODO: verify if this is too slow (because of the conversion to std::string every call
void display_image(renderer_t *renderer, const char *char_path, SDL_Rect *dest, SDL_Rect *source, v3 color)
{
    std::string path(char_path);
    if (!renderer->images.count(path))
    {
        SDL_Texture *tex = IMG_LoadTexture(renderer->sdl, char_path);
        renderer->images[path] = tex;
    }

    SDL_SetTextureColorMod(renderer->images[char_path], (u8) color.x, (u8) color.y, (u8) color.z);
    SDL_RenderCopy(renderer->sdl, renderer->images[char_path], source, dest);
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
