#include "Map.h"
#include "Field.h"

Map::Map(unsigned size)
{
	exist = false;
	this->size = size;
	field_matrix = nullptr;
}

Map::~Map()
{
	if (field_matrix)
	{
		for (unsigned i = 0; i < size; ++i)
			if (field_matrix[i])
				delete[] field_matrix[i];
		delete[] field_matrix;
	}
}

Field& Map::operator () (unsigned x, unsigned y)
{
	return field_matrix[x][y];
}


bool Map::fill(Field val)
{
	if (exist)
	{
		for (unsigned i = 0; i < size; ++i)
			for (unsigned j = 0; j < size; ++j)
				field_matrix[i][j] = val;
		return true;
	}
	return false;
}

bool Map::create()
{
	Field val = Field::states::GRASS;
	field_matrix = new Field * [size];
	for (unsigned i = 0; i < size; ++i)
		field_matrix[i] = new Field[size];
	fill(val);
	return 1;
}