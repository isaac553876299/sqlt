#include "DialogSystem.h"

#include "Input.h"
#include "Render.h"
#include "Textures.h"
#include "Fonts.h"
#include "EntityManager.h"
#include "FadeToBlack.h"

#include "Scene1.h"
#include "Intro.h"
#include "BattleScene.h"

#include "SDL/include/SDL.h"


DialogueSystem::DialogueSystem() {}

DialogueSystem::~DialogueSystem() {}

bool DialogueSystem::Start()
{
	app->dialogueSystem->LoadDialogue("dialogue_tree.xml");
	battleTransition = app->audio->LoadFx("Assets/Audio/FX/battle_transition.wav");
	textBox = app->tex->Load("Assets/Textures/text_box.png");

	return true;
}

bool DialogueSystem::Update(float dt)
{
	if (app->entityManager->playerData.onDialog == true)
	{
		app->render->DrawTexture(textBox, -app->render->camera.x, -app->render->camera.y + 560);

		if (Id == 0)
		{
			if (input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
			{
				playerInput = 0;
				Id = 0;
				PerformDialogue(Id, playerInput);
				actionChecks++;
			}
			if (actionChecks == dialogueTrees[0]->dialogueNodes.size())
			{
				app->entityManager->playerData.onDialog = false;
			}
		}

		if (Id == 1)
		{
			if (input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
			{
				playerInput = 0;
				Id = 1;
				PerformDialogue(Id, playerInput);
				actionChecks++;
			}
			if (actionChecks == dialogueTrees[1]->dialogueNodes.size())
			{
				app->entityManager->playerData.onDialog = false;
				app->playerPosition = app->entityManager->playerData.position;
				app->fade->Fade(app->scene1, app->battleScene, 30);
			}
		}

		if (Id == 2)
		{
			if (input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN || input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
			{
				playerInput = 0;
				Id = 2;
				PerformDialogue(Id, playerInput);
				actionChecks++;
			}
			if (actionChecks == dialogueTrees[2]->dialogueNodes.size())
			{
				app->entityManager->playerData.onDialog = false;
			}
		}

		if (Id == 3)
		{
			if (input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
			{
				playerInput = 0;
				Id = 3;
				PerformDialogue(Id, playerInput);
				actionChecks++;
			}
			if (input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
			{
				playerInput = 0;
				Id = 3;
				PerformDialogue(Id, playerInput);
				actionChecks = 0;
			}
			if (input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
			{
				playerInput = 1;
				Id = 3;
				PerformDialogue(Id, playerInput);
				actionChecks = 0;
			}
			if (input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
			{
				playerInput = 2;
				Id = 3;
				PerformDialogue(Id, playerInput);
				actionChecks = 14;
			}
			if (actionChecks == dialogueTrees[3]->dialogueNodes.size())
			{
				app->entityManager->playerData.onDialog = false;
				app->audio->PlayFx(battleTransition);
				app->fade->Fade(app->intro, app->scene1, 30);
			}
		}
	}

	return true;
}


bool DialogueSystem::CleanUp()
{
	for (int i = 0; i < dialogueTrees.size(); i++)
	{
		for (int j = 0; j < dialogueTrees[i]->dialogueNodes.size(); j++) delete dialogueTrees[i]->dialogueNodes[j];

		dialogueTrees[i]->dialogueNodes.clear();
		delete dialogueTrees[i];
	}
	dialogueTrees.clear();

	return true;
}

void DialogueSystem::PerformDialogue(int treeId, int playerInput)
{
	if (playerInput >= 0 && playerInput < currentNode->dialogueOptions.size())
	{
		for (int i = 0; i < dialogueTrees[treeId]->dialogueNodes.size(); i++)
			if (currentNode->dialogueOptions[playerInput]->nextNode == dialogueTrees[treeId]->dialogueNodes[i]->nodeId)
			{
				currentNode = dialogueTrees[treeId]->dialogueNodes[i];
				break;
			}
	}


}

bool DialogueSystem::LoadDialogue(const char* file)
{
	pugi::xml_parse_result result = dialogues.load_file(file);

	bool ret = true;

	if (result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", file, result.description());
		ret = false;
	}
	else
	{
		pugi::xml_node n = dialogues.first_child().child("dialogueExample");

		for (n; n != NULL; n = n.next_sibling("dialogueExample"))
		{
			DialogueTree* tree = new DialogueTree;
			tree->treeId = n.attribute("Id").as_int();
			LoadNodes(n, tree);
			dialogueTrees.push_back(tree);
		}
	}
	return true;
}

bool DialogueSystem::LoadNodes(pugi::xml_node& trees, DialogueTree* example)
{
	for (pugi::xml_node n = trees.child("node"); n != NULL; n = n.next_sibling("node"))
	{
		DialogueNode* node = new DialogueNode;
		node->text.assign(n.attribute("text").as_string());
		node->nodeId = n.attribute("id").as_int();
		LoadOptions(n, node);
		example->dialogueNodes.push_back(node);
	}

	return true;
}

bool DialogueSystem::LoadOptions(pugi::xml_node& response, DialogueNode* answers)
{
	for (pugi::xml_node option = response.child("dialogue"); option != NULL; option = option.next_sibling("dialogue"))
	{
		DialogueOption* selection = new DialogueOption;
		selection->text.assign(option.attribute("option").as_string());
		selection->nextNode = option.attribute("nextNode").as_int();
		answers->dialogueOptions.push_back(selection);
		answers->answersList.Add((option.attribute("option").as_string()));
	}

	return true;
}