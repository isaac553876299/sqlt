#include "App.h"

int main(int argc, char** argv)
{
	App* app = new App;
	while (app->Update());
	delete app;
	app = nullptr;
	return 0;
}
