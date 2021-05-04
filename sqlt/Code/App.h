#ifndef __APP_H__
#define __APP_H__

#include <iostream>
#include "time.h"
#include "pugixml.hpp"
#include "Timers.h"
#include "List.h"
#include "Share.h"
#include "Scenes.h"

class App
{
public:

	bool windowShown;

	SDL_Window* window = nullptr;

	float offsetx;
	float offsety;
	
	float _dt;
	Timer timer;
	Timer seconds_count;
	unsigned int fps_count;
	unsigned int delays_forced;

	Scene* scene = nullptr;
	float alpha;
	int fading_state;

	App();
	~App();

	bool Update();
	void Fade(int id);

	TTF_Font* debugFont = nullptr;

};

App::App()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_PNG);
	Mix_Init(MIX_INIT_OGG);
	TTF_Init();

	pugi::xml_document config_doc;
	pugi::xml_node config_node;

	pugi::xml_parse_result result = config_doc.load_file("config.xml");
	if (!result) printf("ERROR while loading config.xml file: %s", result.description());
	//if (!config_doc.load_file("config.xml")) printf("ERROR while loading config.xml file: %s", result.description());
	config_node = config_doc.child("config");

	Uint32 windowFlags = SDL_WINDOW_SHOWN;
	if (config_node.child("window").attribute("fullscreen").as_bool(false)) windowFlags |= SDL_WINDOW_FULLSCREEN;
	if (config_node.child("window").attribute("borderless").as_bool(false)) windowFlags |= SDL_WINDOW_BORDERLESS;
	if (config_node.child("window").attribute("resizable").as_bool(false)) windowFlags |= SDL_WINDOW_RESIZABLE;
	unsigned int width = config_node.child("window").attribute("width").as_int(640);
	unsigned int height = config_node.child("window").attribute("height").as_int(360);
	char* title = (char*)config_node.child("window").attribute("title").as_string("no_title");
	window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, windowFlags);
	windowShown = true;

	_dt = float(1.0f / config_node.child("render").attribute("fps").as_int(30));
	Uint32 renderFlags = SDL_RENDERER_ACCELERATED;
	if (config_node.child("render").attribute("vsync").as_bool(false)) renderFlags |= SDL_RENDERER_PRESENTVSYNC;
	share.renderer = SDL_CreateRenderer(window, -1, renderFlags);
	SDL_SetRenderDrawBlendMode(share.renderer, SDL_BLENDMODE_BLEND);

	share.view = (float*)calloc(3, sizeof(float));
	share.view[0] = 0.0f;
	share.view[1] = 0.0f;
	share.view[2] = config_node.child("render").attribute("scale").as_float(1.0f);
	
	offsetx = 0.0f;
	offsety = 0.0f;

	share.mouse = (int*)calloc(5, sizeof(int));
	share.keyboard = (int*)calloc(200, sizeof(int));

	fps_count = 0;
	delays_forced = 0;

	alpha = 0;
	fading_state = 0;

	debugFont = TTF_OpenFont("Assets/Fonts/JMH Typewriter.ttf", 36);

}

App::~App()
{
	free(share.mouse);
	free(share.keyboard);
	free(share.view);

	SDL_DestroyRenderer(share.renderer);
	SDL_DestroyWindow(window);

	TTF_Quit();
	Mix_Quit();
	IMG_Quit();
	SDL_Quit();
}

bool App::Update()
{
	//--------------------------------------------------------------------------------------------- INPUT

	int keyMap[2][4] = { 0,3,3,0,1,2,2,1 };

	const Uint8* keyboardState = SDL_GetKeyboardState(0);
	for (int i = 0; i < 200; ++i) share.keyboard[i] = keyMap[int(keyboardState[i])][share.keyboard[i]];

	Uint32 mouseState = SDL_GetMouseState(&share.mouse[0], &share.mouse[1]);
	for (int i = 2; i < 5; ++i) share.mouse[i] = keyMap[int((SDL_BUTTON(i - 1) & mouseState) > 0)][share.mouse[i]];

	SDL_Event event;
	while (SDL_PollEvent(&event))
		switch (event.type)
		{
		case SDL_QUIT:
			return false;
			break;
		case SDL_WINDOWEVENT_HIDDEN:
		case SDL_WINDOWEVENT_MINIMIZED:
		case SDL_WINDOWEVENT_FOCUS_LOST:
			windowShown = false;
			break;
		case SDL_WINDOWEVENT_SHOWN:
		case SDL_WINDOWEVENT_MAXIMIZED:
		case SDL_WINDOWEVENT_FOCUS_GAINED:
		case SDL_WINDOWEVENT_RESTORED:
			windowShown = true;
			break;
		case SDL_MOUSEWHEEL:
			if (event.wheel.y > 0)
			{
				share.view[2] += 0.1f;
			}
			if (event.wheel.y < 0)
			{
				share.view[2] -= 0.1f;
			}
			break;
		case SDL_MOUSEMOTION:
			/*data.mouseMotion.x = event.motion.xrel / scale;
			data.mouseMotion.y = event.motion.yrel / scale;
			data.mousePosition.x = event.motion.x / scale;
			data.mousePosition.y = event.motion.y / scale;*/
			break;
		}

	SDL_RenderSetScale(share.renderer, share.view[2], share.view[2]);

	if (share.mouse[2] == 1)
	{
		offsetx = share.mouse[0] - share.view[0] * share.view[2];
		offsety = share.mouse[1] - share.view[1] * share.view[2];
	}
	if (share.mouse[2] == 2)
	{
		share.view[0] = (share.mouse[0] - offsetx) / share.view[2];
		share.view[1] = (share.mouse[1] - offsety) / share.view[2];
	}

	//--------------------------------------------------------------------------------------------- FPS
	
	share.dt = timer.ReadS();
	timer.Start();
	//(_dt - dt > 0)
	if (share.dt < _dt)
	{
		SDL_Delay((_dt - share.dt) * 1000);
		++delays_forced;
	}

	++fps_count;
	if (seconds_count.ReadS() > 1)
	{
		seconds_count.Start();
		system("cls");
		printf("%d___%d", fps_count, delays_forced);
		fps_count = 0;
		delays_forced = 0;
	}
	/*static char title[256];
	sprintf_s(title, 256, " | dt: %.3f | ", dt);
	SDL_SetWindowTitle(window, title);*/

	if (KEY_DOWN(SDL_SCANCODE_0)) Fade(0);

	SetRenderDrawColor(255, 255, 255, 255);
	RenderClear();

	if (scene) scene->Update();
	if (fading_state == 1)
	{
		if (alpha <= 255) alpha += 200.0f * share.dt; //'<' may stuck at 0
		if (alpha > 255) //'else' results in flash
		{
			fading_state = 2;
			alpha = 255;
		}
	}
	if (fading_state == 2)
	{
		if (alpha >= 0) alpha -= 200.0f * share.dt; //'>' may stuck at 0
		if (alpha < 0) //'else' results in flash
		{
			fading_state = 0;
			alpha = 0;
		}
	}

	SetRenderDrawColor(255, 0, 0, 255);
	RenderFillRect(0, 0, 50, 50, 1, 0);
	DrawFont(debugFont, 100, 100, GetText("dt (%.3f) scale (%.1f) fade (%.1f)", share.dt, share.view[2], alpha), 2.0f, 1, 0, 0);
	DrawFont(debugFont, 100, 200, GetText("GNYAA!!!!"), 2.0f, 1, 0, 0);
	DrawFont(debugFont, 100, 300, GetText("GNYAA!!!!"), 2.0f, 1, 0, 1);
	DrawFont(debugFont, 100, 400, GetText("GNYAA!!!!"), 2.0f, 1, 0, 2);

	SetRenderDrawColor(0, 0, 0, alpha);
	RenderFillRect(0, 0, 1280, 720, 0, 1);
	//RenderClear();//no alpha?
	RenderPresent();

	return true;
}

void App::Fade(int id)
{
	delete scene;
	scene = nullptr;
	switch (id)
	{
	case 0: scene = new Scene0; break;
	case 1: scene = new Scene0; break;
	case 2: scene = new Scene0; break;
	case 3: scene = new Scene0; break;
	}
	fading_state = 1;
}

#endif
