/*
 *  Header information for each gamepiece.
 */
#include <vector>
#include "board.h"

typedef std::pair<int,int> Square;

// Parent class for all piece types.
class Piece {
public:
    char getPieceType() const { return pieceType;}
    bool isPieceWhite() const { return isWhite;}
    int getPieceValue() const { return pieceValue;}
    const Square& getLocation() const { return location;}

    // Pawns and Kings require extra information of if they have been moved and their last location.
    // As such, this function is virtual as these said classes need to update some of their variables.
    virtual void updateLocation(int r, int c) { location.first = r; location.second = c;}

    virtual const std::vector<Square>& getPossibleMoves(const Board& board) const;

protected:
    char pieceType;
    int pieceValue;
    bool isWhite;
    
    Square location;
    std::vector<Square> moveList;
};

class Pawn : public Piece {
    public:
    
    private:
};

class Knight : public Piece {

};

class Bishop : public Piece {

};

class Rook : public Piece {

};

class Queen : public Piece {

};

class King : public Piece {

};
