//
// Created by BE129 on 11/19/2025.
//



#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <limits> // Required for input clearing

// --- 1. ENUMS AND CONSTANTS ---

// Define the two players
enum Player {
    NONE = 0,
    RED = 1,  // Starts at the bottom of the board (Rows 5, 6, 7)
    BLACK = 2 // Starts at the top of the board (Rows 0, 1, 2)
};

// Define the possible states of a square on the board
enum SquareType {
    EMPTY,
    RED_PIECE,
    BLACK_PIECE,
    RED_KING,
    BLACK_KING
};

const int BOARD_SIZE = 8;
const char PIECE_SYMBOLS[] = {' ', 'R', 'B', 'K', 'k'}; // Corresponding symbols for display

// --- 2. PIECE CLASS ---

/**
 * @class Piece
 * @brief Represents a single checker on the board.
 */
class Piece {
public:
    Player owner;
    bool isKing;
    int row;
    int col;

    Piece(Player p, int r, int c) : owner(p), isKing(false), row(r), col(c) {}

    // Default constructor for placeholder, should generally not be used
    Piece() : owner(NONE), isKing(false), row(-1), col(-1) {}

    // Convert the piece state to a displayable symbol
    char getSymbol() const {
        if (owner == NONE) return ' ';
        if (isKing) {
            return (owner == RED) ? PIECE_SYMBOLS[RED_KING] : PIECE_SYMBOLS[BLACK_KING];
        } else {
            return (owner == RED) ? PIECE_SYMBOLS[RED_PIECE] : PIECE_SYMBOLS[BLACK_PIECE];
        }
    }

    // Promote the piece to a King
    void makeKing() {
        isKing = true;
    }

    // Update the piece's position
    void setPosition(int r, int c) {
        row = r;
        col = c;
    }
};

// --- 3. BOARD CLASS ---

/**
 * @class Board
 * @brief Manages the 8x8 game grid and piece placement.
 */
class Board {
private:
    // 8x8 grid holding pointers to Piece objects
    Piece* grid[BOARD_SIZE][BOARD_SIZE];

public:
    Board() {
        // Initialize the grid to be all empty pointers (nullptr)
        for (int i = 0; i < BOARD_SIZE; ++i) {
            for (int j = 0; j < BOARD_SIZE; ++j) {
                grid[i][j] = nullptr;
            }
        }
    }

    // Destructor to clean up dynamically allocated pieces
    ~Board() {
        for (int i = 0; i < BOARD_SIZE; ++i) {
            for (int j = 0; j < BOARD_SIZE; ++j) {
                delete grid[i][j];
                grid[i][j] = nullptr;
            }
        }
    }

    // Sets up the board with 12 pieces for each player
    void initializeBoard() {
        // Clear any existing pieces
        for (int i = 0; i < BOARD_SIZE; ++i) {
            for (int j = 0; j < BOARD_SIZE; ++j) {
                delete grid[i][j];
                grid[i][j] = nullptr;
            }
        }

        // BLACK pieces (start at top, rows 0, 1, 2)
        for (int r = 0; r < 3; ++r) {
            for (int c = 0; c < BOARD_SIZE; ++c) {
                // Pieces are only placed on "dark" squares (row + col is odd)
                if ((r + c) % 2 != 0) {
                    grid[r][c] = new Piece(BLACK, r, c);
                }
            }
        }

        // RED pieces (start at bottom, rows 5, 6, 7)
        for (int r = 5; r < BOARD_SIZE; ++r) {
            for (int c = 0; c < BOARD_SIZE; ++c) {
                if ((r + c) % 2 != 0) {
                    grid[r][c] = new Piece(RED, r, c);
                }
            }
        }
    }

    // Prints the current state of the board to the console
    void displayBoard() const {
        std::cout << "\n    A B C D E F G H (Columns)" << std::endl;
        std::cout << "  -----------------" << std::endl;
        for (int i = 0; i < BOARD_SIZE; ++i) {
            std::cout << i + 1 << " |"; // Row number (1-8)
            for (int j = 0; j < BOARD_SIZE; ++j) {
                if (grid[i][j]) {
                    std::cout << " " << grid[i][j]->getSymbol();
                } else {
                    // Only show a marker for playable (dark) squares
                    if ((i + j) % 2 != 0) {
                         std::cout << " ."; // Playable empty square
                    } else {
                         std::cout << "  "; // Unplayable square
                    }
                }
            }
            std::cout << " |" << std::endl;
        }
        std::cout << "  -----------------" << std::endl;
    }

    // Getter for a piece at a specific location
    Piece* getPiece(int r, int c) const {
        // Check bounds before accessing
        if (r < 0 || r >= BOARD_SIZE || c < 0 || c >= BOARD_SIZE) {
            return nullptr;
        }
        return grid[r][c];
    }

    // Moves a piece from (r1, c1) to (r2, c2)
    void movePiece(int r1, int c1, int r2, int c2) {
        Piece* piece = grid[r1][c1];
        if (piece) {
            // Update the grid
            grid[r2][c2] = piece;
            grid[r1][c1] = nullptr;
            // Update the piece's internal position
            piece->setPosition(r2, c2);
        }
    }

    // Removes a captured piece (called after a jump)
    void removePiece(int r, int c) {
        Piece* capturedPiece = grid[r][c];
        if (capturedPiece) {
            delete capturedPiece; // Free the memory
            grid[r][c] = nullptr; // Set the square to empty
        }
    }
};

// --- 4. GAME MANAGER CLASS ---

/**
 * @class CheckersGame
 * @brief Manages the overall game flow, rules, and player turns.
 */
class CheckersGame {
private:
    Board board;
    Player currentPlayer;

    // Struct to represent a potential move/jump
    struct Move {
        int startR, startC, endR, endC;
    };

    // Helper function to check if coordinates are within the board bounds
    bool isInBounds(int r, int c) const {
        return r >= 0 && r < BOARD_SIZE && c >= 0 && c < BOARD_SIZE;
    }

    // Checks if a specific move is a valid *simple* (non-jump) move
    bool isSimpleMoveValid(int r1, int c1, int r2, int c2) const {
        Piece* piece = board.getPiece(r1, c1);
        if (!piece || board.getPiece(r2, c2)) {
            return false; // No piece at start or target is occupied
        }

        // Must move exactly one diagonal square
        if (std::abs(r2 - r1) != 1 || std::abs(c2 - c1) != 1) {
            return false;
        }

        // Check direction for non-kings
        if (!piece->isKing) {
            if (piece->owner == RED && (r2 - r1) > 0) return false; // Red must move up (smaller row index)
            if (piece->owner == BLACK && (r2 - r1) < 0) return false; // Black must move down (larger row index)
        }

        return true;
    }

    // Checks if a specific move is a valid *jump* (capture) move
    // This is the core logic for capturing
    bool isJumpValid(int r1, int c1, int r2, int c2) const {
        Piece* piece = board.getPiece(r1, c1);
        if (!piece || board.getPiece(r2, c2)) {
            return false; // No piece at start or target is occupied
        }

        // Must move exactly two diagonal squares
        if (std::abs(r2 - r1) != 2 || std::abs(c2 - c1) != 2) {
            return false;
        }

        // Calculate the position of the jumped piece
        int jumpedR = (r1 + r2) / 2;
        int jumpedC = (c1 + c2) / 2;
        Piece* jumpedPiece = board.getPiece(jumpedR, jumpedC);

        // Check if there is an opponent's piece to jump over
        if (!jumpedPiece || jumpedPiece->owner == piece->owner || jumpedPiece->owner == NONE) {
            return false;
        }

        // Check direction for non-kings
        if (!piece->isKing) {
            if (piece->owner == RED && (r2 - r1) > 0) return false; // Red must move up (smaller row index)
            if (piece->owner == BLACK && (r2 - r1) < 0) return false; // Black must move down (larger row index)
        }

        return true;
    }

    // Finds all possible jumps for a single piece
    std::vector<Move> getPossibleJumpsForPiece(int r, int c) const {
        std::vector<Move> jumps;
        Piece* piece = board.getPiece(r, c);
        if (!piece) return jumps;

        // Possible jump directions: (-2, -2), (-2, +2), (+2, -2), (+2, +2)
        int directions[4][2] = {{-2, -2}, {-2, 2}, {2, -2}, {2, 2}};

        for (int i = 0; i < 4; ++i) {
            int r2 = r + directions[i][0];
            int c2 = c + directions[i][1];

            if (isInBounds(r2, c2) && isJumpValid(r, c, r2, c2)) {
                jumps.push_back({r, c, r2, c2});
            }
        }
        return jumps;
    }

    // Finds ALL possible jumps for the current player on the board
    std::vector<Move> getAllPossibleJumps() const {
        std::vector<Move> allJumps;
        for (int r = 0; r < BOARD_SIZE; ++r) {
            for (int c = 0; c < BOARD_SIZE; ++c) {
                Piece* piece = board.getPiece(r, c);
                if (piece && piece->owner == currentPlayer) {
                    std::vector<Move> jumps = getPossibleJumpsForPiece(r, c);
                    allJumps.insert(allJumps.end(), jumps.begin(), jumps.end());
                }
            }
        }
        return allJumps;
    }

    // Finds all possible simple moves for the current player
    std::vector<Move> getAllPossibleSimpleMoves() const {
        std::vector<Move> allMoves;
        for (int r = 0; r < BOARD_SIZE; ++r) {
            for (int c = 0; c < BOARD_SIZE; ++c) {
                Piece* piece = board.getPiece(r, c);
                if (piece && piece->owner == currentPlayer) {
                    // Check all 4 surrounding diagonal squares
                    int directions[4][2] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
                    for (int i = 0; i < 4; ++i) {
                        int r2 = r + directions[i][0];
                        int c2 = c + directions[i][1];

                        if (isInBounds(r2, c2) && isSimpleMoveValid(r, c, r2, c2)) {
                            allMoves.push_back({r, c, r2, c2});
                        }
                    }
                }
            }
        }
        return allMoves;
    }

    // Helper to switch the current player
    void switchPlayer() {
        currentPlayer = (currentPlayer == RED) ? BLACK : RED;
    }

    // Executes the actual move, including kinging and capture
    bool executeMove(int r1, int c1, int r2, int c2) {
        // 1. Perform the movement
        board.movePiece(r1, c1, r2, c2);

        // 2. Check for Capture (if it was a jump move)
        if (std::abs(r2 - r1) == 2) {
            int capturedR = (r1 + r2) / 2;
            int capturedC = (c1 + c2) / 2;
            board.removePiece(capturedR, capturedC);
            std::cout << "-> PIECE CAPTURED at " << (char)('A' + capturedC) << capturedR + 1 << "!" << std::endl;

            // 3. Check for multi-jump opportunity
            if (!getPossibleJumpsForPiece(r2, c2).empty()) {
                std::cout << "-> MULTI-JUMP AVAILABLE! Player " << (currentPlayer == RED ? "RED" : "BLACK")
                          << " must continue jumping from " << (char)('A' + c2) << r2 + 1 << "." << std::endl;
                // Force the same player to take another turn from the new position
                return true;
            }
        }

        // 4. Check for Kinging
        Piece* piece = board.getPiece(r2, c2);
        if (piece) {
            if (piece->owner == RED && r2 == 0) { // Red reaches Black's back rank
                piece->makeKing();
                std::cout << "-> RED piece KINGED at " << (char)('A' + c2) << r2 + 1 << "!" << std::endl;
            } else if (piece->owner == BLACK && r2 == BOARD_SIZE - 1) { // Black reaches Red's back rank
                piece->makeKing();
                std::cout << "-> BLACK piece KINGED at " << (char)('A' + c2) << r2 + 1 << "!" << std::endl;
            }
        }

        // 5. Normal turn end
        switchPlayer();
        return false; // Turn is over
    }

    // Checks for win/loss condition (no more pieces or no more valid moves)
    Player checkForWin() const {
        int redPieces = 0;
        int blackPieces = 0;
        for (int i = 0; i < BOARD_SIZE; ++i) {
            for (int j = 0; j < BOARD_SIZE; ++j) {
                Piece* p = board.getPiece(i, j);
                if (p) {
                    if (p->owner == RED) redPieces++;
                    if (p->owner == BLACK) blackPieces++;
                }
            }
        }

        if (redPieces == 0) return BLACK;
        if (blackPieces == 0) return RED;

        // Check if the current player has any possible moves (jumps or simple moves)
        if (getAllPossibleJumps().empty() && getAllPossibleSimpleMoves().empty()) {
             // If the current player has no moves, the other player wins
             return (currentPlayer == RED) ? BLACK : RED;
        }

        return NONE; // No winner yet
    }

    // Parses user input like "A3 to B4" into coordinates (r1, c1, r2, c2)
    bool parseInput(const std::string& input, int& r1, int& c1, int& r2, int& c2) {
        // Expected format: XN to YM (e.g., A6 to B5)
        if (input.length() < 7) return false;

        // Convert column letters (A-H) to index (0-7)
        c1 = std::toupper(input[0]) - 'A';
        r1 = input[1] - '1'; // Convert row digit (1-8) to index (0-7)

        // Expected " to " at index 2
        if (input.substr(2, 4) != " to ") return false;

        c2 = std::toupper(input[6]) - 'A';
        r2 = input[7] - '1';

        // Final coordinate checks
        if (!isInBounds(r1, c1) || !isInBounds(r2, c2)) {
            return false;
        }

        // Check that the move is actually a move and not staying in place
        if (r1 == r2 && c1 == c2) {
            return false;
        }

        return true;
    }

public:
    CheckersGame() : currentPlayer(RED) {}

    void run() {
        board.initializeBoard();
        std::cout << "===========================================" << std::endl;
        std::cout << "      WELCOME TO C++ CONSOLE CHECKERS      " << std::endl;
        std::cout << "===========================================" << std::endl;
        std::cout << "Red (R) starts at the bottom. Black (B) at the top." << std::endl;
        std::cout << "Input format: [COLROW] to [COLROW] (e.g., A6 to B5)" << std::endl;

        while (true) {
            board.displayBoard();

            Player winner = checkForWin();
            if (winner != NONE) {
                std::cout << "\n*******************************************" << std::endl;
                std::cout << "        PLAYER " << (winner == RED ? "RED" : "BLACK") << " WINS!         " << std::endl;
                std::cout << "*******************************************" << std::endl;
                break;
            }

            // Get available moves/jumps for the current player
            std::vector<Move> forcedJumps = getAllPossibleJumps();
            bool jumpIsForced = !forcedJumps.empty();

            std::cout << "\n--- Player " << (currentPlayer == RED ? "RED (R/K)" : "BLACK (B/k)") << "'s Turn ---" << std::endl;
            if (jumpIsForced) {
                std::cout << "!!! JUMP IS MANDATORY !!! You must take a jump. !!!" << std::endl;
            }

            std::string input;
            int r1, c1, r2, c2;
            bool turnComplete = false;

            // Loop until a valid move is made
            while (!turnComplete) {
                std::cout << "Enter move (e.g., A6 to B5) or 'exit': ";
                std::getline(std::cin, input);

                if (input == "exit" || input == "quit") {
                    std::cout << "Game exited by player." << std::endl;
                    return;
                }

                // Parse user input
                if (!parseInput(input, r1, c1, r2, c2)) {
                    std::cout << "Invalid input format or coordinates. Try again (e.g., A6 to B5)." << std::endl;
                    continue;
                }

                // Get the piece to move
                Piece* piece = board.getPiece(r1, c1);

                // 1. Basic checks
                if (!piece || piece->owner != currentPlayer) {
                    std::cout << "Invalid selection. That square is empty or doesn't belong to you." << std::endl;
                    continue;
                }

                bool isJump = std::abs(r2 - r1) == 2;
                bool moveIsValid = false;
                bool keepJumping = false; // For multi-jumps

                // 2. Main Logic: Check if move is valid based on rules
                if (jumpIsForced) {
                    if (isJump) {
                        if (isJumpValid(r1, c1, r2, c2)) {
                            moveIsValid = true;
                            keepJumping = executeMove(r1, c1, r2, c2);
                            turnComplete = !keepJumping;
                        } else {
                            std::cout << "Invalid jump. You must capture an opponent's piece." << std::endl;
                        }
                    } else {
                        std::cout << "A jump is available and MUST be taken. Please enter a valid jump move." << std::endl;
                    }
                } else {
                    // No jump is forced, so check for simple move
                    if (isJump) {
                        if (isJumpValid(r1, c1, r2, c2)) {
                            moveIsValid = true;
                            keepJumping = executeMove(r1, c1, r2, c2);
                            turnComplete = !keepJumping;
                        } else {
                             std::cout << "Invalid jump (no opponent piece to capture)." << std::endl;
                        }
                    } else if (isSimpleMoveValid(r1, c1, r2, c2)) {
                        moveIsValid = true;
                        executeMove(r1, c1, r2, c2);
                        turnComplete = true; // Simple move always ends the turn
                    } else {
                        std::cout << "Invalid simple move. Check diagonal movement and direction rules." << std::endl;
                    }
                }
            }
        }
    }
};

// --- 5. MAIN FUNCTION ---

int main() {
    // Set standard output to not synchronize with C standard streams for better performance
    std::ios_base::sync_with_stdio(false);

    // Create and run the game
    CheckersGame game;
    game.run();

    return 0;
}