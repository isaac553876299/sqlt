#ifndef __USERINTERFACE_H__
#define __USERINTERFACE_H__

#include "Share.h"
#include "List.h"

enum ControlState
{
	CONTROL_STATE_DISABLE,
	CONTROL_STATE_NORMAL,
	CONTROL_STATE_FOCUS,
	CONTROL_STATE_PRESS,
	CONTROL_STATE_RELEASE
};

enum ControlType
{
	CONTROL_TYPE_BUTTON,
	CONTROL_TYPE_CHECKBOX,
	CONTROL_TYPE_SLIDER,

};

class Control
{
public:
	ControlType type;
	const char* text;
	ControlState state;
	SDL_Rect rect;

	bool toggled;

	float value;

	Control(ControlType _type, const char* _text, ControlState _state, int x, int y, int w, int h)
	{
		type = _type;
		state = _state;
		text = _text;
		rect = { x,y,w,h };

		toggled = false;

		value = 0;
	}
	~Control()
	{

	}
	void Update()
	{

	}
	void Draw()
	{

	}
};

class UserInterface
{
public:
	List<Control*> controls;

	UserInterface();
	~UserInterface();
	void Update();
	void Draw();
	void AddControl(ControlType _type, const char* _text, ControlState _state, int x, int y, int w, int h);
};

UserInterface::UserInterface()
{

}

UserInterface::~UserInterface()
{
	controls.Clear();
}

void UserInterface::Update()
{
	SDL_Point mouse1 = { (share.mouse[0] - share.view[0]), (share.mouse[1] - share.view[1]) };
	SDL_Point mouse = { share.mouse[0],share.mouse[1] };
	ListItem<Control*>* control = controls.start;
	while (control)
	{
		if (control->data->state != CONTROL_STATE_DISABLE)
		{
			if (control->data->state == CONTROL_STATE_RELEASE)
			{
				control->data->state = CONTROL_STATE_NORMAL;
			}
			if (SDL_PointInRect(&mouse, &control->data->rect))
			{/*return;//no overlay, right¿*/
				if (control->data->state == CONTROL_STATE_NORMAL)
				{
					control->data->state = CONTROL_STATE_FOCUS;
				}
				if (MOUSE_DOWN(SDL_BUTTON_LEFT))
				{
					control->data->state = CONTROL_STATE_PRESS;
				}
				if (MOUSE_UP(SDL_BUTTON_LEFT))
				{
					control->data->state = CONTROL_STATE_RELEASE;
					/*callback*/
				}
			}
		}

		switch (control->data->type)
		{
		case CONTROL_TYPE_BUTTON:

			break;
		case CONTROL_TYPE_CHECKBOX:
			if (control->data->state == CONTROL_STATE_RELEASE)
			{
				control->data->toggled = !control->data->toggled;
			}
			break;
		case CONTROL_TYPE_SLIDER:
			if (control->data->state == CONTROL_STATE_PRESS)
			{
				if (share.mouse[0] > control->data->rect.x + control->data->rect.h / 2 &&/*rect.h/2 lack float precision*/
					share.mouse[0] < control->data->rect.x + control->data->rect.w - control->data->rect.h / 2)
				{
					control->data->value = float(share.mouse[0] - control->data->rect.x - control->data->rect.h / 2) / float(control->data->rect.w);
				}
			}
			break;
		}

		control = control->next;
	}
}

void UserInterface::Draw()
{
	ListItem<Control*>* control = controls.start;
	while (control)
	{
		switch (control->data->type)
		{
		case CONTROL_TYPE_BUTTON:
			SetRenderDrawColor(255, 255, 50 * (control->data->state + 1), 255);
			RenderFillRect(control->data->rect.x, control->data->rect.y, control->data->rect.w, control->data->rect.h, false, true);
			if (control->data->state == CONTROL_STATE_FOCUS)
			{
				SetRenderDrawColor(255, 50 * (control->data->state + 1), 255, 255);
				RenderFillRect(control->data->rect.x, control->data->rect.y, control->data->rect.w, control->data->rect.h, false, true);
			}
			break;
		case CONTROL_TYPE_CHECKBOX:
			SetRenderDrawColor(128, 128, 128, 255);
			RenderFillRect(control->data->rect.x, control->data->rect.y, control->data->rect.w, control->data->rect.h, false, true);
			if (control->data->toggled)
			{
				SetRenderDrawColor(255, 50 * (control->data->state + 1), 255, 255);
				RenderFillRect(control->data->rect.x, control->data->rect.y, control->data->rect.w, control->data->rect.h, false, true);
			}
			break;
		case CONTROL_TYPE_SLIDER:
			SetRenderDrawColor(128, 128, 128, 255);
			RenderFillRect(control->data->rect.x, control->data->rect.y, control->data->rect.w, control->data->rect.h, false, true);
			SetRenderDrawColor(50 * (control->data->state + 1), 255, 255, 255);//if w>h...
			RenderFillRect(control->data->rect.x + control->data->value * control->data->rect.w, control->data->rect.y, control->data->rect.h, control->data->rect.h, false, true);
			break;
		}
		control = control->next;
	}
}

void UserInterface::AddControl(ControlType _type, const char* _text, ControlState _state, int x, int y, int w, int h)
{
	Control* control = new Control(_type, _text, _state, x, y, w, h);
	controls.Add(control);
}

#endif
