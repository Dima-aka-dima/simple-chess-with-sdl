/* TODO: Separate Chess as a game (like an engine) and GUI / interactions with GUI*/ 

#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "definitions.hpp"
#include "moves.hpp"

Window* window = new Window();
SDL_Renderer* renderer;
Board* board = new Board();


bool running = true;

TTF_Font* font;
SDL_Texture* texturePieces;
SDL_Texture* textureTiles;

Selection selection;
Mouse mouse;
Pickup picked;

// TODO: better button handling
void updateResetButtonScreenPosition(){

  SDL_Rect rightSpace = {
    board->position.x + board->position.w,
    board->position.y,
    window->position.w - board->position.x - board->position.w,
    board->position.h
  };
  
  resetButton.position = {
    rightSpace.x + (int)(0.05 * rightSpace.h),
    rightSpace.y + (int)(0.0 * rightSpace.h),
    (int) (0.5*rightSpace.h),
    (int) (0.1*rightSpace.h)
  };

  int textWidth, textHeight;
  TTF_SizeText(font, resetButton.text, &textWidth, &textHeight);
    
  float alpha = 0.25;
  int textWidthScaled = alpha * resetButton.position.w;
  int textHeightScaled = textWidthScaled * ((float)textHeight / (float)textWidth);
  
  resetButton.textPosition = {
    resetButton.position.x + (resetButton.position.w - textWidthScaled)/2,
    resetButton.position.y + (resetButton.position.h - textHeightScaled)/2,
    textWidthScaled,
    textHeightScaled,
  };
}

void updateSwitchSideButtonScreenPosition(){
  
  SDL_Rect rightSpace = {
    board->position.x + board->position.w,
    board->position.y,
    window->position.w - board->position.x - board->position.w,
    board->position.h
  };
    
  switchSideButton.position = {
    rightSpace.x + (int)(0.05 * rightSpace.h),
    rightSpace.y + (int)(0.15 * rightSpace.h),
    (int) (0.5*rightSpace.h),
    (int) (0.1*rightSpace.h)
  };

  int textWidth, textHeight;
  TTF_SizeText(font, switchSideButton.text, &textWidth, &textHeight);
  
  float alpha = 0.5;
  int textWidthScaled = alpha * switchSideButton.position.w;
  int textHeightScaled = textWidthScaled * ((float)textHeight / (float)textWidth);
  
  switchSideButton.textPosition = {
    switchSideButton.position.x + (switchSideButton.position.w - textWidthScaled)/2,
    switchSideButton.position.y + (switchSideButton.position.h - textHeightScaled)/2,
    textWidthScaled,
    textHeightScaled,
  };
  
}


SDL_Rect getTileScreenRect(SDL_Point tilePosition){
  int tileSize = board->position.w / 8;
  if(side == White)
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

void updatePickupOnUp(){
  if(!picked.any) return;
  picked.any = false;
  
  if(!SDL_PointInRect(&mouse.position, &board->position)) return;
  SDL_Point tilePosition = getTileIntersection(&mouse.position);

  std::vector<SDL_Point> moves = getMoves(picked.piece);
  std::vector<SDL_Point> captureMoves = getCaptureMoves(picked.piece);

  if(!isAnyPieceAt(tilePosition)){
    for(auto& move: moves)
      if(move == tilePosition){
	setPieceAt(picked.piece, tilePosition);
	deletePieceAt(picked.piece.position);
	// switchSide();
      }
    
    return;
  }

  if(getPieceAt(tilePosition).color == picked.piece.color) return;
    
  for(auto& move: captureMoves){
    if(move == tilePosition){

      setPieceAt(picked.piece, tilePosition);
      deletePieceAt(picked.piece.position);
      // switchSide();
      return;
    }
  }
  
  // TODO: make selection properly
  // selection.same = false;
}

void updatePickupOnDown(){
  if(!SDL_PointInRect(&mouse.position, &board->position)) return;

  SDL_Point tilePosition = getTileIntersection(&mouse.position);
  for(auto& piece: pieces)
    if(tilePosition == piece.position){
      picked.any = true;
      picked.piece = piece;
      picked.position = mouse.position;
    }
}

// TODO: Proper selection!
void updateSelectionOnDown(){
  bool was = selection.any;
  selection.any = false;
  if(!SDL_PointInRect(&mouse.position, &board->position)) return;

  SDL_Point tilePosition = getTileIntersection(&mouse.position);
  if(was && selection.piece.position == tilePosition)
    selection.same = true;

  for(auto& piece: pieces)
    if(tilePosition == piece.position){
      selection.any = true;
      selection.piece = piece;
    }  
}

void updateSelectionOnUp(){
  selection.any = false;
  if(!selection.any) return;
  if(selection.same){
    selection.any = false;
    selection.same = false;
  }
}


void handleInput(SDL_Event event){
  window->updateOnResize();
  board->updateOnResize(window);

  updateResetButtonScreenPosition();
  updateSwitchSideButtonScreenPosition();
  
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

bool isWhite(SDL_Point tile){return ((tile.x + tile.y) % 2 == 0);}

void renderButton(Button& button){
  SetRenderDrawColor(renderer, (SDL_Color){200, 200, 200, 200});

  SDL_RenderFillRect(renderer, &button.position);
  
  renderTextSolid(renderer, font, button.text, {0, 0, 0, 255}, &button.textPosition);

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
  while(running){

    handleInput(event);
    
    SetRenderDrawColor(renderer, BACKGROUND_COLOR);
    SDL_RenderClear(renderer);

    renderTiles();
    renderPieces();
    renderButton(resetButton);
    renderButton(switchSideButton);
    SDL_RenderPresent(renderer);
  }
}
