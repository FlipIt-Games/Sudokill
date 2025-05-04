#pragma once

#include <cstdint>

namespace game {
    enum struct Pickup {
        Life = 1,
        Stamina = 2,
        Damage_Boost = 3,
        Gold = 4,
        Palantir = 5,
        Arrow = 6,
        Teleport = 7,
        None = 8,
        Crystal = 9, 
    };

    enum struct CellState {
        Unknown,
        Visible_Unvisited,
        Visible_Visited
    };

    enum struct Direction {
        North,
        North_East,
        East,
        South_East,
        South,
        South_West,
        West,
        North_West
    };

    extern uint8_t enemy_grid[81];
    extern uint8_t pickup_grid[81];
    extern CellState cell_states[81];
    
    extern CellState current_cell_previous_state;

    extern int player_position;
    extern int player_collected_crystals;
    extern int player_max_health;
    extern int player_health;
    extern int player_previous_health;
    extern int player_max_stamina;
    extern int player_stamina;
    extern int player_damage;
    extern int player_damage_boost;
    extern int player_gold;
    extern int player_gold_to_next_upgrade;

    void init();
    void generate_grid();
    void move_player(Direction direction);
    void teleport(int idx);
}
