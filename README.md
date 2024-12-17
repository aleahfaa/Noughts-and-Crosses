# Noughts-and-Crosses

<p align="center"><b>Programming Languages for Biomedical Engineering - 0014E064 (2024)</b></p>

<p align="center"><a href="https://eng.tmu.edu.tw/">臺北醫學大學</a><br>Taipei Medical University</p>

<p align="center"><img src="https://upload.wikimedia.org/wikipedia/en/b/ba/Taipei_Medical_University_logo_with_namestyle.svg" style="transform: scale(0.5);"></p>

<p>This project is computer program developed as part of Final Project for the <a href="https://im.tmu.edu.tw/course/info/14518">Programming Languages for Biomedical Engineering (醫學工程程式語言)</a> class. This program is made by <a href="https://github.com/aleahfaa">Iffa Amalia</a>.</p>

On behalf of:

**Dr. Tsai Cheng-Yu (蔡承育)**

## Project Description
Noughts and Crosses or widely known as Tic Tac Toe is a two-dimensional game played by people of all ages due to its simplicity. I choose to develop this game using C++ because I believe this game is perfect to learn and understand the basic concepts of programming. The game is played by two players — X and O — who take turns marking the boxes of a 3x3 grid and the objective of the game is for the player to make a horizontal, vertical or diagonal line with three of their symbols. I decided to implement the grid in the code as a two-dimensional matrix (3x3) or I could even use an array of size 9 so as to make the logic of the game’s implementation sophisticated but at the same time scalable.

While constructing the Tic Tac Toe program, I used an organized approach and created the code in modular parts that are clean and can be reused. I implemented functions for initializing the game board and the user interface, managing turns, checking if a move is correct, and evaluating the win states. Because this is a modular system, each function calls only one and is responsible for one task which makes debugging and extensions easier. For example, in the case of outputting the game board display, there is no need to explain where it gets its information – it simply displays the current state of the grid, while a dedicated function checks the cells and prevents improper moves. Designing in this manner has the effect of enhancing maintenance and increasing the efficiency of the whole code.

Representing the board as a two-dimensional array made it very convenient for me to record the moves made by each player and subsequently check for the possibility of winning combinations in the horizontal, vertical and both diagonal lines. I was forced to integrate control structures in C++ like for and while loops to ensure I am able to build a coherent design that loops through the game sequence and checks the status of the board after every move. Conditional statements such as if-else statements are very useful because they ensure that players can only make a move on empty cells and hence maintain a smooth flow of the game.

In addition, implementing Tic Tac Toe also encouraged a practical approach of mastering data manipulation in C++. For instance, the enhanced visualization of the game board status and the players’ actions due to the implementation of arrays made the data more orderly and easy to navigate. The input and output functions, cin and cout, add another layer of complexity to the game since players are able to interact with the game as they make their moves and receive timely updates regarding the events in the game. Such a feature enhances the interactivity of the game, and the overall ease of use of the program because the players interact with the application directly.

## Related C++ Technique
1. Pointers
- `GameState* sharedState;`(used with the `MULTIPLAYER` preprocessor directive)
- A pointer is used when the game runs in multiplayer mode to share the game state between processes. It points to a shared memory region for communication between different instances of the game.
2. Functions
Multiple functions are defined to handle specific tasks like initializing the game (`initializeGame`), rendering the board (`renderBoard`), making a move (`playerTurn`, `aiTurn`), and checking for a winner (`checkWinner`). These functions modularize the game logic and keep the code clean and organized.
3. Call by Value, Call by Reference.<br />
**Call by Reference**
- `void initializeGame(GameState& state)` — The game state is passed by reference, which allows the function to modify the game state directly.
- `void renderBoard(const GameState& state)` — The game state is passed by reference but declared as `const` to prevent modification.
- `bool checkWinner(GameState& state)` — The game state is passed by reference, enabling the function to modify it (e.g., update the winner).
- `void playerTurn(GameState& state, Player player)` — Modifies the game state directly, so it's passed by reference.<br />
**Call by Value**
- `Player currentPlayer = (state.turn == Player::X) ? state.playerRole : (state.playerRole == Player::X ? Player::O : Player::X);` — This expression evaluates the current player by creating a value, not modifying the original state.
4. Static Variables <br />
`static int moveCount = 0;` — The `moveCount` variable is declared as static, meaning it retains its value between function calls. It tracks the number of moves made during the game.
5. Const Variables
- `const GameState& state` — The game state is passed by reference to `renderBoard()` and `checkWinner()` as a constant reference, meaning it cannot be modified in those functions.
- `const` is used to avoid unnecessary copying of large structures (like `GameState`) and ensure that the data remains unmodified inside the function.
6. Inline Functions <br />
- `inline void printCell(char cell)` — The `printCell()` function is defined as `inline` to suggest to the compiler that it should directly replace the function call with the function’s code to reduce overhead (useful for small functions like `printCell()` that are called frequently).
7. Preprocessor Directives <br />
- `#ifdef MULTIPLAYER` — The `MULTIPLAYER` preprocessor directive is used to distinguish between multiplayer and single-player modes. The game will allocate shared memory and use different logic based on whether `MULTIPLAYER` is defined.
- `#endif` — Ends the conditional directive block for multiplayer.
- `#include` — Includes external libraries, such as `iostream`, `stdlib.h`, and others, based on the game’s needs.
8. Overloading
Functions like `checkWinner()` and `playerTurn()` are flexible enough to be used for both single-player and multiplayer modes by handling different scenarios within the same function signature.
9. Conditional Statements
- Player role assignment (`if (playerRole != 'X' && playerRole != 'O')`)
- Move validation (`if (move >= 1 && move <= 9)`)
- Winner determination (`if (state.board[i][0] == state.board[i][1] && ...`)
10. Loop Constructs
- For Loop
  - `for (int i = 0; i < 3; ++i)` — Loops are used to iterate over the rows and columns of the Tic-Tac-Toe board to initialize the game state, render the board, and check for a winner.
- While Loop
  - `while (true)` — A loop that continues until the user makes a valid move or the game ends.
  - `while (state.gameOver == GameStatus::ONGOING)` — Keeps the game running until it’s over.
- Do-While Loop
  - Used in `playerTurn()` and `aiTurn()` to repeat the move prompt until a valid move is made.
11. Type Conversions
- Char to Integer
  - `state.board[i][j] = '0' + count++;` — Converts an integer to a character to fill the board with numbers ('1' to '9').
- Integer to Char
  - `state.board[row][col] = (player == Player::X) ? 'X' : 'O';` — Converts `Player` enum values into characters ('X' or 'O') for display on the board.
- Enum to Integer
  - In expressions like `(state.turn == Player::X)` and `(state.winner == Player::X)` where `Player::X` and `Player::O` are enum values that are implicitly converted to integers (0 and 1, respectively).
12. Enums
- Player Enum
  - `enum Player { X = 0, O = 1 };` — Used to represent the two players (X and O).
- CellStatus Enum
  - `enum CellStatus { EMPTY, X_MARK, O_MARK };` — This enum could be used (though it's not actively used in this code) to represent the state of each cell (whether it's empty or marked by X or O).
- GameStatus Enum
  - `enum GameStatus { ONGOING, DRAW, WIN };` — Represents the status of the game (whether the game is ongoing, has ended in a draw, or has a winner).
13. Struct
- GameState Struct
  - struct GameState { ... };` — A structure that holds the state of the game, including the board, turn, winner, and other necessary information. It organizes and encapsulates all game-related data.
14. Mutex
- `#ifdef MULTIPLAYER` — Shared memory is used in multiplayer mode to synchronize the game state between multiple processes.
- `shm_open`, `ftruncate`, and `mmap` are used to allocate and map shared memory, making the game state accessible across different instances of the game.

## Key Features
1. Support both single-player and multiplayer modes. For single-player mode, the user will be played against the AI.
2. The X and O has color. I am using red color for X and yellow color for O because both of them are safe for blindcolor people.
3. Thread-safe operations for multiplayer support.
4. Simple AI opponent that makes random moves. (I am planning to enhance the AI performance).
5. Shared memory implementation for multiplayer functionality.

## Main Components
1. Game State Management
Use a `GameState` struct to track the board, turns, and game status.
2. Player Management
Support both human and AI players.
3. Board Rendering
Displays a colored, formatted game board.
4. Move Validation
Ensure the moves are valid before applying them.
5. Win Detection
Check for winning conditions in rows, columns, and diagonals.

## Notable Implementation Details
1. Uses conditional compilation (`#ifdef Multiplayer`) to handle multiplayer-specific code.
2. Implements thread safety using mutexes for multiplayer mode.
3. Uses ANSI color codes for visual enhancement.
4. Provides a simple numbered interface (1-9) for move selection.

## Game Flow
1. Player selects game mode (single/multiplayer).
2. Player chooses their role (X or O).
3. Game alternates between players until win/draw.
4. Board updates after each move.
5. Automatic win/draw detection after each move.
