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

struct Control
{
	ControlType type;
	const char* text;
	ControlState state;
	SDL_Rect rect;

	bool toggled;

	float value;

};

class UserInterface
{
public:
	List<Control*> controls;

	UserInterface();
	~UserInterface();
	void Clear();

	void Update();
	void Draw();
	void AddControl(ControlType _type, const char* _text, ControlState _state, int x, int y, int w, int h);
};

UserInterface::UserInterface()
{

}

UserInterface::~UserInterface()
{
	Clear();
}

void UserInterface::Clear()
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
			if (control->data->state == CONTROL_STATE_RELEASE || control->data->state == CONTROL_STATE_FOCUS)
			{
				control->data->state = CONTROL_STATE_NORMAL;
			}
			if (SDL_PointInRect(&mouse, &control->data->rect))
			{/*return;//no overlay, right¿*/
				if (control->data->state == CONTROL_STATE_NORMAL)
				{
					control->data->state = CONTROL_STATE_FOCUS;
					if (MOUSE_DOWN(SDL_BUTTON_LEFT))
					{
						control->data->state = CONTROL_STATE_PRESS;
					}
				}
			}
			if (MOUSE_UP(SDL_BUTTON_LEFT) && control->data->state == CONTROL_STATE_PRESS)
			{
				control->data->state = CONTROL_STATE_RELEASE;
				/*callback*/
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
				control->data->value = float(share.mouse[0] - control->data->rect.x) / float(control->data->rect.w);
				if (control->data->value < 0) control->data->value = 0;
				if (control->data->value > 1) control->data->value = 1;
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
			if (control->data->state == CONTROL_STATE_PRESS)
			{
				SetRenderDrawColor(255, 0, 255, 255);
				RenderDrawRect(control->data->rect.x, control->data->rect.y, control->data->rect.w, control->data->rect.h, true, false, true);
			}
			break;
		case CONTROL_TYPE_CHECKBOX:
			if (control->data->toggled)
			{
				SetRenderDrawColor(255, 0, 255, 255);
				RenderDrawRect(control->data->rect.x, control->data->rect.y, control->data->rect.w, control->data->rect.h, true, false, true);
			}
			break;
		case CONTROL_TYPE_SLIDER:
			SetRenderDrawColor(255, 0, 255, 255);//if w>h...
			RenderDrawRect(control->data->rect.x + control->data->value * control->data->rect.w - control->data->rect.h / 2, control->data->rect.y, control->data->rect.h, control->data->rect.h, true, false, true);
			break;
		}

		switch (control->data->state)
		{
		case CONTROL_STATE_DISABLE: SetRenderDrawColor(255, 0, 0, 255); break;
		case CONTROL_STATE_NORMAL: SetRenderDrawColor(255, 255, 255, 255); break;
		case CONTROL_STATE_FOCUS: SetRenderDrawColor(0, 255, 0, 255); break;
		case CONTROL_STATE_PRESS: SetRenderDrawColor(255, 255, 0, 255); break;
		case CONTROL_STATE_RELEASE: SetRenderDrawColor(0, 255, 255, 255); break;
		}

		RenderDrawRect(control->data->rect.x, control->data->rect.y, control->data->rect.w, control->data->rect.h, false, false, true);

		control = control->next;
	}
}

void UserInterface::AddControl(ControlType _type, const char* _text, ControlState _state, int x, int y, int w, int h)
{
	Control* control = new Control;

	control->type = _type;
	control->state = _state;
	control->text = _text;
	control->rect = { x,y,w,h };

	control->toggled = false;

	control->value = 0;

	controls.Add(control);
}

#endif
