/*
 *  CPP Implemenetation for Pieces
 *  DEPRECATED 9/7/2023
 */

#include "piece.h"

typedef std::pair<unsigned int,unsigned int> Square;
typedef std::tuple<Square,Square,char> Move;

// CONSTRUCTORS

Pawn::Pawn(bool isWhite, int r, int c) {
    this->isWhite = isWhite;
    pieceType = 'p';
    pieceValue = 1;
    location.first = r;
    location.second = c;

    pieceMoved = false;
    pieceMovedTwice = false;

}

Knight::Knight(bool isWhite, int r, int c) {
    this->isWhite = isWhite;
    pieceType = 'n';
    pieceValue = 3;
    location.first = r;
    location.second = c;

}

Bishop::Bishop(bool isWhite, int r, int c) {
    this->isWhite = isWhite;
    pieceType = 'b';
    pieceValue = 3;
    location.first = r;
    location.second = c;

}

Rook::Rook(bool isWhite, int r, int c) {
    this->isWhite = isWhite;
    pieceType = 'r';
    pieceValue = 5;
    location.first = r;
    location.second = c;

    pieceMoved = false;

}

Queen::Queen(bool isWhite, int r, int c) {
    this->isWhite = isWhite;
    pieceType = 'q';
    pieceValue = 9;
    location.first = r;
    location.second = c;

}

King::King(bool isWhite, int r, int c) {
    this->isWhite = isWhite;
    pieceType = 'k';
    pieceValue = 0;
    location.first = r;
    location.second = c;

}

// Update Location special implementation for pawns and kings, accounting for move booleans.

void Pawn::updateLocation(int r, int c) {
    location.first = r;
    location.second = c;

    if(!pieceMovedTwice && pieceMoved) {
        pieceMovedTwice = true;
    } else if(!pieceMoved) {
        pieceMoved = true;
    }
}

void King::updateLocation(int r, int c) {
    location.first = r;
    location.second = c;
    
    if(!pieceMoved) {
        pieceMoved = true;
    }
}
