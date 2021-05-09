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
	RELEASE,

};

enum class ControlType
{
	BUTTON,
	CHECKBOX,
	SLIDER,

};

class Control
{
public:
	ControlType type;
	const char* text;
	UIState state;
	SDL_Texture* texture;
	SDL_Rect rect;
	bool toggled;
	float value;

	Control(ControlType _type, const char* _text, const char* _texture, SDL_Rect _rect, UIState _state = UIState::NORMAL, bool _toggled = false, float _value = 0)
	{
		type = _type;
		text = _text;
		state = _state;
		texture = LoadTexture(_texture);
		rect = _rect;
		toggled = _toggled;
		value = _value;
	}

	~Control()
	{
		SDL_DestroyTexture(texture);
	}

	void Draw()
	{

	}
};

class UserInterface
{
public:

	List<Control*>* controls = nullptr;

	UserInterface();
	~UserInterface();
	void Update();
	void Draw();

};

UserInterface::UserInterface()
{

}

UserInterface::~UserInterface()
{

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
