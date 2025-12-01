#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <sstream>

// --- Constants (using SFML types) ---
const int BOARD_WIDTH = 10;
const int BOARD_HEIGHT = 20;
const int BLOCK_SIZE = 30; // Pixel size of one block
const int WINDOW_WIDTH = BOARD_WIDTH * BLOCK_SIZE + 200; // Extra width for score panel
const int WINDOW_HEIGHT = BOARD_HEIGHT * BLOCK_SIZE;
const float GRAVITY_INTERVAL_SECONDS = 0.5f;

// --- Global Game State (The Core Logic) ---
std::vector<std::vector<int>> board(BOARD_HEIGHT, std::vector<int>(BOARD_WIDTH, 0));
int score = 0; // Global score tracker
int lines_cleared = 0; // Global lines tracker
int current_piece_type = 0;
int current_rotation = 0;
int current_row = 0;
int current_col = 3;
bool game_over = false;
bool is_paused = false; // New pause state

// Simplified Tetromino Definitions
const std::vector<std::vector<std::string>> TETROMINOS = {
    // 0: I-Piece
    {"0000111100000000", "0100010001000100", "0000111100000000", "0100010001000100"},
    // 1: J-Piece
    {"1001110000000000", "0100010011000000", "1110001000000000", "0110010001000000"},
    // 2: L-Piece
    {"0011110000000000", "0110001000100000", "1110100000000000", "0100010011000000"},
    // 3: O-Piece
    {"1100110000000000", "1100110000000000", "1100110000000000", "1100110000000000"},
    // 4: S-Piece
    {"0111100000000000", "1001100100000000", "0111100000000000", "1001100100000000"},
    // 5: T-Piece
    {"0101110000000000", "0100110001000000", "1110010000000000", "0101100010000000"},
    // 6: Z-Piece
    {"1100011000000000", "0100110010000000", "1100011000000000", "0100110010000000"}
};

// Colors corresponding to piece index + 1 (index 0 is empty)
const std::vector<sf::Color> BLOCK_COLORS = {
    sf::Color::Black,       // 0: Empty
    sf::Color::Cyan,        // 1: I-Piece
    sf::Color::Blue,        // 2: J-Piece
    sf::Color(255, 165, 0), // 3: L-Piece (Orange)
    sf::Color::Yellow,      // 4: O-Piece
    sf::Color::Green,       // 5: S-Piece
    sf::Color::Magenta,     // 6: T-Piece (Purple)
    sf::Color::Red          // 7: Z-Piece
};

// --- Forward Declarations ---
bool check_collision(int piece_type, int rotation, int r, int c);
void new_piece();
void lock_piece();
void check_and_clear_lines();

// Gets the character at a specific local coordinate of the current piece's 4x4 matrix
char get_piece_block(int piece_type, int rotation, int r, int c) {
    return TETROMINOS[piece_type][rotation][r * 4 + c];
}

/**
 * @brief Locks the current falling piece into the main game board.
 */
void lock_piece() {
    int piece_color = current_piece_type + 1;
    for (int pr = 0; pr < 4; ++pr) {
        for (int pc = 0; pc < 4; ++pc) {
            if (get_piece_block(current_piece_type, current_rotation, pr, pc) == '1') {
                int br = current_row + pr;
                int bc = current_col + pc;
                if (br >= 0 && br < BOARD_HEIGHT && bc >= 0 && bc < BOARD_WIDTH) {
                    board[br][bc] = piece_color;
                }
            }
        }
    }
}

/**
 * @brief Spawns a new random Tetromino at the top center.
 */
void new_piece() {
    current_piece_type = rand() % TETROMINOS.size();
    current_rotation = 0;
    current_row = 0;
    current_col = BOARD_WIDTH / 2 - 2;
    if (check_collision(current_piece_type, current_rotation, current_row, current_col)) {
        game_over = true;
    }
}

/**
 * @brief Checks if the current piece, at a potential new position, collides.
 */
bool check_collision(int piece_type, int rotation, int r, int c) {
    for (int pr = 0; pr < 4; ++pr) {
        for (int pc = 0; pc < 4; ++pc) {
            if (get_piece_block(piece_type, rotation, pr, pc) == '1') {
                int br = r + pr;
                int bc = c + pc;
                if (br >= BOARD_HEIGHT || bc < 0 || bc >= BOARD_WIDTH) return true;
                if (br >= 0 && board[br][bc] != 0) return true;
            }
        }
    }
    return false;
}

/**
 * @brief Implements the Hard Drop functionality (moves piece instantly to the bottom).
 */
void hard_drop() {
    if (game_over || is_paused) return;

    int drop_row = current_row;
    while (!check_collision(current_piece_type, current_rotation, drop_row + 1, current_col)) {
        drop_row++;
    }
    current_row = drop_row;

    // Lock the piece immediately after dropping
    lock_piece();
    check_and_clear_lines();
    new_piece();
}

/**
 * @brief Checks the board for completed lines, clears them, and updates the global score/lines.
 */
void check_and_clear_lines() {
    int lines_cleared_in_move = 0;
    for (int r = BOARD_HEIGHT - 1; r >= 0; --r) {
        bool line_full = true;
        for (int c = 0; c < BOARD_WIDTH; ++c) {
            if (board[r][c] == 0) {
                line_full = false;
                break;
            }
        }

        if (line_full) {
            lines_cleared_in_move++;
            lines_cleared++; // Update global lines count

            // Shift all rows above the cleared line down
            for (int r_shift = r; r > 0; --r_shift) {
                board[r_shift] = board[r_shift - 1];
            }
            // Clear the top row
            board[0].assign(BOARD_WIDTH, 0);

            // Recheck the current row 'r' since it now contains new content
            r++;
        }
    }

    // Scoring (standard Tetris scoring)
    if (lines_cleared_in_move > 0) {
        // 1-line: 100, 2-line: 300, 3-line: 500, 4-line (Tetris): 800
        const int points[] = {0, 100, 300, 500, 800};
        if (lines_cleared_in_move <= 4) {
             score += points[lines_cleared_in_move];
        }
    }
}

/**
 * @brief Handles drawing the game board, the falling piece, and the UI elements.
 */
void render_game(sf::RenderWindow& window, sf::RectangleShape& block_shape, sf::Font& font) {
    // 1. Draw the locked board pieces
    for (int r = 0; r < BOARD_HEIGHT; ++r) {
        for (int c = 0; c < BOARD_WIDTH; ++c) {
            int color_index = board[r][c];
            if (color_index != 0) {
                block_shape.setFillColor(BLOCK_COLORS[color_index]);
                block_shape.setPosition(static_cast<float>(c * BLOCK_SIZE), static_cast<float>(r * BLOCK_SIZE));
                window.draw(block_shape);
            }
        }
    }

    // 2. Draw the current falling piece
    int piece_color = current_piece_type + 1;
    block_shape.setFillColor(BLOCK_COLORS[piece_color]);

    for (int pr = 0; pr < 4; ++pr) {
        for (int pc = 0; pc < 4; ++pc) {
            if (get_piece_block(current_piece_type, current_rotation, pr, pc) == '1') {
                int br = current_row + pr;
                int bc = current_col + pc;

                if (br >= 0 && br < BOARD_HEIGHT && bc >= 0 && bc < BOARD_WIDTH) {
                    block_shape.setPosition(static_cast<float>(bc * BLOCK_SIZE), static_cast<float>(br * BLOCK_SIZE));
                    window.draw(block_shape);
                }
            }
        }
    }

    // 3. Draw the UI (Score and Controls)
    float ui_x = BOARD_WIDTH * BLOCK_SIZE + 20.f; // Start drawing outside the board

    // Score Display
    sf::Text score_text;
    score_text.setFont(font);
    score_text.setCharacterSize(24);
    score_text.setFillColor(sf::Color::White);
    std::stringstream ss;
    ss << "SCORE:\n" << score << "\n\nLINES:\n" << lines_cleared;
    score_text.setString(ss.str());
    score_text.setPosition(ui_x, 50.f);
    window.draw(score_text);

    // Controls Display
    sf::Text controls_text;
    controls_text.setFont(font);
    controls_text.setCharacterSize(16);
    controls_text.setFillColor(sf::Color(180, 180, 180));
    controls_text.setString(
        "CONTROLS:\n"
        "Left/Right: Move\n"
        "Up: Rotate\n"
        "Down: Soft Drop\n"
        "Space: Hard Drop\n"
        "P: Pause"
    );
    controls_text.setPosition(ui_x, 300.f);
    window.draw(controls_text);

    // 4. Draw Pause/Game Over Screen
    sf::Text status_text;
    status_text.setFont(font);
    status_text.setCharacterSize(48);
    status_text.setFillColor(sf::Color::Red);
    status_text.setOutlineColor(sf::Color::Black);
    status_text.setOutlineThickness(3.f);

    if (is_paused) {
        status_text.setString("PAUSED");
        // Center text on screen
        sf::FloatRect textRect = status_text.getLocalBounds();
        status_text.setOrigin(textRect.left + textRect.width/2.0f, textRect.top + textRect.height/2.0f);
        status_text.setPosition(sf::Vector2f(WINDOW_WIDTH/2.0f, WINDOW_HEIGHT/2.0f));
        window.draw(status_text);
    } else if (game_over) {
        status_text.setString("GAME OVER\nScore: " + std::to_string(score));
        status_text.setCharacterSize(40);

        // Center text on screen
        sf::FloatRect textRect = status_text.getLocalBounds();
        status_text.setOrigin(textRect.left + textRect.width/2.0f, textRect.top + textRect.height/2.0f);
        status_text.setPosition(sf::Vector2f(WINDOW_WIDTH/2.0f, WINDOW_HEIGHT/2.0f));
        window.draw(status_text);
    }
}


/**
 * @brief Main function to initialize SFML and run the game loop.
 */
int main() {
    // 1. Initialization
    srand(static_cast<unsigned int>(time(NULL)));

    // --- Font Loading ---
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        // Fallback for when font is missing. SFML Text will fail gracefully.
        std::cerr << "Error: Could not load font file 'arial.ttf'. UI text will not display." << std::endl;
    }

    new_piece();

    // Create the main window
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "C++ SFML Tetris");
    window.setFramerateLimit(60); // Set max FPS

    // Create a shape object to represent a single Tetris block
    sf::RectangleShape block_shape(sf::Vector2f(BLOCK_SIZE - 1.f, BLOCK_SIZE - 1.f));
    block_shape.setOutlineColor(sf::Color(50, 50, 50));
    block_shape.setOutlineThickness(1.f);

    sf::Clock clock;
    float time_since_last_drop = 0.0f;

    // --- The SFML Game Loop ---
    while (window.isOpen()) {
        float delta_time = clock.restart().asSeconds();

        // 1. Input Handling (Event Loop) - Includes Pause and Hard Drop
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed) {
                // Global controls (Pause)
                if (event.key.code == sf::Keyboard::P) {
                    is_paused = !is_paused;
                    break;
                }

                // Only process movement/rotation if the game is running and not paused
                if (game_over || is_paused) break;

                int new_row = current_row;
                int new_col = current_col;
                int new_rotation = current_rotation;
                bool moved = false;

                if (event.key.code == sf::Keyboard::Left) {
                    new_col--;
                    moved = true;
                } else if (event.key.code == sf::Keyboard::Right) {
                    new_col++;
                    moved = true;
                } else if (event.key.code == sf::Keyboard::Down) {
                    new_row++; // Soft drop
                    moved = true;
                } else if (event.key.code == sf::Keyboard::Up) {
                    new_rotation = (current_rotation + 1) % 4;
                    moved = true;
                } else if (event.key.code == sf::Keyboard::Space) {
                    // Hard Drop: Implemented as a function call
                    hard_drop();
                    // Reset gravity timer immediately after hard drop
                    time_since_last_drop = 0.0f;
                    break;
                }

                if (moved && !check_collision(current_piece_type, new_rotation, new_row, new_col)) {
                    current_row = new_row;
                    current_col = new_col;
                    current_rotation = new_rotation;
                }
            }
        }

        // --- Game Update Logic (Only run if not paused and not over) ---
        if (!is_paused && !game_over) {
            time_since_last_drop += delta_time;

            // 2. Gravity Update - Replaces Chrono Timer
            if (time_since_last_drop >= GRAVITY_INTERVAL_SECONDS) {
                time_since_last_drop = 0.0f;

                if (!check_collision(current_piece_type, current_rotation, current_row + 1, current_col)) {
                    current_row++;
                } else {
                    // Collision detected: lock the piece
                    lock_piece();
                    check_and_clear_lines();
                    new_piece();
                }
            }
        }


        // 3. Rendering
        window.clear(sf::Color(20, 20, 40)); // Dark blue background

        render_game(window, block_shape, font);

        window.display();
    } // End of SFML Game Loop

    return 0;
}