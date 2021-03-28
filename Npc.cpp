#include "Npc.h"


Npc::Npc(int x, int y, std::string name, int game_state) {
	this->npc_x = x;
	this->npc_y = y;
	this->npc_name = name;
	this->game_state = game_state;
	dialogue = new std::string * [3];
	for (unsigned i = 0; i < 3; ++i) {
		dialogue[i] = new std::string[3];
	}
}


void Npc::set_dialogue(std::string dialogue, int i, int j) {
			this->dialogue[i][j] = dialogue;
}

std::string Npc::get_dialogue(int x, int y) {
	return dialogue[x][y];
}

int Npc::get_npc_x() {
	return npc_x;
}

int Npc::get_npc_y() {
	return npc_y;
}

int Npc::get_npc_game_state() {
	return game_state;
}

std::string Npc::get_npc_name() {
	return npc_name;
}