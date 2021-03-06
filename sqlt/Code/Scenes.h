#ifndef __SCENES_H__
#define __SCENES_H__

#include "Share.h"

#include "Map.h"

class Scene
{
public:
	Scene() {}
	~Scene() {}
	virtual void Update() {}
	virtual void Draw() {}
	virtual void OnCollision() {}
};

#include "Scene0.h"
#include "UserInterface.h"

enum SceneType
{
	NONE,
	SCENE_0,

};

class SceneManager
{
public:

	Scene* scene = nullptr;
	Scene* next_scene = nullptr;
	//SceneType scene_id;

	Map map;
	UserInterface userInterface;

	enum FadingState
	{
		FADING_STATE_NONE,
		FADING_STATE_TOBLACK,
		FADING_STATE_FROMBLACK,
	};

	float alpha = 0;
	FadingState fading_state = FADING_STATE_NONE;

	SceneManager();
	~SceneManager();
	void Update();
	void Draw();
	void ChangeScene(SceneType _id);
	void SwapScenePointer();
};

SceneManager::SceneManager()
{
	userInterface.AddControl(CONTROL_TYPE_BUTTON, "buttton", CONTROL_STATE_NORMAL, 500, 400, 200, 20);
	userInterface.AddControl(CONTROL_TYPE_CHECKBOX, "checkboxxx", CONTROL_STATE_NORMAL, 500, 500, 200, 20);
	userInterface.AddControl(CONTROL_TYPE_SLIDER, "sliderrr", CONTROL_STATE_NORMAL, 500, 600, 200, 20);
}

SceneManager::~SceneManager()
{
	delete scene;
	scene = nullptr;
	map.Clear();
	userInterface.Clear();
}

void SceneManager::Update()
{
	userInterface.Update();
	if (scene) scene->Update();

	if (fading_state == FADING_STATE_TOBLACK)
	{
		if (alpha <= 255) alpha += 200.0f * share.dt; //'<' may stuck at 0
		if (alpha > 255) //'else' results in flash
		{
			fading_state = FADING_STATE_FROMBLACK;
			alpha = 255;
			SwapScenePointer();
		}
	}
	if (fading_state == 2)
	{
		if (alpha >= 0) alpha -= 200.0f * share.dt; //'>' may stuck at 0
		if (alpha < 0) //'else' results in flash
		{
			fading_state = FADING_STATE_NONE;
			alpha = 0;
		}
	}
}

void SceneManager::Draw()
{
	SetRenderDrawColor(50, 50, 50, 255);
	RenderClear();

	DrawFont(share.debugFont, 100, 100, GetText("dt (%.3f) scale (%.1f) fade (%.1f)", share.dt, share.view[2], alpha), 2.0f, 1, 0, 0);

	userInterface.Draw();
	if (scene) scene->Draw();

	SetRenderDrawColor(0, 0, 0, alpha);
	RenderDrawRect(0, 0, 1280, 720, true, false, true);
	//RenderClear();//no alpha?
	RenderPresent();
}

void SceneManager::ChangeScene(SceneType _id)
{
	if (fading_state == FADING_STATE_NONE)
	{
		Timer create_scene_timer;
		switch (_id)
		{
		case NONE: next_scene = nullptr;
		case SCENE_0: next_scene = new Scene0; break;
		}
		printf("creating new scene: %.3f seconds (%d ms)\n", create_scene_timer.ReadS(), create_scene_timer.ReadMs());
		fading_state = FADING_STATE_TOBLACK;
	}
}

void SceneManager::SwapScenePointer()
{
	Timer change_scene_timer;
	delete scene;
	scene = nullptr;
	scene = next_scene;
	next_scene = nullptr;
	printf("changing scene: %.3f seconds (%d ms)\n", change_scene_timer.ReadS(), change_scene_timer.ReadMs());
}

#endif
