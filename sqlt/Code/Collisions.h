#ifndef __COLLISIONS_H__
#define __COLLISIONS_H__

#define MAX_COLLIDERS 2000

#include "Share.h"

enum class ColliderType
{
	PLAYER,
	WALL,

	MAX
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
	bool debugDraw = false;

	Collisions();
	~Collisions() {}

	void Update();
	void Draw();

	void AddCollider(SDL_Rect rect, ColliderType type);

	bool matrix[ColliderType::MAX][ColliderType::MAX];
};

Collisions::Collisions()
{
	for (unsigned int i = 0; i < MAX_COLLIDERS; ++i) colliders[i] = nullptr;

	matrix[ColliderType::PLAYER][ColliderType::PLAYER] = false;
	matrix[ColliderType::PLAYER][ColliderType::WALL] = true;
	matrix[ColliderType::PLAYER][ColliderType::MAX] = false;

	matrix[ColliderType::WALL][ColliderType::PLAYER] = true;
	matrix[ColliderType::WALL][ColliderType::WALL] = false;
	matrix[ColliderType::WALL][ColliderType::MAX] = false;

	matrix[ColliderType::MAX][ColliderType::PLAYER] = false;
	matrix[ColliderType::MAX][ColliderType::WALL] = false;
	matrix[ColliderType::MAX][ColliderType::MAX] = false;
}

Collisions::~Collisions()
{
	delete[] colliders;
}

void Collisions::Update()
{
	for (unsigned int i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i])
		{
			for (unsigned int j = 0; j < MAX_COLLIDERS; ++j)
			{
				if (colliders[j])
				{
					if (matrix[colliders[i]->type][colliders[j]->type]
						&& SDL_HasIntersection(&colliders[i]->rect, &colliders[j]->rect)
						&& !colliders[i]->collided && !colliders[j]->collided)
					{
						colliders[i]->collided = true;
						colliders[j]->collided = true;

					}
				}
			}
		}
	}
}

void Collisions::Draw()
{
	if (debugDraw)
	{
		for (unsigned int i = 0; i < MAX_COLLIDERS; ++i)
		{
			if (colliders[i])
			{
				switch (colliders[i]->type)
				{
				case ColliderType::PLAYER: SetRenderDrawColor(0, 255, 0, 128); break;
				case ColliderType::WALL: SetRenderDrawColor(255, 0, 0, 128); break;
				case ColliderType::MAX: SetRenderDrawColor(0, 0, 0, 128); break;
				}
				RenderFillRect(colliders[i]->rect.x, colliders[i]->rect.y, colliders[i]->rect.w, colliders[i]->rect.h);
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
