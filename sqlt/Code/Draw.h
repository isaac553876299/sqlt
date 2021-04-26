#ifndef __DRAW_H__
#define __DRAW_H__

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"

extern SDL_Renderer* renderer;

void DrawFont(TTF_Font* font, SDL_Color color, float camerax, float cameray, float scale, int x, int y, float s, const char* text)
{
	//const unsigned int size = 512;
	//static char debug[size];
	//sprintf_s(debug, size, "FPS: %d", fps);
	SDL_Surface* textSurface = TTF_RenderText_Solid(font, text, color);//TTF_RenderText_Shaded
	SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
	SDL_Rect rect{ (x + camerax) / scale,(y + cameray) / scale,0,0 };
	SDL_QueryTexture(textTexture, 0, 0, &rect.w, &rect.h);
	rect.w *= s / scale;
	rect.h *= s / scale;
	SDL_RenderCopy(renderer, textTexture, 0, &rect);
	SDL_FreeSurface(textSurface);
	SDL_DestroyTexture(textTexture);
}

#endif
