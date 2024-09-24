# chess3d

Creating a 3D chess program to learn openGL and dive back into working with C++.

https://github.com/James-Wickenden/chess3d

---

## Todo:


- Parse model OBJs and MTLs into 3d space with openGL
- Add camera controls w tutorial help with mouse/arrow key movement
- Create the game logic for chess, based on building the incremental steps of a gameplay session:
	- Moving pieces
	- Finding all valid moves given a piece/board
	- Detecting check
	- Capturing pieces
	- Storing moves as notation
	- Advanced moves: en passant, castling
	- Detecting checkmate
- Undoing moves with a gamestate stack
- Adding a PGN to the game window
- Saving/loading games
- Sound effects!
- Playing w a bot by API?

## In progress:

- Create the game logic for chess, based on building the incremental steps of a gameplay session:
	- Pieces
	- Defining a board as a structure of pieces and empty squares
- Create low poly 3d chess models

## Completed:

- Create the game logic for chess, based on building the incremental steps of a gameplay session:
	- Board initiating

---

## On discovering valid moves for a piece:

Pieces in chess move differently, and some have particular move rules which require additional cases.

The board is divided into horizontal *files* and vertical *ranks*.

- Pawns move one square forward unless on their starting file, in which case they can move two squares forward.
A pawn cannot capture in front but can capture diagonally.

- Rooks move horizontally and vertically as far as they like along the rank and file they stand on.

- Bishops can move diagonally as far as they like.

- The queen is a combination of a rook and a bishop.

- Knights move in an 'L' shape, two squares in one direction and one square in the perpendicular direction. 

- The king can move to any adjacent square.

- If a king and rook are both unmoved and stand on their file with no pieces between them, they can *castle*; that is, switch sides, 
allowing the king to move either two (kingside) or three (queenside) squares in one move. Castling cannot occur through check-
that is, if the king or any of the squares between the king and rook are targeted by an enemy piece.

- If a pawn has moved three squares forward, and an enemy pawn moves two squares forward from its starting file in one move to land adjacent to the pawn,
the pawn can move diagonally forward and capture the enemy pawn. This is *en passant*.
En passant can only happen the move immediately following the enemy pawn moving.

- Furthermore, a piece cannot move if doing so would allow the king to be captured by an enemy piece.

So, the piece moving logic must be designed as follows.

1. A piece is selected by the player. We know the piece's rank and file index and the state of the whole board.

2. There are different algorithms for each piece depending on how it moves. 
We first build a vector of *prospective* moves. These are the moves that a piece could make if we do not worry about putting our king in check.

- If pawn, look at the square in front of the piece. If its empty, it can move there. 
If either of the forward diagonals are occupied by an enemy piece, it can move there too.

- If rook, then start at the piece and iterate along the rank until an occupied square is found. 
For each square, also 
Every free square between the rook and the occupied square can be moved to.
If the occupied square is an enemy piece, that square can also be moved to.
Then iterate along the rank in the opposite direction, and repeat this for the file.

- If bishop, perform a similar procedure as for the rook but along the diagonals.

- If queen, combine the procedures for the rook and bishop.

- If knight, any combination of +/-1 square horizonally/vertically and +/-2 squares vertically/horizontally are possible, 
so long as the destination square is on the board.

- If king, the empty or enemy squares adjacent to the king are all possible.
A separate check for if the castling logic is permissible should be done here.

3. Next, the list of prospective moves should be examined and reworked into a subset of these moves for *legal*- actually playable- moves.

- Assume the piece can make the move. Create a copy of the board where the prospective move under test has been made.

- Run the prospective move making algorithm on every enemy piece and collate the results.
If the king is targeted by any enemy piece, then making the prospective move would jeoparise the king and should be discarded.
