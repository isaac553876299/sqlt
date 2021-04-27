#ifndef __DRAW_H__
#define __DRAW_H__

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"

#include "stdarg.h"

void RenderCopy(SDL_Renderer* renderer, float* camera, SDL_Texture* texture, SDL_Rect* srcrect, int dst_x, int dst_y, int dst_w, int dst_h, float size = 1.0f)
{
	SDL_Rect dst{ dst_x, dst_y, dst_w, dst_h };
	if (dst.w == 0 && dst.h == 0) SDL_QueryTexture(texture, 0, 0, &dst.w, &dst.h);
	dst.x = (dst.x + camera[0]) * camera[2];
	dst.y = (dst.y + camera[1]) * camera[2];
	dst.w = dst.w * size * camera[2];
	dst.h = dst.h * size * camera[2];
	dst = { int((dst.x + camera[0]) * camera[2]),int((dst.y + camera[1]) * camera[2]),int(dst.w * size * camera[2]),int(dst.h * size * camera[2]) };
	SDL_RenderCopy(renderer, texture, srcrect, &dst);
}

char* GetText(const char* _Format, ...)
{
	char result[4096];
	va_list vaargs;
	va_start(vaargs, _Format);
	vsprintf_s(result, 4096, _Format, vaargs);
	va_end(vaargs);
	return result;
}

void DrawFont(SDL_Renderer* renderer, float* camera, TTF_Font* font, int dst_x, int dst_y, int dst_w, int dst_h, const char* text, float size = 1.0f, SDL_Color color = { 0,0,0,255 })
{
	//TTF_RenderText_Shaded//TTF_RenderText_Blended
	RenderCopy(renderer, camera, SDL_CreateTextureFromSurface(renderer, TTF_RenderText_Solid(font, text, color)), 0, dst_x, dst_y, dst_w, dst_h, size);
}

#endif
