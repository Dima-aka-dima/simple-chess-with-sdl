#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "definitions.hpp"
#include "gui.hpp"
#include "chess.hpp"

bool running;

SDL_Texture* texturePieces;
SDL_Texture* textureTiles;

GUI::Selection selection;
GUI::Pickup picked;
Mouse mouse;

Window* window = new Window();
SDL_Renderer* renderer;

GameMode gameMode = Game;

GUI::Board* boardElement = new GUI::Board();
Chess::Board* board = new Chess::Board();

SDL_Point getTileIntersection(SDL_Point* point){
  for(int i = 0; i < 8; i++)
    for(int j = 0; j < 8; j++){
      SDL_Rect rect = boardElement->getTileScreenRect({j, i});
      
      if(SDL_PointInRect(point, &rect)) return SDL_Point({j, i});
    }
  
  return SDL_Point({-1, -1});
}


// Returns whether or not the move was made
bool makeMove(Chess::Piece piece, SDL_Point position){
  if(gameMode == Game){
    std::vector<SDL_Point> moves = board->moves[piece];
    std::vector<SDL_Point> captureMoves = board->captureMoves[piece];

    if(!board->any(position)){
      for(auto& move: moves)
	if(move == position){
	  board->makeMove(piece, position);
	  board->updateMoves();
	  board->switchTurn();
	  return true;
	}
      return false;
    }

    if((*board)[position].color == piece.color) return false;
    
    for(auto& move: captureMoves){
      if(move == position){
	board->makeMove(piece, position);
	board->updateMoves();
	board->switchTurn();
	return true;
      }
    }

  return false;
  
  } else if(gameMode == Free){
    if(!board->any(position) || ((*board)[position].color != piece.color)){
      board->makeMove(piece, position);
      board->updateMoves();
      return true;
    }
    return false;
  }

  return false;
}
   
void updatePickupOnUp(){
  if(!picked.any) return;
  picked.any = false;
  
  if(!SDL_PointInRect(&mouse.position, &boardElement->position)) return;
  SDL_Point tile = getTileIntersection(&mouse.position);
  
  bool moveWasMade = makeMove(picked.piece, tile);
  if(moveWasMade){
    if(SWITCH_SIDE_MODE) boardElement->switchSide();
    if(board->isMate(board->turn)){
      std::cout << "Mate! ";
      if(board->turn == Chess::White) std::cout << "Black wins\n";
      else std::cout << "White wins\n";
    }
  }
}

void updatePickupOnDown(){
  if(!SDL_PointInRect(&mouse.position, &boardElement->position)) return;
  SDL_Point tile = getTileIntersection(&mouse.position);
  
  if(board->any(tile)){
    Chess::Piece piece = (*board)[tile];
    if(gameMode == Free || piece.color == board->turn){
      picked.any = true;
      picked.piece = piece;
      picked.position = mouse.position;
    }
  }
}

// TODO: Proper selection!
void updateSelectionOnDown(){

  selection.any = false;
  if(!SDL_PointInRect(&mouse.position, &boardElement->position)) return;
  SDL_Point tile = getTileIntersection(&mouse.position);

  if(board->any(tile)){
    Chess::Piece piece = (*board)[tile];
    if(gameMode == Free || piece.color == board->turn){
      selection.any = true;
      selection.piece = piece;
    }
  }

}

void updateSelectionOnUp(){
  selection.any = false;
  
}


void handleInput(SDL_Event event){
  window->updateOnResize();
  boardElement->updateOnResize(window);

  resetButton.updateOnResize(window, boardElement);
  switchSideButton.updateOnResize(window, boardElement);
  
  while(SDL_PollEvent(&event)){  
    switch(event.type){
    case SDL_QUIT:
      running = false; break;
    case SDL_MOUSEMOTION:
      mouse.position = {event.motion.x, event.motion.y};
      break;
      
    case SDL_MOUSEBUTTONUP:
      updatePickupOnUp();
      updateSelectionOnUp();
      if(SDL_PointInRect(&mouse.position, &resetButton.position)){
        boardElement->reset();
	board->reset();
      }
      if(SDL_PointInRect(&mouse.position, &switchSideButton.position))
	boardElement->switchSide();
    
      break;
      
    case SDL_MOUSEBUTTONDOWN:
      updatePickupOnDown();
      updateSelectionOnDown();
      break;
      
    case SDL_KEYDOWN:
      switch(event.key.keysym.sym){
      case SDLK_f: window->changeFullscreen(); break;
      case SDLK_q: running = false; break;
      } break;
    }
  }
}

void renderTiles(){

  for(int i = 0; i < 8; i++)
    for(int j = 0; j < 8; j++) {
      SDL_Rect tileDstRect = boardElement->getTileScreenRect({j, i});
      
      SDL_Rect tileSrcRect = {0, 0, SPRITE_TILE_SIZE, SPRITE_TILE_SIZE};
      if(!Chess::isWhite({j, i})) tileSrcRect.y += SPRITE_TILE_SIZE;
      SDL_RenderCopy(renderer, textureTiles, &tileSrcRect, &tileDstRect);
    }

  
  if(selection.any){
    SDL_Rect tileSrcRect = {SPRITE_TILE_SIZE, 0, SPRITE_TILE_SIZE, SPRITE_TILE_SIZE};;
    if(!Chess::isWhite(selection.piece.position)) tileSrcRect.y += SPRITE_TILE_SIZE;

    
    SDL_Rect tileDstRect = boardElement->getTileScreenRect(selection.piece.position);

    SDL_RenderCopy(renderer, textureTiles, &tileSrcRect, &tileDstRect);

    
    std::vector<SDL_Point> moves = board->moves[selection.piece];
    std::vector<SDL_Point> captureMoves = board->captureMoves[selection.piece];

    for(auto& move: moves){
      tileDstRect = boardElement->getTileScreenRect({move.x, move.y});
      tileSrcRect = {2*SPRITE_TILE_SIZE, 0, SPRITE_TILE_SIZE, SPRITE_TILE_SIZE};
      
      SDL_RenderCopy(renderer, textureTiles, &tileSrcRect, &tileDstRect);
    }
    
    for(auto& move: captureMoves){
      tileDstRect = boardElement->getTileScreenRect({move.x, move.y});
      tileSrcRect = {2*SPRITE_TILE_SIZE, SPRITE_TILE_SIZE, SPRITE_TILE_SIZE, SPRITE_TILE_SIZE};
      
      SDL_RenderCopy(renderer, textureTiles, &tileSrcRect, &tileDstRect);
    }
  }
  
}

SDL_Rect getPieceSrcRect(Chess::Piece& piece){
  return {
    piece.spritePosition.x*SPRITE_PIECE_SIZE,
    piece.spritePosition.y*SPRITE_PIECE_SIZE,
    SPRITE_PIECE_SIZE, SPRITE_PIECE_SIZE};}

SDL_Rect getPieceDstRect(Chess::Piece& piece){
  return boardElement->getTileScreenRect(piece.position);}

void renderPieces(){
    
  for(auto& piece: board->pieces){
    if(picked.any && picked.piece.position == piece.position) continue;

    SDL_Rect pieceSrcRect = getPieceSrcRect(piece);
    SDL_Rect pieceDstRect = getPieceDstRect(piece);

    SDL_RenderCopy(renderer, texturePieces, &pieceSrcRect, &pieceDstRect);    
  }

  if(picked.any){
    SDL_Rect pieceSrcRect = getPieceSrcRect(picked.piece);
    SDL_Rect pieceDstRect = getPieceDstRect(picked.piece);

    pieceDstRect.x += mouse.position.x - picked.position.x;
    pieceDstRect.y += mouse.position.y - picked.position.y;

    SDL_RenderCopy(renderer, texturePieces, &pieceSrcRect, &pieceDstRect);  
  }

}

int main(){
  std::cout << "Hello, world!" << std::endl; 

  renderer = SDL_CreateRenderer(window->sdlWindow, -1, 0);

  SetRenderDrawColor(renderer, BACKGROUND_COLOR);
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);

  IMG_Init(IMG_INIT_PNG);
  
  texturePieces = loadTexture(renderer, "pieces.png");
  textureTiles = loadTexture(renderer, "tiles.png");

  TTF_Init();
  font = TTF_OpenFont("./SpaceMono-Regular.ttf", 200);

  SDL_Event event;
  running = true;
  while(running){

    handleInput(event);
    
    SetRenderDrawColor(renderer, BACKGROUND_COLOR);
    SDL_RenderClear(renderer);

    renderTiles();
    renderPieces();
    resetButton.render(renderer);
    switchSideButton.render(renderer);
    SDL_RenderPresent(renderer);
  }
}
