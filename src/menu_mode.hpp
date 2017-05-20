enum options_e
{
    OPTION_START = 0,
    OPTION_SETTINGS,

    OPTION_EXIT,

    OPTION_COUNT
};

enum settings_options_e
{
    SETTINGS_OPTION_VOLUME = 0,
    SETTINGS_OPTION_VSYNC,
    SETTINGS_OPTION_AA,

    SETTINGS_OPTION_BACK,

    SETTINGS_OPTION_COUNT
};

enum menu_types_e
{
    MAIN_MENU = 1,
    SETTINGS_MENU
};

struct menu_mode_t
{
    int selected_option;
    int alpha_selected_option;
    bool alpha_increasing;

    menu_types_e current_menu;

    std::list<particle_t *> *particles;

    double time_since_last_spawn;
    double last_angle;
};
