
#include "Field.h"

class Map
{
private:
	unsigned size;
	bool exist;

public:
	Map(unsigned size);
	~Map();
	bool create();
	bool fill(Field val);

	Field& operator () (unsigned x, unsigned y);
	Field** field_matrix;
};
