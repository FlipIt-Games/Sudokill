#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include "game.h"

time_t seed;

uint8_t game::enemy_grid[81];
uint8_t game::pickup_grid[81];
game::CellState game::cell_states[81];

game::CellState game::current_cell_previous_state;

int game::player_position = 81 / 2;
int game::player_collected_crystals = 0;
int game::player_max_health = 10;
int game::player_health = player_max_health;
int game::player_previous_health;
int game::player_max_stamina = 10;
int game::player_stamina = 10;
int game::player_damage = 10;
int game::player_damage_boost = 0;
int game::player_gold = 0;
int game::player_gold_to_next_upgrade = 1;

bool contains(uint8_t arr[], int length, uint8_t value);
bool try_insert(uint8_t grid[], int idx, uint8_t value);

void generate_enemy_grid(uint8_t grid[]) {
    uint8_t tries[81] = {0};
    uint8_t tried[81 * 9];

    for (int i = 0; i < 81;) {
        while(true) {
            if (tries[i] == 9) {
                tries[i] = 0;
                grid[i--] = 0;
                grid[i] = 0;
                memset(&tried[i * 9], 0, sizeof(uint8_t) * 9);
                break;
            }

            int value;
            while(true) {
                value = (rand() % 9) + 1;

                if (i == 81 / 2 && (value == 1 || value == 2 || value == 3 || value == 4 || value == 5)) {
                    continue;
                }

                if (!contains(&tried[i * 9], tries[i], value)) {
                    break;
                }
            }

            tried[(i * 9) + tries[i]++] = value;
            if (try_insert(grid, i, value)) {
                grid[i++] = value;
                break;
            }
        }
    }
}


void generate_pickup_grid(uint8_t grid[]) {
    uint8_t tries[81] = {0};
    uint8_t tried[81 * 9];

    auto center = 81 / 2;
    // grid[center] = (uint8_t)game::Pickup::None;

    for (int i = 0; i < 81;) {
        auto idx = (center + i) % 81;

        while(true) {
            if (tries[idx] == 9) {
                tries[idx] = 0;
                grid[(center + i--) % 81] = 0;
                grid[(center + i) % 81] = 0;
                memset(&tried[idx * 9], 0, sizeof(uint8_t) * 9);
                break;
            }

            int value;
            while(true) {
                value = (rand() % 9) + 1;
                if (!contains(&tried[idx * 9], tries[idx], value)) {
                    break;
                }
            }

            tried[(idx * 9) + tries[idx]++] = value;
            if (try_insert(grid, idx, value)) {
                grid[(center + i++) % 81] = value;
                break;
            }
        }
    }
}

void game::init() {
    game::current_cell_previous_state = game::CellState::Visible_Visited;

    memset(game::enemy_grid, 0, sizeof(uint8_t) * 81);
    memset(game::pickup_grid, 0, sizeof(uint8_t) * 81);
    seed = time(nullptr);
    srand(seed);
}

void game::generate_grid() {
    generate_enemy_grid(enemy_grid);
    printf("Generated enemy grid");

    generate_pickup_grid(pickup_grid);
    printf("Generated pickup grid");

    for (int i = 0; i < 81; i++) {
        if (pickup_grid[i] == (uint8_t)game::Pickup::None) {
            cell_states[i] = CellState::Visible_Visited;
        } else {
            cell_states[i] = CellState::Unknown;
        }
    }

    cell_states[player_position] = CellState::Visible_Visited;
    cell_states[player_position - 2] = CellState::Visible_Unvisited;
    cell_states[player_position + 2] = CellState::Visible_Unvisited;
    cell_states[player_position - 8] = CellState::Visible_Unvisited;
    cell_states[player_position - 10] = CellState::Visible_Unvisited;
    cell_states[player_position + 8] = CellState::Visible_Unvisited;
    cell_states[player_position + 10] = CellState::Visible_Unvisited;
    cell_states[player_position + 18] = CellState::Visible_Unvisited;
    cell_states[player_position - 18] = CellState::Visible_Unvisited;
}

void game::move_player(game::Direction direction) {
    int next_pos = player_position;
    switch (direction) {
        case game::Direction::North:
            next_pos -= 9;
            break;
        case game::Direction::North_East:
            next_pos -= 8;
            break;
        case game::Direction::East:
            next_pos += 1;
            break;
        case game::Direction::South_East:
            next_pos += 10;
            break;
        case game::Direction::South:
            next_pos += 9;
            break;
        case game::Direction::South_West:
            next_pos += 8;
            break;
        case game::Direction::West:
            next_pos -= 1;
            break;
        case game::Direction::North_West:
            next_pos -= 10;
            break;
        default: break;
    }

    if (next_pos < 0 || next_pos >= 81) {
        return;
    }
    
    teleport(next_pos);
}

void game::teleport(int idx) {
    player_position = idx;

    game::current_cell_previous_state = cell_states[player_position];
    if (game::current_cell_previous_state != game::CellState::Visible_Visited) {
        player_previous_health = player_health;
        player_stamina -= 1;

        auto enemy_health = enemy_grid[player_position];
        if ((game::Pickup)pickup_grid[player_position] == (game::Pickup::Crystal)) {
            enemy_health *= 2;
        }

        while(true) {
            int damage = player_damage + player_damage_boost;
            printf("You hit the enemy inflicting %d damage\n", damage);
            player_damage_boost = 0;

            if (damage < enemy_health) {
                enemy_health -= damage;
            } else {
                break;
            }

            player_health -= enemy_health;
            printf("Enemy hits you, dealing %d damage\n", enemy_health);
            if (player_health <= 0) {
                player_health = 0;
                return;
            }
        }

        switch ((game::Pickup)pickup_grid[player_position]) {
            case game::Pickup::Life:
                player_health = player_health + enemy_grid[player_position] < player_max_health 
                    ? player_health + enemy_grid[player_position]
                    : player_max_health;
                break;
            case game::Pickup::Stamina:
                player_stamina = player_stamina + enemy_grid[player_position] < player_max_stamina 
                    ? player_stamina + enemy_grid[player_position]
                    : player_max_stamina;
                break;
            case game::Pickup::Damage_Boost:
                player_damage_boost = enemy_grid[player_position];
                break;
            case game::Pickup::Gold:
                player_gold += enemy_grid[player_position];
            default: break;
        }
    }
    
    cell_states[player_position] = CellState::Visible_Visited;
}

bool contains(uint8_t arr[], int length, uint8_t value) {
    for (int i = 0; i < length; i++) {
        if (arr[i] == value) {
            return true;
        }
    }

    return false;
}

bool try_insert(uint8_t grid[], int idx, uint8_t value) {
    auto row = idx / 9;
    auto col = idx % 9; 

    // Validate Row
    for (int i = 0; i < 9; i++) {
        if (grid[(row * 9) + i] == value) {
            return false;
        }
    }

    // Validate Col
    for (int i = 0; i < 9; i++) {
        if (grid[col + (i * 9)] == value) {
            return false;
        }
    }

    // Validate Subgrid
    auto subgrid_start = ((row / 3 * 3) * 9) + (col / 3 * 3);
    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 3; c++) {
            if (grid[subgrid_start + (r * 9) + c] == value) {
                return false;
            }
        }
    }

    return true;
}
