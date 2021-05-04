#ifndef __COLLIDER_H__
#define __COLLIDER_H__

#include "SDL_Rect.h"

#define MAX_LISTENERS 300

enum class ColliderType
{
	PLAYER,
	WALL,


	MAX
};

struct Collider
{
	SDL_Rect rect;
	bool pendingToDelete = false;
	ColliderType type;

	Collider(SDL_Rect rectangle, ColliderType type);

	void SetPos(int x, int y);

	bool Intersects(const SDL_Rect& r) const;
};

Collider::Collider(SDL_Rect rectangle, ColliderType _type)
{
	rect = rectangle;
	type = _type;
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
	return SDL_HasIntersection(&rect, &r);
}

#endif
