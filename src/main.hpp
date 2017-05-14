typedef int8_t s8;
typedef uint8_t u8;
typedef int16_t s16;
typedef uint16_t u16;
typedef int32_t s32;
typedef uint32_t u32;
typedef int64_t s64;
typedef uint64_t u64;

// ---------------------
// renderer
// ---------------------
struct renderer_t
{
    SDL_Renderer *sdl;
    // TODO: unordered map
    std::map<std::string, SDL_Texture *> images;
};

// ---------------------
// input
// ---------------------
struct mouse_t
{
    int x;
    int y;
};

struct input_t
{
    // keyboard
    const u8 *keys_pressed;

    // mouse
    mouse_t mouse;
};

