enum game_state_type_e
{
    GAME_MODE = 1,
    PAUSE_MODE,
    MENU_MODE
};

struct game_state_t
{
    game_state_type_e type;

    game_mode_t *game;
    menu_mode_t *menu;
    pause_mode_t *pause;

    bool vsync;
    bool anti_aliasing;
    int volume;

    bool running;
};
