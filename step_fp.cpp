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

struct GameState {
    char board[3][3];
    int turn;      // 0 for X, 1 for O
    bool gameOver;
    char winner;   // 'x', 'o', or 'd' for draw
    bool multiplayer;
    char playerRole;  // 'X' or 'O'
};

#ifdef MULTIPLAYER
GameState* sharedState;
#else
GameState localState;
#endif

// initialize the game state with numbers 1-9 in the cells
void initializeGame(GameState &state) {
    int count = 1;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            state.board[i][j] = '0' + count++;  // store numbers 1-9 as chars
        }
    }
    state.turn = 0;
    state.gameOver = false;
    state.winner = ' ';
}

// render the board with numbers 1-9 for empty cells
void renderBoard(const GameState &state) {
    cout << "Board:\n";
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            cout << " " << state.board[i][j] << " ";  // display the current symbol (X, O, or number)
            if (j < 2) {
                cout << "|";  // column separator
            }
        }
        if (i < 2) {
            cout << "\n-----------";  // row separator
        }
        cout << "\n";
    }
    cout << "\n";
}

// check if there's a winner
bool checkWinner(GameState &state) {
    // rows, columns, and diagonals check
    for (int i = 0; i < 3; ++i) {
        if (state.board[i][0] == state.board[i][1] && state.board[i][1] == state.board[i][2] && state.board[i][0] != ' ') {
            state.winner = state.board[i][0];
            return true;
        }
        if (state.board[0][i] == state.board[1][i] && state.board[1][i] == state.board[2][i] && state.board[0][i] != ' ') {
            state.winner = state.board[0][i];
            return true;
        }
    }
    if (state.board[0][0] == state.board[1][1] && state.board[1][1] == state.board[2][2] && state.board[0][0] != ' ') {
        state.winner = state.board[0][0];
        return true;
    }
    if (state.board[0][2] == state.board[1][1] && state.board[1][1] == state.board[2][0] && state.board[0][2] != ' ') {
        state.winner = state.board[0][2];
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
        state.winner = 'Draw';
    }
    return draw || state.winner != ' ';
}

// player's turn to make a move
void playerTurn(GameState &state, char player) {
    int move;
    while (true) {
        cout << "Player " << player << ", enter your move (1-9): ";
        cin >> move;
        // map the number 1-9 to board positions
        if (move >= 1 && move <= 9) {
            int row = (move - 1) / 3;  // convert to row (0-2)
            int col = (move - 1) % 3;  // convert to column (0-2)
            if (state.board[row][col] != 'X' && state.board[row][col] != 'O') {
                state.board[row][col] = player;  // mark the board with the player's symbol
                break;
            } else {
                cout << "Cell already occupied. Try again.\n";
            }
        } else {
            cout << "Invalid move. Enter a number between 1 and 9.\n";
        }
    }
}

// ai move (simple ai that chooses a random empty spot)
void aiTurn(GameState &state, char aiPlayer) {
    srand(time(0));  // seed for random number generation
    int move;
    while (true) {
        move = rand() % 9 + 1;  // random number between 1 and 9
        int row = (move - 1) / 3;
        int col = (move - 1) % 3;
        if (state.board[row][col] != 'X' && state.board[row][col] != 'O') {
            state.board[row][col] = aiPlayer;
            break;
        }
    }
}

// main game loop
void gameLoop() {
#ifdef MULTIPLAYER
    GameState &state = *sharedState;
#else
    GameState &state = localState;
#endif
    while (!state.gameOver) {
        lock_guard<mutex> lock(game_mutex);
        renderBoard(state);  // display the current state of the board
        char currentPlayer = state.turn == 0 ? state.playerRole : (state.playerRole == 'X' ? 'O' : 'X');  // switch between X and O
        if (state.turn == 0 || state.multiplayer) {
            playerTurn(state, currentPlayer);  // player's move
        } else {
            aiTurn(state, currentPlayer);  // ai's move
            cout << "AI chose move for " << currentPlayer << "\n";
        }
        if (checkWinner(state)) {  // check if the game is over
            state.gameOver = true;
            renderBoard(state);  // display the final board
            cout << "Game Over! ";
            if (state.winner == 'D') {
                cout << "It's a draw!\n";
            } else {
                cout << "Player " << state.winner << " wins!\n";
            }
        } else {
            state.turn = 1 - state.turn;  // switch turns between X and O
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
    if (gameMode == 1) {
        cout << "You are playing single player mode\n";
    } else if (gameMode == 2) {
        cout << "You are playing Multiplayer\n";
    } else {
        cout << "Invalid game mode\n";
        return 1;
    }
#ifdef MULTIPLAYER
    // Initialize shared memory
    int fd = shm_open("/tic_tac_toe", O_CREAT | O_RDWR, 0666);
    ftruncate(fd, sizeof(GameState));
    sharedState = (GameState *)mmap(0, sizeof(GameState), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    initializeGame(*sharedState);
    sharedState->multiplayer = (gameMode == 2);
    sharedState->playerRole = playerRole;
#else
    initializeGame(localState);
    localState.multiplayer = (gameMode == 2);
    localState.playerRole = playerRole;
#endif
    gameLoop();
#ifdef MULTIPLAYER
    munmap(sharedState, sizeof(GameState));
    shm_unlink("/tic_tac_toe");
#endif
    return 0;
}
