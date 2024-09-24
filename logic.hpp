// logic.hpp

#pragma once

#include <vector>
#include <map>
#include <iostream>

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

        Square();
        Square(Piece piece, Colour colour);
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