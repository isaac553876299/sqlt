#ifndef __COLLISIONS_H__
#define __COLLISIONS_H__

#define MAX_COLLIDERS 2000

#include "Module.h"
#include "Collider.h"

class Collisions : public Module
{
public:
	Collisions(bool startEnabled);

	// Destructor
	virtual ~Collisions() {}

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// Adds a new collider to the list
	Collider* AddCollider(SDL_Rect rect, Collider::Type type, Module* listener = nullptr);
	Collider* AddDynamicCollider(SDL_Rect rect, Collider::Type type, Module* listener = nullptr);

	// Removes the collider memory and removes it from the colliders array
	void RemoveCollider(Collider* collider);

	// Draws all existing colliders with some transparency
	void DebugDraw();
	void DebugRequest();


	bool matrix[Collider::Type::MAX][Collider::Type::MAX];

private:
	// All existing colliders in the scene
	Collider* colliders[MAX_COLLIDERS] = { nullptr };

	// The collision matrix. Defines the interaction for two collider types
	// If set two false, collider 1 will ignore collider 2


	// Simple debugging flag to draw all colliders
	bool debug = false;
};

#include "Collisions.h"

#include "App.h"
#include "Input.h"
#include "Log.h"
#include "Render.h"

Collisions::Collisions(bool startEnabled) : Module()
{
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
		colliders[i] = nullptr;

	matrix[Collider::Type::PLAYER][Collider::Type::WALL] = true;
	matrix[Collider::Type::PLAYER][Collider::Type::PLAYER] = false;
	matrix[Collider::Type::PLAYER][Collider::Type::NPC] = true;
	matrix[Collider::Type::PLAYER][Collider::Type::ENEMY] = true;
	matrix[Collider::Type::PLAYER][Collider::Type::TP1TO2] = true;
	matrix[Collider::Type::PLAYER][Collider::Type::TP2TO1] = true;

	matrix[Collider::Type::WALL][Collider::Type::WALL] = false;
	matrix[Collider::Type::WALL][Collider::Type::PLAYER] = true;

	matrix[Collider::Type::NPC][Collider::Type::PLAYER] = true;

	matrix[Collider::Type::ENEMY][Collider::Type::PLAYER] = true;

	matrix[Collider::Type::FRONTPLAYER][Collider::Type::PENCIL] = true;
	matrix[Collider::Type::FRONTPLAYER][Collider::Type::FRONTPLAYER] = false;

	matrix[Collider::Type::PENCIL][Collider::Type::FRONTPLAYER] = true;
	matrix[Collider::Type::PENCIL][Collider::Type::PENCIL] = false;

	matrix[Collider::Type::TP1TO2][Collider::Type::PLAYER] = true;
	matrix[Collider::Type::TP1TO2][Collider::Type::TP1TO2] = false;

	matrix[Collider::Type::TP2TO1][Collider::Type::PLAYER] = true;
	matrix[Collider::Type::TP2TO1][Collider::Type::TP2TO1] = false;


}

// Called before render is available
bool Collisions::Awake(pugi::xml_node&)
{
	return true;
}

// Called before the first frame
bool Collisions::Start()
{
	return true;
}

// Called each loop iteration
bool Collisions::PreUpdate()
{


	return true;
}

bool Collisions::Update(float dt)
{
	// Remove all colliders scheduled for deletion
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] != nullptr && colliders[i]->pendingToDelete == true)
		{
			delete colliders[i];
			colliders[i] = nullptr;
		}
	}

	Collider* c1;
	Collider* c2;

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		// skip empty colliders
		if (colliders[i] == nullptr)
			continue;

		c1 = colliders[i];

		// avoid checking collisions already checked
		for (uint k = i + 1; k < MAX_COLLIDERS; ++k)
		{
			// skip empty colliders
			if (colliders[k] == nullptr)
				continue;

			c2 = colliders[k];

			if (matrix[c1->type][c2->type] && c1->Intersects(c2->rect))
			{
				for (uint i = 0; i < MAX_LISTENERS; ++i)
					if (c1->listeners[i] != nullptr) c1->listeners[i]->OnCollision(c1, c2);

				for (uint i = 0; i < MAX_LISTENERS; ++i)
					if (c2->listeners[i] != nullptr) c2->listeners[i]->OnCollision(c2, c1);
			}

		}
	}

	return true;
}

bool Collisions::PostUpdate()
{
	if (debug)
		DebugDraw();


	return true;
}

// Called before quitting
bool Collisions::CleanUp()
{
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] != nullptr)
		{
			delete colliders[i];
			colliders[i] = nullptr;
		}
	}

	return true;
}

Collider* Collisions::AddCollider(SDL_Rect rect, Collider::Type type, Module* listener)
{
	Collider* ret = nullptr;

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] == nullptr)
		{
			ret = colliders[i] = new Collider(rect, type, listener);
			break;
			LOG("%d ", i);
		}
	}

	return ret;
}

void Collisions::RemoveCollider(Collider* collider)
{
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] == collider)
		{
			delete colliders[i];
			colliders[i] = nullptr;
		}
	}
}

void Collisions::DebugDraw()
{
	Uint8 alpha = 80;
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] == nullptr)
			continue;

		switch (colliders[i]->type)
		{
		case Collider::Type::NONE: // white
			app->render->DrawRectangle(colliders[i]->rect, 255, 255, 255, alpha);
			break;

		case Collider::Type::WALL:
			app->render->DrawRectangle(colliders[i]->rect, 0, 255, 0, alpha);

		case Collider::Type::PLAYER:
			app->render->DrawRectangle(colliders[i]->rect, 0, 0, 255, alpha);
			break;
		case Collider::Type::NPC:
			app->render->DrawRectangle(colliders[i]->rect, 255, 0, 255, alpha);
			break;
		case Collider::Type::PENCIL:
			app->render->DrawRectangle(colliders[i]->rect, 255, 0, 150, alpha);
			break;
		case Collider::Type::FRONTPLAYER:
			app->render->DrawRectangle(colliders[i]->rect, 50, 0, 90, alpha);
			break;
		case Collider::Type::TP1TO2:
			app->render->DrawRectangle(colliders[i]->rect, 50, 0, 90, alpha);
			break;
		}
	}
}

void Collisions::DebugRequest()
{
	debug = !debug;
}

#endif
