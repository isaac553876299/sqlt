#ifndef __COLLISIONS_H__
#define __COLLISIONS_H__

#define MAX_COLLIDERS 2000

#include "Share.h"

enum ColliderType
{
	COLLIDER_TYPE_PLAYER,
	COLLIDER_TYPE_WALL,

	COLLIDER_TYPE_MAX
};

struct Collider
{
	SDL_Rect rect;
	ColliderType type;
	bool collided;
};

class Collisions
{
public:

	Collider* colliders[MAX_COLLIDERS] = { nullptr };
	bool debug_draw;

	Collisions();
	~Collisions();
	void Clear();

	void Update();
	void Draw();

	void AddCollider(SDL_Rect rect, ColliderType type);

	bool matrix[COLLIDER_TYPE_MAX][COLLIDER_TYPE_MAX];

};

Collisions::Collisions()
{
	Clear();

	matrix[COLLIDER_TYPE_PLAYER][COLLIDER_TYPE_PLAYER] = false;
	matrix[COLLIDER_TYPE_PLAYER][COLLIDER_TYPE_WALL] = true;

	matrix[COLLIDER_TYPE_WALL][COLLIDER_TYPE_PLAYER] = true;
	matrix[COLLIDER_TYPE_WALL][COLLIDER_TYPE_WALL] = false;


}

Collisions::~Collisions()
{
	Clear();
}

void Collisions::Clear()
{
	debug_draw = false;
	for (unsigned int i = 0; i < MAX_COLLIDERS; ++i)
	{
		delete colliders[i];
		colliders[i] = nullptr;
	}
}

void Collisions::Update()
{
	for (unsigned int i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i])
		{
			for (unsigned int j = i + 1; j < MAX_COLLIDERS; ++j)
			{
				if (colliders[j])
				{
					if (matrix[colliders[i]->type][colliders[j]->type]
						&& SDL_HasIntersection(&colliders[i]->rect, &colliders[j]->rect)
						&& !colliders[i]->collided && !colliders[j]->collided)
					{
						colliders[i]->collided = true;
						colliders[j]->collided = true;
						//callback
					}
				}
			}
		}
	}
}

void Collisions::Draw()
{
	if (debug_draw)
	{
		for (unsigned int i = 0; i < MAX_COLLIDERS; ++i)
		{
			if (colliders[i])
			{
				switch (colliders[i]->type)
				{
				case ColliderType::COLLIDER_TYPE_PLAYER: SetRenderDrawColor(0, 255, 0, 128); break;
				case ColliderType::COLLIDER_TYPE_WALL: SetRenderDrawColor(255, 0, 0, 128); break;
				}
				RenderDrawRect(colliders[i]->rect.x, colliders[i]->rect.y, colliders[i]->rect.w, colliders[i]->rect.h);
			}
		}
	}
}

void Collisions::AddCollider(SDL_Rect rect, ColliderType type)
{
	for (unsigned int i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (!colliders[i])
		{
			colliders[i] = new Collider{ rect, type, false };
			break;
		}
	}
}

#endif
