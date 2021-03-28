#pragma once
class Field
{
private:
	int state = 0;

public:
	enum states { GRASS, SAND, ROCK, DOORFORWARD, DOORBACK, NPC, ENEMY, DEAD_ENEMY, DECORATION }; // GRASS = 0, SAND = 1, ROCK = 2, DOORFORWARD = 3, DOORBACK = 4, NPC = 5, ENEMY = 6, DEAD_ENEMY = 7, DECORATION = 8
	Field(int = states::GRASS);
	void set_state(int st);
	int get_state();
};

