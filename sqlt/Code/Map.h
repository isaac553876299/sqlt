#ifndef __MAP_H__
#define __MAP_H__

#include "List.h"
#include "pugixml.hpp"
#include "Share.h"

struct TileSet
{
	const char* name;
	int	first_id;
	int margin;
	int	spacing;
	int	tile_w;
	int	tile_h;
	int	tileset_w;
	int	tileset_h;
	SDL_Texture* texture;

	SDL_Rect GetTileRect(int id) const;
};

struct Properties
{
	struct Property { const char* name; int value; };
	List<Property*> list;
	Property property;

	~Properties() { list.Clear(); }

	int GetProperty(const char* name, int default_value = 0) const;

};

struct MapLayer
{
	const char* name;
	int width;
	int height;
	unsigned int* data = nullptr;
	Properties properties;

	~MapLayer() { delete data; data = nullptr; }

	unsigned int Get(int x, int y) const { return data[x + y * width]; }

};

struct MapData
{
	int width;
	int	height;
	int	tile_w;
	int	tile_h;
	List<TileSet*> tilesets;
	List<MapLayer*> layers;
};

class Map
{
public:

	MapData data;
	pugi::xml_document mapFile;
	bool mapLoaded;
	bool drawColliders = false;

	Map();
	~Map();

	void Load(const char* path);

	void Draw();
	void DrawLayer(int num);

	void CleanUp();

	//MapToWorld
	int mx2wx(int x) const;
	int my2wy(int y) const;
	//WorldToMap
	int wx2mx(int x) const;
	int wy2my(int y) const;

	void LoadMap();
	TileSet* LoadTileset(pugi::xml_node& tileset_node);
	MapLayer* LoadLayer(pugi::xml_node& node);
	Properties* LoadProperties(pugi::xml_node& node);
	TileSet* GetTilesetFromTileId(int id) const;
	void LoadColliders();

	void CreateWalkabilityMap(int& width, int& height, unsigned char** buffer) const;

};

Map::Map()
{

}

Map::~Map()
{

}

void Map::Draw()
{
	if (mapLoaded)
	{
		for (int i = 0; i < data.layers.size; ++i)
		{
			if ((data.layers[i]->properties.GetProperty("draw", 1) != 0) || drawColliders) DrawLayer(i);
		}
	}
}

void Map::DrawLayer(int num)
{
	if (num < data.layers.size)
	{
		for (int y = 0; y < data.height; ++y)
		{
			for (int x = 0; x < data.width; ++x)
			{
				int tileId = data.layers[num]->Get(x, y);

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

	while (item->data)
	{
		if (!item->next)
		{
			set = item->data;
			break;
		}
		if ((item->data->first_id < id) && item->next->data->first_id > id)
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
	int relativeId = id - first_id;
	SDL_Rect rect =
	{
		margin + ((tile_w + spacing) * (relativeId % tileset_w)),
		margin + ((tile_h + spacing) * (relativeId / tileset_w)),
		tile_w,
		tile_h
	};
	return rect;
}

void Map::CleanUp()
{
	printf("Unloading map");

	data.tilesets.Clear();
	data.layers.Clear();

	mapFile.reset();
}

void Map::Load(const char* file)
{
	pugi::xml_parse_result result = mapFile.load_file(file);

	if (!result) printf("Could not load map xml file %s. pugi error: %s", file, result.description());

	LoadMap();

	pugi::xml_node tileset = mapFile.child("map").child("tileset");
	while (tileset)
	{
		TileSet* set = LoadTileset(tileset);

		data.tilesets.Add(set);

		tileset = tileset.next_sibling("tileset");
	}

	pugi::xml_node layer = mapFile.child("map").child("layer");
	while (layer)
	{
		MapLayer* lay = LoadLayer(layer);

		data.layers.Add(lay);

		pugi::xml_node propertiesNode = layer.child("properties");
		while (propertiesNode)
		{
			lay->properties = *LoadProperties(propertiesNode);
			propertiesNode = propertiesNode.next_sibling("properties");
		}

		layer = layer.next_sibling("layer");
	}

	printf("Successfully parsed map file: %s", file);

	mapLoaded = true;
}

void Map::LoadMap()
{
	pugi::xml_node map = mapFile.child("map");
	if (map)
	{
		data.width = map.attribute("width").as_int(0);
		data.height = map.attribute("height").as_int(0);
		data.tile_w = map.attribute("tilewidth").as_int(0);
		data.tile_h = map.attribute("tileheight").as_int(0);
	}
	else printf("Error parsing map xml file: Cannot find 'map' tag.");
}

TileSet* Map::LoadTileset(pugi::xml_node& tileset_node)
{
	TileSet* set = new TileSet;
	set->first_id = tileset_node.attribute("first_id").as_int(0);
	set->name = tileset_node.attribute("name").as_string();
	set->margin = tileset_node.attribute("margin").as_int(0);
	set->spacing = tileset_node.attribute("spacing").as_int(0);
	set->tile_h = tileset_node.attribute("tileheight").as_int(0);
	set->tile_w = tileset_node.attribute("tilewidth").as_int(0);
	set->tileset_w = tileset_node.attribute("columns").as_int(0);
	set->tileset_h = tileset_node.attribute("tilecount").as_int(0) / set->tileset_w;
	set->texture = LoadTexture(tileset_node.child("image").attribute("source").as_string());
	//set width/height num using texture size and tile size
	return set;
}

MapLayer* Map::LoadLayer(pugi::xml_node& node)
{
	MapLayer* layer = new MapLayer;
	layer->name = node.attribute("name").as_string();
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
	layer->properties = *LoadProperties(node);
	return layer;
}

Properties* Map::LoadProperties(pugi::xml_node& node)
{
	Properties* properties = new Properties;
	pugi::xml_node property = node.child("property");

	Properties::Property* Prop = new Properties::Property();

	while (property)
	{
		Prop->name = property.attribute("name").as_string();
		Prop->value = property.attribute("value").as_int(0);
		properties->list.Add(Prop);
		property = property.next_sibling("property");
	}
	return properties;
}

int Properties::GetProperty(const char* value, int defaultValue) const
{
	for (int i = 0; i < list.size; i++)
	{
		if (strcmp(list[i]->data->name, value) == 0)
		{
			return list[i]->data->value;
		}
	}

	return defaultValue;
}

void Map::LoadColliders()
{
	if (mapLoaded == false) return;

	ListItem<MapLayer*>* L = data.layers.start;
	while (L)
	{
		MapLayer* layer = L->data;
		if (layer->properties.GetProperty("Collisions", 1) == 1)
		{
			for (int y = 0; y < layer->height; ++y)
			{
				for (int x = 0; x < layer->width; ++x)
				{
					SDL_Rect rect = { mx2wx(x), my2wy(y), data.tile_w, data.tile_h };
					if (layer->Get(x, y) != 0)
					{
						AddCollider(rect, Collider::Type::WALL);
					}

				}
			}
		}
		L = L->next;
	}
}

void Map::CreateWalkabilityMap(int& width, int& height, unsigned char** buffer) const
{
	ListItem<MapLayer*>* item = data.layers.start;

	while (item && item->data->properties.GetProperty("Nodraw", 0) == 0) item = item->next;

	MapLayer* layer = item->data;

	unsigned char* map = new unsigned char[layer->width * layer->height];
	//memset(map, 1, layer->width * layer->height);

	for (int y = 0; y < data.height; ++y)
	{
		for (int x = 0; x < data.width; ++x)
		{
			int tileId = layer->Get(x, y);

			TileSet* tileset = (tileId > 0) ? GetTilesetFromTileId(tileId) : 0;

			if (tileset) map[x + y * layer->width] = (tileId - tileset->first_id) > 0 ? 0 : 1;
		}
	}

	*buffer = map;
	width = data.width;
	height = data.height;
}

#endif