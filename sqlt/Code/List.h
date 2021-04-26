#ifndef __LIST_H__
#define __LIST_H__

template<class tdata>
struct ListItem
{
	tdata data;
	ListItem<tdata>* next;
	ListItem<tdata>* prev;

	ListItem(const tdata& _data)
	{
		data = _data;
		next = prev = nullptr;
	}
};

template<class tdata>
class List
{
public:

	ListItem<tdata>* start;
	ListItem<tdata>* end;

	unsigned int size;

	List()
	{
		start = end = nullptr;
		size = 0;
	}

	~List()
	{
		Clear();
	}

	ListItem<tdata>* Add(const tdata& item)
	{
		ListItem<tdata>* newItem = new ListItem<tdata>(item);

		if (!start) start = end = newItem;
		else
		{
			newItem->prev = end;
			end->next = newItem;
			end = newItem;
		}

		++size;
		return(newItem);
	}

	void Del(ListItem<tdata>* item)
	{
		item->prev->next = item->next;
		item->next->prev = item->prev;
		delete item;
		--size;
	}

	void Clear()
	{
		ListItem<tdata>* tmpData = start;
		ListItem<tdata>* tmpNext;

		while(tmpData)
		{
			tmpNext = tmpData->next;
			delete tmpData;
			tmpData = tmpNext;
		}

		start = end = nullptr;
		size = 0;
	}

};

#endif
