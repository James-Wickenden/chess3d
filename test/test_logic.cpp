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

TEST(GetPieceNotationMapTest, SquaresPieceToChar)
{
    ASSERT_EQ(get_piece_notation_map(Piece::EMPTY),  " ");
    ASSERT_EQ(get_piece_notation_map(Piece::PAWN),   "");
    ASSERT_EQ(get_piece_notation_map(Piece::ROOK),   "R");
    ASSERT_EQ(get_piece_notation_map(Piece::KNIGHT), "N");
    ASSERT_EQ(get_piece_notation_map(Piece::BISHOP), "B");
    ASSERT_EQ(get_piece_notation_map(Piece::QUEEN),  "Q");
    ASSERT_EQ(get_piece_notation_map(Piece::KING),   "K");
}

TEST(RoundtripTests, IntToSquareToInt)
{
	ASSERT_EQ(convert_chessboard_square_to_int("a1"), vector<int>({ 0, 0 }));
    ASSERT_EQ(convert_chessboard_square_to_int("h1"), vector<int>({ 0, 7 }));
    ASSERT_EQ(convert_chessboard_square_to_int("a8"), vector<int>({ 7, 0 }));
    ASSERT_EQ(convert_chessboard_square_to_int("h8"), vector<int>({ 7, 7 }));
    ASSERT_EQ(convert_chessboard_square_to_int("d4"), vector<int>({ 3, 3 }));
    ASSERT_EQ(convert_chessboard_square_to_int("e2"), vector<int>({ 1, 4 }));
}

TEST(DeepCloneBoardTest, AssertDeepClonedBoardHasCorrectMetadata)
{
	Chessboard original_board("positions/test_ep.txt");

    vector<vector<Square>> cloned_board = deep_clone_board(original_board.board);
    
    for (int row = 0; row < DIM_SIZE; row++)
    {
        for (int col = 0; col < DIM_SIZE; col++)
        {
            ASSERT_EQ(cloned_board[row][col].row,        original_board.board[row][col].row);
            ASSERT_EQ(cloned_board[row][col].col,        original_board.board[row][col].col);
            ASSERT_EQ(cloned_board[row][col].piece,      original_board.board[row][col].piece);
            ASSERT_EQ(cloned_board[row][col].colour,     original_board.board[row][col].colour);
            ASSERT_EQ(cloned_board[row][col].has_moved,  original_board.board[row][col].has_moved);
            ASSERT_EQ(cloned_board[row][col].when_moved, original_board.board[row][col].when_moved);
        }
	}
}

TEST(SwitchPiecesTest, AssertSwitchingPiecesCorrectness)
{
	Chessboard test_board("positions/starting_position.txt");

	// Move white pawn from e2 to e4
    switch_pieces(&test_board, { 1, 4 }, { 3, 4 }); 

	// Check that the destination square has the correct piece, colour and metadata
	ASSERT_EQ(test_board.board[3][4].piece, Piece::PAWN);
    ASSERT_EQ(test_board.board[3][4].colour, Colour::WHITE);
	ASSERT_TRUE(test_board.board[3][4].has_moved);
	ASSERT_EQ(test_board.board[3][4].when_moved.back(), test_board.move_no);

	// Check that the original square is now empty
    ASSERT_EQ(test_board.board[1][4].piece, Piece::EMPTY);
    ASSERT_EQ(test_board.board[1][4].colour, Colour::EMPTY);
    ASSERT_TRUE(test_board.board[1][4].has_moved);
	ASSERT_TRUE(test_board.board[1][4].when_moved.empty());
}

TEST(CheckmateStalemateTest, DetectStalemateCorrectly)
{
    // Not initially in stalemate as there is a black pawn move
    Chessboard stalemate_board("positions/test_stalemate.txt");
    get_valid_and_attacking_moves(&stalemate_board);
    ASSERT_FALSE(test_for_checkmate_stalemate(&stalemate_board, Colour::BLACK, Colour::WHITE));

	// Remove the g2 pawn and check for stalemate again
    int p_row = 1, p_col = 6;
    stalemate_board.board[p_row][p_col] = Square(p_row, p_col);
    get_valid_and_attacking_moves(&stalemate_board);
	ASSERT_TRUE(test_for_checkmate_stalemate(&stalemate_board, Colour::BLACK, Colour::WHITE));
}

TEST(CheckmateStalemateTest, DetectCheckmateCorrectly)
{
    // Not initially in checkmate as there is a black pawn in the way
    Chessboard checkmate_board("positions/test_checkmate.txt");
    get_valid_and_attacking_moves(&checkmate_board);
    ASSERT_FALSE(test_for_checkmate_stalemate(&checkmate_board, Colour::BLACK, Colour::WHITE));
    
    // Remove the f8 pawn and check for checkmate again
    int p_row = 7, p_col = 5;
    checkmate_board.board[p_row][p_col] = Square(p_row, p_col);
    get_valid_and_attacking_moves(&checkmate_board);
	ASSERT_TRUE(test_for_checkmate_stalemate(&checkmate_board, Colour::BLACK, Colour::WHITE));
}

TEST(GetPlyNotationTest, AssertSimplePlyNotationCorrectness)
{
	// Test a simple pawn move from e2 to e4
    Chessboard test_board("positions/starting_position.txt");
	vector<int> target_position = { 1, 4 }, destination_position = { 3, 4 };
    switch_pieces(&test_board, target_position, destination_position);
    string ply_notation = get_ply_notation(&test_board, target_position, destination_position, false);
	ASSERT_EQ(ply_notation, "e4");
}

Chessboard generate_notation_test_board_and_capture(vector<int> target_position, vector<int> destination_position)
{
    Chessboard test_board("positions/test_notation.txt");
    get_valid_and_attacking_moves(&test_board);
    switch_pieces(&test_board, target_position, destination_position);
	return test_board;
}

TEST(GetPlyNotationTest, AssertCapturePlyNotationCorrectness)
{
    Chessboard test_board;
	vector<int> target_position, destination_position;
    string ply_notation;

    // Move white pawn from h6 to g7 to capture black rook
    target_position = { 5, 7 }, destination_position = { 6, 6 };
    test_board = generate_notation_test_board_and_capture(target_position, destination_position);
    ply_notation = get_ply_notation(&test_board, target_position, destination_position, true);
    ASSERT_EQ(ply_notation, "hxg7");

    // Move white pawn from f6 to g7 to capture black rook
    target_position = { 5, 5 }, destination_position = { 6, 6 };
    test_board = generate_notation_test_board_and_capture(target_position, destination_position);
    ply_notation = get_ply_notation(&test_board, target_position, destination_position, true);
    ASSERT_EQ(ply_notation, "fxg7");

    // Move white bishop from b3 to c4 to capture black rook
    target_position = { 2, 1 }, destination_position = { 3, 2 };
    test_board = generate_notation_test_board_and_capture(target_position, destination_position);
    ply_notation = get_ply_notation(&test_board, target_position, destination_position, true);
    ASSERT_EQ(ply_notation, "B3xc4");

    // Move white bishop from b5 to c4 to capture black rook
    target_position = { 4, 1 }, destination_position = { 3, 2 };
    test_board = generate_notation_test_board_and_capture(target_position, destination_position);
    ply_notation = get_ply_notation(&test_board, target_position, destination_position, true);
    ASSERT_EQ(ply_notation, "Bb5xc4");

    // Move white bishop from d5 to c4 to capture black rook
    target_position = { 4, 3 }, destination_position = { 3, 2 };
    test_board = generate_notation_test_board_and_capture(target_position, destination_position);
    ply_notation = get_ply_notation(&test_board, target_position, destination_position, true);
    ASSERT_EQ(ply_notation, "Bdxc4");

    // Move white knight from e5 to c4 to capture black rook
    target_position = { 4, 4 }, destination_position = { 3, 2 };
    test_board = generate_notation_test_board_and_capture(target_position, destination_position);
    ply_notation = get_ply_notation(&test_board, target_position, destination_position, true);
    ASSERT_EQ(ply_notation, "Ne5xc4");

    // Move white knight from e3 to c4 to capture black rook
    target_position = { 2, 4 }, destination_position = { 3, 2 };
    test_board = generate_notation_test_board_and_capture(target_position, destination_position);
    ply_notation = get_ply_notation(&test_board, target_position, destination_position, true);
    ASSERT_EQ(ply_notation, "Ne3xc4");

    // Move white knight from b2 to c4 to capture black rook
    target_position = { 1, 1 }, destination_position = { 3, 2 };
    test_board = generate_notation_test_board_and_capture(target_position, destination_position);
    ply_notation = get_ply_notation(&test_board, target_position, destination_position, true);
    ASSERT_EQ(ply_notation, "Nbxc4");
}
