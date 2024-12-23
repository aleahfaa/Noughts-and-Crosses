#include <iostream>
#include <stdlib.h>
#include <vector>
#include <thread>
#include <mutex>
#include <ctime>
#include <algorithm>
#include <limits>

#ifdef Multiplayer
#include <sys/mman.h> // shared memory operations
#include <fcntl.h>    // title control options
#include <unistd.h>   // POSIX operating system API
#endif

#define Red "\033[31m"
#define Yellow "\033[33m"
#define Reset "\033[0m" // reset to default

using namespace std;

mutex game_mutex; // thread-safe operations in multiplayer mode

// enumerations for player, cell status, and game status
enum Player { X = 0, O = 1 };
enum CellStatus { EMPTY, X_MARK, O_MARK };
enum GameStatus { ONGOING, DRAW, WIN };

// structure to represent the game state
struct GameState {
    char board[3][3]; // board
    Player turn;      // current player's turn
    GameStatus gameOver; // game status
    Player winner;       // winner
    bool multiplayer;    // multiplayer mode flag
    Player playerRole;   // player's role
};

#ifdef Multiplayer
GameState* sharedState; // pointer to shared memory for multiplayer mode
#else
GameState localState;   // local game state for single-player mode
#endif

static int moveCount = 0; // counter for total moves

// function to print a single cell of the board
inline void printCell(char cell) {
    if (cell == 'X') {
        cout << Red << " X " << Reset;
    } else if (cell == 'O') {
        cout << Yellow << " O " << Reset;
    } else { // empty cell with number
        cout << " " << cell << " ";
    }
}

// initialize the game board and state
void initializeGame(GameState& state) {
    int count = 1;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            state.board[i][j] = '0' + count++; // fill cells with numbers 1-9
        }
    }
    state.turn = Player::X;  // x always starts first
    state.gameOver = GameStatus::ONGOING;
    // state.winner = Player::X; // default winner
    // cout << "Game initialized with X starting.\n";
}

// render the current state of the board
void renderBoard(const GameState& state) {
    cout << "Board:\n";
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            printCell(state.board[i][j]); // print each cell
            if (j < 2) cout << "|";    // column separator
        }
        if (i < 2) cout << "\n-----------"; // row separator
        cout << "\n";
    }
    cout << "\n";
}

// check for a winner or a draw
bool checkWinner(GameState& state) {
    // check rows and columns for a win
    for (int i = 0; i < 3; ++i) {
        // rows
        if (state.board[i][0] == state.board[i][1] && state.board[i][1] == state.board[i][2]) {
            if (state.board[i][0] == 'X' || state.board[i][0] == 'O') {
                state.winner = (state.board[i][0] == 'X') ? Player::X : Player::O;
                // cout << "Winner found in row " << i << "\n";
                return true;
            }
        }
        // columns
        if (state.board[0][i] == state.board[1][i] && state.board[1][i] == state.board[2][i]) {
            if (state.board[0][i] == 'X' || state.board[0][i] == 'O') {
                state.winner = (state.board[0][i] == 'X') ? Player::X : Player::O;
                // cout << "Winner found in column " << i << "\n";
                return true;
            }
        }
    }
    // check diagonals for a win
    if (state.board[0][0] == state.board[1][1] && state.board[1][1] == state.board[2][2]) {
        if (state.board[0][0] == 'X' || state.board[0][0] == 'O') {
            state.winner = (state.board[0][0] == 'X') ? Player::X : Player::O;
            // cout << "Winner found in main diagonal\n";
            return true;
        }
    }
    if (state.board[0][2] == state.board[1][1] && state.board[1][1] == state.board[2][0]) {
        if (state.board[0][2] == 'X' || state.board[0][2] == 'O') {
            state.winner = (state.board[0][2] == 'X') ? Player::X : Player::O;
            // cout << "Winner found in anti-diagonal\n";
            return true;
        }
    }
    // check for a draw (no empty cells left)
    bool draw = true;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (state.board[i][j] != 'X' && state.board[i][j] != 'O') {
                draw = false;
            }
        }
    }
    if (draw) {
        // state.winner = Player::X; // default winner, draw indicator
        cout << "Game is a draw.\n";
        return true;
    }
    return false;
}

// handle a player's turn
void playerTurn(GameState& state, Player player) {
    int move;
    while (true) {
        cout << "Player " << (player == Player::X ? "X" : "O") << ", enter your move (1-9): ";
        cin >> move;
        if (move >= 1 && move <= 9) { // ensure move is within range
            int row = (move - 1) / 3;
            int col = (move - 1) % 3;
            if (state.board[row][col] != 'X' && state.board[row][col] != 'O') { // check if cell is empty
                state.board[row][col] = (player == Player::X) ? 'X' : 'O';
                // cout << "Player " << (player == Player::X ? "X" : "O") << " made a move at (" << row << ", " << col << ")\n";
                break;
            } else {
                cout << "Cell already occupied. Try again.\n";
            }
        } else {
            cout << "Invalid move. Enter a number between 1 and 9.\n";
        }
    }
    moveCount++;
    // cout << "Total moves: " << moveCount << "\n";
}

// handle AI's turn
void aiTurn(GameState& state, Player aiPlayer) {
    // try to find a winning move for the ai
    for (int move = 1; move <= 9; ++move) {
        int row = (move - 1) / 3;
        int col = (move - 1) % 3;
        if (state.board[row][col] != 'X' && state.board[row][col] != 'O') {
            char temp = state.board[row][col];
            state.board[row][col] = (aiPlayer == Player::X) ? 'X' : 'O';
            if (checkWinner(state)) {
                // cout << "AI found a winning move at (" << row << ", " << col << ")\n";
                moveCount++;
                return;
            }
            state.board[row][col] = temp; // revert move
        }
    }
    // try to block the opponent from winning
    for (int move = 1; move <= 9; ++move) {
        int row = (move - 1) / 3;
        int col = (move - 1) % 3;
        if (state.board[row][col] != 'X' && state.board[row][col] != 'O') {
            char temp = state.board[row][col];
            Player userPlayer = (aiPlayer == Player::X) ? Player::O : Player::X;
            state.board[row][col] = (userPlayer == Player::X) ? 'X' : 'O';
            if (checkWinner(state)) {
                // user can win, block this move
                state.board[row][col] = (aiPlayer == Player::X) ? 'X' : 'O';
                // cout << "AI blocked a winning move at (" << row << ", " << col << ")\n";
                moveCount++;
                return;
            }
            state.board[row][col] = temp; // revert move
        }
    }
    // make the first available move
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (state.board[i][j] != 'X' && state.board[i][j] != 'O') {
                state.board[i][j] = (aiPlayer == Player::X) ? 'X' : 'O';
                // cout << "AI made a move at (" << i << ", " << j << ")\n";
                moveCount++;
                return;
            }
        }
    }
}

// main game loop
void gameLoop() {
#ifdef Multiplayer
    GameState& state = *sharedState;
#else
    GameState& state = localState;
#endif
    while (state.gameOver == GameStatus::ONGOING) {
        lock_guard<mutex> lock(game_mutex); // ensure thread safety
        renderBoard(state); // display the board
        if (state.turn == state.playerRole) {
            // cout << "Player's turn.\n";
            playerTurn(state, state.playerRole); // handle player's move
        } else {
            Player aiOrOther = (state.playerRole == Player::X) ? Player::O : Player::X;
            if (!state.multiplayer) {
                // cout << "AI's turn.\n";
                aiTurn(state, aiOrOther); // handle ai's move
                // cout << "AI played move for " << (aiOrOther == Player::X ? "X" : "O") << "\n";
            } else {
                // cout << "Other player's turn.\n";
                playerTurn(state, aiOrOther); // handle other player's move
            }
        }
        if (checkWinner(state)) {
            state.gameOver = (state.winner == Player::X || state.winner == Player::O) ? GameStatus::WIN : GameStatus::DRAW;
            renderBoard(state); // display final board state
            // announce game result
            cout << "Game Over!\n" << endl;
            if (state.gameOver == GameStatus::WIN) {
            //if (state.winner == Player::X || state.winner == Player::O) {
                cout << "Player " << (state.winner == Player::X ? "X" : "O") << " wins!\n";
            } else if (state.gameOver == GameStatus::DRAW) {
                cout << "It's a draw!\n" << endl;
            }
        } else {
            state.turn = (state.turn == Player::X) ? Player::O : Player::X; // switch turns
            // cout << "Next turn: " << (state.turn == Player::X ? "X" : "O") << "\n";
        }
    }
}

int main() {
    // game setup and mode selection
    int gameMode;
    cout << "Choose game mode:\n" << endl;
    cout << "1. Single Player\n" << endl;
    cout << "2. Multiplayer\n" << endl;
    cin >> gameMode;
    if (gameMode == 1) {
        cout << "You are playing single player mode\n" << endl;
    } else if (gameMode == 2) {
        cout << "You are playing Multiplayer\n" << endl;
    }else {
        cout << "Invalid game mode.\nPlease choose 1 (Single Player) or 2 (Multiplayer)" << endl;
        cin >> gameMode;
    }
    // user role selection
    char playerRole;
    cout << "Choose your role (X or O): " << endl;
    cin >> playerRole;
    while (playerRole != 'X' && playerRole != 'O') {
        cout << "Invalid choice. Please choose X or O: " << endl;
        cin >> playerRole;
    }
    Player role = (playerRole == 'X') ? Player::X : Player::O;
#ifdef Multiplayer
    // initialize shared memory for multiplayer mode
    int fd = shm_open("/tic_tac_toe", O_CREAT | O_RDWR, 0666);
    ftruncate(fd, sizeof(GameState));
    sharedState = (GameState*)mmap(0, sizeof(GameState), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    initializeGame(*sharedState);
    sharedState->multiplayer = (gameMode == 2);
    sharedState->playerRole = role;
#else
    // initialize local game state for single player
    initializeGame(localState);
    localState.multiplayer = (gameMode == 2);
    localState.playerRole = role;
#endif
    // cout << "Starting game loop...\n";
    gameLoop();
// cleanup shared memory in multiplayer mode
#ifdef Multiplayer
    munmap(sharedState, sizeof(GameState));
    shm_unlink("/tic_tac_toe");
#endif
    cout << "Game ended. Exiting the program.\n" << endl;
    return 0;
}
