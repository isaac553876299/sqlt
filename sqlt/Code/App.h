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

class App
{
public:

	bool windowShown;

	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;

	int* mouse = nullptr;
	int* keyboard = nullptr;

	float* camera = nullptr;
	float offsetx;
	float offsety;
	
	float dt;
	float _dt;
	Timer timer;
	Timer seconds_count;
	unsigned int fps_count = 0;
	unsigned int delays_forced = 0;

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
	renderer = SDL_CreateRenderer(window, -1, renderFlags);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	camera = (float*)calloc(3, sizeof(float));
	camera[0] = 0.0f;
	camera[1] = 0.0f;
	camera[2] = config_node.child("render").attribute("scale").as_float(1.0f);
	
	offsetx = 0.0f;
	offsety = 0.0f;

	mouse = (int*)calloc(5, sizeof(int));
	keyboard = (int*)calloc(200, sizeof(int));

	debugFont = TTF_OpenFont("Assets/Fonts/JMH Typewriter.ttf", 36);

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
	//--------------------------------------------------------------------------------------------- INPUT

	int keyMap[2][4] = { 0,3,3,0,1,2,2,1 };

	const Uint8* keyboardState = SDL_GetKeyboardState(0);
	for (int i = 0; i < 200; ++i) keyboard[i] = keyMap[int(keyboardState[i])][keyboard[i]];

	Uint32 mouseState = SDL_GetMouseState(&mouse[0], &mouse[1]);
	for (int i = 2; i < 5; ++i) mouse[i] = keyMap[int((SDL_BUTTON(i - 1) & mouseState) > 0)][mouse[i]];

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
				camera[2] += 0.1f;
				camera[0] = (mouse[0] - offsetx) / camera[2];
				camera[1] = (mouse[1] - offsety) / camera[2];
			}
			if (event.wheel.y < 0)
			{
				camera[2] -= 0.1f;
				camera[0] /= camera[2];
				camera[1] /= camera[2];
			}
			break;
		case SDL_MOUSEMOTION:
			/*data.mouseMotion.x = event.motion.xrel / scale;
			data.mouseMotion.y = event.motion.yrel / scale;
			data.mousePosition.x = event.motion.x / scale;
			data.mousePosition.y = event.motion.y / scale;*/
			break;
		}

	SDL_RenderSetScale(renderer, camera[2], camera[2]);

	if (mouse[2] == 1)
	{
		offsetx = mouse[0] - camera[0] * camera[2];
		offsety = mouse[1] - camera[1] * camera[2];
	}
	if (mouse[2] == 2)
	{
		camera[0] = (mouse[0] - offsetx) / camera[2];
		camera[1] = (mouse[1] - offsety) / camera[2];
	}

	//--------------------------------------------------------------------------------------------- FPS
	
	dt = timer.ReadS();
	timer.Start();
	//(_dt - dt > 0)
	if (dt < _dt)
	{
		SDL_Delay((_dt - dt) * 1000);
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

	//--------------------------------------------------------------------------------------------- STEP
	
	if (keyboard[SDL_SCANCODE_0] == 1) Fade(0);

	SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
	SDL_RenderClear(renderer);

	if (scene) scene->Update(dt);
	if (fading_state == 1)
	{
		if (alpha < 255) alpha += 200.0f * dt;
		else fading_state = 2;
	}
	if (fading_state == 2)
	{
		if (alpha > 0) alpha -= 200.0f * dt;
		else fading_state = 0;
	}

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, alpha);
	SDL_Rect rect{ 0,0,1280,720 };
	SDL_RenderFillRect(renderer, &rect);
	//SDL_RenderClear(renderer);

	DrawFont(renderer, camera, debugFont, 100, 100, GetText("dt (%.3f) scale (%.1f) fade (%.2f)", dt, camera[2], alpha), 1.0f, 0);
	DrawFont(renderer, camera, debugFont, 100, 200, GetText("GNYAA!!!!"), 1.0f, 1, { 255,128,128,255 }, { 255,255,255,128 });
	DrawFont(renderer, camera, debugFont, 100, 300, GetText("GNYAA!!!!"), 1.0f, 1, { 128,128,255,255 }, { 255,255,255,128 });
	DrawFont(renderer, camera, debugFont, 100, 400, GetText("GNYAA!!!!"), 1.0f, 1, { 128,255,128,255 }, { 255,255,255,128 });
	DrawFont(renderer, camera, debugFont, 100, 500, GetText("GNYAA!!!!"), 1.0f, 2, { 255,255,255,50 });
	DrawFont(renderer, camera, debugFont, 100, 600, GetText("GNYAA!!!!"), 1.0f, 0, { 255,255,255,50 });

	SDL_RenderPresent(renderer);

	return true;
}

void App::Fade(int id)
{
	delete scene;
	scene = nullptr;
	switch (id)
	{
	case 0: scene = new Scene0; break;
	}
	fading_state = 1;
}

#endif
