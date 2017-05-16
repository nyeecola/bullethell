pause_mode_t *initialize_pause_mode()
{
    pause_mode_t *pause = (pause_mode_t *) calloc(1, sizeof(*pause));
    assert(pause);

    // TODO
    pause->selected_option = OPTION_START;
    pause->alpha_selected_option = 0;
    pause->alpha_increasing = true;

    return pause;
}
