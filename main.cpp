/* TODO: Separate Chess as a game (like an engine) and GUI / interactions with GUI*/ 

#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "definitions.hpp"
#include "moves.hpp"


bool running;

SDL_Texture* texturePieces;
SDL_Texture* textureTiles;

GUI::Selection selection;
GUI::Pickup picked;
Mouse mouse;

SDL_Rect getTileScreenRect(SDL_Point tilePosition){
  using namespace GUI;
  
  int tileSize = board->position.w / 8;
  if(board->side == White)
    return {
      board->position.x + tilePosition.x*tileSize,
      board->position.y + (7 - tilePosition.y)*tileSize,
      tileSize, tileSize};
 else
   return {
     board->position.x + tilePosition.x*tileSize,
     board->position.y + tilePosition.y*tileSize,
     tileSize, tileSize};
}

SDL_Point getTileIntersection(SDL_Point* point){
  for(int i = 0; i < 8; i++)
    for(int j = 0; j < 8; j++){
      SDL_Rect rect = getTileScreenRect({j, i});
      
      if(SDL_PointInRect(point, &rect)) return SDL_Point({j, i});
    }
  
  return SDL_Point({-1, -1});
}


// Returns whether or not the move was made
bool makeMove(Piece piece, SDL_Point position){
  if(gameMode == Game){
    std::vector<SDL_Point> moves = getMoves(piece);
    std::vector<SDL_Point> captureMoves = getCaptureMoves(piece);

    if(!isAnyPieceAt(position)){
      for(auto& move: moves)
	if(move == position){
	  setPieceAt(piece, position);
	  deletePieceAt(piece.position);
	  switchTurn();
	  return true;
	}
      return false;
    }

    if(getPieceAt(position).color == piece.color) return false;
    
    for(auto& move: captureMoves){
      if(move == position){

	setPieceAt(piece, position);
	deletePieceAt(piece.position);
	switchTurn();
	return true;
      }
    }

  return false;
  
  } else if(gameMode == Free){
    if(!isAnyPieceAt(position) || (getPieceAt(position).color != piece.color)){
      setPieceAt(piece, position);
      deletePieceAt(piece.position);
      return true;
    }
    return false;
  }

  return false;
}
   
void updatePickupOnUp(){
  using namespace GUI;
  
  if(!picked.any) return;
  picked.any = false;
  
  if(!SDL_PointInRect(&mouse.position, &board->position)) return;
  SDL_Point tilePosition = getTileIntersection(&mouse.position);
  
  bool moveWasMade = makeMove(picked.piece, tilePosition);
  if(moveWasMade && false) switchSide();
  
  // TODO: make selection properly
  // selection.same = false;
}

void updatePickupOnDown(){
  using namespace GUI;
  
  if(!SDL_PointInRect(&mouse.position, &board->position)) return;

  SDL_Point tilePosition = getTileIntersection(&mouse.position);
  for(auto& piece: pieces)
    if(tilePosition == piece.position && (gameMode == Free || piece.color == turn)){
      picked.any = true;
      picked.piece = piece;
      picked.position = mouse.position;
    }
}

// TODO: Proper selection!
void updateSelectionOnDown(){
  using namespace GUI;

  selection.any = false;
  if(!SDL_PointInRect(&mouse.position, &board->position)) return;
  SDL_Point tilePosition = getTileIntersection(&mouse.position);

  for(auto& piece: pieces)
    if(tilePosition == piece.position && (gameMode == Free || piece.color == turn)){
      selection.any = true;
      selection.piece = piece;
    } 
}

void updateSelectionOnUp(){
  using namespace GUI;

  selection.any = false;
  
}


void handleInput(SDL_Event event){
  window->updateOnResize();
  board->updateOnResize();

  resetButton.updateOnResize();
  switchSideButton.updateOnResize();
  
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
      if(SDL_PointInRect(&mouse.position, &resetButton.position))
        reset();
      if(SDL_PointInRect(&mouse.position, &switchSideButton.position))
	switchSide();
    
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
      SDL_Rect tileDstRect = getTileScreenRect({j, i});
      
      SDL_Rect tileSrcRect = {0, 0, SPRITE_TILE_SIZE, SPRITE_TILE_SIZE};
      if(!isWhite({j, i})) tileSrcRect.y += SPRITE_TILE_SIZE;
      SDL_RenderCopy(renderer, textureTiles, &tileSrcRect, &tileDstRect);
    }

  
  if(selection.any){
    SDL_Rect tileSrcRect = {SPRITE_TILE_SIZE, 0, SPRITE_TILE_SIZE, SPRITE_TILE_SIZE};;
    if(!isWhite(selection.piece.position)) tileSrcRect.y += SPRITE_TILE_SIZE;
    
    SDL_Rect tileDstRect = getTileScreenRect(selection.piece.position);

    SDL_RenderCopy(renderer, textureTiles, &tileSrcRect, &tileDstRect);

    
    std::vector<SDL_Point> moves = getMoves(selection.piece);
    std::vector<SDL_Point> captureMoves = getCaptureMoves(selection.piece);

    for(auto& move: moves){
      tileDstRect = getTileScreenRect({move.x, move.y});
      tileSrcRect = {2*SPRITE_TILE_SIZE, 0, SPRITE_TILE_SIZE, SPRITE_TILE_SIZE};
      
      SDL_RenderCopy(renderer, textureTiles, &tileSrcRect, &tileDstRect);
    }
    
    for(auto& move: captureMoves){
      tileDstRect = getTileScreenRect({move.x, move.y});
      tileSrcRect = {2*SPRITE_TILE_SIZE, SPRITE_TILE_SIZE, SPRITE_TILE_SIZE, SPRITE_TILE_SIZE};
      
      SDL_RenderCopy(renderer, textureTiles, &tileSrcRect, &tileDstRect);
    }
  }
  
}

SDL_Rect getPieceSrcRect(Piece& piece){
  return {
    piece.spritePosition.x*SPRITE_PIECE_SIZE,
    piece.spritePosition.y*SPRITE_PIECE_SIZE,
    SPRITE_PIECE_SIZE, SPRITE_PIECE_SIZE};}

SDL_Rect getPieceDstRect(Piece& piece){
  return getTileScreenRect(piece.position);}

void renderPieces(){
    
  for(auto& piece: pieces){
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

  pieces = initialPieces;

  
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
