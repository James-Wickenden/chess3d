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
#include <stack>
#include <filesystem>
#include <ctime>

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

    enum class Gamestate
    {
        NORMAL,
        CHECK,
        CHECKMATE,
        STALEMATE,
        NEWGAME
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
        std::vector<int> when_moved; // List of move numbers when that piece was moved
        bool operator==(const Square rhs) const;
        bool operator!=(const Square rhs) const;

        Square();
        Square(int row, int col);
        Square(Piece p, Colour c, int i, int j, bool h_m, std::vector<int> w_m);
    };

    // Define the board as a 2-d array of squares. 
    // Squares can be empty or occupied by a piece.
    // Also maintains metadata for the game:
    //    The game notation, move nuumber, active player, black and white player names, and the result.
    class Chessboard
    {
    public:
        std::map<char, std::tuple<Piece, Colour>> piece_map = {
            { '_', {Piece::EMPTY,  Colour::EMPTY }},
            { 'P', {Piece::PAWN,   Colour::WHITE }},
            { 'R', {Piece::ROOK,   Colour::WHITE }},
            { 'N', {Piece::KNIGHT, Colour::WHITE }},
            { 'B', {Piece::BISHOP, Colour::WHITE }},
            { 'Q', {Piece::QUEEN,  Colour::WHITE }},
            { 'K', {Piece::KING,   Colour::WHITE }},
            { 'p', {Piece::PAWN,   Colour::BLACK }},
            { 'r', {Piece::ROOK,   Colour::BLACK }},
            { 'n', {Piece::KNIGHT, Colour::BLACK }},
            { 'b', {Piece::BISHOP, Colour::BLACK }},
            { 'q', {Piece::QUEEN,  Colour::BLACK }},
            { 'k', {Piece::KING,   Colour::BLACK }}
        };

        std::vector<std::vector<Square>> board;
        std::map<Colour, std::vector<std::tuple<Square, std::vector<Square>>>> valid_moves;
        std::map<Colour, std::vector<std::tuple<Square, std::vector<Square>>>> attacking_moves;
        Colour active_player;
        int move_no;
        std::string notation, white_name, black_name, date, result;

        std::vector<Square> find_valid_moves(Square target);

        Chessboard(std::string start_position);
        Chessboard() : Chessboard("starting_position.txt") {};
    };

    std::vector<std::tuple<Square, std::vector<Square>>> 
        find_all_attackable_squares(Chessboard chessboard, Colour colour, int mode);
}
