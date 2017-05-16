enum entity_type_e
{
    ENTITY_PLAYER = 1,
    ENTITY_ENEMY,
};

enum rotation_type_e
{
    ROTATE_CW = 0,
    ROTATE_CCW,
};

struct color_t
{
    double red;
    double green;
    double blue;
    double alpha;
};

// TODO: make sure these attacks can be used more than once (preserve initial state)
//       (not sure if needed)
struct atk_pattern_t
{
    v2 *spawn_loc;
    double spawn_rate;             // in seconds
    double time_since_last_spawn;  // in seconds
    int spawn_limit;
    int spawn_count;
    int particles_per_spawn;
    double arc_center;
    double arc_size;
    rotation_type_e rotation_type;
    double angle_step;
    double last_angle; // TODO: remember to reset this once the attack is finished
    const char *particle_image; // TODO: add support for invisible particles
    int particle_width;
    int particle_height;
    v3 particle_color;
    double particle_time_to_live;

    // TODO: these should be from orbit center when the particle is an orbital
    double particle_speed;
    v2 particle_accel;

    v2 *particle_orbit_center; // FIXME: currently not used
    double particle_orbit_angle;
    double particle_orbit_speed; // in radians
    double particle_orbit_radius;
    double particle_orbit_delta_radius;
    // TODO: maybe add a pointer to function that can be called every in particle's update
};

struct enemy_only_t
{
    bool stopped;
    bool forced_movement;
    v2 target_loc;
    v2 path[10];
    int path_size;

    double time_since_fight_started;

    // TODO: change this to a linked list
    // NOTE: attacks do not necessarily happen in order, they can even happen simultaneously
    atk_pattern_t atks[10];
};

struct player_only_t
{
    double time_since_last_shot;
    int shot_damage;
    bool hit;
    double time_since_hit; // in seconds
};

struct entity_t
{
    v2 pos;
    double speed;
    // TODO: consider adding velocity and acceleration here (instead of speed)

    // TODO: isolate these in a `image_t` struct
    const char *image_path;
    int w;
    int h;
    int frame;
    bool flip_x;
    bool flip_y;

	// should be a value between 0 and 1000
    int health;

    entity_type_e type;
    union
    {
        enemy_only_t enemy_data;
        player_only_t player_data;
    };
};

struct particle_t
{
    v2 pos;
    v2 velocity;
    v2 acceleration;

    // TODO: make this be only a path so that we don't need the renderer to update the game
    // TODO: make sure the renderer does not render the same image multiple times (hash_table)
    const char *image_path;
    int w;
    int h;
    v3 color;

    entity_type_e owner;
    v2 *orbit_center;
    double orbit_angle;
    double orbit_speed;
    double orbit_radius;
    double orbit_delta_radius;

    bool timed;
    double time_to_live;
};

struct game_mode_t
{
    color_t background_color;

    entity_t player;
    entity_t enemy;

    // TODO: start allocating particles on the heap
    std::list<particle_t *> *particles;
};
