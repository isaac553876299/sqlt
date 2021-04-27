#ifndef __SCENES_H__
#define __SCENES_H__

class Scene
{
public:
	Scene() {}
	~Scene() {}
	virtual bool Update(float dt) { return true; }
};

#endif

#ifndef __SCENE0_H__
#define __SCENE0_H__

class Scene0 : public Scene
{
public:

	Scene0();
	~Scene0();
	bool Update(float dt);

};

Scene0::Scene0()
{

}

Scene0::~Scene0()
{

}

bool Scene0::Update(float dt)
{

	return true;
}

#endif

