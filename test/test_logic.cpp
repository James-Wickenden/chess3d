#include <gtest/gtest.h>
#include "logic.cpp"

using namespace LogicEngine;
using namespace std;

TEST(ConvertIntToChessboardSquareTest, CornersAndStandardSquares)
{
    ASSERT_EQ(convert_int_to_chessboard_square(0, 0), "a1");
    ASSERT_EQ(convert_int_to_chessboard_square(7, 0), "h1");
    ASSERT_EQ(convert_int_to_chessboard_square(0, 7), "a8");
    ASSERT_EQ(convert_int_to_chessboard_square(7, 7), "h8");
    ASSERT_EQ(convert_int_to_chessboard_square(3, 3), "d4");
    ASSERT_EQ(convert_int_to_chessboard_square(4, 1), "e2");
}

TEST(ConvertChessboardSquareToIntTest, BottomLeftCorner)
{
    vector<int> result = convert_chessboard_square_to_int("a1");
    ASSERT_EQ(result.size(), 2);
    ASSERT_EQ(result[0], 0);  // row
    ASSERT_EQ(result[1], 0);  // col
}

TEST(ConvertChessboardSquareToIntTest, TopLeftCorner)
{
    vector<int> result = convert_chessboard_square_to_int("a8");
    ASSERT_EQ(result.size(), 2);
    ASSERT_EQ(result[0], 7);  // row
    ASSERT_EQ(result[1], 0);  // col
}

TEST(GetPieceNotationMapTest, SquaresPieceToChar)
{
    ASSERT_EQ(get_piece_notation_map(Piece::EMPTY), " ");
    ASSERT_EQ(get_piece_notation_map(Piece::PAWN), "");
    ASSERT_EQ(get_piece_notation_map(Piece::ROOK), "R");
    ASSERT_EQ(get_piece_notation_map(Piece::KNIGHT), "N");
    ASSERT_EQ(get_piece_notation_map(Piece::BISHOP), "B");
    ASSERT_EQ(get_piece_notation_map(Piece::QUEEN), "Q");
    ASSERT_EQ(get_piece_notation_map(Piece::KING), "K");
}

// Roundtrip tests: convert int -> square -> int
TEST(RoundtripTests, IntToSquareToInt)
{
	ASSERT_EQ(convert_chessboard_square_to_int("a1"), vector<int>({ 0, 0 }));
    ASSERT_EQ(convert_chessboard_square_to_int("h1"), vector<int>({ 0, 7 }));
    ASSERT_EQ(convert_chessboard_square_to_int("a8"), vector<int>({ 7, 0 }));
    ASSERT_EQ(convert_chessboard_square_to_int("h8"), vector<int>({ 7, 7 }));
    ASSERT_EQ(convert_chessboard_square_to_int("d4"), vector<int>({ 3, 3 }));
    ASSERT_EQ(convert_chessboard_square_to_int("e2"), vector<int>({ 1, 4 }));
}