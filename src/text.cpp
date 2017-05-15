void create_texture_for_glyphs(SDL_Renderer *renderer, u16 index, TTF_Font *ttf_font,
                               font_t *font)
{
    // default font color
    SDL_Color white;
    white.r = 255;
    white.g = 255;
    white.b = 255;
    white.a = 255;

    SDL_Surface *surface = 0;
    if (!(surface = TTF_RenderGlyph_Blended(ttf_font, index, white)))
    {
        puts("Failed to create surface from text.");
        printf("Error: %s\n", TTF_GetError());
        return; // call force_quit or handle this some other way
    }

    SDL_Texture *texture = 0;
    if (!(texture = SDL_CreateTextureFromSurface(renderer, surface)))
    {
        puts("Failed to create texture from surface.");
        printf("Error: %s\n", TTF_GetError());
        return; // call force_quit or handle this some other way
    }

    SDL_FreeSurface(surface);

    font->glyphs[index].texture = texture;
    if(SDL_QueryTexture(texture, 0, 0, &font->glyphs[index].w, &font->glyphs[index].h) < 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return;
    }
}

// Creates glyphs
font_t *initialize_font(renderer_t *renderer, const char *path, int size)
{
    font_t *font = (font_t *) malloc(sizeof(*font));
    assert(font);

    if (TTF_Init() == -1) {
        puts("Failed to initialize TTF.");
        printf("Error: %s\n", TTF_GetError());
        return 0;
    }

    TTF_Font *ttf_font;
    if (!(ttf_font = TTF_OpenFont(path, size)))
    {
        puts("Failed to open font file.");
        printf("Error: %s\n", TTF_GetError());
        return 0;
    }

    // create a texture for each glyph
    for (int i = 0; i < 10; i++)
    {
        create_texture_for_glyphs(renderer->sdl, (u16) ('0' + i), ttf_font, font);
    }
    for (int i = 0; i < 26; i++)
    {
        create_texture_for_glyphs(renderer->sdl, (u16) ('a' + i), ttf_font, font);
    }
    for (int i = 0; i < 26; i++)
    {
        create_texture_for_glyphs(renderer->sdl, (u16) ('A' + i), ttf_font, font);
    }
    create_texture_for_glyphs(renderer->sdl, (u16) ' ', ttf_font, font);
    create_texture_for_glyphs(renderer->sdl, (u16) ':', ttf_font, font);
    create_texture_for_glyphs(renderer->sdl, (u16) ';', ttf_font, font);
    create_texture_for_glyphs(renderer->sdl, (u16) ',', ttf_font, font);
    create_texture_for_glyphs(renderer->sdl, (u16) '.', ttf_font, font);
    create_texture_for_glyphs(renderer->sdl, (u16) '!', ttf_font, font);
    create_texture_for_glyphs(renderer->sdl, (u16) '?', ttf_font, font);
    create_texture_for_glyphs(renderer->sdl, (u16) '(', ttf_font, font);
    create_texture_for_glyphs(renderer->sdl, (u16) ')', ttf_font, font);
    create_texture_for_glyphs(renderer->sdl, (u16) '{', ttf_font, font);
    create_texture_for_glyphs(renderer->sdl, (u16) '}', ttf_font, font);
    create_texture_for_glyphs(renderer->sdl, (u16) '[', ttf_font, font);
    create_texture_for_glyphs(renderer->sdl, (u16) ']', ttf_font, font);
    create_texture_for_glyphs(renderer->sdl, (u16) '+', ttf_font, font);
    create_texture_for_glyphs(renderer->sdl, (u16) '-', ttf_font, font);
    create_texture_for_glyphs(renderer->sdl, (u16) '*', ttf_font, font);
    create_texture_for_glyphs(renderer->sdl, (u16) '/', ttf_font, font);
    create_texture_for_glyphs(renderer->sdl, (u16) '\\', ttf_font, font);
    create_texture_for_glyphs(renderer->sdl, (u16) '&', ttf_font, font);
    create_texture_for_glyphs(renderer->sdl, (u16) '#', ttf_font, font);
    create_texture_for_glyphs(renderer->sdl, (u16) '@', ttf_font, font);
    create_texture_for_glyphs(renderer->sdl, (u16) '"', ttf_font, font);
    create_texture_for_glyphs(renderer->sdl, (u16) '\'', ttf_font, font);
    create_texture_for_glyphs(renderer->sdl, (u16) '%', ttf_font, font);
    create_texture_for_glyphs(renderer->sdl, (u16) '$', ttf_font, font);
    create_texture_for_glyphs(renderer->sdl, (u16) '=', ttf_font, font);
    create_texture_for_glyphs(renderer->sdl, (u16) '_', ttf_font, font);

    return font;
}

// Draws text on x,y coordinates.
void draw_text(const char *str, renderer_t *renderer, font_t *font, int x, int y, v3 color,
               int alpha)
{
    int len = (int) strlen(str);
    for (int i = 0; i < len; i++)
    {
        SDL_Texture *glyph = font->glyphs[(int) str[i]].texture;
        SDL_Rect rect;

        if(SDL_QueryTexture(glyph, 0, 0, &rect.w, &rect.h) < 0)
        {
            printf("Error: %s\n", SDL_GetError());
            return;
        }

        rect.w = (int) round((float) rect.w);
        rect.h = (int) round((float) rect.h);
        rect.x = x + rect.w * i;
        rect.y = y;

        SDL_SetTextureColorMod(glyph, (u8) color.x, (u8) color.y, (u8) color.z);
        SDL_SetTextureAlphaMod(glyph, (u8) alpha);
        if(SDL_RenderCopy(renderer->sdl, glyph, 0, &rect) < 0)
        {
            printf("Error: %s\n", SDL_GetError());
            return;
        }
    }
}

void draw_centralized_text(const char *str, renderer_t *renderer, font_t * font, int x, int y,
                           int end_x, v3 color, int alpha)
{
    int width = 0;
    for (int i = 0; i < (int) strlen(str); i++)
    {
        width += (int) ((double) font->glyphs[(int) str[i]].w);
    }

    assert(end_x - x - width > 0); // TODO: handle this as an error

    int start = (int) ((end_x - x - width) / 2);
    draw_text(str, renderer, font, start + x, y, color, alpha);
}

// TODO: probably put this in utils.h, or...
// TODO: remove this function, instead create a "calculate_fps" and draw the return value
// Draws the average fps on the screen
// NOTE: this function must be called exactly once every frame in order to give
//       correct results
void draw_fps(double dt, renderer_t *renderer, font_t *font)
{
    static char fps_text[15] = "";
#if 1
    static double timer = 0;
    static int counter = 0;
    timer += dt;
    counter++;
    if (timer > 0.08) 
    {
        snprintf(&fps_text[0], 15, "FPS: %.00f", 1 / (timer / counter));
        timer = 0;
        counter = 0;
    }
#else
        snprintf(&fps_text[0], 15, "FPS: %.00f", 1 / dt);
#endif
    draw_text(&fps_text[0], renderer, font, 0, 0, V3(255, 255, 255), 255);
}
