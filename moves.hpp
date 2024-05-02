#ifndef MOVES_HPP
#define MOVES_HPP

#include "definitions.hpp"


std::vector<SDL_Point> getPawnMoves(SDL_Point p, PieceColor color){
  std::vector<SDL_Point> moves = {};
  if(color == White){
    if(p.y == 7) return {};
    if(!isAnyPieceAt({p.x, p.y + 1})) moves.push_back({p.x, p.y + 1});
    if((p.y == 1) && !isAnyPieceAt({p.x, 2}) && !isAnyPieceAt({p.x, 3}))
      moves.push_back({p.x, 3});
  }
  else{
    if(p.y == 0) return {};
    if(!isAnyPieceAt({p.x, p.y - 1})) moves.push_back({p.x, p.y - 1});
    if((p.y == 6) && !isAnyPieceAt({p.x, 5}) && !isAnyPieceAt({p.x, 4}))
      moves.push_back({p.x, 4});
    
  }
  return moves;
}

std::vector<SDL_Point> getRookMoves(SDL_Point p){
  std::vector<SDL_Point> moves = {};

  for(int x = p.x + 1; x < 8; x++){
    if(isAnyPieceAt({x, p.y})) break;
    moves.push_back({x, p.y});
  }
  
  for(int x = p.x - 1; x >= 0; x--){
    if(isAnyPieceAt({x, p.y})) break;
    moves.push_back({x, p.y});
  }

  for(int y = p.y + 1; y < 8; y++){
    if(isAnyPieceAt({p.x, y})) break;
    moves.push_back({p.x, y});
  }
  
  for(int y = p.y - 1; y >= 0; y--){
    if(isAnyPieceAt({p.x, y})) break;
    moves.push_back({p.x, y});
  }
  
  return moves;
}

std::vector<SDL_Point> getBishopMoves(SDL_Point p){
  std::vector<SDL_Point> moves = {};

  for(int d = 1; ;d++){
    int x = p.x + d, y = p.y + d;
    if(x == 8 || y == 8) break;
    if(isAnyPieceAt({x, y})) break;
    moves.push_back({x, y});
  }

  for(int d = 1; ;d++){
    int x = p.x + d, y = p.y - d;
    if(x == 8 || y == -1) break;
    if(isAnyPieceAt({x, y})) break;
    moves.push_back({x, y});
  }

  for(int d = 1; ;d++){
    int x = p.x - d, y = p.y + d;
    if(x == -1 || y == 8) break;
    if(isAnyPieceAt({x, y})) break;
    moves.push_back({x, y});
  }

  for(int d = 1; ;d++){
    int x = p.x - d, y = p.y - d;
    if(x == -1 || y == -1) break;
    if(isAnyPieceAt({x, y})) break;
    moves.push_back({x, y});
  }

  return moves;
}


std::vector<SDL_Point> getQueenMoves(SDL_Point& position){
  std::vector<SDL_Point> moves = getRookMoves(position);
  for(auto& move: getBishopMoves(position))
    moves.push_back(move);
  return moves;
}

std::vector<SDL_Point> getKnightMoves(SDL_Point& p){
  std::vector<SDL_Point> moves = {};

  std::vector<SDL_Point> deltas = {
    {2, 1}, {2, -1}, {-2, 1}, {-2, -1},
    {1, 2}, {-1, 2}, {1, -2}, {-1, -2}};

  for(auto d: deltas){
    SDL_Point to = p + d;
    if((to.x < 0) || (to.y < 0) || (to.x >= 8) || (to.y >= 8)) continue;

    if(!isAnyPieceAt(to)) moves.push_back(to);
  }
  
  return moves;
}

std::vector<SDL_Point> getPawnCaptureMoves(SDL_Point& p, PieceColor color){
  std::vector<SDL_Point> moves;
  SDL_Point to;
  if(color == White){
    to = p + (SDL_Point){1, 1};
    if(isAnyPieceAt(to) && (getPieceAt(to).color != color)) moves.push_back(to);
    to = p + (SDL_Point){-1, 1};
    if(isAnyPieceAt(to) && (getPieceAt(to).color != color)) moves.push_back(to);
  }else{
    to = p + (SDL_Point){-1, -1};
    if(isAnyPieceAt(to) && (getPieceAt(to).color != color)) moves.push_back(to);
    to = p + (SDL_Point){1, -1};
    if(isAnyPieceAt(to) && (getPieceAt(to).color != color)) moves.push_back(to);
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

  for(int x = p.x + 1; x < 8; x++)
    if(isAnyPieceAt({x, p.y})){
      if(getPieceAt({x, p.y}).color != color)
	moves.push_back({x, p.y});
      break;
    }
  
  for(int x = p.x - 1; x >= 0; x--)
    if(isAnyPieceAt({x, p.y})){
      if(getPieceAt({x, p.y}).color != color)
	moves.push_back({x, p.y});
      break;
    }
  
  for(int y = p.y + 1; y < 8; y++)
    if(isAnyPieceAt({p.x, y})){
      if(getPieceAt({p.x, y}).color != color)
	moves.push_back({p.x, y});
      break;
    }
    
  for(int y = p.y - 1; y >= 0; y--)
    if(isAnyPieceAt({p.x, y})){
      if(getPieceAt({p.x, y}).color != color)
	moves.push_back({p.x, y});
      break;
    }
  
  return moves;
}


std::vector<SDL_Point> getBishopCaptureMoves(SDL_Point p, PieceColor color){
  std::vector<SDL_Point> moves = {};

  for(int d = 1; ;d++){
    SDL_Point to = p + (SDL_Point){d, d};
    if(to.x == 8 || to.y == 8) break;
    if(isAnyPieceAt(to)){
      if(getPieceAt(to).color != color)
	moves.push_back(to);
      break;
    }
  }

  for(int d = 1; ;d++){
    SDL_Point to = p + (SDL_Point){d, -d};
    if(to.x == 8 || to.y == -1) break;
    if(isAnyPieceAt(to)){
      if(getPieceAt(to).color != color)
	moves.push_back(to);
      break;
    }
  }
  
  for(int d = 1; ;d++){
    SDL_Point to = p + (SDL_Point){-d, d};
    if(to.x == -1 || to.y == 8) break;
    if(isAnyPieceAt(to)){
      if(getPieceAt(to).color != color)
	moves.push_back(to);
      break;
    }
  }
  
  for(int d = 1; ;d++){
    SDL_Point to = p + (SDL_Point){-d, -d};
    if(to.x == -1 || to.y == -1) break;
    if(isAnyPieceAt(to)){
      if(getPieceAt(to).color != color)
	moves.push_back(to);
      break;
    }
  }

  return moves;
}


std::vector<SDL_Point> getQueenCaptureMoves(SDL_Point& position, PieceColor color){
  std::vector<SDL_Point> moves = getRookCaptureMoves(position, color);
  for(auto& move: getBishopCaptureMoves(position, color))
    moves.push_back(move);
  return moves;
}

std::vector<SDL_Point> getKnightCaptureMoves(SDL_Point& p, PieceColor color){
  std::vector<SDL_Point> moves = {};

  std::vector<SDL_Point> deltas = {
    {2, 1}, {2, -1}, {-2, 1}, {-2, -1},
    {1, 2}, {-1, 2}, {1, -2}, {-1, -2}};

  for(auto d: deltas){
    SDL_Point to = p + d;
    if((to.x < 0) || (to.y < 0) || (to.x >= 8) || (to.y >= 8)) continue;

    if(isAnyPieceAt(to) && (getPieceAt(to).color != color)) moves.push_back(to);
  }
  
  return moves;
}

std::vector<SDL_Point> getNonKingMoves(Piece piece){
  switch(piece.type){
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
  switch(piece.type){
  case Pawn: return getPawnCaptureMoves(piece.position, piece.color);
  case Rook: return getRookCaptureMoves(piece.position, piece.color);
  case Knight: return getKnightCaptureMoves(piece.position, piece.color);
  case Bishop: return getBishopCaptureMoves(piece.position, piece.color);
  case Queen: return getQueenCaptureMoves(piece.position, piece.color);
  default: return {};
  }
  return {};
}


bool isProtected(SDL_Point position){
  for(auto& piece: pieces){
    if(piece.color == turn) continue;

    // TODO: handle King case
    if(piece.type == King) continue;

    
    std::vector<SDL_Point> moves = getNonKingMoves(piece);
    if(piece.type == Pawn)
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


std::vector<SDL_Point> getKingMoves(SDL_Point& p){
  std::vector<SDL_Point> moves = {};
  
  for(int dx = -1; dx <= 1; dx++)
    for(int dy = -1; dy <= 1; dy++){
      if((dx == 0) && (dy == 0)) continue;
      SDL_Point to = p + (SDL_Point){dx, dy};
      if((to.x < 0) || (to.y < 0) || (to.x >= 8) || (to.y >= 8)) continue;
      if(!isAnyPieceAt(to) && !isProtected(to)) moves.push_back(to);
    }

  return moves;
}

std::vector<SDL_Point> getKingCaptureMoves(SDL_Point& p, PieceColor color){
  std::vector<SDL_Point> moves = {};
  
  for(int dx = -1; dx <= 1; dx++)
    for(int dy = -1; dy <= 1; dy++){
      if((dx == 0) && (dy == 0)) continue;
      SDL_Point to = p + (SDL_Point){dx, dy};
      if((to.x < 0) || (to.y < 0) || (to.x >= 8) || (to.y >= 8)) continue;
      if(isAnyPieceAt(to) && (getPieceAt(to).color != color) && !isProtected(to))
	moves.push_back(to);
    }

  return moves;
}

std::vector<SDL_Point> getCaptureMoves(Piece piece){
  if(piece.type == King) return getKingCaptureMoves(piece.position, piece.color);
  else return getNonKingCaptureMoves(piece);  
}

std::vector<SDL_Point> getMoves(Piece& piece){
  if(piece.type == King) return getKingMoves(piece.position);
  else return getNonKingMoves(piece);  
}

#endif
