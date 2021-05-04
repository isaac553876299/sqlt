#ifndef __SCENE0_H__
#define __SCENE0_H__

#include "Scenes.h"

class Scene0 : public Scene
{
public:

	Scene0();
	~Scene0();
	bool Update();
	void OnCollision();

};

Scene0::Scene0()
{

}

Scene0::~Scene0()
{

}

bool Scene0::Update()
{

	return true;
}

void Scene0::OnCollision()
{

}

#endif
