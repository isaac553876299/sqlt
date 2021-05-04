#ifndef __SCENES_H__
#define __SCENES_H__

#include "Share.h"

class Scene
{
public:
	Scene() {}
	~Scene() {}
	virtual bool Update() { return true; }
	virtual void OnCollision() {}
};

#endif
