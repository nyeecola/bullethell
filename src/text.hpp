struct glyph_t
{
    SDL_Texture *texture;
    int w;
    int h;
};

struct font_t
{
    glyph_t glyphs[256];
};
