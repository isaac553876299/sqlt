#include "EntityManager.h"

#include "App.h"
#include "Log.h"
#include "Textures.h"
#include "Render.h"
#include "Fonts.h"

#include "Scene1.h"
#include "Title.h"

#include "PlayerEntity.h"
#include "Ally1.h"
#include "NPC1.h"
#include "NPC2.h"
#include "NPC3.h"
#include "Enemy1.h"
#include "Enemy2.h"
#include "Enemy3.h"
#include "Pencil.h"
#include "Ball.h"


EntityManager::EntityManager() : Module()
{
	name.Create("entitymanager");
}

bool EntityManager::Awake()
{
	return true;
}

bool EntityManager::Start()
{
	pugi::xml_parse_result result = combatStats.load_file("combat_stats.xml");
	if (result == NULL)
	{
		LOG("Could not load saved game xml file. Pugi error: %s", result.description());
	}

	settingsPost = app->tex->Load("Assets/Textures/postit.png");

	texPlayer = app->tex->Load("Assets/Textures/Entities/Playable/player.png");
	texAlly1 = app->tex->Load("Assets/Textures/Entities/Playable/Ash_idle_anim_32x32.png");

	texNPC1 = app->tex->Load("Assets/Textures/Entities/NPC/Kid_Mitty_32x32.png");
	texNPC2 = app->tex->Load("Assets/Textures/Entities/NPC/Samuel_32x32.png");
	texNPC3 = app->tex->Load("Assets/Textures/Entities/NPC/Rob_32x32.png");

	texItem = app->tex->Load("Assets/Textures/Items/Shine.png");

	texEnemy1 = app->tex->Load("Assets/Textures/Entities/Enemies/Halloween_Kid_1_idle_anim_32x32.png");
	texEnemy3 = app->tex->Load("Assets/Textures/Entities/Enemies/Fishmonger_2_idle_anim_32x32.png");

	playerData.pauseMenu = app->tex->Load("Assets/Textures/Screens/pause_screen.png");
	//resumeButtton = app->tex->Load("Assets/Textures/resume.png");


	resumeButton = new GuiButton(32, {517,304, 240, 60 }, "CONTINUE");
	resumeButton->SetObserver(this);
	resumeButton->SetTexture(app->tex->Load("Assets/Textures/resume.png"), app->tex->Load("Assets/Textures/resume_selected.png"), app->tex->Load("Assets/Textures/resume_pressed.png"));

	settingsButton = new GuiButton(17, { 517, 409, 234, 55 }, "SETTINGS");
	settingsButton->SetObserver(this);
	settingsButton->SetTexture(app->tex->Load("Assets/Textures/settings.png"), app->tex->Load("Assets/Textures/settings_selected.png"), app->tex->Load("Assets/Textures/settings_pressed.png"));

	fullScreen = new GuiCheckBox(7, { 620,400, 300, 60 }, "FULLSCREEN");
	fullScreen->SetObserver(this);
	fullScreen->SetTexture(app->tex->Load("Assets/Textures/fs1.png"), app->tex->Load("Assets/Textures/fs2.png"), app->tex->Load("Assets/Textures/fs2.png"));
	
	exitButton = new GuiButton(11, { 551, 360, 172, 55 }, "CREDITS");
	exitButton->SetObserver(this);
	exitButton->SetTexture(app->tex->Load("Assets/Textures/exit.png"), app->tex->Load("Assets/Textures/exit_selected.png"), app->tex->Load("Assets/Textures/exit_pressed.png"));



	return true;
}

bool EntityManager::PreUpdate()
{
	return true;
}

bool EntityManager::Update(float dt)
{
	if (app->entityManager->playerData.pauseCondition == false)
	{
		settingsEnabled = false;
	}
	ListItem<Entity*>* entity = entityList.start;

	while (entity != nullptr)
	{
		if (entity->data->pendingToDelete)
		{
			delete entity->data;
			entityList.Del(entity);
			entity = entity->next;
			continue;
		}

		entity->data->Update(dt);
		entity = entity->next;
	}

	if (app->entityManager->playerData.pauseCondition)
	{
		resumeButton->Update(app->input, dt);
		settingsButton->Update(app->input, dt);
		exitButton->Update(app->input, dt);
		fullScreen->Update(app->input, dt);
		app->audio->Volume(20, '0');

	}
	if (!app->entityManager->playerData.pauseCondition)
	{
		app->audio->Volume(100, '0');

	}
	if (app->title->exi)	return false;

	return true;
}

bool EntityManager::PostUpdate()
{
	resumeButton->bounds.x = -app->render->camera.x+537;
	resumeButton->bounds.y = -app->render->camera.y+200;
	settingsButton->bounds.x = -app->render->camera.x + 537;
	settingsButton->bounds.y = -app->render->camera.y + 260;
	exitButton->bounds.x = -app->render->camera.x + 557;
	exitButton->bounds.y = -app->render->camera.y + 360;
	fullScreen->bounds.x = -app->render->camera.x + 900;
	fullScreen->bounds.y = -app->render->camera.y + 200;


	for (int i = 0; i < entityList.Count(); i++)
	{
		ListItem<Entity*>* entity = entityList.At(i);
		entity->data->Draw();
	}
	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN) app->entityManager->playerData.pauseCondition = !app->entityManager->playerData.pauseCondition;

	if (app->entityManager->playerData.pauseCondition)
	{
		app->render->DrawTexture(playerData.pauseMenu, -app->render->camera.x, -app->render->camera.y, NULL);
		//fullScreen->Draw(app->render);
		resumeButton->Draw(app->render);
		settingsButton->Draw(app->render);
		exitButton->Draw(app->render);
		if (settingsEnabled)
		{
			app->render->DrawTexture(settingsPost, -app->render->camera.x + 875, -app->render->camera.y + 100, NULL);
			fullScreen->Draw(app->render);
		}
	}
	

	return true;
}

bool EntityManager::CleanUp()
{
	for (int i = 0; i < entityList.Count(); i++)
	{
		ListItem<Entity*>* entity = entityList.At(i);
		entity->data->pendingToDelete = true;
	}

	entityList.Clear();

	return true;
}

bool EntityManager::LoadState(pugi::xml_node& data)
{
	pugi::xml_node entities = data.child("entity");

	pugi::xml_node node = entities.child("player");
	pugi::xml_node node2 = entities.child("enemies");

	pugi::xml_node e;

	int count = 0;

	for (e = node.child("playerdata"); e; e = e.next_sibling("playerdata"))
	{
		float x = e.attribute("x").as_float();
		float y = e.attribute("y").as_float();
		fPoint newPosition = fPoint(x, y);
		entityList.end->data->position = newPosition;
		playerData.level = e.attribute("lvl").as_int();
	}

	return true;
}

bool EntityManager::SaveState(pugi::xml_node& data) const
{
	pugi::xml_node entities = data.append_child("entity");
	pugi::xml_node node = entities.append_child("player");

	for (int i = 0; i < entityList.Count(); i++)
	{
		Entity* e = entityList[i];
		if (e->type == Entity::Type::PLAYER)
		{
			pugi::xml_node eNode = node.append_child("playerdata");
			pugi::xml_attribute x = eNode.append_attribute("x");
			x.set_value(e->position.x);
			pugi::xml_attribute y = eNode.append_attribute("y");
			y.set_value(e->position.y);
			pugi::xml_attribute lvl = eNode.append_attribute("lvl");
			lvl.set_value(playerData.level);
			eNode.next_sibling("playerdata");
		}
	}

	return true;
}

void EntityManager::AddEntity(fPoint position, Entity::Type type)
{
	switch (type)
	{
	//Player//
	case Entity::Type::PLAYER:
		entityPlayer = (Entity*)(new PlayerEntity((Module*)this, position, texPlayer, type));
		entityList.Add(entityPlayer);
		LoadStats(entityPlayer);
		break;
	///////////

	//Allies//
	case Entity::Type::ALLY1:
		entityAlly1 = (Entity*)(new Ally1((Module*)this, position, texAlly1, type));
		entityList.Add(entityAlly1);
		LoadStats(entityAlly1);
		break;
	///////////

	//NPC's//
	case Entity::Type::NPC1:
		entityNPC1 = (Entity*)(new NPC1((Module*)this, position, texNPC1, type));
		entityList.Add(entityNPC1);
		break;
	case Entity::Type::NPC2:
		entityNPC2 = (Entity*)(new NPC2((Module*)this, position, texNPC2, type));
		entityList.Add(entityNPC2);
		break;
	case Entity::Type::NPC3:
		entityNPC3 = (Entity*)(new NPC3((Module*)this, position, texNPC3, type));
		entityList.Add(entityNPC3);
		break;
	///////////

	//Enemies//
	case Entity::Type::EQUILIBRATED_ENEMY:
		entityGhostEnemy = (Entity*)(new Enemy1((Module*)this, position, texEnemy1, type));
		entityList.Add(entityGhostEnemy);
		LoadStats(entityGhostEnemy);
		break;
	case Entity::Type::TANK_ENEMY:
		entityTankEnemy = (Entity*)(new Enemy2((Module*)this, position, texNPC2, type));
		entityList.Add(entityTankEnemy);
		LoadStats(entityTankEnemy);
		break;
	case Entity::Type::DAMAGE_ENEMY:
		entityCanonEnemy = (Entity*)(new Enemy3((Module*)this, position, texEnemy3, type));
		entityList.Add(entityCanonEnemy);
		LoadStats(entityCanonEnemy);
		break;
	//////////
	///ITEMS/////
	case Entity::Type::PENCIL:
		entityPencil = (Entity*)(new Pencil((Module*)this, position, texItem, type));
		entityList.Add(entityPencil);
		break;
	case Entity::Type::BALL:
		entityBall = (Entity*)(new Ball((Module*)this, position, texItem, type));
		entityList.Add(entityBall);
		break;
	////////
	}
}

void EntityManager::OnCollision(Collider* a, Collider* b)
{
	for (int i = 0; i < entityList.Count(); i++)
	{
		ListItem<Entity*>* entity = entityList.At(i);

		if (entity->data->collider == a && b != nullptr)
		{
			entity->data->Collision(b);
		}

		if (entity->data->collider == b && a != nullptr)
		{
			entity->data->Collision(a);
		}
	}
}

void EntityManager::LoadStats(Entity* e)
{
	pugi::xml_node node = combatStats.child("combat");

	switch (e->type)
	{
	case Entity::Type::PLAYER:
		node = node.child("player").child("stats");
		break;
	case Entity::Type::ALLY1:
		node = node.child("allies").child("ally1").child("stats");
		break;
	case Entity::Type::TANK_ENEMY:
		node = node.child("enemies").child("tank").child("stats");
		break;
	case Entity::Type::DAMAGE_ENEMY:
		node = node.child("enemies").child("damage").child("stats");
		break;
	case Entity::Type::EQUILIBRATED_ENEMY:
		node = node.child("enemies").child("balanced").child("stats");
		break;
	default:
		break;
	}

	e->atkPerLvl = node.attribute("lvlatk").as_int();
	e->hpPerLvl = node.attribute("lvlhp").as_int();
	e->defPerLvl = node.attribute("lvldef").as_int();

	e->atk = node.attribute("atk").as_int() + (e->atkPerLvl * (playerData.level - 1));
	e->hp = node.attribute("hp").as_int() + (e->hpPerLvl * (playerData.level - 1));
	e->def = node.attribute("def").as_int() + (e->defPerLvl * (playerData.level - 1));

	e->turnTime = node.attribute("turntime").as_float();
	e->currentHp = e->hp;
}