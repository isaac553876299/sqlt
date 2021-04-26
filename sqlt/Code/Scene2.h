#ifndef __SCENE2_H__
#define __SCENE2_H__

#include "Scenes.h"

class Scene2 : public Scene
{
public:
	bool Update(float dt);
	void Draw();
};

bool Scene2::Update(float dt)
{
	return true;
}

void Scene2::Draw()
{
	SDL_SetRenderDrawColor(renderer, 255, 128, 128, 255);
	SDL_RenderClear(renderer);
}

#endif