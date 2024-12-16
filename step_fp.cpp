#include <iostream>
#include <stdlib.h>
#include <vector>
#include <thread>
#include <mutex>
#include <ctime>
#ifdef MULTIPLAYER
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#endif

using namespace std;

mutex game_mutex;

// enums for game status and player roles
enum Player { X = 0, O = 1 };
enum CellStatus { EMPTY, X_MARK, O_MARK };
enum GameStatus { ONGOING, DRAW, WIN };

// structure to represent the game state
struct GameState {
    char board[3][3];          // board
    Player turn;               // current player's turn (X or O)
    GameStatus gameOver;       // game status (ongoing, draw, win)
    Player winner;             // the winner (X or O)
    bool multiplayer;          // true if multiplayer mode
    Player playerRole;         // the player's role (X or O)
};

#ifdef MULTIPLAYER
GameState* sharedState;
#else
GameState localState;
#endif

// static variable to track total moves
static int moveCount = 0;

// inline function to print a single cell
inline void printCell(char cell) {
    cout << " " << cell << " ";
}

// initialize the game state with numbers 1-9 in the cells
void initializeGame(GameState& state) {
    int count = 1;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            state.board[i][j] = '0' + count++;  // store numbers 1-9 as chars
        }
    }
    state.turn = Player::X;
    state.gameOver = GameStatus::ONGOING;
    state.winner = Player::X;
}

// render the board with numbers 1-9 for empty cells
void renderBoard(const GameState& state) {
    cout << "Board:\n";
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            printCell(state.board[i][j]);
            if (j < 2) cout << "|";  // column separator
        }
        if (i < 2) cout << "\n-----------";  // row separator
        cout << "\n";
    }
    cout << "\n";
}

// check if there's a winner (overloaded for both multiplayer and single-player)
bool checkWinner(GameState& state) {
    // check rows, columns, and diagonals
    for (int i = 0; i < 3; ++i) {
        if (state.board[i][0] == state.board[i][1] && state.board[i][1] == state.board[i][2] && state.board[i][0] != ' ') {
            state.winner = (state.board[i][0] == 'X') ? Player::X : Player::O;
            return true;
        }
        if (state.board[0][i] == state.board[1][i] && state.board[1][i] == state.board[2][i] && state.board[0][i] != ' ') {
            state.winner = (state.board[0][i] == 'X') ? Player::X : Player::O;
            return true;
        }
    }
    if (state.board[0][0] == state.board[1][1] && state.board[1][1] == state.board[2][2] && state.board[0][0] != ' ') {
        state.winner = (state.board[0][0] == 'X') ? Player::X : Player::O;
        return true;
    }
    if (state.board[0][2] == state.board[1][1] && state.board[1][1] == state.board[2][0] && state.board[0][2] != ' ') {
        state.winner = (state.board[0][2] == 'X') ? Player::X : Player::O;
        return true;
    }

    // check for draw
    bool draw = true;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (state.board[i][j] != 'X' && state.board[i][j] != 'O') {
                draw = false;
            }
        }
    }
    if (draw) {
        state.winner = Player::X;  // just for indicating draw
        return true;
    }
    return false;
}

// player's turn to make a move (overloaded for single-player and multiplayer modes)
void playerTurn(GameState& state, Player player) {
    int move;
    while (true) {
        cout << "Player " << (player == Player::X ? "X" : "O") << ", enter your move (1-9): ";
        cin >> move;
        if (move >= 1 && move <= 9) {
            int row = (move - 1) / 3;
            int col = (move - 1) % 3;
            if (state.board[row][col] != 'X' && state.board[row][col] != 'O') {
                state.board[row][col] = (player == Player::X) ? 'X' : 'O';
                break;
            } else {
                cout << "Cell already occupied. Try again.\n";
            }
        } else {
            cout << "Invalid move. Enter a number between 1 and 9.\n";
        }
    }
    moveCount++;
}

// ai move (simple ai that chooses a random empty spot)
void aiTurn(GameState& state, Player aiPlayer) {
    srand(time(0));  // seed for random number generation
    int move;
    while (true) {
        move = rand() % 9 + 1;  // random number between 1 and 9
        int row = (move - 1) / 3;
        int col = (move - 1) % 3;
        if (state.board[row][col] != 'X' && state.board[row][col] != 'O') {
            state.board[row][col] = (aiPlayer == Player::X) ? 'X' : 'O';
            break;
        }
    }
    moveCount++;
}

// main game loop
void gameLoop() {
#ifdef MULTIPLAYER
    GameState& state = *sharedState;
#else
    GameState& state = localState;
#endif
    while (state.gameOver == GameStatus::ONGOING) {
        lock_guard<mutex> lock(game_mutex);
        renderBoard(state);  // display the current state of the board

        Player currentPlayer = (state.turn == Player::X) ? state.playerRole : (state.playerRole == Player::X ? Player::O : Player::X);
        if (state.turn == Player::X || state.multiplayer) {
            playerTurn(state, currentPlayer);  // player's move
        } else {
            aiTurn(state, currentPlayer);  // ai's move
            cout << "AI chose move for " << (currentPlayer == Player::X ? "X" : "O") << "\n";
        }

        if (checkWinner(state)) {  // check if the game is over
            state.gameOver = (state.winner == Player::X || state.winner == Player::O) ? GameStatus::WIN : GameStatus::DRAW;
            renderBoard(state);  // display the final board
            cout << "Game Over! ";
            if (state.winner == Player::X || state.winner == Player::O) {
                cout << "Player " << (state.winner == Player::X ? "X" : "O") << " wins!\n";
            } else {
                cout << "It's a draw!\n";
            }
        } else {
            state.turn = (state.turn == Player::X) ? Player::O : Player::X;  // switch turns between X and O
        }
    }
}

int main() {
    // game mode and role selection
    int gameMode;
    cout << "Choose game mode:\n";
    cout << "1. Single Player\n";
    cout << "2. Multiplayer\n";
    cin >> gameMode;
    char playerRole;
    cout << "Choose your role (X or O): ";
    cin >> playerRole;
    while (playerRole != 'X' && playerRole != 'O') {
        cout << "Invalid choice. Please choose X or O: ";
        cin >> playerRole;
    }
    Player role = (playerRole == 'X') ? Player::X : Player::O;
    if (gameMode == 1) {
        cout << "You are playing single player mode\n";
    } else if (gameMode == 2) {
        cout << "You are playing Multiplayer\n";
    } else {
        cout << "Invalid game mode\n";
        return 1;
    }

#ifdef MULTIPLAYER
    // initialize shared memory
    int fd = shm_open("/tic_tac_toe", O_CREAT | O_RDWR, 0666);
    ftruncate(fd, sizeof(GameState));
    sharedState = (GameState*)mmap(0, sizeof(GameState), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    initializeGame(*sharedState);
    sharedState->multiplayer = (gameMode == 2);
    sharedState->playerRole = role;
#else
    initializeGame(localState);
    localState.multiplayer = (gameMode == 2);
    localState.playerRole = role;
#endif
    gameLoop();
#ifdef MULTIPLAYER
    munmap(sharedState, sizeof(GameState));
    shm_unlink("/tic_tac_toe");
#endif
    return 0;
}
