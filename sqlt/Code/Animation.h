#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include "Draw.h"

class Animation
{
public:

	SDL_Texture* texture = nullptr;
	float speed;
	bool loop;
	SDL_Rect* frame = nullptr;

	float currentFrame;
	unsigned int totalFrames;

	Animation(const char* texture_file_path, float _speed = 0.1f, bool _loop = true, unsigned int max = 10)
	{
		texture = LoadTexture(texture_file_path);
		speed = _speed;
		loop = _loop;
		frame = new SDL_Rect[max];
		currentFrame = 0.0f;
		totalFrames = 0;
	}

	~Animation()
	{
		delete[] frame;
		frame = nullptr;
	}

	void PushBack(const SDL_Rect& rect)
	{
		frame[++totalFrames] = rect;
	}

	void Reset()
	{
		currentFrame = 0.0f;
	}

	void Update(float dt)
	{
		if (currentFrame < totalFrames) currentFrame += speed * dt;
		else if (loop) Reset();
	}

	void Draw(int x, int y)
	{
		if (&frame[int(currentFrame)]) RenderCopy(texture, x, y, &frame[int(currentFrame)]);
	}

};

#endif