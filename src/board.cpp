#include "board.h"
#include <list>
#include <string>

// BOARD-ONLY FUNCTIONS START HERE

Board::Board() {
    // Initial "power" pieces have to be manually assigned
    boardPieces[0][0] = Rook(false, 0, 0);
    boardPieces[7][0] = Rook(true, 7, 0);
    boardPieces[0][1] = Knight(false, 0, 1);
    boardPieces[7][1] = Knight(true, 7, 1);
    boardPieces[0][2] = Bishop(false, 0, 2);
    boardPieces[7][2] = Bishop(true, 7, 2);
    boardPieces[0][3] = Queen(false, 0, 3);
    boardPieces[7][3] = Queen(true, 7, 3);
    boardPieces[0][4] = King(false, 0, 4);
    boardPieces[7][4] = King(true, 7, 4);
    boardPieces[0][5] = Bishop(false, 0, 5);
    boardPieces[7][5] = Bishop(true, 7, 5);
    boardPieces[0][6] = Knight(false, 0, 6);
    boardPieces[7][6] = Knight(true, 7, 6);
    boardPieces[0][7] = Rook(false, 0, 7);
    boardPieces[7][7] = Rook(true, 7, 7);
    // Loop assign for pawns
    for(unsigned int i = 0; i < 7; i++) {
        boardPieces[1][i] = Pawn(false, 1, i);
        boardPieces[6][i] = Pawn(true, 6, i);
    }
    // Add all above pieces to sets of black X white pieces
    for(unsigned int i = 0; i < 3; i++) {
        for(unsigned int j = 0; j < 8; j++) {
            blackPieces.push_back(boardPieces[i][j]);
            whitePieces.push_back(boardPieces[i+6][j]);
        }
    }

    isUpdated = true;
    whiteAttack = blackAttack = 0x0000000000000000;
}

// Adds a single bit to a bitboard
void bitboardAdd(unsigned int r, unsigned int c, Bitboard& b) {
    // Shift a "1" bit to the correct position
    Bitboard temp = 0x0000000000000001 << (r*8 + c);
    // Include all bits in b plus the new bit
    b = b | temp;
}

// Checks for the existance of a single bit in a bitboard
bool bitboardHas(unsigned int r, unsigned c, Bitboard& b) {
    // Shifts a "1" bit to the correct position
    Bitboard temp = 0x0000000000000001 << (r*8 + c);
    // Returns 0 for all bits except the correct one, which will return 1 if it's 1 too
    temp &= b;
    // Check that the integers are equal
    return b == temp;
}

void Board::addCastle(bool isWhite) {
    // Bitboard of enemy moves for checking for check and enemy sight
    Bitboard b = (isWhite) ? blackAttack : whiteAttack;

    King* k  = (isWhite) ? whiteKing : blackKing;
    unsigned int r = k->location.first;
    unsigned int c = k->location.second;
    // Make sure the king is not in check
    if(bitboardHas(r,c,b)) {
        return;
    }
    // Check that the king has not moved
    if(k->pieceMoved) {
        return;
    }
    // Check vacancy around King and that corresponding rooks have not been moved
    bool queensideOpen = false;
    bool kingsideOpen = false;
    if(boardPieces[r][c-1].isNull() && boardPieces[r][c-2].isNull() && boardPieces[r][c-3].isNull()) {
        if(!boardPieces[r][0].isNull() && boardPieces[r][0].pieceType == 'r') {
            Rook* rook = (Rook*)(&boardPieces[r][0]);
            if(!rook->pieceMoved) {
                queensideOpen = true;
            }
        }
    }
    if(boardPieces[r][c+1].isNull() && boardPieces[r][c+2].isNull()) {
        if(!boardPieces[r][7].isNull() && boardPieces[r][7].pieceType == 'r') {
            Rook* rook = (Rook*)(&boardPieces[r][7]);
            if(!rook->pieceMoved) {
                queensideOpen = true;
            }
        }
    }
    // Ensure no spots inbetween are in the enemy's vision, including us (check)
    for(unsigned int i = 1; i < 4 && queensideOpen; i++) {
        if(bitboardHas(r,c-i,b)) {
            queensideOpen = false;
            break;
        }
    }
    for(unsigned int i = 1; i < 3 && kingsideOpen; i++) {
        if(bitboardHas(r,c+i,b)) {
            kingsideOpen = false;
            break;
        }
    }
    // Add either pending which is "true"
    if(queensideOpen) {
        Move m = std::make_tuple(std::make_pair(r,c),std::make_pair(r, c-2),'C');
        k->moveList.push_back(m);
    }
    if(kingsideOpen) {
        Move m = std::make_tuple(std::make_pair(r,c),std::make_pair(r, c+2),'C');
        k->moveList.push_back(m);
    }
}

// Utility functions to help with generateMoves for each piece
// PIECE-SPECIFIC generateMoves do not account for checks. This is because checks require us to have 
// generated the moves of the other pieces, i.e. circular dependency. Checks are done after all moves 
// have been generated.

// Functions kept general with Piece* pointers rather than their specific types as it avoids the requirement of certain casts
void Board::generateMovesPawn(unsigned int r, unsigned int c, Piece* p) {
    // Select correct bitboard of attacking moves, add each move to bitboard
    Bitboard b = (p->isWhite) ? whiteAttack : blackAttack;
    // Downcast from Piece to Pawn
    Pawn* pawn_p = (Pawn*)p;
    // Moving 1 or 2 squares forward
    int i = (pawn_p->isWhite) ? -1 : 1;
    if(boardPieces[r+i][c].isNull()) {
        Move m = std::make_tuple(std::make_pair(r,c),std::make_pair(r + i, c),'N');
        pawn_p->moveList.push_back(m);
    }
    if(boardPieces[r+2*i][c].isNull() && !pawn_p->pieceMoved) {
        Move m = std::make_tuple(std::make_pair(r,c),std::make_pair(r + 2*i, c),'N');
        pawn_p->moveList.push_back(m);
    }

    // Diagonal moves are the only ones added to whiteAttack and blackAttack bitboards as they are the only 
    // moves we can capture. They are always added.
    if(c - 1 >= 0) {
        bitboardAdd(r+i,c-1,b);
    }
    if(c + 1 < 8) {
        bitboardAdd(r+i,c+1,b);
    }

    // Capturing pieces diagonally
    if(c - 1 >= 0 && !boardPieces[r+i][c-1].isNull() && !isSameColor(*p,boardPieces[r+i][c-1])) {
        Move m = std::make_tuple(std::make_pair(r,c),std::make_pair(r + i, c - 1),'X');
        p->moveList.push_back(m);
    }
    if(c + 1 < 8 && !boardPieces[r+i][c+1].isNull() && !isSameColor(*p,boardPieces[r+i][c+1])) {
        Move m = std::make_tuple(std::make_pair(r,c),std::make_pair(r + i, c + 1),'X');
        p->moveList.push_back(m);
    }

    // En Passant
    // When the opposing pawn moves +2 you can take diagonally.
    // Logic: Check if opposing color pawn is next to us L/R. Check if either was moved +2 last move.

    if(c - 1 >= 0 && !boardPieces[r][c-1].isNull() && !isSameColor(*p,boardPieces[r][c-1]) && boardPieces[r][c-1].pieceType == 'p') {
        // For a pawn of the opposite color on the left of the current piece:
        pawn_p = (Pawn*)&boardPieces[r][c-1];
        // Further check that this adjacent pawn was moved +2 last turn.
        if((r == 3 || r == 4) && pawn_p->pieceMoved && !pawn_p->pieceMovedTwice) {
            Move m = std::make_tuple(std::make_pair(r,c),std::make_pair(r + i,c - 1),'E');
            p->moveList.push_back(m);
        }
    }
    if(c + 1 < 8 && !boardPieces[r][c+1].isNull() && !isSameColor(*p,boardPieces[r][c+1]) && boardPieces[r][c+1].pieceType == 'p') {
        // For a pawn of the opposite color on the left of the current piece:
        pawn_p = (Pawn*)&boardPieces[r][c+1];
        // Further check that this adjacent pawn was moved +2 last turn.
        if((r == 3 || r == 4) && pawn_p->pieceMoved && !pawn_p->pieceMovedTwice) {
            Move m = std::make_tuple(std::make_pair(r,c),std::make_pair(r + i,c + 1),'E');
            p->moveList.push_back(m);
        }
    }
}

void Board::generateMovesKnight(unsigned int r, unsigned int c, Piece* p) {
    // Select correct bitboard of attacking moves, add each move to bitboard
    Bitboard b = (p->isWhite) ? whiteAttack : blackAttack;
    // Check bounds. The spot to move to must either be empty or the opposite color (capture)
    if(r - 2 >= 0 && c - 1 >= 0 && (boardPieces[r-2][c-1].isNull() || !isSameColor(*p,boardPieces[r-2][c-1]))) {
        Square loc = std::make_pair(r - 2, c - 1);
        Move m = (p->isNull()) ? std::make_tuple(std::make_pair(r,c),loc,'N') : std::make_tuple(std::make_pair(r,c),loc,'X');
        p->moveList.push_back(m);
        bitboardAdd(r-2,c-1,b);
    }
    if(r - 2 >= 0 && c + 1 < 8 && (boardPieces[r-2][c+1].isNull() || !isSameColor(*p,boardPieces[r-2][c+1]))) {
        Square loc = std::make_pair(r - 2, c + 1);
        Move m = (p->isNull()) ? std::make_tuple(std::make_pair(r,c),loc,'N') : std::make_tuple(std::make_pair(r,c),loc,'X');
        p->moveList.push_back(m);
        bitboardAdd(r-2,c+1,b);
    }
    if(r + 2 < 8 && c + 1 < 8 && (boardPieces[r+2][c+1].isNull() || !isSameColor(*p,boardPieces[r+2][c+1]))) {
        Square loc = std::make_pair(r + 2, c + 1);
        Move m = (p->isNull()) ? std::make_tuple(std::make_pair(r,c),loc,'N') : std::make_tuple(std::make_pair(r,c),loc,'X');
        p->moveList.push_back(m);
        bitboardAdd(r+2,c+1,b);
    }
    if(r + 2 < 8 && c - 1 >= 0 && (boardPieces[r+2][c-1].isNull() || !isSameColor(*p,boardPieces[r+2][c-1]))) {
        Square loc = std::make_pair(r + 2, c - 1);
        Move m = (p->isNull()) ? std::make_tuple(std::make_pair(r,c),loc,'N') : std::make_tuple(std::make_pair(r,c),loc,'X');
        p->moveList.push_back(m);
        bitboardAdd(r+2,c-1,b);
    }
    if(r - 1 >= 0 && c - 2 >= 0 && (boardPieces[r-1][c-2].isNull() || !isSameColor(*p,boardPieces[r-1][c-2]))) {
        Square loc = std::make_pair(r - 1, c - 2);
        Move m = (p->isNull()) ? std::make_tuple(std::make_pair(r,c),loc,'N') : std::make_tuple(std::make_pair(r,c),loc,'X');
        p->moveList.push_back(m);
        bitboardAdd(r-1,c-2,b);
    }
    if(r - 1 >= 0 && c + 2 < 8 && (boardPieces[r-1][c+2].isNull() || !isSameColor(*p,boardPieces[r-1][c+2]))) {
        Square loc = std::make_pair(r - 1, c + 2);
        Move m = (p->isNull()) ? std::make_tuple(std::make_pair(r,c),loc,'N') : std::make_tuple(std::make_pair(r,c),loc,'X');
        p->moveList.push_back(m);
        bitboardAdd(r-1,c+2,b);
    }
    if(r + 1 < 8 && c + 2 < 8 && (boardPieces[r+1][c+2].isNull() || !isSameColor(*p,boardPieces[r+1][c+2]))) {
        Square loc = std::make_pair(r + 1, c + 2);
        Move m = (p->isNull()) ? std::make_tuple(std::make_pair(r,c),loc,'N') : std::make_tuple(std::make_pair(r,c),loc,'X');
        p->moveList.push_back(m);
        bitboardAdd(r+1,c+2,b);
    }
    if(r + 1 < 8 && c - 2 >= 0 && (boardPieces[r+1][c-2].isNull() || !isSameColor(*p,boardPieces[r+1][c-2]))) {
        Square loc = std::make_pair(r + 1, c - 2);
        Move m = (p->isNull()) ? std::make_tuple(std::make_pair(r,c),loc,'N') : std::make_tuple(std::make_pair(r,c),loc,'X');
        p->moveList.push_back(m);
        bitboardAdd(r+1,c-2,b);
    }
}

void Board::generateMovesBishop(unsigned int r, unsigned int c, Piece* p) {
    // Select correct bitboard of attacking moves, add each move to bitboard
    Bitboard b = (p->isWhite) ? whiteAttack : blackAttack;
    // Northwest Loop
    for(unsigned int i = 1; r - i >= 0 && c - i >= 0; i++) {
        if(boardPieces[r-i][c-i].isNull()) {
            Move m = std::make_tuple(std::make_pair(r,c),std::make_pair(r - i, c - i),'N');
            p->moveList.push_back(m);
            bitboardAdd(r-i,c-i,b);
        } else if(!isSameColor(*p,boardPieces[r-i][c-i])) {
            Move m = std::make_tuple(std::make_pair(r,c),std::make_pair(r - i, c - i),'X');
            p->moveList.push_back(m);
            bitboardAdd(r-i,c-i,b);
        } else {
            break;
        }
    }
    // Northeast Loop
    for(unsigned int i = 1; r - i >= 0 && c + i < 8; i++) {
        if(boardPieces[r-i][c+i].isNull()) {
            Move m = std::make_tuple(std::make_pair(r,c),std::make_pair(r - i, c + i),'N');
            p->moveList.push_back(m);
            bitboardAdd(r-i,c+i,b);
        } else if(!isSameColor(*p,boardPieces[r-i][c+i])) {
            Move m = std::make_tuple(std::make_pair(r,c),std::make_pair(r - i, c + i),'X');
            p->moveList.push_back(m);
            bitboardAdd(r-i,c+i,b);
        } else {
            break;
        }
    }
    // Southwest Loop
    for(unsigned int i = 1; r + i < 8 && c - i >= 0; i++) {
        if(boardPieces[r+i][c-i].isNull()) {
            Move m = std::make_tuple(std::make_pair(r,c),std::make_pair(r + i, c - i),'N');
            p->moveList.push_back(m);
            bitboardAdd(r+i,c-i,b);
        } else if(!isSameColor(*p,boardPieces[r+i][c-i])) {
            Move m = std::make_tuple(std::make_pair(r,c),std::make_pair(r + i, c - i),'X');
            p->moveList.push_back(m);
            bitboardAdd(r+i,c-i,b);
        } else {
            break;
        }
    }
    // Southeast Loop
    for(unsigned int i = 1; r + i < 8 && c + i < 8; i++) {
        if(boardPieces[r+i][c+i].isNull()) {
            Move m = std::make_tuple(std::make_pair(r,c),std::make_pair(r + i, c + i),'N');
            p->moveList.push_back(m);
            bitboardAdd(r+i,c+i,b);
        } else if(!isSameColor(*p,boardPieces[r+i][c+i])) {
            Move m = std::make_tuple(std::make_pair(r,c),std::make_pair(r + i, c + i),'X');
            p->moveList.push_back(m);
            bitboardAdd(r+i,c+i,b);
        } else {
            break;
        }
    }
}

void Board::generateMovesRook(unsigned int r, unsigned int c, Piece* p) {
    // Select correct bitboard of attacking moves, add each move to bitboard
    Bitboard b = (p->isWhite) ? whiteAttack : blackAttack;
    // North Loop
    for(unsigned int i = 1; r - i >= 0; i++) {
        if(boardPieces[r-i][c].isNull()) {
            Move m = std::make_tuple(std::make_pair(r,c),std::make_pair(r - i, c),'N');
            p->moveList.push_back(m);
            bitboardAdd(r-i,c,b);
        } else if(!isSameColor(*p,boardPieces[r-i][c])) {
            Move m = std::make_tuple(std::make_pair(r,c),std::make_pair(r - i, c),'X');
            p->moveList.push_back(m);
            bitboardAdd(r-i,c,b);
        } else {
            break;
        }
    }
    // South Loop
    for(unsigned int i = 1; r + i < 8; i++) {
        if(boardPieces[r+i][c].isNull()) {
            Move m = std::make_tuple(std::make_pair(r,c),std::make_pair(r + i, c),'N');
            p->moveList.push_back(m);
            bitboardAdd(r+i,c,b);
        } else if(!isSameColor(*p,boardPieces[r+i][c])) {
            Move m = std::make_tuple(std::make_pair(r,c),std::make_pair(r + i, c),'X');
            p->moveList.push_back(m);
            bitboardAdd(r+i,c,b);
        } else {
            break;
        }
    }
    // West Loop
    for(unsigned int i = 1; c - i >= 0; i++) {
        if(boardPieces[r][c-i].isNull()) {
            Move m = std::make_tuple(std::make_pair(r,c),std::make_pair(r, c - i),'N');
            p->moveList.push_back(m);
            bitboardAdd(r,c-i,b);
        } else if(!isSameColor(*p,boardPieces[r][c-i])) {
            Move m = std::make_tuple(std::make_pair(r,c),std::make_pair(r, c - i),'X');
            p->moveList.push_back(m);
            bitboardAdd(r,c-i,b);
        } else {
            break;
        }
    }
    // East Loop
    for(unsigned int i = 1; c + i < 8; i++) {
        if(boardPieces[r][c+i].isNull()) {
            Move m = std::make_tuple(std::make_pair(r,c),std::make_pair(r, c + i),'N');
            p->moveList.push_back(m);
            bitboardAdd(r,c+i,b);
        } else if(!isSameColor(*p,boardPieces[r][c+i])) {
            Move m = std::make_tuple(std::make_pair(r,c),std::make_pair(r, c + i),'X');
            p->moveList.push_back(m);
            bitboardAdd(r,c+i,b);
        } else {
            break;
        }
    }
}

void Board::generateMovesQueen(unsigned int r, unsigned int c, Piece* p) {
    // Queen is a combination of rook and bishop so the code is simplified
    generateMovesBishop(r,c,p);
    generateMovesRook(r,c,p);
}

void Board::generateMovesKing(unsigned int r, unsigned int c, Piece* p) {
    // Select correct bitboard of attacking moves, add each move to bitboard
    Bitboard b = (p->isWhite) ? whiteAttack : blackAttack;
    // Check the 8 spots around the king
    if(r - 1 >= 0 && c - 1 >= 0 && (boardPieces[r-1][c-1].isNull() || !isSameColor(*p,boardPieces[r-1][c-1]))) {
        Square loc = std::make_pair(r - 1, c - 1);
        Move m = (p->isNull()) ? std::make_tuple(std::make_pair(r,c),loc,'N') : std::make_tuple(std::make_pair(r,c),loc,'X');
        p->moveList.push_back(m);
        bitboardAdd(r-1,c-1,b);
    }
    if(r - 1 >= 0 && (boardPieces[r-1][c].isNull() || !isSameColor(*p,boardPieces[r-1][c]))) {
        Square loc = std::make_pair(r - 1, c);
        Move m = (p->isNull()) ? std::make_tuple(std::make_pair(r,c),loc,'N') : std::make_tuple(std::make_pair(r,c),loc,'X');
        p->moveList.push_back(m);
        bitboardAdd(r-1,c,b);
    }
    if(r - 1 >= 0 && c + 1 < 8 && (boardPieces[r-1][c+1].isNull() || !isSameColor(*p,boardPieces[r-1][c+1]))) {
        Square loc = std::make_pair(r - 1, c + 1);
        Move m = (p->isNull()) ? std::make_tuple(std::make_pair(r,c),loc,'N') : std::make_tuple(std::make_pair(r,c),loc,'X');
        p->moveList.push_back(m);
        bitboardAdd(r-1,c+1,b);
    }
    if(c - 1 >= 0 && (boardPieces[r][c-1].isNull() || !isSameColor(*p,boardPieces[r][c-1]))) {
        Square loc = std::make_pair(r, c - 1);
        Move m = (p->isNull()) ? std::make_tuple(std::make_pair(r,c),loc,'N') : std::make_tuple(std::make_pair(r,c),loc,'X');
        p->moveList.push_back(m);
        bitboardAdd(r,c-1,b);
    }
    if(c + 1 < 8 && (boardPieces[r][c+1].isNull() || !isSameColor(*p,boardPieces[r][c+1]))) {
        Square loc = std::make_pair(r, c + 1);
        Move m = (p->isNull()) ? std::make_tuple(std::make_pair(r,c),loc,'N') : std::make_tuple(std::make_pair(r,c),loc,'X');
        p->moveList.push_back(m);
        bitboardAdd(r,c+1,b);
    }
    if(r + 1 < 8 && c - 1 >= 0 && (boardPieces[r+1][c-1].isNull() || !isSameColor(*p,boardPieces[r+1][c-1]))) {
        Square loc = std::make_pair(r + 1, c - 1);
        Move m = (p->isNull()) ? std::make_tuple(std::make_pair(r,c),loc,'N') : std::make_tuple(std::make_pair(r,c),loc,'X');
        p->moveList.push_back(m);
        bitboardAdd(r+1,c-1,b);
    }
    if(r + 1 < 8 && (boardPieces[r+1][c].isNull() || !isSameColor(*p,boardPieces[r+1][c]))) {
        Square loc = std::make_pair(r + 1, c);
        Move m = (p->isNull()) ? std::make_tuple(std::make_pair(r,c),loc,'N') : std::make_tuple(std::make_pair(r,c),loc,'X');
        p->moveList.push_back(m);
        bitboardAdd(r+1,c,b);
    }
    if(r + 1 < 8 && c + 1 < 8 && (boardPieces[r+1][c+1].isNull() || !isSameColor(*p,boardPieces[r+1][c+1]))) {
        Square loc = std::make_pair(r + 1, c + 1);
        Move m = (p->isNull()) ? std::make_tuple(std::make_pair(r,c),loc,'N') : std::make_tuple(std::make_pair(r,c),loc,'X');
        p->moveList.push_back(m);
        bitboardAdd(r+1,c+1,b);
    }
}

// Generates the moves for a specific piece
// Does NOT take care of check due to circular dependencies
void Board::generateMoves(Piece* p) {
    unsigned int r = p->location.first;
    unsigned int c = p->location.second;
    std::list<Move>& moveList = p->moveList;
    moveList.clear();
    // Call correct function for piece type
    switch(p->pieceType) {
        case 'p':
            generateMovesPawn(r,c,p);
            break;
        case 'n':
            generateMovesKnight(r,c,p);
            break;
        case 'b':
            generateMovesBishop(r,c,p);
            break;
        case 'r':
            generateMovesRook(r,c,p);
            break;
        case 'q':
            generateMovesQueen(r,c,p);
            break;
        case 'k':
            generateMovesKing(r,c,p);
            break;
    }
}

// Performs a move (NOT FOR ACTUAL TURNS, ONLY LOOKING AHEAD)
void Board::forwardMove(const Move& m) {
    unsigned int r1 = std::get<0>(m).first;
    unsigned int c1 = std::get<0>(m).second;
    unsigned int r2 = std::get<1>(m).first;
    unsigned int c2 = std::get<1>(m).second;
    char type = std::get<2>(m);

    Piece* p = &boardPieces[r1][c1];
    lastCapturedPiece = &boardPieces[r2][c2];
    // Make move
    if(type == 'E') {
        // If en passant, update lastCapturedPiece to the captured pawn (one spot below)
        unsigned int i = (p->isWhite) ? -1 : 1;
        lastCapturedPiece = &boardPieces[r2 - i][c2];

        boardPieces[r2][c2] = boardPieces[r1][c1];
        boardPieces[r1][c1] = Piece();
    }
    else if(type == 'C') {
        boardPieces[r2][c2] = boardPieces[r1][c1];
        boardPieces[r1][c1] = Piece();
        // Move rook
        unsigned int rook_c1 = (c2 == 1) ? 0 : 7;
        unsigned int rook_c2 = (c2 == 1) ? 3 : 5;
        boardPieces[r2][rook_c2] = boardPieces[r2][c1];
        boardPieces[r2][rook_c1] = Piece();
        boardPieces[r2][rook_c2].updateLocation(r2,rook_c2);
    } else {
        boardPieces[r2][c2] = boardPieces[r1][c1];
        boardPieces[r1][c1] = Piece();
    }
    p->updateLocation(r2,c2);
}

// Undoes a move (useful with forwardMove to look ahead moves without making a new board)
// REQUIRED: Reverses the last move only. NO ERROR CHECKING DONE FOR THIS!
void Board::reverseMove(const Move& m) {
    unsigned int r1 = std::get<0>(m).first;
    unsigned int c1 = std::get<0>(m).second;
    unsigned int r2 = std::get<1>(m).first;
    unsigned int c2 = std::get<1>(m).second;
    char type = std::get<2>(m);

    Piece* p = &boardPieces[r2][c2];
    // Move main piece back
    boardPieces[r1][c1] = *p;
    p->updateLocation(r1,c1);
    boardPieces[r2][c2] = Piece();
    // Place back captured piece. NOTE: No need to update captured piece location as it hasn't moved
    if(type == 'E') {
        // For en passant, the piece needs to be placed one row offset
        unsigned int i = (p->isWhite) ? -1 : 1;
        boardPieces[r2 - i][c2] = *lastCapturedPiece;
    } else if(type == 'C') {
        // For castle, we need to place back the rook. King has already been placed back above
        unsigned int rook_c1 = (c2 == 1) ? 0 : 7;
        unsigned int rook_c2 = (c2 == 1) ? 3 : 5;
        boardPieces[r1][rook_c1] = boardPieces[r1][rook_c2];
        boardPieces[r1][rook_c1].updateLocation(r1,rook_c1);
        boardPieces[r1][rook_c2] = Piece();
    } else if(type == 'X') {
        boardPieces[r2][c2] = *lastCapturedPiece;
    }
}

void Board::trimCheck() {
    // Remove moves from black which don't escape check
    if(bitboardHas(blackKing->location.first,blackKing->location.second,whiteAttack)) {
        // Loop through all moves of black and remove all which don't result in not being in check
        for(std::list<Piece>::iterator itr = blackPieces.begin(); itr != blackPieces.end(); itr++) {
            for(std::list<Move>::iterator mitr = itr->moveList.begin(); mitr != itr->moveList.end();) {
                Move& m = *mitr;
                forwardMove(m);
                // Did this fix the check?
                if(bitboardHas(blackKing->location.first,blackKing->location.second,whiteAttack)) {
                    mitr = itr->moveList.erase(mitr);
                } else {
                    mitr++;
                }
                reverseMove(m);
            }
        }
    }
    // Remove moves from white which don't escape check
    if(bitboardHas(whiteKing->location.first,whiteKing->location.second,blackAttack)) {
        // Loop through all moves of white and remove all which don't result in not being in check
        for(std::list<Piece>::iterator itr = whitePieces.begin(); itr != whitePieces.end(); itr++) {
            for(std::list<Move>::iterator mitr = itr->moveList.begin(); mitr != itr->moveList.end();) {
                Move& m = *mitr;
                forwardMove(m);
                // Did this fix the check?
                if(bitboardHas(whiteKing->location.first,whiteKing->location.second,blackAttack)) {
                    mitr = itr->moveList.erase(mitr);
                } else {
                    mitr++;
                }
                reverseMove(m);
            }
        }
    }
}

// Main move generation function. Updates the moves of all pieces in the board.
void Board::updateBoard() {
    // Loop through each piece and call generateMoves
    for(std::list<Piece>::iterator itr = whitePieces.begin(); itr != whitePieces.end(); itr++) {
        Piece p = *itr;
        generateMoves(&p);
    }
    for(std::list<Piece>::iterator itr = blackPieces.begin(); itr != blackPieces.end(); itr++) {
        Piece p = *itr;
        generateMoves(&p);
    }
    // Clear out any moves generated which caused checks
    trimCheck();
}