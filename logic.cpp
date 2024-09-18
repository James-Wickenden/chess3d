// logic.cpp

#include "logic.hpp"

using namespace std;
using namespace LogicEngine;

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

vector<Square> Chessboard::get_valid_moves()
{
	return vector<Square>();
}


Chessboard::Chessboard()
{
	/* Generate a new board that should look as follows :
	
	RNBQKBNR
	PPPPPPPP
	________
	________
	________
	________
	PPPPPPPP
	RNBQKBNR

	*/

	const int DIM_SIZE = 8;

	vector<Square> row(DIM_SIZE);
	vector<vector<Square>> b(DIM_SIZE, row);

	board = b;
}


int main()
{
	Chessboard c = Chessboard();
	c.board[0][0].colour = Colour::WHITE;
}