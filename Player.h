#include <SFML/Graphics.hpp>

class Player
{
private:
	int player_hit_points, weapon_damage, amount_of_money, player_dodge_chance, player_start_hit_points,
		player_start_weapon_damage, player_start_amount_of_money;
	bool alive = true;

public:
	Player(int hit_points, int weapon_damage, int amount_of_money, int player_dodge_chance);
	int get_player_hit_points();
	void set_player_hit_points(int hit_points);
	int get_player_weapon_damage();
	void set_player_weapon_damage(int weapon_damage);
	int get_player_amount_of_money();
	int get_player_dodge_chance();
	void set_player_amount_of_money(int amount_of_money);
	bool is_alive();
	void reset_player();
};

