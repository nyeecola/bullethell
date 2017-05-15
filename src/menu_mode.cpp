menu_mode_t *initialize_menu_mode()
{
    menu_mode_t *menu = (menu_mode_t *) calloc(1, sizeof(*menu));
    assert(menu);

    // TODO
    menu->selected_option = OPTION_START;
    menu->alpha_selected_option = 0;
    menu->alpha_increasing = true;

    return menu;
}
