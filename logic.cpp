// logic.cpp

#include "logic.hpp"

using namespace std;

// chessboard constructor
//chessboard::chessboard()
//{
//	/* Generate a new board that should look as follows :
//	
//	RNBQKBNR
//	PPPPPPPP
//	________
//	________
//	________
//	________
//	PPPPPPPP
//	RNBQKBNR
//
//	*/
//
//	const int DIM_SIZE = 8;
//
//	vector<piece> row(DIM_SIZE, '_');
//	vector<vector<piece>> board(DIM_SIZE, row);
//
//	for (int i = 0; i < 8; i++)
//	{
//		board[i][3].empty = true;
//	}
//
//}

Square::Square()
{
	colour = Colour::EMPTY;
	piece = '_';
}

Square::Square(char piece_type, Colour c)
{
	colour = c;
	piece = piece_type;
}


int main()
{
	const int DIM_SIZE = 8;

	vector<Square> row(DIM_SIZE);
	vector<vector<Square>> board(DIM_SIZE, row);

	board[0][0].colour = Colour::WHITE;

}