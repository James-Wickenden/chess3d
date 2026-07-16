#include <gtest/gtest.h>
#include "logic.hpp"
#include "file_handler.hpp"

using namespace LogicEngine;
using namespace ConsoleEngine;
using namespace std;
using namespace FileHandler;
namespace fs = std::filesystem;

TEST(ReadBoardSetupFileTest, ReadFileContentsCorrectly) {
	string expected = 
		"rnbqkbnr"
		"pppppppp"
		"________"
		"________"
		"________"
		"________"
		"PPPPPPPP"
		"RNBQKBNR";
	string actual = read_board_setup_file("positions/starting_position.txt");

	ASSERT_EQ(actual, expected);
}

TEST(LoadGameTest, CorrectlyLoadGameAndParsePgn) {
	fs::path gamepath = fs::current_path().append("positions").append("example.pgn");

	// Redirect cout to stringstream buffer
	streambuf* sbuf = std::cout.rdbuf();
	stringstream cout_buffer;
	cout.rdbuf(cout_buffer.rdbuf());

	// Simulate user input for "exit" to avoid waiting for user input during the test
	string input = "exit\n";
	istringstream iss(input);
	cin.rdbuf(iss.rdbuf());

	load_game(gamepath);

	// Test for the following expected output lines in the console output
	vector<string> expected_lines = {
		"Number of valid moves: 32",
		"PGN : 1.e4 d5 2.exd5 Nf6 3.Qh5 Na6 4.Qxf7+ Kxf7 5.a4 Nb4 6.a5 Nbxd5 7.b3 b5 8.b6ep axb6",
		"WHITE TO MOVE",
		"White_P vs Black_P"
	};
	for (const auto& expected_line : expected_lines) {
		ASSERT_TRUE(cout_buffer.str().find(expected_line) != std::string::npos);
	}

	// When done redirect cout to its old self
	cout_buffer.str("");
	cout.rdbuf(sbuf);
}