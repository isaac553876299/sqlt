#ifndef __SCENES_H__
#define __SCENES_H__

#include "Share.h"

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
	SCENE_0,

};

class SceneManager
{
public:

	Scene* scene = nullptr;
	Scene* next_scene = nullptr;
	//SceneType scene_id;

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

	UserInterface* ui = new UserInterface;
};

SceneManager::SceneManager()
{
	ui->controls.Add(new Control(CONTROL_TYPE_SLIDER, "sliderr", CONTROL_STATE_NORMAL, 500, 500, 200, 20));
	ui->controls.Add(new Control(CONTROL_TYPE_CHECKBOX, "sliderr", CONTROL_STATE_NORMAL, 500, 400, 200, 20));
	ui->controls.Add(new Control(CONTROL_TYPE_BUTTON, "sliderr", CONTROL_STATE_NORMAL, 500, 600, 200, 20));
}

SceneManager::~SceneManager()
{
	delete scene;
	scene = nullptr;
}

void SceneManager::Update()
{
	ui->Update();
	if (scene) scene->Update();

	if (fading_state == FADING_STATE_TOBLACK)
	{
		if (alpha <= 255) alpha += 200.0f * share.dt; //'<' may stuck at 0
		if (alpha > 255) //'else' results in flash
		{
			fading_state = FADING_STATE_FROMBLACK;
			alpha = 255;

			Timer change_scene_timer;
			delete scene;
			scene = nullptr;
			scene = next_scene;
			next_scene = nullptr;
			//for (int i = 0; i < 999999; ++i);
			printf("changing scene: %.3f seconds (%d ms)\n", change_scene_timer.ReadS(), change_scene_timer.ReadMs());
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

	ui->Draw();
	if (scene) scene->Draw();

	SetRenderDrawColor(0, 0, 0, alpha);
	RenderFillRect(0, 0, 1280, 720, 0, 1);
	//RenderClear();//no alpha?
	RenderPresent();
}

void SceneManager::ChangeScene(SceneType _id)
{
	Timer create_scene_timer;
	switch (_id)
	{
	case SCENE_0: next_scene = new Scene0; break;
	}
	//for (int i = 0; i < 999999; ++i);
	printf("creating new scene: %.3f seconds (%d ms)\n", create_scene_timer.ReadS(), create_scene_timer.ReadMs());
	fading_state = FADING_STATE_TOBLACK;
}

#endif
