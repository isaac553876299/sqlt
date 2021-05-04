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
	bool pendingToDelete = false;

	Collider(SDL_Rect rectangle, ColliderType type);

	void SetPos(int x, int y);
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

class Collisions
{
public:
	Collisions();
	virtual ~Collisions() {}

	void Update(float dt);
	void Draw();

	Collider* AddCollider(SDL_Rect rect, ColliderType type);

	void RemoveCollider(Collider* collider);

	void DebugRequest();

	bool matrix[ColliderType::MAX][ColliderType::MAX];

private:
	Collider* colliders[MAX_COLLIDERS] = { nullptr };

	// The collision matrix. Defines the interaction for two collider types
	// If set two false, collider 1 will ignore collider 2

	bool debug = false;
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
	for (unsigned int i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i])
		{
			delete colliders[i];
			colliders[i] = nullptr;
		}
	}
}

void Collisions::Update(float dt)
{
	for (unsigned int i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] && colliders[i]->pendingToDelete)
		{
			delete colliders[i];
			colliders[i] = nullptr;
		}
	}

	Collider* c1;
	Collider* c2;

	for (unsigned int i = 0; i < MAX_COLLIDERS; ++i)
	{
		// skip empty colliders
		if (!colliders[i]) continue;

		c1 = colliders[i];

		// avoid checking collisions already checked
		for (unsigned int k = i + 1; k < MAX_COLLIDERS; ++k)
		{
			// skip empty colliders
			if (!colliders[k]) continue;

			c2 = colliders[k];

			if (matrix[c1->type][c2->type] && SDL_HasIntersection(&c1->rect, &c2->rect))
			{
				for (unsigned int i = 0; i < MAX_LISTENERS; ++i)
					if (c1->listeners[i]) c1->listeners[i]->OnCollision(c1, c2);

				for (unsigned int i = 0; i < MAX_LISTENERS; ++i)
					if (c2->listeners[i]) c2->listeners[i]->OnCollision(c2, c1);

			}

		}
	}
}

void Collisions::Draw()
{
	if (debug)
	{
		Uint8 alpha = 128;
		for (unsigned int i = 0; i < MAX_COLLIDERS; ++i)
		{
			if (!colliders[i]) continue;

			switch (colliders[i]->type)
			{
			case ColliderType::PLAYER: SetRenderDrawColor(0, 255, 0, alpha); break;
			case ColliderType::WALL: SetRenderDrawColor(255, 0, 0, alpha); break;
			case ColliderType::MAX: SetRenderDrawColor(0, 0, 0, alpha); break;
			}

			RenderFillRect(colliders[i]->rect.x, colliders[i]->rect.y, colliders[i]->rect.w, colliders[i]->rect.h);
		}
	}
}

Collider* Collisions::AddCollider(SDL_Rect rect, ColliderType type)
{
	Collider* ret = nullptr;

	for (unsigned int i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (!colliders[i])
		{
			ret = colliders[i] = new Collider(rect, type);
			break;
		}
	}

	return ret;
}

void Collisions::RemoveCollider(Collider* collider)
{
	for (unsigned int i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] == collider)
		{
			delete colliders[i];
			colliders[i] = nullptr;
			break;
		}
	}
}

void Collisions::DebugRequest()
{
	debug = !debug;
}

#endif
