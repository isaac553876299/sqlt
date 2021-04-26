#ifndef __APP_H__
#define __APP_H__

#include <iostream>
#include "string.h"
#include "time.h"
#include "pugixml.hpp"
#include "Timers.h"
#include "List.h"
#include "Draw.h"
#include "SDL_mixer.h"
#include "Scenes.h"

extern SDL_Renderer* renderer;

class App
{
public:

	bool isActive;

	SDL_Window* window = nullptr;

	SDL_Rect camera;
	float scale;
	bool windowShown;

	float camerax;
	float cameray;
	float offsetx;
	float offsety;

	float _dt;
	float dt;
	Timer timer;

	int* mouse = nullptr;
	int* keyboard = nullptr;

	SceneManager* sceneManager = nullptr;

	TTF_Font* font = nullptr;

	App();
	~App();

	bool Update();

};

App::App()
{
	isActive = true;
	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_PNG);
	//Mix_Init();
	TTF_Init();

	font = TTF_OpenFont("Assets/Fonts/Wedgie Regular.ttf", 36);

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

	scale = config_node.child("render").attribute("scale").as_float(1.0f);
	_dt = float(1.0f / config_node.child("render").attribute("fps").as_int(30));
	Uint32 renderFlags = SDL_RENDERER_ACCELERATED;
	if (config_node.child("render").attribute("vsync").as_bool(false)) renderFlags |= SDL_RENDERER_PRESENTVSYNC;
	renderer = SDL_CreateRenderer(window, -1, renderFlags);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	camera = { 0,0,int(width),int(height) };
	camerax = 0.0f;
	cameray = 0.0f;
	offsetx = 0.0f;
	offsety = 0.0f;

	mouse = (int*)calloc(7, sizeof(int));
	keyboard = (int*)calloc(200, sizeof(int));

	sceneManager = new SceneManager(1);

}

App::~App()
{
	free(mouse);
	free(keyboard);

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	TTF_Quit();
	Mix_Quit();
	IMG_Quit();
	SDL_Quit();
}

bool App::Update()
{
	/*
	//if (mouse[2] == 1) mouse[2] = 2;
	//if (mouse[3] == 1) mouse[3] = 2;
	//if (mouse[2] == 3) mouse[2] = 0;
	//if (mouse[3] == 3) mouse[3] = 0;

	SDL_GetMouseState(&mouse[0], &mouse[1]);
	SDL_GetRelativeMouseState(&mouse[0], &mouse[1]);

	SDL_Event event;
	while (SDL_PollEvent(&event))
		switch (event.type)
		{
		case SDL_QUIT:
			isActive = false;
			break;
		case SDL_MOUSEBUTTONDOWN:
			//if (event.button.button == SDL_BUTTON_LEFT) mouse[2] = 1;
			//if (event.button.button == SDL_BUTTON_RIGHT) mouse[3] = 1;
			//if (event.button.button == SDL_BUTTON_MIDDLE) scale = 1.0f;
			//mouse[event.button.button - 1 + 2] = 1;
			break;
		case SDL_MOUSEBUTTONUP:
			//if (event.button.button == SDL_BUTTON_LEFT) mouse[2] = 3;
			//if (event.button.button == SDL_BUTTON_RIGHT) mouse[3] = 3;
			//mouse[event.button.button - 1 + 2] = 3;
			break;
		case SDL_MOUSEWHEEL:
			//if (event.wheel.y > 0) scale += 0.1f;
			//if (event.wheel.y < 0) scale -= 0.1f;
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
		case SDL_MOUSEMOTION:
			/*data.mouseMotion.x = event.motion.xrel / scale;
			data.mouseMotion.y = event.motion.yrel / scale;
			data.mousePosition.x = event.motion.x / scale;
			data.mousePosition.y = event.motion.y / scale;
			break;
		}

	const Uint8* keys = SDL_GetKeyboardState(0);

	int keyMap[2][4] = { 0,3,3,0,1,2,2,1 };
	for (int i = 0; i < 200; ++i) keyboard[i] = keyMap[(int)(keys[i])][keyboard[i]];
	for (int i = 2; i < 7; ++i) mouse[i] = keyMap[SDL_GetRelativeMouseState(0, 0) & SDL_BUTTON(i - 1)][mouse[i]];

	SDL_RenderSetScale(renderer, scale, scale);

	if (mouse[2] == 1)
	{
		offsetx = mouse[0] - camerax * scale;
		offsety = mouse[1] - cameray * scale;
	}
	if (mouse[2] == 2)
	{
		camerax = (mouse[0] - offsetx) / scale;
		cameray = (mouse[1] - offsety) / scale;
	}*/
	//--------------------------------------------------------------------------------------------- INPUT

	//SDL_GetMouseState(&mouse[0], &mouse[1]);
	Uint32 mouseState = SDL_GetRelativeMouseState(&mouse[0], &mouse[1]);

	const Uint8* keyboardState = SDL_GetKeyboardState(0);

	int keyMap[2][4] = { 0,3,3,0,1,2,2,1 };
	for (int i = 0; i < 200; ++i) keyboard[i] = keyMap[int(keyboardState[i])][keyboard[i]];
	for (int i = 2; i < 7; ++i)
	{
		std::cout << int(mouseState & SDL_BUTTON(i - 1)) << std::endl;
		//mouse[i] = keyMap[int(SDL_BUTTON(i - 1) & mouseState)][mouse[i]];
	}
	system("cls");

	SDL_Event event;
	while (SDL_PollEvent(&event))
		switch (event.type)
		{
		case SDL_QUIT:
			isActive = false;
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
		case SDL_MOUSEMOTION:
			/*data.mouseMotion.x = event.motion.xrel / scale;
			data.mouseMotion.y = event.motion.yrel / scale;
			data.mousePosition.x = event.motion.x / scale;
			data.mousePosition.y = event.motion.y / scale;*/
			break;
		}

	SDL_RenderSetScale(renderer, scale, scale);

	if (mouse[2] == 1)
	{
		offsetx = mouse[0] - camerax * scale;
		offsety = mouse[1] - cameray * scale;
	}
	if (mouse[2] == 2)
	{
		camerax = (mouse[0] - offsetx) / scale;
		cameray = (mouse[1] - offsety) / scale;
	}

	//--------------------------------------------------------------------------------------------- FPS
	
	dt = timer.ReadS();
	timer.Start();
	//(_dt - dt > 0)
	if (dt < _dt)
	{
		SDL_Delay(_dt - dt);
	}
	/*system("cls");

	/*static char title[256];
	sprintf_s(title, 256, " | dt: %.3f | ", dt);
	SDL_SetWindowTitle(window, title);*/

	//--------------------------------------------------------------------------------------------- STEP
	
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderClear(renderer);

	sceneManager->Update(dt, mouse, keyboard);

	//--------------------------------------------------------------------------------------------- DEBUG

	const unsigned int size = 512;
	char debug[size];
	sprintf_s(debug, size, "dt: %.5f", dt);
	DrawFont(font, { 255,255,255,255 }, 0, 0, 1, 100, 100, 1, debug);
	sprintf_s(debug, size, "_dt: %.3f", _dt);
	DrawFont(font, { 255,255,255,255 }, 0, 0, 1, 100, 150, 1, debug);
	sprintf_s(debug, size, "fps: %.3f", float(1.0f / dt));
	DrawFont(font, { 255,255,255,255 }, 0, 0, 1, 100, 200, 1, debug);
	sprintf_s(debug, size, "alpha: %.2f", sceneManager->alpha);
	DrawFont(font, { 255,255,255,255 }, 0, 0, 1, 100, 250, 1, debug);
	sprintf_s(debug, size, "fad: %d", sceneManager->fad);
	DrawFont(font, { 255,255,255,255 }, 0, 0, 1, 100, 300, 1, debug);

	SDL_RenderPresent(renderer);

	//--------------------------------------------------------------------------------------------- RETURN
	return isActive;
}

#endif
