/*
 *  Header information for board. Manages arrangement of pieces, their movements, and evaluation.
 */

#ifndef __board_h
#define __board_h

#include <list>
#include <string>
#include "piece.h"

typedef uint64_t Bitboard;

// START OF BOARD CLASS

class Board {
    public:
    Board();

    // Board-level functions for pieces
    void generateMoves(Piece* p);
    
    // Checks for special moves and conditions

    // Checks for special board conditions
    bool isCheck(bool isWhite);
    bool causesCheck(const Move& m);
    bool isStalemate(bool isWhite);

    // Funtional methods to facilitate move and updating the board
    bool validMove(const Square& loc1, const Square& loc2);
    bool move(const Square& loc1, const Square& loc2);
    void updateBoard();

    private:
    // Helper functions which need access to boardPieces
    void trimCheck();
    void addCastle(bool isWhite);
    void generateMovesPawn(unsigned int r, unsigned int c, Piece* p);
    void generateMovesKnight(unsigned int r, unsigned int c, Piece* p);
    void generateMovesBishop(unsigned int r, unsigned int c, Piece* p);
    void generateMovesRook(unsigned int r, unsigned int c, Piece* p);
    void generateMovesQueen(unsigned int r, unsigned int c, Piece* p);
    void generateMovesKing(unsigned int r, unsigned int c, Piece* p);
    void forwardMove(const Move& m);
    void reverseMove(const Move& m);

    Piece boardPieces[8][8];
    // Pointers and maps which keep immediate reference to important pieces
    // Lists used for rapid erase and insert
    std::list<Piece> whitePieces;
    std::list<Piece> blackPieces;

    Bitboard whiteAttack;
    Bitboard blackAttack;
    
    // NOTE: If no piece was captured last move, this is NULL. It holds whatever was in the square a piece was moved to
    Piece* lastCapturedPiece;

    King* whiteKing;
    King* blackKing;
    // Avoids redundancy wih updating board and pieces. Makes sure it's done only once between positions
    bool isUpdated;
};

#endif