/*
 *  Header information for each gamepiece. Structs handle all piece-level
 * operations. Board-level operations found in "board.h"
 */

#ifndef __piece_h
#define __piece_h

#include <string>
#include <list>
#include <tuple>

typedef std::pair<unsigned int, unsigned int> Square;
// Updated as of 9/11/23 to use tuple instead of pair, thus storing STARTING LOCATION, ENDING LOCATION, MOVE TYPE
typedef std::tuple<Square,Square,char> Move;

// Parent struct for all piece types.
struct Piece {
  Piece() : pieceType(' ') {}

  char getPieceType() const { return pieceType; }
  bool isPieceWhite() const { return isWhite; }
  int getPieceValue() const { return pieceValue; }
  const Square &getLocation() const { return location; }
  const std::list<Move> &getMoves() const { return moveList; }

  bool isNull() { return pieceType == ' '; }

  // Pawns and Kings require extra information of if they have been moved and
  // their last location. As such, this function is virtual as these said
  // classes need to update some of their variables.
  virtual void updateLocation(int r, int c) {
    location.first = r;
    location.second = c;
  }

  // Generates moves for this piece, stored in moveList. Said list can be easily
  // retrieved by a separate function.
  virtual bool setMoves(std::list<Move> &moveList);

  // For sets which store pieces
  bool operator<(const Piece& other) { return pieceType < other.pieceType; }

  char pieceType;
  int pieceValue;
  bool isWhite;

  // Locations stored in pair of row,column
  // Moves stored in pair of location (Square) and a character signaling some specific move types
  // Codes: N = Normal, X = Capture, E = En Passant, C = Castle (May add + for checks but unsure)
  Square location;
  // Lists used for rapid erase and insert
  std::list<Move> moveList;
};

struct Pawn : public Piece {
  Pawn(bool isWhite, int r, int c);
  void updateLocation(int r, int c);

  bool pieceMoved;
  bool pieceMovedTwice;
};

struct Knight : public Piece {

  Knight(bool isWhite, int r, int c);
};

struct Bishop : public Piece {

  Bishop(bool isWhite, int r, int c);
};

struct Rook : public Piece {

  Rook(bool isWhite, int r, int c);

  bool pieceMoved;
};

struct Queen : public Piece {

  Queen(bool isWhite, int r, int c);
};

struct King : public Piece {

  King(bool isWhite, int r, int c);
  bool isMoved() const { return pieceMoved; }
  void updateLocation(int r, int c);

  bool pieceMoved;
};

// USEFUL FUNCTIONS
inline bool isSameColor(const Piece& p1, const Piece& p2) { return p1.isWhite == p2.isWhite; }

#endif