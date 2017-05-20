particle_t *spawn_particle_towards(v2 pos, v2 vector, int owner, double speed, v2 acceleration,
                                   const char *image, int w, int h, v3 color, double ttl)
{
    assert(image);

	// DEBUG
#if 0
	static int count = 0;
	printf("count = %d\n", ++count);
#endif

    v2 direction;
    if (!is_null_vector(vector)) direction = math_normalize(vector);
    else direction = vector;

    particle_t *particle = (particle_t *) malloc(sizeof(*particle));
    assert(particle);
    particle->owner = (entity_type_e) owner;
    particle->pos = pos;
    particle->velocity = direction * speed;
    particle->acceleration = acceleration;
    particle->image_path = image;
    particle->w = w;
    particle->h = h;
    particle->color = color;
    particle->orbit_center = 0;
    particle->orbit_angle = 0;
    particle->orbit_speed = 0;
    if (ttl > 0.0001)
    {
        particle->timed = true;
        particle->time_to_live = ttl;
    }
    else
    {
        particle->timed = false;
    }

    return particle;
}

particle_t *spawn_particle_orbiting(v2 *center, double radius, double delta_radius,
                                    double start_angle, int owner, double speed,
                                    const char *image, int w, int h, v3 color, double ttl)
{
    assert(image);

	// DEBUG
#if 0
	static int count = 0;
	printf("count = %d\n", ++count);
#endif

    particle_t *particle = (particle_t *) malloc(sizeof(*particle));
    assert(particle);
    particle->owner = (entity_type_e) owner;
    particle->pos = V2(0, 0);
    particle->velocity = V2(0, 0);
    particle->acceleration = V2(0, 0);
    particle->image_path = image;
    particle->w = w;
    particle->h = h;
    particle->color = color;
    particle->orbit_center = center;
    particle->orbit_angle = start_angle;
    particle->orbit_speed = speed;
    particle->orbit_radius = radius;
    particle->orbit_delta_radius = delta_radius;
    if (ttl > 0.0001)
    {
        particle->timed = true;
        particle->time_to_live = ttl;
    }
    else
    {
        particle->timed = false;
    }

    return particle;
}

void update_particle_position(particle_t *particle, double dt)
{
    if (particle->orbit_center)
    {
        particle->orbit_angle += particle->orbit_speed * dt;
        while (particle->orbit_angle > PI * 2)
        {
            particle->orbit_angle -= PI * 2;
        }

        particle->orbit_radius += particle->orbit_delta_radius * dt;
        particle->pos.x = particle->orbit_center->x + particle->orbit_radius * cos(particle->orbit_angle);
        particle->pos.y = particle->orbit_center->y + particle->orbit_radius * sin(particle->orbit_angle);
    }
    else
    {
        particle->velocity += particle->acceleration * dt;
        particle->pos += particle->velocity * dt;
    }
}

// handles collisions with enemies and players, also applies it's effects
// returns true if collided and false if didn't
bool detect_particle_collision(game_mode_t *game, particle_t *particle)
{
    // collision detection
    if (particle->owner == ENTITY_PLAYER)
    {
        // TODO: figure out how to check only for current enemies
        // TODO: create hitbox_t struct for particles
        if (test_point_in_circle(particle->pos, game->enemy.pos, 28))
        {
            game->enemy.health -= game->player.player_data.shot_damage;
            return true;
        }
    }
    // TODO: figure out how to do this based on the enemy
    else if (particle->owner == ENTITY_ENEMY)
    {
        // TODO: create hitbox_t struct for particles
        if (test_point_in_circle(particle->pos, game->player.pos, particle->w / 4))
        {
            game->player.health--;
            game->player.player_data.hit = true;
            return true;
        }
    }

    return false;
}

// creates an attack using the state passed by `atk`
// read the atk_pattern_t structure for more information
inline void do_atk(std::list<particle_t *> *particles, int owner, atk_pattern_t *atk, double dt)
{
    if (atk->time_since_last_spawn >= atk->spawn_rate)
    {
        assert(!atk->spawn_limit || atk->spawn_count <= atk->spawn_limit);
        if (atk->spawn_limit && atk->spawn_count == atk->spawn_limit) return;
        ++atk->spawn_count;

        atk->time_since_last_spawn = atk->time_since_last_spawn - atk->spawn_rate + dt;

        double x, y;
        for (int i = 0; i < atk->particles_per_spawn; i++)
        {
            int step = atk->rotation_type == ROTATE_CW ? 1 : -1;

            double radians = ((atk->arc_size / atk->particles_per_spawn) * i * step) +
                             (atk->last_angle += atk->angle_step * step) +
                             atk->arc_center - atk->arc_size / 2;
            x = cos(radians);
            y = sin(radians);

            particle_t *particle;
            if (atk->particle_orbit_center)
            {
                particle = spawn_particle_orbiting(atk->particle_orbit_center,
                                                   atk->particle_orbit_radius,
                                                   atk->particle_orbit_delta_radius,
                                                   radians,
                                                   owner,
                                                   atk->particle_orbit_speed,
                                                   atk->particle_image,
                                                   atk->particle_width,
                                                   atk->particle_height,
                                                   atk->particle_color,
                                                   atk->particle_time_to_live);
            }
            else
            {
                particle = spawn_particle_towards(*atk->spawn_loc,
                                                  V2(x, y),
                                                  owner,
                                                  atk->particle_speed,
                                                  atk->particle_accel,
                                                  atk->particle_image,
                                                  atk->particle_width,
                                                  atk->particle_height,
                                                  atk->particle_color,
                                                  atk->particle_time_to_live);
            }
            particles->push_back(particle);
        }
    }
    else
    {
        atk->time_since_last_spawn += dt;
    }
}

// does all ai actions
void ai_do_actions(game_mode_t *game, double dt)
{
    assert(game);
    assert(dt > 0);

    // TODO: different behaviors
    //       probably each enemy should have different movement patterns (with a few exceptions)
    //       also, each enemy should have different attacks (maybe some few exceptions as well)

    // TODO: change this for a loop iterating over all ai controlled entities
    entity_t *enemy = &game->enemy;
    assert(enemy->type == ENTITY_ENEMY);

    // TODO: maybe consider movement an action? or something different?
    //       (find a way to only move from time to time)
    //       (also, it should be possible to synchronize movement with attacks)
    // movement
    {
        // if stopped, acquire new target location
        if (!enemy->enemy_data.forced_movement && enemy->enemy_data.stopped)
        {
            int new_target = rand() % enemy->enemy_data.path_size;
            enemy->enemy_data.target_loc = enemy->enemy_data.path[new_target];
            enemy->enemy_data.stopped = false;
        }

        // move to current target location
        v2 offset = enemy->enemy_data.target_loc - enemy->pos;
        double magnitude = math_magnitude(offset);
        v2 direction;
        if (!is_null_vector(offset)) direction = math_normalize(offset);
        else direction = offset;
        v2 velocity = direction * enemy->speed * dt;
        if (magnitude < enemy->speed * dt)
        {
            enemy->pos += offset;
            enemy->enemy_data.stopped = true;
        }
        else
        {
            enemy->pos += velocity;
        }
    }

    // attacks
    {
        if (enemy->enemy_data.time_since_fight_started > 5 &&
            enemy->enemy_data.time_since_fight_started < 8)
        {
            do_atk(game->particles, enemy->type, &enemy->enemy_data.atks[0], dt);
        }

        if (enemy->enemy_data.time_since_fight_started > 2 &&
            enemy->enemy_data.time_since_fight_started < 9)
        {
            do_atk(game->particles, enemy->type, &enemy->enemy_data.atks[1], dt);
            do_atk(game->particles, enemy->type, &enemy->enemy_data.atks[2], dt);
        }

        // atk while stopped
        {
            if (enemy->enemy_data.time_since_fight_started > 10 &&
                    enemy->enemy_data.time_since_fight_started < 20)
            {
                if (!enemy->enemy_data.forced_movement)
                {
                    enemy->enemy_data.forced_movement = true;
                    enemy->enemy_data.stopped = false;

                    v2 loc = V2(300, 100);
                    enemy->enemy_data.target_loc = loc;
                }

                if (enemy->enemy_data.stopped)
                {
                    v2 player_dir = game->player.pos - enemy->pos;
                    if (is_null_vector(player_dir))
                    {
                        player_dir = math_normalize(player_dir);
                    }
                    double angle = atan2(player_dir.y, player_dir.x);
                    enemy->enemy_data.atks[3].arc_center = angle;
                    do_atk(game->particles, enemy->type, &enemy->enemy_data.atks[3], dt);

                    // only once
                    do_atk(game->particles, enemy->type, &enemy->enemy_data.atks[9], dt);
                }
            }

            if (enemy->enemy_data.time_since_fight_started > 23 &&
                    enemy->enemy_data.time_since_fight_started < 27)
            {
                do_atk(game->particles, enemy->type, &enemy->enemy_data.atks[6], dt);
            }

            if (enemy->enemy_data.time_since_fight_started > 27)
            {
                enemy->enemy_data.forced_movement = false;
            }
        }

        if (enemy->enemy_data.time_since_fight_started > 30 &&
            enemy->enemy_data.time_since_fight_started < 34)
        {
            do_atk(game->particles, enemy->type, &enemy->enemy_data.atks[7], dt);
        }

        if (enemy->enemy_data.time_since_fight_started > 31 &&
            enemy->enemy_data.time_since_fight_started < 35)
        {
            do_atk(game->particles, enemy->type, &enemy->enemy_data.atks[4], dt);
            do_atk(game->particles, enemy->type, &enemy->enemy_data.atks[5], dt);
        }

        if (enemy->enemy_data.time_since_fight_started > 35)
        {
            do_atk(game->particles, enemy->type, &enemy->enemy_data.atks[8], dt);
        }
    }

    enemy->enemy_data.time_since_fight_started += dt;

    // TODO: figure out what to do once the fight is over
    //       (maybe destroy enemy or reset timer to reuse?)
}

game_mode_t *reset_game(game_mode_t *game)
{
    assert(game);

    std::list<particle_t *>::iterator at, end;
    for (at = game->particles->begin(), end = game->particles->end();
         at != end;
         ++at)
    {
        free(*at);
    }
    free(game->particles);
    free(game);
    return initialize_game_mode();
}

void do_players_actions(game_mode_t *game, input_t *input, double dt)
{
    assert(game);
    assert(input);
    assert(dt > 0);

    // movement
    {
        double speed = game->player.speed;
        if (input->keys_pressed[SDL_SCANCODE_LSHIFT] || input->keys_pressed[SDL_SCANCODE_RSHIFT])
        {
            speed *= 0.6;
        }
        v2 offset_dir = {0, 0};
        game->player.frame = 2;
        game->player.flip_x = false;
        game->player.flip_y = false;
        if (input->keys_pressed[SDL_SCANCODE_UP])
        {
            offset_dir.y = -1;
            if (!input->keys_pressed[SDL_SCANCODE_RSHIFT] &&
                !input->keys_pressed[SDL_SCANCODE_LSHIFT])
            {
                game->player.frame = 4;
            }
        }
        if (input->keys_pressed[SDL_SCANCODE_DOWN])
        {
            offset_dir.y = 1;
            game->player.frame = 0;
        }
        if (input->keys_pressed[SDL_SCANCODE_LEFT])
        {
            offset_dir.x = -1;
            if (game->player.frame == 4)
            {
                game->player.frame = 5;
            }
            if (game->player.frame == 2)
            {
                game->player.frame = 3;
            }
            if (game->player.frame == 0)
            {
                game->player.frame = 1;
            }
            game->player.flip_x = true;
        }
        if (input->keys_pressed[SDL_SCANCODE_RIGHT])
        {
            offset_dir.x = 1;
            if (game->player.frame == 4)
            {
                game->player.frame = 5;
            }
            if (game->player.frame == 2)
            {
                game->player.frame = 3;
            }
            if (game->player.frame == 0)
            {
                game->player.frame = 1;
            }
        }
        if (!is_null_vector(offset_dir))
        {
            offset_dir = math_normalize(offset_dir);
        }
        else
        {
            game->player.frame = 2;
        }
        game->player.pos += offset_dir * speed * dt;

        if (game->player.pos.x < 0)
        {
            game->player.pos.x = 0;
        }
        if (game->player.pos.y < 0)
        {
            game->player.pos.y = 0;
        }
        if (game->player.pos.x > DEFAULT_SCREEN_WIDTH)
        {
            game->player.pos.x = DEFAULT_SCREEN_WIDTH;
        }
        if (game->player.pos.y > DEFAULT_SCREEN_HEIGHT)
        {
            game->player.pos.y = DEFAULT_SCREEN_HEIGHT;
        }
    }

    // attack
    {
        // FIXME: not sure if needed (may cause problems? like firing inconsistently)
        double excess_time = game->player.player_data.time_since_last_shot - PLAYER_SHOT_COOLDOWN;
        if (input->keys_pressed[SDL_SCANCODE_Z] && excess_time > 0)
        {
            game->player.player_data.time_since_last_shot = dt;
            v2 dir = V2(0, -1);
            v2 accel = V2(0, 0);
            int shot_speed = 600;
            int shot_w = 4;
            int shot_h = 14;
            v3 color = V3(255, 255, 255);

            v2 pos;

            pos.x = game->player.pos.x - 7;
            pos.y = game->player.pos.y;
            particle_t *particle = spawn_particle_towards(pos, dir, ENTITY_PLAYER,
                                                          shot_speed, accel, PLAYER_SHOT_PATH,
                                                          shot_w, shot_h, color, 0);
            game->particles->push_back(particle);

            /*pos.x = game->player.pos.x - 4;
            pos.y = game->player.pos.y;
            particle = spawn_particle_towards(pos, dir, ENTITY_PLAYER,
                                              shot_speed, accel,
                                              PLAYER_SHOT_PATH, shot_w, shot_h, color, 0);
            game->particles->push_back(particle);*/

            /*pos.x = game->player.pos.x + 4;
            pos.y = game->player.pos.y;
            particle = spawn_particle_towards(pos, dir, ENTITY_PLAYER,
                                              shot_speed, accel,
                                              PLAYER_SHOT_PATH, shot_w, shot_h, color, 0);
            game->particles->push_back(particle);*/

            pos.x = game->player.pos.x + 7;
            pos.y = game->player.pos.y;
            particle = spawn_particle_towards(pos, dir, ENTITY_PLAYER,
                                              shot_speed, accel,
                                              PLAYER_SHOT_PATH, shot_w, shot_h, color, 0);
            game->particles->push_back(particle);
        }

        game->player.player_data.time_since_last_shot += dt;
    }

    // TODO: special attacks
}

void render_entity(renderer_t *renderer, entity_t entity)
{
    SDL_Rect dest;
    dest.x = (int) round(entity.pos.x - entity.w / 2);
    dest.y = (int) round(entity.pos.y - entity.h / 2);
    dest.w = entity.w;
    dest.h = entity.h;

    SDL_Rect source;
    source.x = entity.frame * entity.w;
    source.y = 0;
    source.w = entity.w;
    source.h = entity.h;

    display_image(renderer, entity.image_path, &dest, &source, V3(255, 255, 255),
                  255, entity.flip_x, entity.flip_y);
}

