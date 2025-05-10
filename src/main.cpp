#include <clocale>
#include <cwchar>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "box_chars.h"

#define COLOR_RED       L"\x1b[31m"
#define COLOR_GREEN     L"\x1b[32m"
#define COLOR_YELLOW    L"\x1b[33m"
#define COLOR_BLUE      L"\x1b[34m"
#define COLOR_MAGENTA   L"\x1b[35m"
#define COLOR_CYAN      L"\x1b[36m"
#define COLOR_RESET     L"\x1b[0m"

#define MOVE_HOME           "\x1b[H"
#define MOVE                "\x1b[%d;%dH" 
#define MOVE_UP             "\x1b[%dA"
#define MOVE_DOWN           "\x1b[%dB"
#define CLEAR_LINE          "\x1b[2K"
#define CLEAR_SCREEN        "\x1b[2J"
#define CLEAR_FROM_CURSOR   "\x1b[0J"
#define SAVE_CURSOR_POS     "\x1b[s"
#define RESTORE_CURSOR_POS  "\x1b[u"

enum struct TextAlignment {
    Left, 
    Center, 
    Right
};

void print_stats();
void clear_logs();
void print_grid();

int write(
        wchar_t dest[], 
        const wchar_t src[], 
        int line_len, 
        int src_len, 
        TextAlignment alignment = TextAlignment::Left, 
        const wchar_t color[] = nullptr, 
        const wchar_t style[] = nullptr);

const int grid_size = 9;
const int col_per_cell = 11;
const int line_per_cell = 4;

int row_length;
int col_length;

wchar_t *stat_str;
wchar_t *grid_str;
wchar_t *logs_str;

bool debug_grid = false;

int main(int argc, char **argv) {
    // INITIALIZATION

    if (argc >= 2) {
        debug_grid = strcmp(argv[1], "debug");
    }

    setlocale(LC_ALL, "en_US.UTF-8");

    row_length = (grid_size * (col_per_cell + 1) + 2);
    col_length = (grid_size * (line_per_cell + 1) + 1);
    auto total_char = (row_length * col_length) + 1;

    grid_str = (wchar_t*)malloc(sizeof(wchar_t) * total_char * 2);
    memset(grid_str, L'\0', sizeof(wchar_t) * total_char);
    grid_str[total_char -1] = L'\0';

    game::init();
    game::generate_grid();

    printf(CLEAR_SCREEN);
    printf(MOVE_HOME);

    // UPDATE LOOP
    char input[10];
    printf(MOVE, 4 + col_length, 0);

    while(true) {
        print_stats();
        print_grid();
        printf("Where are you gonna go next?. Input a direction (W, A, S, D)\n");
        // fgets(input, 10, stdin);
        
        while(true) {

        }

        game::Direction direction;
        if (input[0] == 'W')        { direction = game::Direction::North; }
        else if (input[0] == 'A')   { direction = game::Direction::West; }
        else if (input[0] == 'S')   { direction = game::Direction::South; }
        else if (input[0] == 'D')   { direction = game::Direction::East; }
        else { continue; }

        game::move_player(direction);

skip_input:
        print_stats();
        print_grid();
        clear_logs();

        if (game::current_cell_previous_state != game::CellState::Visible_Visited) {
            printf("You fought bravely but lost %d health point\n", game::player_previous_health - game::player_health);

            auto pickup = (game::Pickup)game::pickup_grid[game::player_position];
            switch (pickup) {
                case game::Pickup::Life:
                    printf("You found health and gained %d health points\n", game::enemy_grid[game::player_position]);
                    break;
                case game::Pickup::Stamina:
                    printf("You found a chicken and gained %d stamina points\n", game::enemy_grid[game::player_position]);
                    break;
                case game::Pickup::Damage_Boost:
                    printf("You found a sword and gained a temporary damage boost of %d\n", game::enemy_grid[game::player_position]);
                    break;
                case game::Pickup::Gold:
                    printf("You found %d gold\n", game::enemy_grid[game::player_position]);
                    
                    break;
                case game::Pickup::Palantir: {
                   printf("You found a vision stone. Enter the coodinate of a cell you'd like to peek at.\n");
                   fgets(input, 10, stdin);
                   int idx_to_peek = ((input[0] - 'A') * 9) + input[1] - '1';
                   if (game::cell_states[idx_to_peek] == game::CellState::Unknown) {
                       game::cell_states[idx_to_peek] = game::CellState::Visible_Unvisited;
                   }
                   clear_logs();
                   break;
                }
                case game::Pickup::Arrow: {
                    printf("You found an arrow. You can shoot a cell and inflict damage to it\n");
                    fgets(input, 10, stdin);
                    auto idx_to_shoot = ((input[0] - 'A') * 9) + input[1] - '1';
                    if (game::cell_states[idx_to_shoot] != game::CellState::Visible_Visited) {
                        int next_value = game::enemy_grid[idx_to_shoot] - game::enemy_grid[game::player_position];
                        game::enemy_grid[idx_to_shoot] = next_value >= 1 ? next_value : 1;
                    }
                    print_grid();
                    clear_logs();
                    break;
                }
                case game::Pickup::Teleport: {
                    printf("You found a teleport. Enter the coordinate of a cell to teleport to.\n");
                    fgets(input, 10, stdin);
                    auto idx_to_tp = ((input[0] - 'A') * 9) + input[1] - '1';
                    game::teleport(idx_to_tp);
                    print_grid();
                    print_stats();
                    goto skip_input;
                }
                case game::Pickup::None:
                    printf("There is nothing to collect here\n");
                    break;
                case game::Pickup::Crystal:
                    printf("You found a crystal. Yeah! You can somehow use it to buy a new upgrade\n");
                    game::player_collected_crystals += 1;
                    if (game::player_gold >= game::player_gold_to_next_upgrade) {
                        printf("You can buy a new upgrade.\n");
                        printf("Enter H to upgrade Health.\n");
                        printf("Enter S to upgrade Stamina.\n");
                        printf("Enter D to upgrade Damage.\n");

                        fgets(input, 10, stdin);
                        switch (input[0]) {
                            case 'H': game::player_max_health += 3; break;
                            case 'S': game::player_max_stamina += 3; break;
                            case 'D': game::player_damage += 1; break;
                        }

                        game::player_gold -= game::player_gold_to_next_upgrade;
                        game::player_gold_to_next_upgrade += 1;
                        print_grid();
                        print_stats();
                        clear_logs();
                    }
                    break;
                default: break;
            }

            if (game::player_health <= 0) {
                printf("Unfortunately, you died!");
                goto exit;
            }
        } else {
            printf("You already visited this cell\n");
        }
    }

    // EXIT
exit:
    return 0;
}

void print_stats() {
    printf(SAVE_CURSOR_POS);
    printf(MOVE_HOME);
    printf(CLEAR_LINE "Health: %d / %d  |  Stamina: %d / %d  |  Damage: %d (%d)\n", 
        game::player_health, game::player_max_health,
        game::player_stamina, game:: player_max_stamina,
        game::player_damage, game::player_damage_boost);

    printf(CLEAR_LINE "Crystals: %d / 9\n", game::player_collected_crystals);
    printf(CLEAR_LINE "Gold: %d\n", game::player_gold);
    printf(RESTORE_CURSOR_POS);
}

void clear_logs() {
    printf(MOVE, 4 + col_length, 0);
    printf(CLEAR_FROM_CURSOR);
}

void print_grid() {
    printf(SAVE_CURSOR_POS);
    printf(MOVE, 4, 0);
    auto total_line = (grid_size * (line_per_cell + 1)) + 1;

    // Top Border
    int current_char_idx = 0;
    auto endl_idx = current_char_idx + row_length;

    grid_str[current_char_idx] = CORNER_TOPLEFT_DOUBLE;
    while(++current_char_idx < endl_idx -2) {
        if (current_char_idx % (col_per_cell + 1) == 0) {
                grid_str[current_char_idx] = current_char_idx / (col_per_cell + 1) % 3 != 0 
                    ? JUNCTION_TOP_SINGLE_DOUBLE
                    : JUNCTION_TOP_DOUBLE;
        } else {
            grid_str[current_char_idx] = LINE_HORIZONTAL_DOUBLE;
        }
    }
    grid_str[current_char_idx++] = CORNER_TOPRIGHT_DOUBLE;
    grid_str[current_char_idx++] = L'\n';

    for (int line = 1; line < total_line -1; line++) {
        auto col = 0;
        bool is_fat_line = line / (line_per_cell + 1) % 3 == 0;

        switch (line % (line_per_cell + 1)) {
            case 0: 
                grid_str[current_char_idx] = is_fat_line ? JUNCTION_LEFT_DOUBLE : JUNCTION_LEFT_DOUBLE_SINGLE;

                col = 0;
                while(++col < row_length -1) {
                    current_char_idx++;
                    bool is_fat_col = col  / (col_per_cell + 1) % 3 == 0;

                    wchar_t box_char;
                    if (col % (col_per_cell + 1) == 0) {
                        if (is_fat_line && is_fat_col) { box_char = JUNCTION_CENTER_DOUBLE; }
                        else if (is_fat_line) { box_char = JUNCTION_CENTER_SINGLE_VERT; }
                        else if (is_fat_col) { box_char = JUNCTION_CENTER_DOUBLE_VERT; }
                        else { box_char = JUNCTION_CENTER_SINGLE; }
                    } else {
                        box_char = is_fat_line ? LINE_HORIZONTAL_DOUBLE : LINE_HORIZONTAL_SINGLE;
                    }

                    grid_str[current_char_idx] = box_char;
                }

                grid_str[current_char_idx++] = is_fat_line ? JUNCTION_RIGHT_DOUBLE : JUNCTION_RIGHT_DOUBLE_SINGLE;
                break;
            case 1: 
                grid_str[current_char_idx++] =  LINE_VERTICAL_DOUBLE;
                for (int i = 0; i < 9; i++) {
                    auto grid_idx = (line / (line_per_cell + 1) * 9) + i;
                    grid_str[current_char_idx++] = 'A' + (grid_idx / 9);
                    grid_str[current_char_idx++] = '1' + (grid_idx % 9);

                    int len = col_per_cell - 2;
                    wmemset(&grid_str[current_char_idx], L' ', len);
                    current_char_idx += len;

                    grid_str[current_char_idx++] = (i + 1) % 3 == 0 ? LINE_VERTICAL_DOUBLE : LINE_VERTICAL_SINGLE;
                }
                break;
            case 2:
                grid_str[current_char_idx++] =  LINE_VERTICAL_DOUBLE;
                for (int i = 0; i < 9; i++) {
                    auto grid_idx = (line / (line_per_cell + 1) * 9) + i;

                    auto state = game::cell_states[grid_idx];
                    const wchar_t *color = nullptr;
                    wchar_t value[1];

                    if (state == game::CellState::Unknown) {
                        value[0] = L'?';
                    }

                    if (state == game::CellState::Visible_Unvisited) {
                        color = COLOR_RED;
                        value[0] = '0' + game::enemy_grid[grid_idx];
                    } 

                    if (state == game::CellState::Visible_Visited) {
                        color = COLOR_GREEN;
                        value[0] = '0' + game::enemy_grid[grid_idx];
                    }

                    if (debug_grid) {
                        color = nullptr;
                        value[0] = '0' + game::enemy_grid[grid_idx];
                    }

                    current_char_idx += write(&grid_str[current_char_idx], value, col_per_cell, 1, TextAlignment::Center, color);
                    grid_str[current_char_idx++] = (i + 1) % 3 == 0 ? LINE_VERTICAL_DOUBLE : LINE_VERTICAL_SINGLE;
                }
                break;
            case 3:
                grid_str[current_char_idx++] =  LINE_VERTICAL_DOUBLE;
                for (int i = 0; i < 9; i++) {
                    auto grid_idx = (line / (line_per_cell + 1) * 9) + i;

                    auto state = game::cell_states[grid_idx];
                    const wchar_t *color = nullptr;
                    const wchar_t *value = L"?";

                    if (state != game::CellState::Unknown || debug_grid) {
                        color = state == game::CellState::Visible_Unvisited ? COLOR_RED : COLOR_GREEN;
                        switch ((game::Pickup)game::pickup_grid[grid_idx]) {
                            case game::Pickup::Life: value = L"Life"; break;
                            case game::Pickup::Stamina: value = L"Stamina"; break;
                            case game::Pickup::Damage_Boost: value = L"Sword"; break;
                            case game::Pickup::Gold: value = L"Gold"; break;
                            case game::Pickup::Palantir: value = L"Palantir"; break;
                            case game::Pickup::Arrow: value = L"Arrow"; break;
                            case game::Pickup::Teleport: value = L"Teleport"; break;
                            case game::Pickup::None: value = L"None"; break;
                            case game::Pickup::Crystal: value = L"Crystal"; break;
                        }
                    }

                    if (debug_grid) {
                        color = nullptr;
                    }

                    current_char_idx += write(&grid_str[current_char_idx], value, col_per_cell, wcslen(value), TextAlignment::Center, color);
                    grid_str[current_char_idx++] = (i + 1) % 3 == 0 ? LINE_VERTICAL_DOUBLE : LINE_VERTICAL_SINGLE;
                }
                break;
            case 4: 
                grid_str[current_char_idx++] =  LINE_VERTICAL_DOUBLE;

                for (int i = 0; i < 9; i++) {
                    auto grid_idx = (line / (line_per_cell + 1) * 9) + i;
                    if (grid_idx == game::player_position) {
                        current_char_idx += write(&grid_str[current_char_idx], L"PLAYER", col_per_cell, 6, TextAlignment::Center, COLOR_BLUE);
                        grid_str[current_char_idx++] = i % 3 == 0 ? LINE_VERTICAL_DOUBLE : LINE_VERTICAL_SINGLE;
                    } else {
                        wmemset(&grid_str[current_char_idx], L' ', col_per_cell);
                        current_char_idx += col_per_cell;
                        grid_str[current_char_idx++] = (i + 1) % 3 == 0 ? LINE_VERTICAL_DOUBLE : LINE_VERTICAL_SINGLE;
                    }
                }
                break;
            default: 
                grid_str[current_char_idx++] = LINE_VERTICAL_DOUBLE;
                col = 0;
                while(++col < row_length) {
                    bool is_fat_col = col  / (col_per_cell + 1) % 3 == 0;
                    current_char_idx++;

                    if (col % (col_per_cell + 1) == 0) {
                        grid_str[current_char_idx] = is_fat_line ? LINE_VERTICAL_DOUBLE : LINE_VERTICAL_SINGLE; 
                    } else {
                        grid_str[current_char_idx] = L' ';
                    }
                }
                break; 
        }

        grid_str[current_char_idx++] = L'\n';
    }

    // Bottom Border
    endl_idx = current_char_idx + row_length;
    grid_str[current_char_idx] = CORNER_BOTTOMLEFT_DOUBLE;
    auto col = 0;
    while(++col < row_length -1) {
        current_char_idx++;
        bool is_fat_col = col  / (col_per_cell + 1) % 3 == 0;
        if (col % (col_per_cell + 1) == 0) {
            grid_str[current_char_idx] = is_fat_col ? JUNCTION_BOTTOM_DOUBLE : JUNCTION_BOTTOM_SINGLE_DOUBLE;
        } else {
            grid_str[current_char_idx] = LINE_HORIZONTAL_DOUBLE;
        }
    }
    grid_str[current_char_idx++] = CORNER_BOTTOMRIGHT_DOUBLE;
    grid_str[current_char_idx++] = L'\n';
    grid_str[current_char_idx++] = L'\0';

    wprintf(grid_str);
    printf(RESTORE_CURSOR_POS);
}

int write(
        wchar_t dest[], 
        const wchar_t src[], 
        int line_len, 
        int src_len, 
        TextAlignment alignment, 
        const wchar_t color[], 
        const wchar_t style[]) 
{
    int offset = 0;
    if (alignment == TextAlignment::Right) {
        offset += line_len - src_len;
        wmemset(dest, L' ', offset);
    }

    if (alignment == TextAlignment::Center) {
        offset += (line_len - src_len) % 2 == 0
            ? (line_len - src_len) / 2
            : ((line_len - src_len) / 2) + 1;

        wmemset(dest, L' ', offset);
    }

    if (color != nullptr) {
        auto l = wcslen(color);
        memcpy(&dest[offset], color, sizeof(wchar_t) * l);
        offset += l;
    }

    if (style != nullptr) {
        auto l = wcslen(style);
        memcpy(&dest[offset], style, sizeof(wchar_t) * l);
        offset += l;
    }

    memcpy(&dest[offset], src, sizeof(wchar_t) * src_len);
    offset += src_len;

    if (color != nullptr || style != nullptr) {
        auto l = wcslen(COLOR_RESET);
        memcpy(&dest[offset], COLOR_RESET, sizeof(wchar_t) * l);
        offset += l;
    }

    if (alignment == TextAlignment::Left) {
        wmemset(&dest[offset], L' ', line_len - src_len);
        offset += line_len - src_len;
    }

    if (alignment == TextAlignment::Center) {
        auto char_c = (line_len - src_len) / 2;
        wmemset(&dest[offset], L' ', char_c);
        offset += char_c;
    }

    return offset;   
}
