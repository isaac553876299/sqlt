#ifndef __USERINTERFACE_H__
#define __USERINTERFACE_H__

#include "SDL.h"
#include "SDL_image.h"
#include "List.h"
#include "Share.h"

enum UIState
{
	UI_STATE_DISABLE,
	UI_STATE_NORMAL,
	UI_STATE_FOCUS,
	UI_STATE_PRESS,
	UI_STATE_RELEASE
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
	UIState state;
	SDL_Rect rect;
	Control(ControlType _type, const char* _text, int x, int y, int w, int h)
	{
		type = _type;
		text = _text;
		state = UI_STATE_NORMAL;
		rect = { x,y,w,h };
	}
};

struct Button : public Control
{
	Button(ControlType _type, const char* _text, int x, int y, int w, int h) : Control(_type, _text, x, y, w, h)
	{
		
	}
};

struct CheckBox : public Control
{
	bool toggled;
	CheckBox(ControlType _type, const char* _text, int x, int y, int w, int h, bool _toggled = false) : Control(_type, _text, x, y, w, h)
	{
		toggled = _toggled;
	}
};

struct Slider : public Control
{
	float value;
	Slider(ControlType _type, const char* _text, int x, int y, int w, int h, float _value = 0.0f) : Control(_type, _text, x, y, w, h)
	{
		value = _value;
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
	controls->Clear();
}

void UserInterface::Update()
{
	SDL_Point mouse = { (share.mouse[0] - share.view[0]), (share.mouse[1] - share.view[1]) };
	ListItem<Control*>* tmp = controls->start;
	while (tmp)
	{
		if (SDL_PointInRect(&mouse, &tmp->data->rect))
		{
			tmp->data->state = UI_STATE_FOCUS;
			if (MOUSE_DOWN(SDL_BUTTON_LEFT))
			{
				switch (tmp->data->state)
				{
				case UI_STATE_DISABLE:
					switch (tmp->data->type)
					{
					case CONTROL_TYPE_BUTTON:

						break;
					case CONTROL_TYPE_CHECKBOX:

						break;
					case CONTROL_TYPE_SLIDER:

						break;

					}
					break;
				case UI_STATE_NORMAL:
					switch (tmp->data->type)
					{
					case CONTROL_TYPE_BUTTON:

						break;
					case CONTROL_TYPE_CHECKBOX:

						break;
					case CONTROL_TYPE_SLIDER:

						break;

					}
					break;
				case UI_STATE_FOCUS:
					switch (tmp->data->type)
					{
					case CONTROL_TYPE_BUTTON:

						break;
					case CONTROL_TYPE_CHECKBOX:

						break;
					case CONTROL_TYPE_SLIDER:

						break;

					}
					break;
				case UI_STATE_PRESS:
					switch (tmp->data->type)
					{
					case CONTROL_TYPE_BUTTON:

						break;
					case CONTROL_TYPE_CHECKBOX:

						break;
					case CONTROL_TYPE_SLIDER:

						break;

					}
					break;
				case UI_STATE_RELEASE:
					switch (tmp->data->type)
					{
					case CONTROL_TYPE_BUTTON:

						break;
					case CONTROL_TYPE_CHECKBOX:

						break;
					case CONTROL_TYPE_SLIDER:

						break;

					}
					break;
				}
			}
		}
		tmp = tmp->next;
	}

	if (state != GuiControlState::DISABLED)
	{
		int mouseX, mouseY;
		input->GetMousePosition(mouseX, mouseY);
		mouseX -= app->render->camera.x;
		mouseY -= app->render->camera.y;
		// Check collision between mouse and button bounds
		if ((mouseX > bounds.x) && (mouseX < (bounds.x + bounds.w)) &&
			(mouseY > bounds.y) && (mouseY < (bounds.y + bounds.h)))
		{
			state = GuiControlState::FOCUSED;

			if (input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_REPEAT)
			{
				state = GuiControlState::PRESSED;
			}

			// If mouse button pressed -> Generate event!
			if (input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_UP)
			{
				NotifyObserver();
			}
		}
		else state = GuiControlState::NORMAL;
	}
}

void UserInterface::Draw()
{
	ListItem<Control*>* tmp = controls->start;
	while (tmp)
	{
		switch (tmp->data->type)
		{
		case CONTROL_TYPE_BUTTON:

			break;
		case CONTROL_TYPE_CHECKBOX:

			break;
		case CONTROL_TYPE_SLIDER:

			break;

		}
		tmp = tmp->next;
	}
}

#endif
