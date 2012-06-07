#ifndef _INCLUDE_GAME_H
#define _INCLUDE_GAME_H

#define MAX_DT 0.1

/**
    Physics
    */

#define COULOMB 0.1
#define GRAV 600

#define SPHERE_DRAG_COEFFICENT 0.45

void* main_iteration(ALLEGRO_THREAD *, void *);
void* compute_gravity(ALLEGRO_THREAD *, void *);
void* compute_electrostatics(ALLEGRO_THREAD *, void *);
void* compute_collisions(ALLEGRO_THREAD *, void *);
bool initialize_iteration_threads(game_shared_data*);
void terminate_iteration(game_shared_data *);

#endif
