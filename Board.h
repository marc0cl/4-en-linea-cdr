#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <string>

class Board {
public:
    static const int ROWS = 6;
    static const int COLS = 7;
    static const char EMPTY;
    static const char PLAYER1;
    static const char PLAYER2;

    Board();
    bool is_valid_move(int col) const;
    bool make_move(int col, char player);
    bool check_winner(char player) const;
    bool is_draw() const;
    std::string to_string() const;

private:
    std::vector<std::vector<char>> board;
};

#endif // BOARD_H
