
#include <string>

class Enemy
{
private:
	int enemy_x, enemy_y, game_state, enemy_hit_points, type, enemy_dodge_chance, enemy_weapon_damage, enemy_start_hit_points;
	bool enemy_alive = true;
	std::string enemy_name;

public:
	enum types { RAT, BOSS };
	Enemy(int x, int y, std::string name, int game_state, int type);
	Enemy();
	int get_enemy_x();
	int get_enemy_y();
	int get_enemy_hit_points();
	void set_enemy_hit_points(int hit_points);
	int get_enemy_game_state();
	int get_enemy_type();
	std::string get_enemy_type(bool overloaded_function_return_string);
	int get_enemy_dodge_chance();
	int get_enemy_weapon_damage();
	bool enemy_is_alive();
	std::string get_enemy_name();
	void reset_enemy();
};

