#ifndef __DRAW_H__
#define __DRAW_H__

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"

#include "stdarg.h"

/*
void RenderCopy(SDL_Renderer* renderer, float* camera, SDL_Texture* texture, SDL_Rect* srcrect, int dst_x, int dst_y, int dst_w = 0, int dst_h = 0, float size = 1.0f)
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
*/

void RenderCopy(SDL_Renderer* renderer, float* camera, SDL_Texture* texture, SDL_Rect* srcrect, int dst_x, int dst_y, float size = 1.0f, bool use_camera = true, bool use_scale = true)
{
	SDL_Rect dstrect{ dst_x, dst_y, 0,0 };
	SDL_QueryTexture(texture, 0, 0, &dstrect.w, &dstrect.h);
	dstrect.w = dstrect.w * size;
	dstrect.h = dstrect.h * size;

	if (use_camera)
	{
		dstrect.x += camera[0];
		dstrect.y += camera[1];
	}
	if (use_scale)
	{
		dstrect.x *= camera[2];
		dstrect.y *= camera[2];
		dstrect.w *= camera[2];
		dstrect.h *= camera[2];
	}

	SDL_RenderCopy(renderer, texture, srcrect, &dstrect);
}

char* GetText(const char* _Format, ...)
{
	char _Buffer[4096];
	va_list _ArgList;
	va_start(_ArgList, _Format);
	vsprintf_s(_Buffer, 4096, _Format, _ArgList);
	va_end(_ArgList);
	return _Buffer;
}

void DrawFont(SDL_Renderer* renderer, float* camera, TTF_Font* font, int dst_x, int dst_y, const char* text, float size = 1.0f, int style = 0, SDL_Color fg = { 0,0,0,255 }, SDL_Color bg = { 255,255,255,255 })
{
	switch (style)
	{
	case 0: RenderCopy(renderer, camera, SDL_CreateTextureFromSurface(renderer, TTF_RenderText_Solid(font, text, fg)), 0, dst_x, dst_y, size); break;
	case 1: RenderCopy(renderer, camera, SDL_CreateTextureFromSurface(renderer, TTF_RenderText_Shaded(font, text, fg, bg)), 0, dst_x, dst_y, size); break;
	case 2: RenderCopy(renderer, camera, SDL_CreateTextureFromSurface(renderer, TTF_RenderText_Blended(font, text, fg)), 0, dst_x, dst_y, size); break;
	}
}

#endif
