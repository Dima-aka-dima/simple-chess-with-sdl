#ifndef CHESS_HPP
#define CHESS_HPP

#include <optional>
#include <array>

namespace Chess{

  
enum PieceName{Rook, Knight, Bishop, King, Queen, Pawn};

enum PieceColor{Black, White};

typedef struct {
  PieceName name;
  PieceColor color;
  bool isNone = true;
} PieceType;

template <typename T, std::size_t R, std::size_t C>
using Array2d = std::array<std::array<T, C>, R>;
  
typedef struct{
  PieceName name;
  PieceColor color;
  SDL_Point position;
  SDL_Point spritePosition;
} Piece;
  

bool isWhite(SDL_Point p){return ((p.x + p.y) % 2 == 0);}

const std::vector<Piece> initialPieces = {
  {Rook, White, {0, 0}, {4, 0}},
  {Rook, White, {7, 0}, {4, 0}},
  {Rook, Black, {0, 7}, {4, 1}},
  {Rook, Black, {7, 7}, {4, 1}},
  
  {Knight, White, {1, 0}, {3, 0}},
  {Knight, White, {6, 0}, {3, 0}},
  {Knight, Black, {1, 7}, {3, 1}},
  {Knight, Black, {6, 7}, {3, 1}},
  
  {Bishop, White, {2, 0}, {2, 0}},
  {Bishop, White, {5, 0}, {2, 0}},
  {Bishop, Black, {2, 7}, {2, 1}},
  {Bishop, Black, {5, 7}, {2, 1}},
  
  {Queen, White, {4, 0}, {1, 0}},
  {Queen, Black, {4, 7}, {1, 1}},
  
  {King, White, {3, 0}, {0, 0}},
  {King, Black, {3, 7}, {0, 1}},
  
  {Pawn, White, {0, 1}, {5, 0}},
  {Pawn, White, {1, 1}, {5, 0}},
  {Pawn, White, {2, 1}, {5, 0}},
  {Pawn, White, {3, 1}, {5, 0}},
  {Pawn, White, {4, 1}, {5, 0}},
  {Pawn, White, {5, 1}, {5, 0}},
  {Pawn, White, {6, 1}, {5, 0}},
  {Pawn, White, {7, 1}, {5, 0}},
  
  {Pawn, Black, {0, 6}, {5, 1}},
  {Pawn, Black, {1, 6}, {5, 1}},
  {Pawn, Black, {2, 6}, {5, 1}},
  {Pawn, Black, {3, 6}, {5, 1}},
  {Pawn, Black, {4, 6}, {5, 1}},
  {Pawn, Black, {5, 6}, {5, 1}},
  {Pawn, Black, {6, 6}, {5, 1}},
  {Pawn, Black, {7, 6}, {5, 1}}
};
  
struct Board{
  std::vector<Piece> pieces = initialPieces;
  Array2d<PieceType, 8, 8> board;
  PieceColor turn = White;

  Board(){
    for(auto& piece: pieces)
      board[piece.position.x][piece.position.y] ={piece.name, piece.color, false};
  }
  
  void switchTurn(){
    if(turn == White) turn = Black;
    else turn = White;}
  
  void reset(){pieces = initialPieces; turn = White;}

  bool any(SDL_Point position)
  {
    for(auto& piece: pieces) if(piece.position == position) return true;
    return false;
  }

  Piece operator[](SDL_Point position){
    for(auto& piece: pieces) if(piece.position == position) return piece;
    return {};    
  }

  void setPieceAt(Piece piece, SDL_Point position)
  {
    piece.position = position;
    for(size_t i = 0; i < pieces.size(); i++)
      if(pieces[i].position == position){pieces[i] = piece; return;}
    pieces.push_back(piece);
  }

  void deletePieceAt(SDL_Point position)
  {
    for(size_t i = 0; i < pieces.size(); i++)
      if(pieces[i].position == position){
	pieces.erase(pieces.begin() + i); break;}
  }


  
  std::vector<SDL_Point> getPawnMoves(SDL_Point p, PieceColor color){
    std::vector<SDL_Point> moves = {};
    if(color == White){
      if(p.y == 7) return {};
      if(!any({p.x, p.y + 1})) moves.push_back({p.x, p.y + 1});
      if((p.y == 1) && !any({p.x, 2}) && !any({p.x, 3}))
	moves.push_back({p.x, 3});
    }
    else{
      if(p.y == 0) return {};
      if(!any({p.x, p.y - 1})) moves.push_back({p.x, p.y - 1});
      if((p.y == 6) && !any({p.x, 5}) && !any({p.x, 4}))
	moves.push_back({p.x, 4});
    
    }
    return moves;
  }

  bool is(SDL_Point p){
    return SDL_PointInRect(&p, new SDL_Rect({0, 0, 8, 8}));}
  
  std::vector<SDL_Point> getRookMoves(SDL_Point p){
    std::vector<SDL_Point> moves = {};
    SDL_Point to;
    for(int d = 1; d < 8; d++){
      to = {p.x, p.y + d}; if(is(to) && !any(to)) moves.push_back(to); else break;}
    for(int d = 1; d < 8; d++){
      to = {p.x, p.y - d}; if(is(to) && !any(to)) moves.push_back(to); else break;}
    for(int d = 1; d < 8; d++){
      to = {p.x + d, p.y}; if(is(to) && !any(to)) moves.push_back(to); else break;}
    for(int d = 1; d < 8; d++){
      to = {p.x - d, p.y}; if(is(to) && !any(to)) moves.push_back(to); else break;}

    return moves;
  }
  
  std::vector<SDL_Point> getBishopMoves(SDL_Point p){
    std::vector<SDL_Point> moves = {};
    SDL_Point to;
    for(int d = 1; d < 8; d++){
      to = {p.x + d, p.y + d}; if(is(to) && !any(to)) moves.push_back(to); else break;}
    for(int d = 1; d < 8; d++){
      to = {p.x + d, p.y - d}; if(is(to) && !any(to)) moves.push_back(to); else break;}
    for(int d = 1; d < 8; d++){
      to = {p.x - d, p.y + d}; if(is(to) && !any(to)) moves.push_back(to); else break;}
    for(int d = 1; d < 8; d++){
      to = {p.x - d, p.y - d}; if(is(to) && !any(to)) moves.push_back(to); else break;}

    return moves;
  }


  std::vector<SDL_Point> getQueenMoves(SDL_Point& position){
    std::vector<SDL_Point> moves = getRookMoves(position);
    std::vector<SDL_Point> diagonalMoves = getBishopMoves(position);
    moves.insert(moves.end(), diagonalMoves.begin(), diagonalMoves.end());
    return moves;
  }

  std::vector<SDL_Point> getKnightMoves(SDL_Point& p){
    std::vector<SDL_Point> moves = {};
    std::vector<SDL_Point> knightDeltas = {
      {2, 1}, {2, -1}, {-2, 1}, {-2, -1},
      {1, 2}, {-1, 2}, {1, -2}, {-1, -2}};
    SDL_Point to;
    for(auto d: knightDeltas){
      to = p + d; if(is(to) && !any(to)) moves.push_back(to);}
    return moves;
  }

  std::vector<SDL_Point> getPawnCaptureMoves(SDL_Point& p, PieceColor color){
    std::vector<SDL_Point> moves;
    SDL_Point to;
    if(color == White){
      to = p + (SDL_Point){1, 1};
      if(any(to) && ((*this)[to].color != color)) moves.push_back(to);
      to = p + (SDL_Point){-1, 1};
      if(any(to) && ((*this)[to].color != color)) moves.push_back(to);
    }else{
      to = p + (SDL_Point){-1, -1};
      if(any(to) && ((*this)[to].color != color)) moves.push_back(to);
      to = p + (SDL_Point){1, -1};
      if(any(to) && ((*this)[to].color != color)) moves.push_back(to);
    }
    return moves;
  }


  std::vector<SDL_Point> getPawnPossibleCaptureMoves(SDL_Point& p, PieceColor color){
    std::vector<SDL_Point> moves;
    SDL_Point to;
    if(color == White){
      to = p + (SDL_Point){1, 1};
      moves.push_back(to);
      to = p + (SDL_Point){-1, 1};
      moves.push_back(to);
    }else{
      to = p + (SDL_Point){-1, -1};
      moves.push_back(to);
      to = p + (SDL_Point){1, -1};
      moves.push_back(to);
    }
    return moves;
  }

  std::vector<SDL_Point> getRookCaptureMoves(SDL_Point p, PieceColor color){
    std::vector<SDL_Point> moves = {};
    SDL_Point to;
    for(int d = 1; d < 8; d++){
      to = {p.x, p.y + d};
      if(is(to) && any(to)){if((*this)[to].color != color) moves.push_back(to);break;}}
    for(int d = 1; d < 8; d++){
      to = {p.x, p.y - d};
      if(is(to) && any(to)){if((*this)[to].color != color) moves.push_back(to);break;}}
    for(int d = 1; d < 8; d++){
      to = {p.x + d, p.y};
      if(is(to) && any(to)){if((*this)[to].color != color) moves.push_back(to);break;}}
    for(int d = 1; d < 8; d++){
      to = {p.x - d, p.y};
      if(is(to) && any(to)){if((*this)[to].color != color) moves.push_back(to);break;}}
      
    return moves;
  }

  std::vector<SDL_Point> getBishopCaptureMoves(SDL_Point p, PieceColor color){
    std::vector<SDL_Point> moves = {};
    SDL_Point to;
    for(int d = 1; d < 8; d++){
      to = {p.x + d, p.y + d};
      if(is(to) && any(to)){if((*this)[to].color != color) moves.push_back(to);break;}}
    for(int d = 1; d < 8; d++){
      to = {p.x + d, p.y - d};
      if(is(to) && any(to)){if((*this)[to].color != color) moves.push_back(to);break;}}
    for(int d = 1; d < 8; d++){
      to = {p.x - d, p.y + d};
      if(is(to) && any(to)){if((*this)[to].color != color) moves.push_back(to);break;}}
    for(int d = 1; d < 8; d++){
      to = {p.x - d, p.y - d};
      if(is(to) && any(to)){if((*this)[to].color != color) moves.push_back(to);break;}}
      
    return moves;
  }
  
  std::vector<SDL_Point> getQueenCaptureMoves(SDL_Point& position, PieceColor color){
    std::vector<SDL_Point> moves = getRookCaptureMoves(position, color);
    std::vector<SDL_Point> diagonalMoves = getBishopCaptureMoves(position, color);
    moves.insert(moves.end(), diagonalMoves.begin(), diagonalMoves.end());
    return moves;
  }

  
  std::vector<SDL_Point> getKnightCaptureMoves(SDL_Point& p, PieceColor color){
    std::vector<SDL_Point> moves = {};
    std::vector<SDL_Point> knightDeltas = {
      {2, 1}, {2, -1}, {-2, 1}, {-2, -1},
      {1, 2}, {-1, 2}, {1, -2}, {-1, -2}};
    SDL_Point to;
    for(auto d: knightDeltas){
      to = p + d; if(is(to) && any(to) && ((*this)[to].color != color)) moves.push_back(to);}
    return moves;
  }

  std::vector<SDL_Point> getNonKingMoves(Piece piece){
    switch(piece.name){
    case Pawn: return getPawnMoves(piece.position, piece.color);
    case Rook: return getRookMoves(piece.position);
    case Knight: return getKnightMoves(piece.position);
    case Bishop: return getBishopMoves(piece.position);
    case Queen: return getQueenMoves(piece.position);
    default: return {};
    }
    return {};  
  }

  std::vector<SDL_Point> getNonKingCaptureMoves(Piece piece){
    switch(piece.name){
    case Pawn: return getPawnCaptureMoves(piece.position, piece.color);
    case Rook: return getRookCaptureMoves(piece.position, piece.color);
    case Knight: return getKnightCaptureMoves(piece.position, piece.color);
    case Bishop: return getBishopCaptureMoves(piece.position, piece.color);
    case Queen: return getQueenCaptureMoves(piece.position, piece.color);
    default: return {};
    }
    return {};
  }

  /*
  bool isProtected(SDL_Point position){
    for(auto& piece: pieces){
      if(piece.color == turn) continue;

      // TODO: handle King case
      if(piece.name == King) continue;

    
      std::vector<SDL_Point> moves = getNonKingMoves(piece);
      if(piece.name == Pawn)
	moves = getPawnPossibleCaptureMoves(piece.position, piece.color);
    
      for(auto& move: moves)
	if(move == position) return true;

   
      Piece invertedPiece = piece;
      invertedPiece.color = turn;
      moves = getNonKingCaptureMoves(invertedPiece);
      for(auto& move: moves)
	if(move == position) return true;
    }

    return false;
  }
  */
  
  std::vector<SDL_Point> getKingMoves(SDL_Point& p){
    std::vector<SDL_Point> moves = {};

    SDL_Point kingDeltas[8] = {
      {-1, -1}, {1, 1}, {1, -1}, {-1, 1}, {0, 1}, {1, 0}, {0, -1}, {-1, 0}};

    SDL_Point to;
    for(auto d: kingDeltas){
      to = p + d; if(is(to) && !any(to)) moves.push_back(to);}

    return moves;
  }

  std::vector<SDL_Point> getKingCaptureMoves(SDL_Point& p, PieceColor color){
    std::vector<SDL_Point> moves = {};

    SDL_Point kingDeltas[8] = {
      {-1, -1}, {1, 1}, {1, -1}, {-1, 1}, {0, 1}, {1, 0}, {0, -1}, {-1, 0}};

    SDL_Point to;
    for(auto d: kingDeltas){
      to = p + d; if(is(to) && any(to) && ((*this)[to].color != color)) moves.push_back(to);}

    return moves;
  }

  std::vector<SDL_Point> getCaptureMoves(Piece piece){
    if(piece.name == King) return getKingCaptureMoves(piece.position, piece.color);
    else return getNonKingCaptureMoves(piece);  
  }

  std::vector<SDL_Point> getMoves(Piece& piece){
    if(piece.name == King) return getKingMoves(piece.position);
    else return getNonKingMoves(piece);  
  }

};
  
  
}


#endif
