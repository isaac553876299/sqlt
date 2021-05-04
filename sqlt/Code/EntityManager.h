#ifndef _ENTITYMANAGER_H_
#define _ENTITYMANAGER_H_

#include "Module.h"
#include "List.h"
#include "Point.h"
#include "Entity.h"

#include "GuiButton.h"
#include "GuiCheckBox.h"
#include "Title.h"

#include "PugiXml/src/pugixml.hpp"

class EntityManager : public Module
{
public:
	EntityManager();

	// Destructor
	virtual ~EntityManager()
	{}

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	bool LoadState(pugi::xml_node&);
	bool SaveState(pugi::xml_node&) const;

	void AddEntity(fPoint position, Entity::Type type);

	void OnCollision(Collider* a, Collider* b);

	void LoadStats(Entity* e);

	bool OnGuiMouseClickEvent(GuiControl* control)
	{
		app->title->OnGuiMouseClickEvent(control);
		return true;
	}


	struct PlayerData 
	{
		fPoint position;

		//Pause Menu
		bool pauseCondition = false;
		bool debug = false;
		bool resetCamera;

		SDL_Texture* pauseMenu = nullptr;
		SDL_Texture* resumeButtton = nullptr;

		SDL_Rect cameraBckUp;

		//Dialog
		bool onDialog = false;

		//Combat Stats
		int level = 1;

		//Items
		int Pencil = 0;
		int ball = 0;
		int book = 0;
		int calculator = 0;
		int cola = 0;
		int coffee = 0;
		int fak = 0;//first aid kit
		int tipex = 0;
		int eraser = 0;
		int rule = 0;
		int celo = 0;
		int snack = 0;
		int PencilSharp = 0;
	};

	GuiButton* resumeButton;
	GuiButton* settingsButton;
	GuiButton* exitButton;

	GuiCheckBox* fullScreen;

	int winCount = 0;

	PlayerData playerData;

	List<Entity*> entityList;

	pugi::xml_document combatStats;

	bool settingsEnabled = false;

private:
	SDL_Texture* texPlayer;
	SDL_Texture* texAlly1;
	SDL_Texture* settingsPost;

	SDL_Texture* texNPC1;
	SDL_Texture* texNPC2;
	SDL_Texture* texNPC3;

	SDL_Texture* texEnemy1;
	SDL_Texture* texEnemy3;

	SDL_Texture* texItem;


	Entity* entityPlayer;
	Entity* entityAlly1;

	Entity* entityNPC1;
	Entity* entityNPC2;
	Entity* entityNPC3;

	Entity* entityGhostEnemy;
	Entity* entityTankEnemy;
	Entity* entityCanonEnemy;

	Entity* entityPencil;
	Entity* entityBall;

	//Fonts
	int scoreFont = -1;
	char scoreText[10] = { "\0" };
};

#endif // _ENTITYMANAGER_H_