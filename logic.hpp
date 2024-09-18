// logic.hpp

#ifndef LOGIC // include guard
#define LOGIC

#include <vector>

namespace LOGIC {

    enum class Colour {
        WHITE,
        BLACK,
        EMPTY
    };

    // Define the board as a 2-d array of squares. 
    // Squares can be empty or occupied by a piece.
    //class Chessboard
    //{
    //public:
    //    std::vector<std::vector<piece>> board;
    //
    //    Chessboard();
    //
    //};


    // Define a square. A square can be empty or occupied by a piece
    class Square
    {
    public:
        Colour colour;
        char piece;

        Square();
        Square(char piece_type, Colour colour);

        Square get_valid_moves();
    };

}

#endif /* LOGIC */
