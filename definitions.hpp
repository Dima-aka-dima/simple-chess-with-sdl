#ifndef DEFINITIONS_HPP
#define DEFINITIONS_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>

#define SPRITE_PIECE_SIZE 450
#define SPRITE_TILE_SIZE 450

const SDL_Color BACKGROUND_COLOR = {255, 255, 255, 255};
const SDL_Color BLACK_TILE_COLOR = {118, 150, 86, 255};
const SDL_Color WHITE_TILE_COLOR = {238, 238, 210, 255};
const SDL_Color SELECTED_TILE_BLACK_COLOR = {187, 203, 43, 255};
const SDL_Color SELECTED_TILE_WHITE_COLOR =  {247, 247, 105, 255};

#define SWITCH_SIDE_MODE false

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

enum GameMode{Free, Game};



typedef struct{
  SDL_Point position = {0, 0};
} Mouse;



#endif
