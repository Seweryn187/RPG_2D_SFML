#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <stdlib.h>
#include "Player.h"
#include "Npc.h"
#include "Enemy.h"
#include "Map.h"

class Engine
{
private:
	int player_x = 1;
	int	player_y = 21;
	int game_state = 200;
	int old_game_state = 0;
	int old_player_x;
	int old_player_y;
	int fight_action = 0;
	int which_button = 0;
	bool door_back_used = false;
	bool door_forward_used = false;
	std::string player_fight_log, enemy_fight_log, quest_string;
	sf::Sprite player0, player1, player2, player_big, npc, dialogue_window1, grass, sand, rock, door, background, enemy, enemy_big, dead_enemy, HP, sword,
		coin, game_over_background, log_window, stat_window, fight_background, wooden_wall, wooden_floor, control_window, bar, table, bush, barrel, enemy_boss,
		enemy_big_boss, dead_enemy_boss, window_s, quest_window, brick_wall;
	sf::Texture player0_tx, player1_tx, player2_tx, player_big_tx, npc_tx, dialogue_window_tx, grass_tx, sand_tx, rock_tx, door_tx, background_tx, enemy_tx,
		enemy_big_tx, dead_enemy_tx, HP_tx, sword_tx, coin_tx, game_over_background_tx, log_window_tx, stat_window_tx, fight_background_tx, wooden_wall_tx, wooden_floor_tx, control_window_tx,
		bar_tx, table_tx, bush_tx, barrel_tx, enemy_boss_tx, enemy_big_boss_tx, dead_enemy_boss_tx, window_tx, quest_window_tx, brick_wall_tx;
	sf::Font font;
	sf::Text npc_response_t, basic_npc_response_t, player_response_t, money_t, player_weapon_damage_t, player_dodge_chance_t, player_hp_t, game_over_t, list_of_actions,
		enemy_hp_t, enemy_weapon_damage_t, enemy_dodge_chance_t, enemy_type_t, player_last_action_t, enemy_last_action_t, controls_t, button_name, title, enemy_name, player_name, quest_text, quest_name;
	sf::View credits_view;


public:
	static Engine* engine_pt;
	sf::RenderWindow* window;
	sf::Event event;

	// Blok dotycz¹cy uruchomienia gry
	Engine();
	void game_mode(int x, int y);
	void game_mode();
	void run();

	//Blok eventów oraz rzeczy zwi¹zane z klawiatur¹
	void change_level(int game_state);
	void handle_events();
	void player_action();
	void movement(int m_direction);

	//Blok rysowania i ogarniania wiêkszych rzeczy jak mapa, koniec gry itp.
	void print_menu();
	void print_title();
	void print_button(std::string name, int i, int j, int offset, bool button_selected);
	void print_credits();
	void set_scene();
	void set_doors();
	void print_map();
	void border();
	void print_fight_scene();
	void game_over();

	//Blok umieszczania gracza itp.
	void set_player_position(int x, int y);
	void place_npcs();
	void place_enemys();

	//Blok przygotowañ
	void make_enemys();
	void prepare_textures();
	void make_dialogue(std::string greeting, std::string basic_replay, std::string farewell,
		std::string dialogue_opt_1, std::string dialogue_opt_2, std::string dialogue_opt_3, std::string answer1,
		std::string answer2, std::string answer3, std::string first_contact, std::string after_quest);

	//Blok interakcji
	void fight_handler();

	//Blok rysowania
	void draw_sand(int i, int j);
	void draw_grass(int i, int j);
	void draw_rock(int i, int j);
	void draw_wooden_wall(int i, int j);
	void draw_wooden_floor(int i, int j);
	void draw_brick_wall(int i, int j);
	void draw_bar(int i, int j);
	void draw_door(int i, int j);
	void draw_window(int i, int j);
	void draw_quest(int i, int j);
	void draw_table(int i, int j);
	void draw_bush(int i, int j);
	void draw_barrel(int i, int j);
	void draw_npc(int i, int j);
	void draw_enemy_rat(int i, int j);
	void draw_enemy_rat_big(int i, int j);
	void draw_dead_enemy_rat(int i, int j);
	void draw_enemy_boss(int i, int j);
	void draw_enemy_boss_big(int i, int j);
	void draw_dead_enemy_boss(int i, int j);
	void draw_background();
	void draw_game_over_background();
	void draw_player_statistics(int i, int j);
	void draw_help(int i, int j);
	void draw_enemy_statistics(int i, int j, Enemy enemy);
	void draw_player(int i, int j);
	void draw_player_big(int i, int j);
	void draw_dialogue();
	void draw_dialogue_window();
	void draw_action_window(int i, int j);
	void draw_stat_window(int i, int j);
	void draw_controls_window(int i, int j);
	void draw_log_window(int i, int j);
	void draw_quest_window(int i, int j);
	void draw_fight_background();

	//Utylity
	void reset();
	void which_button_guard();
	bool is_hit(int dodge_chance);
	std::string combine_string(std::string string1, std::string string2, std::string string3);
	unsigned** make_line(float x1, float y1, float x2, float y2);
};

