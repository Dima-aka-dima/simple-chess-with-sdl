#ifndef GUI_HPP
#define GUI_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <functional>

#include "definitions.hpp"
#include "chess.hpp"

struct Window{
  SDL_Window* sdlWindow;
  SDL_Rect position;
  uint32_t flags;
  
  Window(){
    position = {10, 10, 1200, 800};
    flags = SDL_WINDOW_RESIZABLE;
    sdlWindow = SDL_CreateWindow("Simple chess GUI", position.x, position.y, position.w, position.h, flags);
  }
  
  void updateOnResize(){
    SDL_GetWindowSize(this->sdlWindow, &this->position.w, &this->position.h);};
  
  /* TODO: bugfix, sometimes doesn't correctly resize after exiting fullscreen*/
  void changeFullscreen(){
    flags = SDL_GetWindowFlags(sdlWindow);
    SDL_SetWindowFullscreen(sdlWindow, (flags & SDL_WINDOW_FULLSCREEN_DESKTOP) ?
			    0 : SDL_WINDOW_FULLSCREEN_DESKTOP);
  };
};


namespace GUI{

  
struct Board{
  SDL_Rect position;
  Chess::PieceColor side = Chess::White;
    
  void updateOnResize(Window* window){
    position = {(int) (0.05*window->position.h), (int) (0.05*window->position.h), 0, 0};
    int size = 0;
    if(window->position.h > window->position.w) size = 0.9 * window->position.h;
    else size = 0.9 * window->position.h;
    position.h = position.w = size;
  }

  SDL_Rect getTileScreenRect(SDL_Point position){
  
    int size = this->position.w / 8;
    if(side == Chess::White)
      return {
	this->position.x + position.x*size,
        this->position.y + (7 - position.y)*size,
	size, size};
    else
      return {
	this->position.x + position.x*size,
        this->position.y + position.y*size,
	size, size};
  }

  void reset(){ side = Chess::White; };
  
  void switchSide(){
    if(side == Chess::White) side = Chess::Black;
    else side = Chess::White;}

};

typedef struct{
  bool any = false;
  bool same = false;
  Chess::Piece piece = {};
} Selection;

typedef struct{
  bool any = false;
  Chess::Piece piece = {};
  SDL_Point position = {0, 0};
} Pickup;

struct Button{
  SDL_Rect position;
  SDL_Rect textPosition;
  const char* text;

  void render(SDL_Renderer* renderer){
    SetRenderDrawColor(renderer, (SDL_Color){200, 200, 200, 200});    
    SDL_RenderFillRect(renderer, &position);
    renderTextSolid(renderer, font, text, {0, 0, 0, 255}, &textPosition);
  }

  std::function<void(Button&, Window*, Board*)> __updateOnResize;
  
  void updateOnResize(Window* window, Board* board){__updateOnResize(*this, window, board);};
};
  
}


void updateResetButtonOnResize(GUI::Button& button, Window* window, GUI::Board* board){
  using namespace GUI;
  
  SDL_Rect rightSpace = {
    board->position.x + board->position.w,
    board->position.y,
    window->position.w - board->position.x - board->position.w,
    board->position.h
  };
  
  button.position = {
    rightSpace.x + (int)(0.05 * rightSpace.h),
    rightSpace.y + (int)(0.0 * rightSpace.h),
    (int) (0.5*rightSpace.h),
    (int) (0.1*rightSpace.h)
  };

  int textWidth, textHeight;
  TTF_SizeText(font, button.text, &textWidth, &textHeight);
    
  float alpha = 0.25;
  int textWidthScaled = alpha * button.position.w;
  int textHeightScaled = textWidthScaled * ((float)textHeight / (float)textWidth);
  
  button.textPosition = {
    button.position.x + (button.position.w - textWidthScaled)/2,
    button.position.y + (button.position.h - textHeightScaled)/2,
    textWidthScaled,
    textHeightScaled,
  };
}


void updateSwitchSideButtonOnResize(GUI::Button& button, Window* window, GUI::Board* board){
  using namespace GUI;
    
  SDL_Rect rightSpace = {
    board->position.x + board->position.w,
    board->position.y,
    window->position.w - board->position.x - board->position.w,
    board->position.h
  };
    
  button.position = {
    rightSpace.x + (int)(0.05 * rightSpace.h),
    rightSpace.y + (int)(0.15 * rightSpace.h),
    (int) (0.5*rightSpace.h),
    (int) (0.1*rightSpace.h)
  };

  int textWidth, textHeight;
  TTF_SizeText(font, button.text, &textWidth, &textHeight);
  
  float alpha = 0.5;
  int textWidthScaled = alpha * button.position.w;
  int textHeightScaled = textWidthScaled * ((float)textHeight / (float)textWidth);
  
  button.textPosition = {
    button.position.x + (button.position.w - textWidthScaled)/2,
    button.position.y + (button.position.h - textHeightScaled)/2,
    textWidthScaled,
    textHeightScaled,
  };
  
}


GUI::Button resetButton = {{}, {}, "Reset", updateResetButtonOnResize};
GUI::Button switchSideButton = {{}, {}, "Switch Side", updateSwitchSideButtonOnResize};


#endif
