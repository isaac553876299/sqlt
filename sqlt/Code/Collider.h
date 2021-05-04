#ifndef __COLLIDER_H__
#define __COLLIDER_H__

#include "SDL/include/SDL_Rect.h"

#define MAX_LISTENERS 300

class Module;

struct Collider
{
	enum Type
	{
		NONE = -1,
		PLAYER,
		FRONTPLAYER,
		WALL,
		TP1TO2,
		TP2TO1,
		NPC,
		ENEMY,
		PENCIL,

		MAX
	};

	enum Height
	{
		UNDEFINED
	};

	//Methods
	Collider(SDL_Rect rectangle, Type type, Module* listener = nullptr);

	void SetPos(int x, int y);

	bool Intersects(const SDL_Rect& r) const;

	void AddListener(Module* listener);

	//Variables
	SDL_Rect rect;
	bool pendingToDelete = false;
	Type type;
	Height height;
	Module* listeners[MAX_LISTENERS] = { nullptr };
};

Collider::Collider(SDL_Rect rectangle, Type type, Module* listener) : rect(rectangle), type(type)
{
	listeners[0] = listener;
}

void Collider::SetPos(int x, int y)
{
	rect.x = x;
	rect.y = y;
}

bool Collider::Intersects(const SDL_Rect& r) const
{
	return (rect.x < r.x + r.w &&
		rect.x + rect.w > r.x &&
		rect.y < r.y + r.h &&
		rect.h + rect.y > r.y);
}

void Collider::AddListener(Module* listener)
{
	for (int i = 0; i < MAX_LISTENERS; ++i)
	{
		if (listeners[i] == nullptr)
		{
			listeners[i] = listener;
			break;
		}

		//Simple security check to avoid adding the same listener twice
		else if (listeners[i] == listener)
			break;
	}
}

#endif
