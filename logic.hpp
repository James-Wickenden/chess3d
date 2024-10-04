// logic.hpp

#pragma once
#pragma warning( disable : 26451 )

#include <vector>
#include <map>
#include <iostream>
#include <string>
#include <algorithm>
#include <tuple>

namespace LogicEngine 
{
    enum class Colour 
    {
        WHITE,
        BLACK,
        EMPTY
    };

    enum class Piece 
    {
        EMPTY,
        PAWN,
        ROOK,
        KNIGHT,
        BISHOP,
        QUEEN,
        KING
    };

    // Define a square. A square can be empty or occupied by a piece
    class Square
    {
    public:
        Colour colour;
        Piece piece;
        int row;
        int col;
        bool has_moved;
        bool operator==(const Square rhs) const;
        bool operator!=(const Square rhs) const;

        Square();
        Square(int row, int col);
        Square(Piece piece, Colour colour);
    };


    // Define the board as a 2-d array of squares. 
    // Squares can be empty or occupied by a piece.
    class Chessboard
    {
    public:
        std::vector<std::vector<Square>> board;

        Square selected_square;
        std::vector<Square> valid_moves;

        std::vector<Square> find_valid_moves(Square target);
        Chessboard();
    };
}