#ifndef __SHARE_H__
#define __SHARE_H__

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "SDL_mixer.h"

#include "stdarg.h"

#define KEY_IDLE(x) share.keyboard[x] == 0
#define KEY_DOWN(x) share.keyboard[x] == 1
#define KEY_REPEAT(x) share.keyboard[x] == 2
#define KEY_UP(x) share.keyboard[x] == 3

#define MOUSE_IDLE(x) share.mouse[x + 1] == 0
#define MOUSE_DOWN(x) share.mouse[x + 1] == 1
#define MOUSE_REPEAT(x) share.mouse[x + 1] == 2
#define MOUSE_UP(x) share.mouse[x + 1] == 3

struct Share
{
	SDL_Renderer* renderer = nullptr;
	int* mouse = nullptr;
	int* keyboard = nullptr;
	float* view = nullptr;
	float dt = 0.0f;

	TTF_Font* debugFont = nullptr;
};
Share share;

void RenderCopy(SDL_Texture* texture, SDL_Rect* srcrect, float _x, float _y, float size = 1.0f, bool use_camera = true, bool avoid_scale = false)
{
	float x = _x + share.view[0] * int(use_camera);
	float y = _y + share.view[1] * int(use_camera);
	int _w = 0;
	int _h = 0;
	SDL_QueryTexture(texture, 0, 0, &_w, &_h);
	float w = _w * size;
	float h = _h * size;
	if (avoid_scale)
	{
		x /= share.view[2];
		y /= share.view[2];
		w /= share.view[2];
		h /= share.view[2];
	}
	SDL_Rect dstrect{ x,y,w,h };
	SDL_RenderCopy(share.renderer, texture, srcrect, &dstrect);
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

void DrawFont(TTF_Font* font, int dst_x, int dst_y, const char* text, float size = 1.0f, bool use_camera = true, bool avoid_scale = false, int style = 0, SDL_Color fg = { 0,0,0,255 }, SDL_Color bg = { 255,255,255,255 })
{
	SDL_Surface* surface = nullptr;
	SDL_Texture* texture = nullptr;
	switch (style)
	{
	case 0: surface = TTF_RenderText_Solid(font, text, fg); texture = SDL_CreateTextureFromSurface(share.renderer, surface); break;
	case 1: surface = TTF_RenderText_Shaded(font, text, fg, bg); texture = SDL_CreateTextureFromSurface(share.renderer, surface); break;
	case 2: surface = TTF_RenderText_Blended(font, text, fg); texture = SDL_CreateTextureFromSurface(share.renderer, surface); break;
	}
	RenderCopy(texture, 0, dst_x, dst_y, size, use_camera, avoid_scale);
	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);

}

SDL_Texture* LoadTexture(const char* file)
{
	return IMG_LoadTexture(share.renderer, file);
}

void RenderFillRect(float _x, float _y, int _w, int _h, bool use_camera = true, bool avoid_scale = false)
{
	float x = _x + share.view[0] * int(use_camera);
	float y = _y + share.view[1] * int(use_camera);
	float w = _w;
	float h = _h;
	if (avoid_scale)
	{
		x /= share.view[2];
		y /= share.view[2];
		w /= share.view[2];
		h /= share.view[2];
	}
	SDL_Rect rect{ x,y,w,h };
	SDL_RenderFillRect(share.renderer, &rect);
}

void SetRenderDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	SDL_SetRenderDrawColor(share.renderer, r, g, b, a);
}

void RenderClear()
{
	SDL_RenderClear(share.renderer);
}

void RenderPresent()
{
	SDL_RenderPresent(share.renderer);
}

#endif
