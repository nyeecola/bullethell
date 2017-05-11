game_mode_t *initialize_game_mode()
{
    game_mode_t *game = (game_mode_t *) calloc(1, sizeof(*game));
    assert(game);
    game->particles = new std::list<particle_t *>();

    // background_color
    game->background_color.red = 60;
    game->background_color.green = 60;
    game->background_color.blue = 200;
    game->background_color.alpha = 255;

    // keyboard controlled ball
    game->player = {};
    game->player.type = ENTITY_PLAYER;
    game->player.image_path = BALL_IMG_PATH;
    game->player.w = 30;
    game->player.h = 42;
    game->player.pos = V2(300, 700);
    game->player.speed = 200;
    game->player.health = 3;
    game->player.player_data.shot_damage = 1;

    // enemy
    {
        game->enemy = {};
        game->enemy.type = ENTITY_ENEMY;
        game->enemy.image_path = BALL_IMG_PATH;
        game->enemy.w = 100;
        game->enemy.h = 100;
        game->enemy.pos = V2(400, 60);
        game->enemy.speed = 60;
        game->enemy.enemy_data.time_since_fight_started = 0.0;
		game->enemy.health = 600;

        // movement
        {
            game->enemy.enemy_data.stopped = true;
            game->enemy.enemy_data.path[0] = V2(150, 150);
            game->enemy.enemy_data.path[1] = V2(500, 100);
            game->enemy.enemy_data.path[2] = V2(280, 200);
            game->enemy.enemy_data.path[3] = V2(30, 80);
            game->enemy.enemy_data.path[4] = V2(300, 120);
            game->enemy.enemy_data.path[5] = V2(530, 180);
            game->enemy.enemy_data.path_size = 6;
        }

        // attacks
        {
            // fast spiral from enemy
            {
                atk_pattern_t atk = {};
                atk.particle_image = BALL_IMG_PATH;
                atk.particle_width = 20;
                atk.particle_height = 20;
                atk.particle_color = V3(0, 0, 0);
                atk.spawn_loc = &game->enemy.pos;
                atk.spawn_rate = 0.2;
                atk.particles_per_spawn = 20;
                atk.arc_center = 0;
                atk.arc_size = PI * 2;
                atk.angle_step = 0.0;
                atk.particle_speed = 250;
                atk.particle_accel = V2(0, 0);
                atk.particle_orbit_center = V2(0, 0);
                game->enemy.enemy_data.atks[0] = atk;
            }

            // two medium-to-fast spirals from the top corners of the screen
            // also, two fast circle cannons from the top corners of the screen
            {
                atk_pattern_t atk = {};

                atk.particle_image = BALL_IMG_PATH;
                atk.particle_width = 30;
                atk.particle_height = 30;
                atk.spawn_rate = 0.25;
                atk.particles_per_spawn = 12;
                atk.arc_center = 0;
                atk.arc_size = PI * 2;
                atk.angle_step = 0.006;
                atk.particle_speed = 150;
                atk.particle_accel = V2(0, 0);
                atk.particle_orbit_center = V2(0, 0);

                atk.rotation_type = ROTATE_CW;
                atk.particle_color = V3(180, 180, 255);
                v2 *atk1_loc = (v2 *) malloc(sizeof(*atk1_loc));
                assert(atk1_loc);
                *atk1_loc = V2(50, 150);
                atk.spawn_loc = atk1_loc;
                game->enemy.enemy_data.atks[1] = atk;

                //atk.rotation_type = ROTATE_CCW;
                atk.particle_color = V3(255, 180, 180);
                v2 *atk2_loc = (v2 *) malloc(sizeof(*atk2_loc));
                assert(atk2_loc);
                *atk2_loc = V2(550, 150);
                atk.spawn_loc = atk2_loc;
                game->enemy.enemy_data.atks[2] = atk;

                atk.particle_speed = 70;

                atk.particle_accel = V2(66, 180);
                atk.particle_color = V3(180, 180, 255);
                v2 *atk4_loc = (v2 *) malloc(sizeof(*atk4_loc));
                assert(atk4_loc);
                *atk4_loc = V2(50, 150);
                atk.spawn_loc = atk4_loc;
                game->enemy.enemy_data.atks[4] = atk;

                atk.particle_accel = V2(-66, 180);
                atk.particle_color = V3(255, 180, 180);
                v2 *atk5_loc = (v2 *) malloc(sizeof(*atk5_loc));
                assert(atk5_loc);
                *atk5_loc = V2(550, 150);
                atk.spawn_loc = atk5_loc;
                game->enemy.enemy_data.atks[5] = atk;
            }

            // dense small arc
            {
                atk_pattern_t atk = {};
                atk.particle_image = BALL_IMG_PATH;
                atk.particle_width = 18;
                atk.particle_height = 22;
                atk.particle_color = V3(0, 255, 0);
                atk.spawn_loc = &game->enemy.pos;
                atk.spawn_rate = 0.1;
                atk.particles_per_spawn = 10;
                atk.arc_center = PI / 2 + ((PI / 16) / atk.particles_per_spawn) / 2;
                atk.arc_size = PI / 16 + (PI / 16) / atk.particles_per_spawn;
                atk.angle_step = 0.0;
                atk.particle_speed = 500;
                atk.particle_accel = V2(0, 0);
                atk.particle_orbit_center = V2(0, 0);
                game->enemy.enemy_data.atks[3] = atk;
            }

            // dense slow divisor all angles
            {
                atk_pattern_t atk = {};
                atk.particle_image = BALL_IMG_PATH;
                atk.particle_width = 18;
                atk.particle_height = 18;
                atk.particle_color = V3(255, 0, 255);
                atk.spawn_loc = &game->enemy.pos;
                atk.spawn_rate = 0.06;
                atk.particles_per_spawn = 14;
                atk.arc_center = 0;
                atk.arc_size = PI * 2;
                atk.angle_step = 0.001;
                atk.particle_speed = 85;
                atk.particle_accel = V2(0, 0);
                atk.particle_orbit_center = V2(0, 0);
                game->enemy.enemy_data.atks[6] = atk;
            }

            // fast all angles non-rotating attack from enemy
            {
                atk_pattern_t atk = {};
                atk.particle_image = BALL_IMG_PATH;
                atk.particle_width = 20;
                atk.particle_height = 20;
                atk.particle_color = V3(0, 0, 0);
                atk.spawn_loc = &game->enemy.pos;
                atk.spawn_rate = 0.34;
                atk.particles_per_spawn = 14;
                atk.arc_center = 0;
                atk.arc_size = PI * 2;
                atk.angle_step = 0.0;
                atk.particle_speed = 200;
                atk.particle_accel = V2(0, 0);
                atk.particle_orbit_center = V2(0, 0);
                game->enemy.enemy_data.atks[7] = atk;
            }

            // slow spiral from enemy
            {
                atk_pattern_t atk = {};
                atk.particle_image = BALL_IMG_PATH;
                atk.particle_width = 80;
                atk.particle_height = 80;
                atk.particle_color = V3(255, 100, 255);
                atk.spawn_loc = &game->enemy.pos;
                atk.spawn_rate = 0.45;
                atk.particles_per_spawn = 14;
                atk.arc_center = 0;
                atk.arc_size = PI * 2;
                atk.angle_step = 0.02;
                atk.particle_speed = 70;
                atk.particle_accel = V2(0, 0);
                atk.particle_orbit_center = V2(0, 0);
                game->enemy.enemy_data.atks[8] = atk;
            }
        }
    }

    return game;
}

