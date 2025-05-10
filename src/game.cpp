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
int game::player_damage = 1;
int game::player_damage_boost = 0;
int game::player_gold = 0;
int game::player_gold_to_next_upgrade = 1;

uint8_t base_enemy_grid[] = {
    3, 6, 7, 1, 4, 8, 2, 5, 9,
    5, 1, 9, 6, 2, 7, 4, 3, 8,
    4, 8, 2, 5, 9, 3, 6, 7, 1,
    6, 9, 1, 4, 7, 2, 5, 8, 3,
    8, 4, 3, 9, 5, 1, 7, 6, 2,
    7, 2, 5, 8, 3, 6, 9, 1, 4,
    9, 3, 4, 7, 1, 5, 8, 2, 6,
    2, 7, 6, 3, 8, 4, 1, 9, 5,
    1, 5, 8, 2, 6, 9, 3, 4, 7 
};

uint8_t base_item_grid[] = {
    3, 5, 8, 2, 4, 7, 1, 6, 9,
    2, 9, 4, 1, 8, 6, 3, 7, 5,
    7, 1, 6, 9, 3, 5, 8, 2, 4,
    9, 2, 5, 8, 1, 4, 7, 3, 6,
    8, 6, 1, 7, 5, 3, 9, 4, 2,
    4, 7, 3, 6, 9, 2, 5, 8, 1,
    6, 8, 2, 5, 7, 1, 4, 9, 3,
    5, 3, 7, 4, 2, 9, 6, 1, 8,
    1, 4, 9, 3, 6, 8, 2, 5, 7 
};



bool contains(uint8_t arr[], int length, uint8_t value);
bool try_insert(uint8_t grid[], int idx, uint8_t value);

void swap_rows(int row_a, int row_b);
void swap_columns(int column_a, int column_b);
void swap_big_rows(int row_a, int row_b);
void swap_big_columns(int column_a, int column_b);
void swap_numbers(uint8_t grid[], int n_a, int n_b);

void generate_grids();

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
    generate_grids(); 

    // for (int i = 0; i < 81; i++) {
    //     if (pickup_grid[i] == (uint8_t)game::Pickup::None) {
    //         cell_states[i] = CellState::Visible_Visited;
    //     } else {
    //         cell_states[i] = CellState::Unknown;
    //     }
    // }

    cell_states[player_position] = CellState::Visible_Visited;
    cell_states[player_position - 2] = CellState::Visible_Unvisited;
    cell_states[player_position + 2] = CellState::Visible_Unvisited;
    cell_states[player_position - 8] = CellState::Visible_Unvisited;
    cell_states[player_position - 10] = CellState::Visible_Unvisited;
    cell_states[player_position + 8] = CellState::Visible_Unvisited;
    cell_states[player_position + 10] = CellState::Visible_Unvisited;
    cell_states[player_position + 18] = CellState::Visible_Unvisited;
    cell_states[player_position - 18] = CellState::Visible_Unvisited;

    cell_states[player_position + 6] = CellState::Visible_Unvisited;
    cell_states[player_position + 12] = CellState::Visible_Unvisited;
    cell_states[player_position + 26] = CellState::Visible_Unvisited;
    cell_states[player_position + 28] = CellState::Visible_Unvisited;
    cell_states[player_position - 6] = CellState::Visible_Unvisited;
    cell_states[player_position - 12] = CellState::Visible_Unvisited;
    cell_states[player_position - 26] = CellState::Visible_Unvisited;
    cell_states[player_position - 28] = CellState::Visible_Unvisited;

    cell_states[4] = CellState::Visible_Unvisited;
    cell_states[36] = CellState::Visible_Unvisited;
    cell_states[44] = CellState::Visible_Unvisited;
    cell_states[76] = CellState::Visible_Unvisited;

}

void generate_grids() {
    memcpy(game::enemy_grid, base_enemy_grid, 81);
    memcpy(game::pickup_grid, base_item_grid, 81);

    swap_big_rows(0, 2);

    // Initial shuffling of big row and columns
    for (int i = 0; i < 10; i++) {
        auto mode_rnd = rand() % 2;
        auto rnd_a = rand() % 3;
        int rnd_b;
        
        do {
            rnd_b = rand() % 3;
        } while(rnd_b == rnd_a);

        if (mode_rnd == 0) {
            swap_big_rows(rnd_a, rnd_b);
        } else {
            swap_big_columns(rnd_a, rnd_b);
        }
    }

    // Makes sure center cell is None and >= 5
    auto center_enemy = game::enemy_grid[81 / 2];
    if (center_enemy <= 5) {
        int target;
        do {
            target = 5 + (rand() % 5);
        } while(target == center_enemy);
        swap_numbers(game::enemy_grid, center_enemy, target);
    }

    auto center_item = game::pickup_grid[81/2];
    if (center_item != (int)game::Pickup::None) {
        swap_numbers(game::pickup_grid, center_item, (int)game::Pickup::None);
    }

    // Makes sure every movable cell from center is <= 4
    uint8_t movables[4];
    movables[0] = game::enemy_grid[(81 / 2) - 1];
    movables[1] = game::enemy_grid[(81 / 2) + 1];
    movables[2] = game::enemy_grid[(81 / 2) - 9];
    movables[3] = game::enemy_grid[(81 / 2) + 9];

    for (int i = 0; i < 4; i++) {
        if (movables[i] >= 5) {
            int target;
            do {
                target = (rand() % 4 ) + 1;
            } while(contains(movables, 4, target));
            swap_numbers(game::enemy_grid, movables[i], target);
            movables[i] = target;
        }
    }

    // Makes sure every movable cell from center is interresting in early game
    const uint8_t interresting_items[4] = {
        (uint8_t)game::Pickup::Gold,
        (uint8_t)game::Pickup::Crystal,
        (uint8_t)game::Pickup::Damage_Boost,
        (uint8_t)game::Pickup::Arrow,
    };
    
    uint8_t pickables[4];
    pickables[0] = game::pickup_grid[(81 / 2) - 1];
    pickables[1] = game::pickup_grid[(81 / 2) + 1];
    pickables[2] = game::pickup_grid[(81 / 2) - 9];
    pickables[3] = game::pickup_grid[(81 / 2) + 9];

    for (int i = 0; i < 4; i++) {
        bool has_interesting_item = false;
        for (int j = 0; j < 4; j++) {
            if (pickables[i] == interresting_items[j]) {
                has_interesting_item = true;
                break;
            }
        }

        if (!has_interesting_item) {
            uint8_t target;
            do {
                target = interresting_items[rand() % 4];
            } while(contains(pickables, 4, target));
            swap_numbers(game::pickup_grid, pickables[i], target);
            pickables[i] = target;
        }
    }
}

void game::move_player(game::Direction direction) {
    int next_pos = player_position;
    switch (direction) {
        case game::Direction::North:
            next_pos -= 9;
            break;
        case game::Direction::East:
            next_pos += 1;
            break;
        case game::Direction::South:
            next_pos += 9;
            break;
        case game::Direction::West:
            next_pos -= 1;
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
    if (game::current_cell_previous_state != game::CellState::Visible_Visited && game::pickup_grid[player_position] != (uint8_t)game::Pickup::None) {
        player_previous_health = player_health;

        auto enemy_health = enemy_grid[player_position];
        if ((game::Pickup)pickup_grid[player_position] == (game::Pickup::Crystal)) {
            enemy_health *= 2;
        }

        while(true) {
            int damage = player_damage + player_damage_boost;
            if (player_stamina == player_max_stamina) {
                damage *= 2;
            }

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

        player_stamina -= 1;

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
                break;
            case game::Pickup::Crystal:
                player_health = player_max_health;
                player_stamina = player_max_stamina;
                break;
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

void swap_rows(int row_a, int row_b) {
    uint8_t cache[9];
    memcpy(cache, &game::enemy_grid[row_a * 9], 9);

    memcpy(&game::enemy_grid[row_a * 9], &game::enemy_grid[row_b * 9], 9);
    memcpy(&game::enemy_grid[row_b * 9], cache, 9);

    memcpy(cache, &game::pickup_grid[row_a * 9], 9);

    memcpy(&game::pickup_grid[row_a * 9], &game::pickup_grid[row_b * 9], 9);
    memcpy(&game::pickup_grid[row_b * 9], cache, 9);
}

void swap_columns(int column_a, int column_b) {
    uint8_t cache;

    for (int i = 0; i < 9; i++) {
        cache = game::enemy_grid[(i * 9) + column_a];
        game::enemy_grid[(i * 9) + column_a] = game::enemy_grid[(i * 9) + column_b];
        game::enemy_grid[(i * 9) + column_b] = cache;

        cache = game::pickup_grid[(i * 9) + column_a];
        game::pickup_grid[(i * 9) + column_a] = game::pickup_grid[(i * 9) + column_b];
        game::pickup_grid[(i * 9) + column_b] = cache;
    }
}

void swap_numbers(uint8_t grid[], int n_a, int n_b) { 
    for (int i = 0; i < 81; i++) {
        if (grid[i] == n_a) {
            grid[i] = n_b;
        } else if (grid[i] == n_b) {
            grid[i] = n_a;
        }
    }
}

void swap_big_rows(int row_a, int row_b) {
    row_a *= 3;
    row_b *= 3;

    swap_rows(row_a, row_b);
    swap_rows(row_a + 1, row_b + 1);
    swap_rows(row_a + 2, row_b + 2);
}

void swap_big_columns(int column_a, int column_b) {
    column_a *= 3;
    column_b *= 3;

    swap_columns(column_a, column_b);
    swap_columns(column_a + 1, column_b + 1);
    swap_columns(column_a + 2, column_b + 2);
}
