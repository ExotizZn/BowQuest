#include <stdio.h>

#include "../include/player.h"
#include "../include/save.h"
#include "../include/appstate.h"

void saveGame(void *data) {
    AppState *as = data;
    Player *player = as->player;
    FILE * file = fopen("save.txt", "w");

    if(!file || !player) return;

    fprintf(file, "%d\n", player->level);
    fprintf(file, "%f\n", player->speed);
    fprintf(file, "%d\n", player->coins);
    fprintf(file, "%f\n", player->player_stats->projectile_delay);
    fprintf(file, "%f\n", player->player_stats->projectile_speed);
    fprintf(file, "%f\n", player->player_stats->projectile_damage);
    fprintf(file, "%f\n", player->player_stats->crit_chance);
    fprintf(file, "%f\n", player->player_stats->armor);
    fprintf(file, "%f\n", player->player_stats->regen);
    fclose(file);
    SDL_Log("Game saved succesfuly.");
}

bool loadGame(void *data) {
    AppState *as = data;
    FILE * file = fopen("save.txt", "r");

    if(!file) return false;

    Player *new_player = NULL;
    initPlayer(&new_player);

    float level = 0, speed = 0, projectile_delay = 0, projectile_speed = 0, projectile_damage = 0, crit_chance = 0, armor = 0, regen = 0;
    int coins = 0;

    fscanf(file, "%f", &level);
    fscanf(file, "%f", &speed);
    fscanf(file, "%d", &coins);
    fscanf(file, "%f", &projectile_delay);
    fscanf(file, "%f", &projectile_speed);
    fscanf(file, "%f", &projectile_damage);
    fscanf(file, "%f", &crit_chance);
    fscanf(file, "%f", &armor);
    fscanf(file, "%f", &regen);

    new_player->level = level;
    new_player->speed = speed;
    new_player->coins = coins;
    new_player->player_stats->projectile_delay = projectile_delay;
    new_player->player_stats->projectile_speed = projectile_speed;
    new_player->player_stats->projectile_damage = projectile_damage;
    new_player->player_stats->projectile_delay = crit_chance;
    new_player->player_stats->armor = armor;
    new_player->player_stats->regen = regen;

    as->player = new_player;

    fclose(file);
    return true;
}