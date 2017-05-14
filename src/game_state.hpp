enum game_state_type_e
{
    GAME_MODE = 1,
    MENU_MODE
};

struct game_state_t
{
    game_state_type_e type;
    // NOTE: probably should create an union for this if I'm wasting
    //       too much memory
    game_mode_t *game;
    menu_mode_t *menu;
};
