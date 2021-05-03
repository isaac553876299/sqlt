#ifndef __SCENES_H__
#define __SCENES_H__

#include "Share.h"

class Scene
{
public:
	Scene() {}
	~Scene() {}
	virtual bool Update() { return true; }
};

#endif

#ifndef __SCENE0_H__
#define __SCENE0_H__

class Scene0 : public Scene
{
public:

	Scene0();
	~Scene0();
	bool Update();

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

#endif

