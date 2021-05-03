#ifndef __MAP_H__
#define __MAP_H__

#include "List.h"
#include "pugixml.hpp"
#include "Share.h"

struct TileSet
{
	SString	name;
	int	firstgid;
	int margin;
	int	spacing;
	int	tileWidth;
	int	tileHeight;

	SDL_Texture* texture;
	int	texWidth;
	int	texHeight;
	int	numTilesWidth;
	int	numTilesHeight;
	int	offsetX;
	int	offsetY;

	SDL_Rect GetTileRect(int id) const;
};

struct Properties
{
	struct Property
	{
		SString name;
		int value;
	};

	~Properties()
	{
		//...
	}


	int GetProperty(const char* name, int default_value = 0) const;

	List<Property*> list;
	Property property;
};

struct MapLayer
{
	SString	name;
	int width;
	int height;
	unsigned int* data = nullptr;

	Properties properties;

	MapLayer()
	{

	}

	~MapLayer()
	{
		delete data;
		data = nullptr;
	}

	inline unsigned int Get(int x, int y) const
	{
		return data[x + y * width];
	}
};

struct MapData
{
	int width;
	int	height;
	int	tileWidth;
	int	tileHeight;
	List<TileSet*> tilesets;
	List<MapLayer*> layers;
};

class Map
{
public:

	Map(pugi::xml_node& config);
	~Map();

	void Draw();

	void DrawLayer(int num);

	bool CleanUp();

	bool Load(const char* path);

	iPoint MapToWorld(int x, int y) const;
	iPoint WorldToMap(int x, int y) const;

	void ShowCollider() { drawColliders = !drawColliders; }

	void LoadColliders();

private:

	bool LoadMap();
	bool LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadLayer(pugi::xml_node& node, MapLayer* layer);
	bool LoadProperties(pugi::xml_node& node, Properties& properties);
	TileSet* GetTilesetFromTileId(int id) const;

public:
	MapData data;

private:


	pugi::xml_document mapFile;
	const char* folder;
	bool mapLoaded;

	bool drawColliders = false;

	SDL_Texture* tileX = nullptr;
};

Map::Map(pugi::xml_node& config)
{
	folder = "Assets/Maps/";// config.child("folder").attribute("name").as_string("Assets/Maps/");
}

Map::~Map()
{

}

void Map::Draw()
{
	if (!mapLoaded) return;

	for (int i = 0; i < data.layers.size; ++i)
	{
		if ((data.layers[i]->properties.GetProperty("drawable", 1) != 0) || drawColliders) DrawLayer(i);
	}
}


void Map::DrawLayer(int num)
{
	if (num < data.layers.size)
	{
		MapLayer* layer = data.layers[num];

		app->render->scale = scale;

		for (int y = 0; y < data.height; ++y)
		{
			for (int x = 0; x < data.width; ++x)
			{
				int tileId = layer->Get(x, y);

				if (tileId > 0)
				{
					TileSet* tileset = GetTilesetFromTileId(tileId);

					SDL_Rect rec = tileset->GetTileRect(tileId);
					iPoint pos = MapToWorld(x, y);

					app->render->DrawTexture(tileset->texture, pos.x + tileset->offsetX, pos.y + tileset->offsetY, &rec);
				}
			}
		}

		app->render->scale = 1;
	}
}

iPoint Map::MapToWorld(int x, int y) const
{
	iPoint ret;

	ret.x = x * data.tileWidth;
	ret.y = y * data.tileHeight;

	return ret;
}

// L05: TODO 2: Add orthographic world to map coordinates
iPoint Map::WorldToMap(int x, int y) const
{
	iPoint ret(0, 0);
	ret.x = x / data.tileWidth;
	ret.y = y / data.tileHeight;
	// L05: TODO 3: Add the case for isometric maps to WorldToMap

	return ret;
}

TileSet* Map::GetTilesetFromTileId(int id) const
{
	ListItem<TileSet*>* item = data.tilesets.start;
	TileSet* set = item->data;

	while (item->data != nullptr)
	{
		if (item->next == nullptr)
		{
			set = item->data;
			break;
		}
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
	SDL_Rect rect = { 0 };

	int relativeId = id - firstgid;
	rect.w = tileWidth;
	rect.h = tileHeight;
	rect.x = margin + ((rect.w + spacing) * (relativeId % numTilesWidth));
	rect.y = margin + ((rect.h + spacing) * (relativeId / numTilesWidth));

	return rect;
}

// Called before quitting
bool Map::CleanUp()
{
	LOG("Unloading map");

	ListItem<TileSet*>* item;
	item = data.tilesets.start;

	while (item != NULL)
	{
		delete item->data;
		item->data = nullptr;
		item = item->next;
	}
	data.tilesets.Clear();

	ListItem<MapLayer*>* item2;
	item2 = data.layers.start;

	while (item2 != NULL)
	{
		delete item2->data;
		item2->data = nullptr;
		item2 = item2->next;
	}
	data.layers.Clear();

	mapFile.reset();

	return true;
}

// Load new map
bool Map::Load(const char* filename)
{
	bool ret = true;
	SString tmp("%s%s", folder.GetString(), filename);

	pugi::xml_parse_result result = mapFile.load_file(tmp.GetString());

	if (result == NULL)
	{
		printf("Could not load map xml file %s. pugi error: %s", filename, result.description());
		ret = false;
	}

	// Load general info
	if (ret == true)
	{
		ret = LoadMap();
	}

	pugi::xml_node tileset;
	for (tileset = mapFile.child("map").child("tileset"); tileset && ret; tileset = tileset.next_sibling("tileset"))
	{
		TileSet* set = new TileSet();

		if (ret == true) ret = LoadTilesetDetails(tileset, set);

		if (ret == true) ret = LoadTilesetImage(tileset, set);

		data.tilesets.Add(set);
	}

	pugi::xml_node layer;
	for (layer = mapFile.child("map").child("layer"); layer && ret; layer = layer.next_sibling("layer"))
	{
		MapLayer* lay = new MapLayer();

		ret = LoadLayer(layer, lay);

		if (ret == true)
			data.layers.Add(lay);

		pugi::xml_node propertiesNode;
		for (propertiesNode = layer.child("properties"); propertiesNode && ret; propertiesNode = propertiesNode.next_sibling("properties"))
		{
			Properties* property = new Properties();

			ret = LoadProperties(propertiesNode, *property);

			lay->properties = *property;
		}
	}

	if (ret == true)
	{
		printf("Successfully parsed Successfully parsed map XML file: %s", filename);
		printf("width: %d  height: %d", data.width, data.height);

		for (int i = 0; i < data.tilesets.Count(); i++)
		{
			printf("Tileset %d ----", i + 1);
			printf("name: %s firstgid: %i", data.tilesets.At(i)->data->name.GetString(), data.tilesets.At(i)->data->firstgid);
			printf("tile width: %d tile height: %d", data.tilesets.At(i)->data->texWidth, data.tilesets.At(i)->data->texHeight);
			printf("spacing: %d margin: %d", data.tilesets.At(i)->data->spacing, data.tilesets.At(i)->data->margin);
		}
	}

	mapLoaded = ret;

	return ret;
}

bool Map::LoadMap()
{
	bool ret = true;
	pugi::xml_node map = mapFile.child("map");

	if (map == NULL)
	{
		printf("Error parsing map xml file: Cannot find 'map' tag.");
		ret = false;
	}
	else
	{
		data.width = map.attribute("width").as_int(0);
		data.height = map.attribute("height").as_int(0);
		data.tileWidth = map.attribute("tilewidth").as_int(0);
		data.tileHeight = map.attribute("tileheight").as_int(0);
	}

	return ret;
}

bool Map::LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;

	set->firstgid = tileset_node.attribute("firstgid").as_int(0);
	set->name = tileset_node.attribute("name").as_string();
	set->margin = tileset_node.attribute("margin").as_int(0);
	set->spacing = tileset_node.attribute("spacing").as_int(0);
	set->tileHeight = tileset_node.attribute("tileheight").as_int(0);
	set->tileWidth = tileset_node.attribute("tilewidth").as_int(0);
	set->numTilesWidth = tileset_node.attribute("columns").as_int(0);
	set->numTilesHeight = tileset_node.attribute("tilecount").as_int(0) / set->numTilesWidth;
	return ret;
}

bool Map::LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	pugi::xml_node image = tileset_node.child("image");


	if (image == NULL)
	{
		printf("Error parsing tileset xml file: Cannot find 'image' tag.");
		ret = false;
	}
	else
	{
		// L03: DONE: Load Tileset image
		set->texture = app->tex->Load(PATH(folder.GetString(), image.attribute("source").as_string()));
		int w, h;
		SDL_QueryTexture(set->texture, NULL, NULL, &w, &h);
		set->texWidth = image.attribute("width").as_int();

		if (set->texWidth <= 0)
		{
			set->texWidth = w;
		}

		set->texHeight = image.attribute("height").as_int();

		if (set->texHeight <= 0)
		{
			set->texHeight = h;
		}

		set->numTilesWidth = set->texWidth / set->tileWidth;
		set->numTilesHeight = set->texHeight / set->tileHeight;
	}

	return ret;
}

bool Map::LoadLayer(pugi::xml_node& node, MapLayer* layer)
{
	bool ret = true;

	layer->name = node.attribute("name").as_string("");
	layer->width = node.attribute("width").as_int(0);
	layer->height = node.attribute("height").as_int(0);
	int size = layer->height * layer->width;
	layer->data = new unsigned int[size];

	pugi::xml_node tile = node.child("data").child("tile");
	for (int i = 0; i < size; i++)
	{
		layer->data[i] = tile.attribute("gid").as_int(0);
		tile = tile.next_sibling("tile");
	}
	LoadProperties(node, layer->properties);
	return ret;
}

bool Map::LoadProperties(pugi::xml_node& node, Properties& properties)
{
	bool ret = true;


	pugi::xml_node property_ = node.child("property");

	Properties::Property* Prop = new Properties::Property();

	for (property_; property_ && ret; property_ = property_.next_sibling("property"))
	{
		Prop->name = property_.attribute("name").as_string("");
		Prop->value = property_.attribute("value").as_int(0);
		properties.list.Add(Prop);
	}

	return ret;
}

int Properties::GetProperty(const char* value, int defaultValue) const
{
	for (int i = 0; i < list.Count(); i++)
	{
		if (strcmp(list.At(i)->data->name.GetString(), value) == 0)
		{
			if (list.At(i)->data->value != defaultValue) return list.At(i)->data->value;
			else return defaultValue;
		}
	}

	return defaultValue;
}

void Map::LoadColliders()
{
	if (mapLoaded == false) return;


	ListItem<MapLayer*>* L = data.layers.start;
	ListItem<TileSet*>* T = data.tilesets.start;
	TileSet* tileSet = data.tilesets.start->data;
	unsigned int num = 401;
	while (L != nullptr)
	{
		MapLayer* layer = L->data;
		if (layer->properties.GetProperty("Collisions", 1) == 0)
		{

			L = L->next;
			continue;


		}

		for (int y = 0; y < layer->height; y++)
		{
			for (int x = 0; x < layer->width; x++)
			{


				int u = layer->Get(x, y);
				iPoint pos = MapToWorld(x, y);
				SDL_Rect n = { pos.x , pos.y, data.tileWidth, data.tileHeight };

				if (u != 0)
				{
					if (layer->properties.GetProperty("Collisions", 1) == 1)
					{
							app->collisions->AddCollider(n, Collider::Type::WALL, this);
					}
				}

			}
		}
		L = L->next;
	}
}

bool Map::CreateWalkabilityMap(int& width, int& height, uchar** buffer) const
{
	bool ret = false;
	ListItem<MapLayer*>* item;
	item = data.layers.start;

	for (item = data.layers.start; item != NULL; item = item->next)
	{
		MapLayer* layer = item->data;

		if (layer->properties.GetProperty("Nodraw", 0) == 0)
			continue;

		uchar* map = new uchar[layer->width * layer->height];
		memset(map, 1, layer->width * layer->height);

		for (int y = 0; y < data.height; ++y)
		{
			for (int x = 0; x < data.width; ++x)
			{
				int i = (y * layer->width) + x;

				int tileId = layer->Get(x, y);
				TileSet* tileset = (tileId > 0) ? GetTilesetFromTileId(tileId) : NULL;

				if (tileset != NULL)
				{
					map[i] = (tileId - tileset->firstgid) > 0 ? 0 : 1;
				}
			}
		}

		*buffer = map;
		width = data.width;
		height = data.height;
		ret = true;

		break;
	}

	return ret;
}

#endif
