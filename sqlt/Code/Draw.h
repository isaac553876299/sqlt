#ifndef __DRAW_H__
#define __DRAW_H__

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"

#include "stdarg.h"

extern SDL_Renderer* renderer;
extern float scale;
extern float camerax;
extern float cameray;
extern TTF_Font* fonts[];

SDL_Texture* LoadTexture(const char* file_path)
{
	return IMG_LoadTexture(renderer, file_path);
}

void RenderCopy(SDL_Texture* texture, int x, int y, const SDL_Rect* srcrect = nullptr, float size = 1.0f, bool use_camera = true, bool use_scale = true)
{
	const SDL_Rect dstrect =
	{
		(x + int(use_camera) * camerax) / scale,
		(y + int(use_camera) * cameray) / scale,
		srcrect->w * size * int(use_scale) * scale,
		srcrect->h * size * int(use_scale) * scale
	};
	SDL_RenderCopy(renderer, texture, srcrect, &dstrect);
}

char* GetText(const char* _Format, ...)
{
	char tmp_string[4096];
	char tmp_string2[4096];
	va_list  vaargs;

	va_start(vaargs, _Format);
	vsprintf_s(tmp_string, 4096, _Format, vaargs);
	va_end(vaargs);
	sprintf_s(tmp_string2, 4096, _Format, tmp_string);
	return tmp_string2;
}

void DrawFont(int x, int y, float s, const char* text, SDL_Color color = { 0,0,0,255 }, unsigned int font_id = 0)
{
	SDL_Surface* textSurface = TTF_RenderText_Solid(fonts[0], text, color);//TTF_RenderText_Shaded//TTF_RenderText_Blended
	SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
	/*SDL_Rect rect{ (x + camerax) / scale,(y + cameray) / scale,0,0 };
	SDL_QueryTexture(textTexture, 0, 0, &rect.w, &rect.h);
	rect.w *= s / scale;
	rect.h *= s / scale;*/
	RenderCopy(textTexture, x, y, 0, s, false, false);
	SDL_FreeSurface(textSurface);
	SDL_DestroyTexture(textTexture);
}

void SetColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	SDL_SetRenderDrawColor(renderer, r, g, b, a);
}

void RenderClear()
{
	SDL_RenderClear(renderer);
}

void RenderPresent()
{
	SDL_RenderPresent(renderer);
}

#endif
