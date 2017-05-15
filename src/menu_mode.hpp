enum options_e
{
    OPTION_START = 0,
    OPTION_SETTINGS,

    OPTION_BLANK,

    OPTION_EXIT,

    OPTION_COUNT
};

struct menu_mode_t
{
    int selected_option;
    int alpha_selected_option;
    bool alpha_increasing;
};
