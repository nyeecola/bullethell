menu_mode_t *initialize_menu_mode()
{
    menu_mode_t *menu = (menu_mode_t *) calloc(1, sizeof(*menu));
    assert(menu);

    menu->selected_option = OPTION_START;
    menu->alpha_selected_option = 0;
    menu->alpha_increasing = true;
    menu->current_menu = MAIN_MENU;
    menu->particles = new std::list<particle_t *>();

    return menu;
}

void enter_settings_menu(game_state_t *game_state)
{
    game_state->menu->current_menu = SETTINGS_MENU;
    game_state->menu->selected_option = SETTINGS_OPTION_VOLUME;
}
