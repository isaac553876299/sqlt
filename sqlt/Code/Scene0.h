#ifndef __SCENE0_H__
#define __SCENE0_H__

#include "Scenes.h"

class Scene0 : public Scene
{
public:

	Scene0();
	~Scene0();
	void Update();
	void Draw();
	void OnCollision();

};

Scene0::Scene0()
{

}

Scene0::~Scene0()
{

}

void Scene0::Update()
{

}

void Scene0::Draw()
{
	SetRenderDrawColor(255, 0, 0, 255);
	RenderFillRect(0, 0, 50, 50, 1, 0);
	DrawFont(share.debugFont, 100, 200, GetText("GNYAA!!!!"), 2.0f, 1, 0, 0);
	DrawFont(share.debugFont, 100, 300, GetText("GNYAA!!!!"), 2.0f, 1, 0, 1);
	DrawFont(share.debugFont, 100, 400, GetText("GNYAA!!!!"), 2.0f, 1, 0, 2);
}

void Scene0::OnCollision()
{

}

#endif
