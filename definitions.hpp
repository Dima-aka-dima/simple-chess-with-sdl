#ifndef DEFINITIONS_HPP
#define DEFINITIONS_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <functional>

#define WINDOW_INITIAL_WIDTH 1200
#define WINDOW_INITIAL_HEIGHT 800
#define SPRITE_PIECE_SIZE 450
#define SPRITE_TILE_SIZE 450

const SDL_Color BACKGROUND_COLOR = {255, 255, 255, 255};
const SDL_Color BLACK_TILE_COLOR = {118, 150, 86, 255};
const SDL_Color WHITE_TILE_COLOR = {238, 238, 210, 255};
const SDL_Color SELECTED_TILE_BLACK_COLOR = {187, 203, 43, 255};
const SDL_Color SELECTED_TILE_WHITE_COLOR =  {247, 247, 105, 255};

bool operator==(SDL_Point const& a, SDL_Point const& b){
  return (a.x == b.x) && (a.y == b.y);}

SDL_Point operator+(SDL_Point const& a, SDL_Point const& b){
  return (SDL_Point){a.x + b.x, a.y + b.y};}

SDL_Point operator-(SDL_Point const& a, SDL_Point const& b){
  return (SDL_Point){a.x - b.x, a.y - b.y};}

void SetRenderDrawColor(SDL_Renderer* renderer, SDL_Color color) {
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);}


SDL_Texture* loadTexture(SDL_Renderer* renderer, std::string path){
  SDL_Surface* surface = IMG_Load(path.c_str());
  SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_FreeSurface(surface);
  return texture;
};


void renderTextSolid(SDL_Renderer* renderer, TTF_Font* font, const char* text, SDL_Color c, SDL_Rect* r){
  SDL_Surface* textSurface = TTF_RenderText_Solid(font, text, c);
  SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
  SDL_FreeSurface(textSurface);

  SDL_RenderCopy(renderer, textTexture, NULL, r);
  SDL_DestroyTexture(textTexture);
}

TTF_Font* font;


enum PieceType{Rook, Knight, Bishop, King, Queen, Pawn};

enum PieceColor{Black, White};

typedef struct{
  PieceType type;
  PieceColor color;
  SDL_Point position;
  SDL_Point spritePosition;
} Piece;

enum GameMode{Free, Game};

struct Window{
  SDL_Window* sdlWindow;
  SDL_Rect position;
  uint32_t flags;
  
  Window(){
    position = {10, 10, WINDOW_INITIAL_WIDTH, WINDOW_INITIAL_HEIGHT};
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


Window* window = new Window();
SDL_Renderer* renderer;


std::vector<Piece> pieces;
PieceColor turn = White;
GameMode gameMode = Game;

namespace GUI{

  
struct Board{
  SDL_Rect position;
  PieceColor side;
  
  Board(){ side = White; }
  
  void updateOnResize(){
    position = {(int) (0.05*window->position.h), (int) (0.05*window->position.h), 0, 0};
    int size = 0;
    if(window->position.h > window->position.w) size = 0.9 * window->position.h;
    else size = 0.9 * window->position.h;
    position.h = position.w = size;
  }
};

typedef struct{
  bool any = false;
  bool same = false;
  Piece piece = {};
} Selection;

typedef struct{
  bool any = false;
  Piece piece = {};
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

  std::function<void(Button&)> __updateOnResize;
  
  void updateOnResize(){__updateOnResize(*this);};
};
  
}

GUI::Board* board = new GUI::Board();


void updateResetButtonOnResize(GUI::Button& button){
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


void updateSwitchSideButtonOnResize(GUI::Button& button){
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
  
bool isAnyPieceAt(SDL_Point position)
{
  for(auto& piece: pieces) if(piece.position == position) return true;
  return false;
}


Piece getPieceAt(SDL_Point position)
{
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


typedef struct{
  SDL_Point position = {0, 0};
} Mouse;


void switchSide(){
  if(board->side == White) board->side = Black;
  else board->side = White;}

void switchTurn(){
  if(turn == White) turn = Black;
  else turn = White;}

bool isWhite(SDL_Point p){return ((p.x + p.y) % 2 == 0);}


std::vector<Piece> initialPieces = {
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

void reset(){pieces = initialPieces; board->side = White;}


#endif
