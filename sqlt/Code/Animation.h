#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include "Share.h"

class Animation
{
public:

	SDL_Texture* texture = nullptr;
	float speed;
	bool loop;
	SDL_Rect* frames = nullptr;

	float currentFrame;
	unsigned int totalFrames;

	Animation(const char* texture_file, float _speed = 0.1f, bool _loop = true, unsigned int max = 10)
	{
		texture = LoadTexture(texture_file);
		speed = _speed;
		loop = _loop;
		frames = new SDL_Rect[max];
		currentFrame = 0.0f;
		totalFrames = 0;
	}

	~Animation()
	{
		SDL_DestroyTexture(texture);
		delete[] frames;
		frames = nullptr;
	}

	void PushBack(const SDL_Rect& rect, int repeats = 1)
	{
		SDL_Rect r = rect;
		for (int i = 0; i < repeats; ++i)
		{
			r.x += r.w * i;
			frames[++totalFrames] = r;
		}
	}

	void Reset()
	{
		currentFrame = 0.0f;
	}

	void Update()
	{
		if (currentFrame < totalFrames) currentFrame += speed * share.dt;
		else if (loop) Reset();
	}

	void Draw(int x, int y)
	{
		if (&frames[int(currentFrame)]) RenderCopy(texture, &frames[int(currentFrame)], x, y);
	}

};

#endif