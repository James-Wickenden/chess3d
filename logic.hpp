// logic.hpp

#pragma once

#include <vector>

namespace LogicEngine {

    enum class Colour {
        WHITE,
        BLACK,
        EMPTY
    };

    // Define a square. A square can be empty or occupied by a piece
    class Square
    {
    public:
        Colour colour;
        char piece;

        Square();
        Square(char piece_type, Colour colour);
    };


    // Define the board as a 2-d array of squares. 
    // Squares can be empty or occupied by a piece.
    class Chessboard
    {
    public:
        std::vector<std::vector<Square>> board;

        std::vector<Square> get_valid_moves();
        Chessboard();
    };
}