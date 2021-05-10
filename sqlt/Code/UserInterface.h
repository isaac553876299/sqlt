#ifndef __USERINTERFACE_H__
#define __USERINTERFACE_H__

#include "SDL.h"
#include "SDL_image.h"
#include "List.h"
#include "Share.h"

enum class UIState
{
	DISABLE,
	NORMAL,
	FOCUS,
	PRESS,
	RELEASE
};

enum class ControlType
{
	BUTTON,
	CHECKBOX,
	SLIDER,

};

struct Control
{
	ControlType type;
	const char* text;
	UIState state;
	SDL_Rect rect;
};

struct Button : public Control
{

};

struct CheckBox : public Control
{
	bool toggled;
};

struct Slider : public Control
{
	float value;
};

class UserInterface
{
public:

	List<Control*>* controls = nullptr;

	UserInterface();
	~UserInterface();
	void AddControl(ControlType type, const char* text, int x, int y, int w, int h);
	void Update();
	void Draw();

};

UserInterface::UserInterface()
{

}

UserInterface::~UserInterface()
{

}

void UserInterface::AddControl(ControlType type, const char* text, int x, int y, int w, int h)
{
	Control* control = new Control{ type,text,UIState::NORMAL,{x,y,w,h} };
	controls->Add(control);
}

void UserInterface::Update()
{
	ListItem<Control*>* tmp = controls->start;
	while (tmp)
	{
		switch (tmp->data->type)
		{
		case ControlType::BUTTON:

			break;
		case ControlType::CHECKBOX:

			break;
		case ControlType::SLIDER:

			break;

		}
		tmp = tmp->next;
	}
}

void UserInterface::Draw()
{
	ListItem<Control*>* tmp = controls->start;
	while (tmp)
	{
		switch (tmp->data->type)
		{
		case ControlType::BUTTON:

			break;
		case ControlType::CHECKBOX:

			break;
		case ControlType::SLIDER:

			break;

		}
		tmp = tmp->next;
	}
}

#endif
