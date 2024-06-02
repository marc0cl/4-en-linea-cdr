#include "Board.h"

const char Board::EMPTY = '.';
const char Board::PLAYER1 = 'C';
const char Board::PLAYER2 = 'S';

Board::Board() : board(ROWS, std::vector<char>(COLS, EMPTY)) {}

bool Board::is_valid_move(int col) const {
    return board[0][col] == EMPTY;
}

bool Board::make_move(int col, char player) {
    if (!is_valid_move(col)) return false;
    for (int i = ROWS - 1; i >= 0; --i) {
        if (board[i][col] == EMPTY) {
            board[i][col] = player;
            return true;
        }
    }
    return false;
}

bool Board::check_winner(char player) const {
    // Check horizontal
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS - 3; ++j) {
            if (board[i][j] == player && board[i][j + 1] == player && board[i][j + 2] == player && board[i][j + 3] == player) {
                return true;
            }
        }
    }
    // Check vertical
    for (int i = 0; i < ROWS - 3; ++i) {
        for (int j = 0; j < COLS; ++j) {
            if (board[i][j] == player && board[i + 1][j] == player && board[i + 2][j] == player && board[i + 3][j] == player) {
                return true;
            }
        }
    }
    // Check diagonal (bottom left to top right)
    for (int i = 3; i < ROWS; ++i) {
        for (int j = 0; j < COLS - 3; ++j) {
            if (board[i][j] == player && board[i - 1][j + 1] == player && board[i - 2][j + 2] == player && board[i - 3][j + 3] == player) {
                return true;
            }
        }
    }
    // Check diagonal (top left to bottom right)
    for (int i = 0; i < ROWS - 3; ++i) {
        for (int j = 0; j < COLS - 3; ++j) {
            if (board[i][j] == player && board[i + 1][j + 1] == player && board[i + 2][j + 2] == player && board[i + 3][j + 3] == player) {
                return true;
            }
        }
    }
    return false;
}

bool Board::is_draw() const {
    for (int i = 0; i < COLS; ++i) {
        if (board[0][i] == EMPTY) return false;
    }
    return true;
}

std::string Board::to_string() const {
    std::string board_str;
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            board_str += board[i][j];
            board_str += ' ';
        }
        board_str += '\n';
    }
    return board_str;
}