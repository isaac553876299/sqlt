#ifndef __SCENES_H__
#define __SCENES_H__

#include <iostream>
#include "Draw.h"

extern SDL_Renderer* renderer;
extern int* mouse;
extern int* keyboard;

extern float dt;

class Scene
{
public:
	Scene() {}
	~Scene() {}
	virtual bool Update() { return false; }
	virtual void Draw() {}
};

#include "Scene1.h"
#include "Scene2.h"

class SceneManager
{
public:

	Scene* scene = nullptr;
	float alpha = 255;
	bool fad = false;

	SceneManager(int id);
	~SceneManager();

	void Update();

	void Fade(int id);
};

SceneManager::SceneManager(int id)
{
	Fade(id);
}

SceneManager::~SceneManager()
{
	delete scene;
	scene = nullptr;
}

void SceneManager::Update()
{
	if (keyboard[SDL_SCANCODE_ESCAPE] == 1);// isActive = false;
	if (keyboard[SDL_SCANCODE_0] == 1) Fade(0);
	if (keyboard[SDL_SCANCODE_1] == 1) Fade(1);
	if (keyboard[SDL_SCANCODE_2] == 1) Fade(2);

	if (scene) scene->Update();
	if (fad)
	{
		if (alpha > 0) alpha -= 100.0f * dt;
		else
		{
			alpha = 255;
			fad = false;
		}
	}

	if (scene) scene->Draw();
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, alpha);
	SDL_Rect rect{ 0,0,1280,720 };
	SDL_RenderFillRect(renderer, &rect);
	//SDL_RenderClear(renderer);
}

void SceneManager::Fade(int id)
{
	delete scene;
	scene = nullptr;
	switch (id)
	{
	case 0: scene = new Scene; break;
	case 1: scene = new Scene1; break;
	case 2: scene = new Scene2; break;
	}
	fad = true;
}

#endif
