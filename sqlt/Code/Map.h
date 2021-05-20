#ifndef __MAP_H__
#define __MAP_H__

#include "List.h"
#include "pugixml.hpp"
#include "Share.h"
#include "Collisions.h"

struct TileSet
{
	const char* name;
	int	firstgid;
	int margin, spacing;
	int	tile_w, tile_h;
	int	amount_w, amount_h;
	SDL_Texture* texture;

	SDL_Rect GetTileRect(int id) const;
	~TileSet() { SDL_DestroyTexture(texture); delete name; }
};

struct MapLayer
{
	const char* name;
	int width, height;
	unsigned int* data = nullptr;

	~MapLayer() { delete data; data = nullptr; delete name; }
	unsigned int Get(int x, int y) const { return data[x + y * width]; }
};

struct MapData
{
	int width, height;
	int	tile_w, tile_h;
	List<TileSet*> tilesets;
	List<MapLayer*> layers;
	bool* walkability;
};

class Map
{
public:

	MapData data;
	pugi::xml_document mapFile;
	bool drawColliders = false;

	Collisions collisions;

	Map();
	~Map();

	void Load(const char* path);

	void Draw();

	//MapToWorld
	int mx2wx(int x) const;
	int my2wy(int y) const;
	//WorldToMap
	int wx2mx(int x) const;
	int wy2my(int y) const;

	void LoadMap();
	TileSet* LoadTileset(pugi::xml_node& tileset_node);
	MapLayer* LoadLayer(pugi::xml_node& layer_node);

	TileSet* GetTilesetFromTileId(int id) const;

	void LoadColliders();

};

Map::Map()
{

}

Map::~Map()
{
	printf("Unloading map");
	data.tilesets.Clear();
	data.layers.Clear();
	delete[] data.walkability;
	mapFile.reset();
}

void Map::Draw()
{
	for (int i = 0; i < data.layers.size; ++i)
	{
		if (strcmp(data.layers[i]->name, "Collisions") || drawColliders)
		{
			for (int y = 0; y < data.height; ++y)
			{
				for (int x = 0; x < data.width; ++x)
				{
					int tileId = data.layers[i]->Get(x, y);
					if (tileId > 0)
					{
						TileSet* tileset = GetTilesetFromTileId(tileId);
						SDL_Rect rect = tileset->GetTileRect(tileId);
						RenderCopy(tileset->texture, &rect, mx2wx(x), my2wy(y));
					}
				}
			}
		}
	}
}

//MapToWorld
int Map::mx2wx(int x) const { return x * data.tile_w; }
int Map::my2wy(int y) const { return y * data.tile_h; }
//WorldToMap
int Map::wx2mx(int x) const { return x / data.tile_w; }
int Map::wy2my(int y) const { return y / data.tile_h; }

TileSet* Map::GetTilesetFromTileId(int id) const
{
	ListItem<TileSet*>* item = data.tilesets.start;
	TileSet* set = item->data;

	while (item->data && item->next)
	{
		if ((item->data->firstgid < id) && item->next->data->firstgid > id)
		{
			set = item->data;
			break;
		}
		item = item->next;
	}

	return set;
}

SDL_Rect TileSet::GetTileRect(int id) const
{
	int relativeId = id - firstgid;
	SDL_Rect rect =
	{
		margin + ((tile_w + spacing) * (relativeId % amount_w)),
		margin + ((tile_h + spacing) * (relativeId / amount_w)),
		tile_w,
		tile_h
	};
	return rect;
}

void Map::Load(const char* file)
{
	pugi::xml_parse_result result = mapFile.load_file(file);
	if (!result) printf("Could not load map xml file %s. pugi error: %s", file, result.description());

	LoadMap();

	pugi::xml_node tileset = mapFile.child("map").child("tileset");
	while (tileset)
	{
		TileSet* til = LoadTileset(tileset);
		data.tilesets.Add(til);
		//data.tilesets.Add(LoadTileset(tileset));//leak¿
		tileset = tileset.next_sibling("tileset");
	}

	pugi::xml_node layer = mapFile.child("map").child("layer");
	while (layer)
	{
		MapLayer* lay = LoadLayer(layer);
		data.layers.Add(lay);
		//data.layers.Add(LoadLayer(layer));//leak?
		layer = layer.next_sibling("layer");
	}

	printf("Successfully parsed map file: %s", file);
}

void Map::LoadMap()
{
	pugi::xml_node map = mapFile.child("map");
	data.width = map.attribute("width").as_int(0);
	data.height = map.attribute("height").as_int(0);
	data.tile_w = map.attribute("tilewidth").as_int(0);
	data.tile_h = map.attribute("tileheight").as_int(0);
}

TileSet* Map::LoadTileset(pugi::xml_node& tileset_node)
{
	TileSet* set = new TileSet;
	set->firstgid = tileset_node.attribute("firstgid").as_int(0);
	set->name = tileset_node.attribute("name").as_string();
	set->margin = tileset_node.attribute("margin").as_int(0);
	set->spacing = tileset_node.attribute("spacing").as_int(0);
	set->tile_h = tileset_node.attribute("tileheight").as_int(0);
	set->tile_w = tileset_node.attribute("tilewidth").as_int(0);
	set->amount_w = tileset_node.attribute("columns").as_int(0);
	set->amount_h = tileset_node.attribute("tilecount").as_int(0) / set->amount_w;
	char file[256];
	sprintf_s(file, 256, "Maps/%s", tileset_node.child("image").attribute("source").as_string());
	set->texture = LoadTexture(file);
	//set width/height num using texture size and tile size
	return set;
}

MapLayer* Map::LoadLayer(pugi::xml_node& layer_node)
{
	MapLayer* layer = new MapLayer;
	layer->name = layer_node.attribute("name").as_string();
	layer->width = layer_node.attribute("width").as_int(0);
	layer->height = layer_node.attribute("height").as_int(0);

	int size = layer->height * layer->width;
	layer->data = new unsigned int[size];

	pugi::xml_node tile = layer_node.child("data").child("tile");
	for (int i = 0; i < size; i++)
	{
		layer->data[i] = tile.attribute("gid").as_int(0);
		tile = tile.next_sibling("tile");
	}

	return layer;
}

void Map::LoadColliders()
{
	ListItem<MapLayer*>* L = data.layers.start;
	//break earlier if item null
	while (L && strcmp(L->data->name,"Collisions")) L = L->next;
	MapLayer* layer = L->data;

	data.walkability = new bool[layer->width * layer->height];
	memset(data.walkability, 1, layer->width * layer->height);

	for (int y = 0; y < layer->height; ++y)
	{
		for (int x = 0; x < layer->width; ++x)
		{
			int tileId = layer->Get(x, y);

			SDL_Rect rect = { mx2wx(x), my2wy(y), data.tile_w, data.tile_h };
			//if (tileId != 0) AddCollider(rect, Collider::Type::WALL);

			TileSet* tileset = (tileId > 0) ? GetTilesetFromTileId(tileId) : 0;
			if (tileset) data.walkability[x + y * layer->width] = (tileId - tileset->firstgid) > 0 ? 0 : 1;
		}
	}

	bool* added_colliders;
	added_colliders = new bool[layer->width * layer->height];
	memset(data.walkability, 0, layer->width * layer->height);

	for (unsigned int y = 0; y < layer->height; ++y)
	{
		for (unsigned int x = 0; x < layer->width; ++x)
		{
			if (data.walkability[x + y * layer->width] && !added_colliders[x + y * layer->width])
			{
				unsigned int i_x = 1, i_y = 1;

				while (data.walkability[(x + i_x) + y * layer->width] == true && added_colliders[(x + i_x) + y * layer->width] == false && ((x + i_x) < layer->width)) ++i_x;
				/*&& (x + i_x) < width ::: limit search loop at priority axis (row/column) end*/
				while (data.walkability[x + (y + i_y) * layer->width] == true && added_colliders[x + (y + i_y) * layer->width] == false) ++i_y;

				if (i_x >= i_y)
				{
					SDL_Rect rect{ (x * data.tile_w),(y * data.tile_h),(i_x * data.tile_w),(data.tile_h) };
					collisions.AddCollider(rect, COLLIDER_TYPE_WALL);
					for (unsigned int i = x; i < x + i_x; ++i) added_colliders[i + y * layer->height] = 1;
				}
				if (i_y > i_x)
				{
					SDL_Rect rect{ (x * data.tile_w),(y * data.tile_h),(data.tile_w),(i_y * data.tile_h) };
					collisions.AddCollider(rect, COLLIDER_TYPE_WALL);
					for (unsigned int i = y; i < y + i_y; ++i) added_colliders[x + i * layer->height] = 1;
				}
			}
		}
	}
}

#endif
