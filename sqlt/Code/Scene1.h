#ifndef __SCENE1_H__
#define __SCENE1_H__

#include "Scenes.h"

class Scene1 : public Scene
{
public:
	bool Update();
	void Draw();
};

bool Scene1::Update()
{
	return true;
}

void Scene1::Draw()
{
	SDL_SetRenderDrawColor(renderer, 128, 255, 128, 255);
	SDL_RenderClear(renderer);
}

#endif