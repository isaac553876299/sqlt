#ifndef __ASSETS_H__
#define __ASSETS_H__

#include "Share.h"
#include "List.h"

class AssetsPack
{
public:
	List<SDL_Texture*>* textures;
	List<TTF_Font*>* fonts;

	AssetsPack();
	~AssetsPack();

	int LoadTexture(const char* file);
	int LoadFont(const char* file);
};

AssetsPack::AssetsPack()
{

}

AssetsPack::~AssetsPack()
{
	textures->Clear();
	fonts->Clear();
}

int AssetsPack::LoadTexture(const char* file)
{
	textures->Add(IMG_LoadTexture(file));
}

int AssetsPack::LoadFont(const char* file)
{
	fonts->Add(TTF_OpenFont(file));
}

#endif
