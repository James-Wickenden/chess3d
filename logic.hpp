// logic.hpp

#pragma once
#pragma warning( disable : 26451 )

#include <vector>
#include <map>
#include <iostream>
#include <string>
#include <algorithm>
#include <tuple>
#include <fstream>

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
        std::map<Colour, std::vector<std::tuple<Square, std::vector<Square>>>> valid_moves;
        Colour active_player;

        std::vector<Square> find_valid_moves(Square target);

        Chessboard(std::string start_position);
        Chessboard() : Chessboard("starting_position.txt") {};
    };

    std::vector<std::tuple<Square, std::vector<Square>>> 
        find_all_attackable_squares(std::vector<std::vector<Square>> board, Colour colour, bool assume_king);
}
