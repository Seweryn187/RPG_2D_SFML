
#include <string>

class Npc
{
private:
	int npc_x, npc_y, game_state;
	std::string npc_name;
	std::string **dialogue;

public:
	Npc(int x, int y, std::string name, int game_state);
	void set_dialogue(std::string dialogue, int i, int j);
	std::string get_dialogue(int x, int y);
	int get_npc_x();
	int get_npc_y();
	int get_npc_game_state();
	std::string get_npc_name();
};

