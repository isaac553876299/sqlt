#include "App.h"

SDL_Renderer* renderer = nullptr;
//float scale;

int main(int argc, char** argv)
{
	App* app = new App;
	while (app->Update());
	delete app;
	app = nullptr;
	return 0;
}
