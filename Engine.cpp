#include "Engine.h"

// Zmienne globalne
int which_enemy = -1;
int which_response = 0;
int killed_rats = 0;
bool on_quest = false;
bool is_talking = false;
bool first_contact = true;
bool player_turn = false;
bool after_fight = false;
bool game_continue = false;
unsigned SIZE = 30;
int direction = 0;
const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;
const float scale = 0.8;
const float invScale = 1 / scale; 
const int NUMBER_OF_ENEMYS = 3;

Map map(SIZE);
Npc npc1 = Npc(24, 4, "Innkeeper", 1);
Enemy array_of_enemys[NUMBER_OF_ENEMYS];
Player player = Player(10, 1, 10, 20);
sf::Color grey(0x808080ff);
std::string** tab_npc;


// Blok dotycz¹cy uruchomienia gry

Engine::Engine() {
	game_mode();
	window->setFramerateLimit(60);
}

void Engine::game_mode(int x, int y)
{
	window = new sf::RenderWindow(sf::VideoMode(x, y), "Inns and rats: Roleplaying game");
}

void Engine::game_mode()
{
	window = new sf::RenderWindow(sf::VideoMode::getFullscreenModes()[0], "Inns and rats: Roleplaying game", sf::Style::Fullscreen);
}


void Engine::run()
{
	map.create();
	make_dialogue("Hi", "", "Goodbye.", "Give me beer", "Do you want some help?", "I will do it", 
		"Ok, here you are.", "Yes, I have problem with rats in the basement.", "Good luck, take my old sword.");
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			npc1.set_dialogue(tab_npc[i][j], i, j);
		}
	}
	prepare_textures();
	make_enemys();
	while (true) {
		switch (game_state) {
		case -1:
			while (game_state == -1) {
				game_over();
				handle_events();
				window->display();
			}
			break;
		case 0:
			if (!after_fight) {
				set_player_position(1, 21);
			}
			else {
				set_player_position(old_player_x, old_player_y);
				after_fight = false;
			}
			if (door_back_used) {
				set_player_position(6, 1);
				door_back_used = false;
			}
			while (game_state == 0) {
				if (!player.is_alive()) {
					game_state = -1;
				}
				set_scene();
				border();
				set_doors();
				print_map();
				draw_player(player_x, player_y);
				handle_events();
				if (is_talking) {
					draw_dialogue_window();
					draw_dialogue();
				}
				window->display();
			}
			break;
		case 1:
			if (!after_fight) {
				set_player_position(5, 28);
			}
			else {
				set_player_position(old_player_x, old_player_y);
				after_fight = false;
			}
			if (door_forward_used) {
				set_player_position(6, 28);
				door_forward_used = false;
			}
			if (door_back_used) {
				set_player_position(28, 4);
				door_back_used = false;
			}
			while (game_state == 1) {
				if (!player.is_alive()) {
					game_state = -1;
				}
				set_scene();
				border();
				set_doors();
				print_map();
				draw_bar(23, 1);
				draw_player(player_x, player_y);
				handle_events();
				if (is_talking) {
					draw_dialogue_window();
					draw_dialogue();
				}
				window->display();
			}
			break;
		case 2:
			if (!after_fight) {
				set_player_position(7, 28);
			}
			else {
				set_player_position(old_player_x, old_player_y);
				enemy_fight_log = "";
				after_fight = false;
			}
			if (door_forward_used) {
				set_player_position(28, 28);
				door_forward_used = false;
			}
			while (game_state == 2) {
				if (!player.is_alive()) {
					game_state = -1;
				}
				set_scene();
				border();
				set_doors();
				print_map();
				draw_player(player_x, player_y);
				handle_events();
				if (is_talking) {
					draw_dialogue_window();
					draw_dialogue();
				}
				window->display();
			}
		case 100:
			old_player_x = player_x;
			old_player_y = player_y;
			while (game_state == 100) {
				if (!player.is_alive()) {
					game_state = -1;
				}
				print_fight_scene();
				handle_events();
				fight_handler();
				window->display();
			}
			after_fight = true;
			break;
		case 200:
			while (game_state == 200) {
				print_menu();
				handle_events();
				window->display();
			}
			break;
		case 201:
			while (game_state == 201){
				print_credits();
			}
			break;
		}
	}
}

void Engine::make_enemys() {
	array_of_enemys[0] = Enemy(5, 10, "Basement rat", 2, Enemy::types::RAT);
	array_of_enemys[1] = Enemy(20, 10, "Basement rat", 2, Enemy::types::RAT);
	array_of_enemys[2] = Enemy(15, 3, "King of rats", 2, Enemy::types::BOSS);
}


//Blok eventów oraz rzeczy zwi¹zane z klawiatur¹

void Engine::change_level(int game_state) {
	old_game_state = this->game_state;
	this->game_state = game_state;
}


void Engine::handle_events()
{
	while (window->pollEvent(event)) {
		if ((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::Escape) && (game_state >= 0)) {
			change_level(200);
		}
		if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::A) && (is_talking == false)) {
			movement(0);
		}
		if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::W) && (is_talking == false)) {
			movement(1);
		}
		if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::S) && (is_talking == false)) {
			movement(2);
		}
		if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::D) && (is_talking == false)) {
			movement(3);
		}
		if ((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::Space)) {
			player_action();
		}
		if ((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::Q)) {
			is_talking = false;
		}
		if ((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::Num1) && (is_talking == true)) {
			if (player.get_player_amount_of_money() > 0 && player.get_player_hit_points() < 10) {
				player.set_player_amount_of_money(player.get_player_amount_of_money() - 2);
				player.set_player_hit_points(player.get_player_hit_points() + 1);
			}
			which_response = 1;
			first_contact = false;
		}
		if ((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::Num2) && (is_talking == true)) {
			which_response = 2;
			first_contact = false;
		}
		if ((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::Num3) && (is_talking == true)) {
			if (player.get_player_weapon_damage() < 2) {
				player.set_player_weapon_damage(player.get_player_weapon_damage() + 1);
			}
			which_response = 3;
			on_quest = true;
			first_contact = false;
		}
		if ((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::Num1) && (game_state>=100) && (game_state < 200)) {
			fight_action = 1;
		}
		if ((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::Num2) && (game_state >= 100) && (game_state < 200)) {
			fight_action = 2;
		}
		if ((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::Up) && (game_state >= 200)) {
			which_button -= 1;
			which_button_guard();
		}
		if ((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::Down) && (game_state >= 200)) {
			which_button += 1;
			which_button_guard();
		}
		if ((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::Enter) && (game_state >= 200)) {
			switch (which_button) {
			case 0:
				reset();
				change_level(0);
				break;
			case 1:
				change_level(201);
				break;
			case 2:
				window->close();
				break;
			}
		}
		if ((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::Escape) && (game_state == 201)) {
			change_level(0);
		}
		if ((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::Enter) && (game_state == -1)) {
			change_level(200);
		}
		if ((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::Escape) && (game_state == -1)) {
			window->close();
		}

	}
}

void Engine::player_action() {
	if ((map(player_x+1,player_y).get_state() == Field::states::DOORFORWARD || map(player_x - 1, player_y).get_state() == Field::states::DOORFORWARD)
		|| (map(player_x, player_y+1).get_state() == Field::states::DOORFORWARD || map(player_x, player_y - 1).get_state() == Field::states::DOORFORWARD)) {
		door_forward_used = true;
		change_level(game_state + 1);
	}
	if ((map(player_x + 1, player_y).get_state() == Field::states::DOORBACK || map(player_x - 1, player_y).get_state() == Field::states::DOORBACK)
		|| (map(player_x, player_y + 1).get_state() == Field::states::DOORBACK || map(player_x, player_y - 1).get_state() == Field::states::DOORBACK)) {
		door_back_used = true;
		change_level(game_state - 1);
	}
	if ((map(player_x + 1, player_y).get_state() == Field::states::NPC || map(player_x - 1, player_y).get_state() == Field::states::NPC)
		|| (map(player_x, player_y + 1).get_state() == Field::states::NPC || map(player_x, player_y - 1).get_state() == Field::states::NPC)) {
		is_talking = true;
	}
	if ((map(player_x + 1, player_y).get_state() == Field::states::ENEMY || map(player_x - 1, player_y).get_state() == Field::states::ENEMY)
		|| (map(player_x, player_y + 1).get_state() == Field::states::ENEMY || map(player_x, player_y - 1).get_state() == Field::states::ENEMY)) {
		for (int i = 0; i < NUMBER_OF_ENEMYS; i++) {
			if ((player_x + 1 == array_of_enemys[i].get_enemy_x() && player_y == array_of_enemys[i].get_enemy_y()) || (player_x - 1 == array_of_enemys[i].get_enemy_x() && player_y == array_of_enemys[i].get_enemy_y())
				|| (player_x == array_of_enemys[i].get_enemy_x() && player_y + 1== array_of_enemys[i].get_enemy_y()) || (player_x == array_of_enemys[i].get_enemy_x() && player_y - 1 == array_of_enemys[i].get_enemy_y())) {
				which_enemy = i;
				change_level(100);
			}
		}
	}
}

void Engine::movement(int m_direction) {
	switch (m_direction) {
	case 0:
		if (map(player_x - 1, player_y).get_state() < 2) {
			direction = 1;
			player_x--;
			break;
		}
		break;
	case 1:
		if (map(player_x, player_y - 1).get_state() < 2) {
			direction = 2;
			player_y--;
			break;
		}
		break;
	case 2:
		if (map(player_x, player_y + 1).get_state() < 2) {
			direction = 0;
			player_y++;
			break;
		}
		break;
	case 3:
		if (map(player_x + 1, player_y).get_state() < 2) {
			direction = 0;
			player_x++;
			break;
		}
		break;
	}
}


//Blok rysowania i ogarniania wiêkszych rzeczy jak mapa, koniec gry itp.


void Engine::print_menu() {
	draw_background();
	switch (which_button) {
	case 0:
		print_title();
		print_button("START", 800, 250, 10, true);
		print_button("CREDITS", 800, 450, -15, false);
		print_button("EXIT", 800, 650, 30, false);
		break;
	case 1:
		print_title();
		print_button("START", 800, 250, 10, false);
		print_button("CREDITS", 800, 450, -15, true);
		print_button("EXIT", 800, 650, 30, false);
		break;
	case 2:
		print_title();
		print_button("START", 800, 250, 10, false);
		print_button("CREDITS", 800, 450, -15, false);
		print_button("EXIT", 800, 650, 30, true);
		break;
	}
}

void Engine::print_title() {
	title.setFont(font);
	title.setCharacterSize(70);
	title.setOutlineColor(sf::Color::Black);
	title.setOutlineThickness(5);
	title.setFillColor(sf::Color::White);
	title.setStyle(sf::Text::Bold);
	title.setPosition(500, 50);
	title.setString("Inns and rats: Roleplaying game");
	window->draw(title);
}

void Engine::print_button(std::string name, int i, int j, int offset, bool button_selected) {
	sf::RectangleShape button;
	button.setFillColor(grey);
	if (button_selected) {
		button.setOutlineColor(sf::Color::Red);
	}
	else {
		button.setOutlineColor(sf::Color::Black);
	}
	button.setOutlineThickness(5);
	button.setSize(sf::Vector2f(280,100));
	button.setPosition(i, j);
	button_name.setFont(font);
	button_name.setCharacterSize(50);
	button_name.setOutlineColor(sf::Color::Black);
	button_name.setOutlineThickness(5);
	button_name.setFillColor(sf::Color::White);
	button_name.setStyle(sf::Text::Bold);
	button_name.setPosition(i+50+offset, j+20);
	button_name.setString(name);
	window->draw(button);
	window->draw(button_name);
}

void Engine::print_credits() {
	credits_view.setSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	credits_view.setCenter(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
	credits_view.setViewport(sf::FloatRect(0, (1 - scale) / 2, scale, scale));
	sf::Text titleCredit("Credits", font, SCREEN_HEIGHT / 12);
	titleCredit.setPosition((SCREEN_WIDTH - titleCredit.getGlobalBounds().width) / 2, 0);
	titleCredit.setFillColor(sf::Color::White);
	sf::Text creditText("Creators and Lead Developers:", font, SCREEN_WIDTH / 24);
	creditText.setPosition((SCREEN_WIDTH * invScale - creditText.getGlobalBounds().width) / 2, SCREEN_HEIGHT);
	creditText.setFillColor(sf::Color::White);
	sf::Text creditText2("Seweryn Cholewa and Dariusz Stefanski", font, SCREEN_WIDTH / 24);
	creditText2.setPosition((SCREEN_WIDTH * invScale - creditText2.getGlobalBounds().width) / 2, creditText.getGlobalBounds().top + creditText.getGlobalBounds().height);
	creditText2.setFillColor(sf::Color::Red);
	bool quit = false;
	while (window->isOpen() && !quit) {
		sf::Event event;
		while (window->pollEvent(event)) {
			if ((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::Escape) && (game_state == 201)) {
				change_level(200);
				quit = true;
			}
		}
			credits_view.move(0, SCREEN_HEIGHT * 0.0005);
			if (credits_view.getCenter().y > SCREEN_HEIGHT * 2 + creditText.getGlobalBounds().height + creditText2.getGlobalBounds().height) {
				credits_view.setCenter(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
			}
			window->clear();
			window->setView(credits_view);
			window->draw(creditText);
			window->draw(creditText2);
			window->setView(window->getDefaultView());
			window->draw(titleCredit);
			handle_events();
			window->display();
	}
}

void Engine::set_scene() {
	unsigned** tab = 0;
	switch (game_state) {
	case 0:
		for (int i = 0; i < SIZE; i++) {
			for (int j = 0; j < SIZE; j++) {
				map(i, j).set_state(Field::states::GRASS);
			}
		}
		tab = new unsigned* [30];
		for (unsigned h = 0; h < 30; h++) {
			tab[h] = new unsigned[2];
		}
		tab = make_line(0, 21, 5, 21);
		int x, y;
		for (unsigned i = 0; i < 6; i++) {
			x = tab[i][0];
			y = tab[i][1];
			map(x, y).set_state(Field::states::SAND);
		}
		for (unsigned h = 0; h < 30; h++) {
			tab[h] = new unsigned[2];
		}
		tab = make_line(6, 0, 6, 20);
		for (unsigned i = 0; i < 20; i++) {
			x = tab[i][0];
			y = tab[i][1];
			map(x, y).set_state(Field::states::SAND);
		}
		map(15, 10).set_state(Field::states::DECORATION);
		map(3, 3).set_state(Field::states::DECORATION);
		map(20, 25).set_state(Field::states::DECORATION);
		break;
	case 1:
		for (int i = 0; i < SIZE; i++) {
			for (int j = 0; j < SIZE; j++) {
				map(i, j).set_state(Field::states::SAND);
			}
		}
		place_npcs();
		map(5, 10).set_state(Field::states::DECORATION);
		map(10, 10).set_state(Field::states::DECORATION);
		map(5, 20).set_state(Field::states::DECORATION);
		map(10, 20).set_state(Field::states::DECORATION);
		for (int k = 1; k < 11; k++) {
			map(23, k).set_state(Field::states::DECORATION);
		}
		break;
	case 2:
		for (int i = 0; i < SIZE; i++) {
			for (int j = 0; j < SIZE; j++) {
				map(i, j).set_state(Field::states::SAND);
			}
		}
		map(3, 4).set_state(Field::states::DECORATION);
		map(10, 10).set_state(Field::states::DECORATION);
		map(20, 15).set_state(Field::states::DECORATION);
		map(25, 25).set_state(Field::states::DECORATION);
		place_enemys();
		break;
	}
}

void Engine::set_doors() {
	switch (game_state) {
	case 0:
		map(6, 0).set_state(Field::states::DOORFORWARD);
		break;
	case 1:
		map(6, 29).set_state(Field::states::DOORBACK);
		map(29, 4).set_state(Field::states::DOORFORWARD);
		break;
	case 2:
		map(28, 29).set_state(Field::states::DOORBACK);
		break;
	}
}	

void Engine::print_map() {
	draw_background();
	draw_quest(200, 500);
	draw_controls(200, 150);
	draw_player_statistics(1500, 150);
	for (unsigned i = 0; i < SIZE; i++)
	{
		for (unsigned j = 0; j < SIZE; j++)
		{

			if (map(i, j).get_state() == Field::states::GRASS) {
				draw_grass(i, j);
			}
			if (map(i, j).get_state() == Field::states::SAND) {
				switch (game_state) {
				case 0:
					draw_sand(i, j);
					break;
				case 1:
					draw_wooden_floor(i, j);
					break;
				case 2:
					draw_wooden_floor(i, j);
					break;
				}
			}
			if (map(i, j).get_state() == Field::states::ROCK) {
				switch (game_state) {
				case 0:
					if (j != 0) {
						draw_grass(i, j);
						draw_rock(i, j);
					}
					else if (i == 10 || i == 20) {
						draw_wooden_wall(i, j);
						draw_window(i, j);
					}
					else {
						draw_wooden_wall(i, j);
					}
					break;

				case 1:
					if (i == 10 && j == 29 || i == 20 && j == 29) {
						draw_wooden_wall(i, j);
						draw_window(i, j);
					}
					else {
						draw_wooden_wall(i, j);
					}
					break;
				case 2:
					draw_brick_wall(i, j);
					break;
				}
			}
			if (map(i, j).get_state() == Field::states::DOORFORWARD || map(i, j).get_state() == Field::states::DOORBACK) {
				draw_door(i, j);
			}
			if (map(i, j).get_state() == Field::states::NPC) {
				switch (game_state) {
				case 0:
					draw_grass(i, j);
					break;
				case 1:
					draw_wooden_floor(i, j);
					break;
				}
				draw_npc(i, j);
			}
			if (map(i, j).get_state() == Field::states::ENEMY) {
				switch (game_state) {
				case 0:
					draw_grass(i, j);
					break;
				case 1:
					draw_wooden_floor(i, j);
					break;
				case 2:
					draw_wooden_floor(i, j);
					break;
				}
				for (int k = 0; k < NUMBER_OF_ENEMYS; k++) {
					if(array_of_enemys[k].get_enemy_x() == i && array_of_enemys[k].get_enemy_y() == j){
						if (array_of_enemys[k].enemy_is_alive()) {
							if (array_of_enemys[k].get_enemy_type() == Enemy::types::RAT) {
								draw_enemy_rat(i, j);
							}
							else {
								draw_enemy_boss(i, j);
							}
						}
						else {
							map(i, j).set_state(Field::states::DEAD_ENEMY);
							if (array_of_enemys[k].get_enemy_type() == Enemy::types::RAT) {
								draw_dead_enemy_rat(i, j);
							}
							else {
								draw_dead_enemy_boss(i, j);
							}
						}
					}
				}
			}
			if (map(i, j).get_state() == Field::states::DECORATION) {
				switch (game_state) {
				case 0:
					draw_grass(i, j);
					draw_bush(i, j);
					break;
				case 1:
					draw_wooden_floor(i, j);
					if (i == 23 && j > 0 && j < 11) {
					}
					else {
						draw_table(i, j);
					}
					break;
				case 2:
					draw_wooden_floor(i, j);
					draw_barrel(i,j);
					break;
				}
			}
		}
	}
}

void Engine::border() {
	unsigned** tab = 0;
	tab = new unsigned* [30];
	for (unsigned h = 0; h < 30; h++) {
		tab[h] = new unsigned[2];
	}
	tab = make_line(-1, 0, 30, 0);
	int x, y;
	for (unsigned i = 0; i < 30; i++) {
		x = tab[i][0];
		y = tab[i][1];
		map(x, y).set_state(2);
	}
	tab = make_line(0, 0, 0, 30);
	for (unsigned i = 0; i < 30; i++) {
		x = tab[i][0];
		y = tab[i][1];
		map(x, y).set_state(2);
	}
	tab = make_line(-1, 29, 30, 29);
	for (unsigned i = 0; i < 30; i++) {
		x = tab[i][0];
		y = tab[i][1];
		map(x, y).set_state(2);
	}
	tab = make_line(29, 0, 29, 29);
	for (unsigned i = 0; i < 30; i++) {
		x = tab[i][0];
		y = tab[i][1];
		map(x, y).set_state(2);
	}
}

void Engine::print_fight_scene() {
	draw_fight_background();
	draw_player_statistics(200, 350);
	draw_action_window(450, 350);
	draw_enemy_statistics(900, 100, array_of_enemys[which_enemy]);
	if (array_of_enemys[which_enemy].get_enemy_type() == Enemy::types::RAT) {
		draw_enemy_rat_big(1150, 70);
	}
	else {
		draw_enemy_boss_big(1150, 70);
	}
	draw_player_big(250, 675);
	draw_log_window(1150,675);
}



void Engine::game_over() {
	game_over_t.setFont(font);
	game_over_t.setCharacterSize(120);
	game_over_t.setOutlineColor(sf::Color::Black);
	game_over_t.setOutlineThickness(5);
	game_over_t.setFillColor(sf::Color::Red);
	game_over_t.setStyle(sf::Text::Bold);
	game_over_t.setPosition(500, 250);
	game_over_t.setString("      Game Over\n      ESC to quit\nEnter to go to menu");
	draw_game_over_background();
	window->draw(game_over_t);
}

//Blok umieszczania gracza itp.

void Engine::set_player_position(int x, int y) {
	player_x = x;
	player_y = y;
}

void Engine::place_npcs() {
	if (game_state == npc1.get_npc_game_state()) {
		map(npc1.get_npc_x(), npc1.get_npc_y()).set_state(Field::states::NPC);
	}

}

void Engine::place_enemys() {
	for (int i = 0; i < NUMBER_OF_ENEMYS; i++) {
		if (game_state == array_of_enemys[i].get_enemy_game_state()) {
			map(array_of_enemys[i].get_enemy_x(), array_of_enemys[i].get_enemy_y()).set_state(Field::states::ENEMY);
		}
	}
}

//Blok przygotowañ



void Engine::prepare_textures() {
	player0_tx.loadFromFile("textures/player_0.png");
	player0.setTexture(player0_tx);
	player1_tx.loadFromFile("textures/player_1.png");
	player1.setTexture(player1_tx);
	player2_tx.loadFromFile("textures/player_2.png");
	player2.setTexture(player2_tx);
	player_big_tx.loadFromFile("textures/player_big.png");
	player_big.setTexture(player_big_tx);
	npc_tx.loadFromFile("textures/npc.bmp");
	npc.setTexture(npc_tx);
	dialogue_window_tx.loadFromFile("textures/dialogue_window.png");
	dialogue_window1.setTexture(dialogue_window_tx);
	control_window_tx.loadFromFile("textures/stat_window.png");
	control_window.setTexture(control_window_tx);
	quest_window_tx.loadFromFile("textures/stat_window.png");
	quest_window.setTexture(quest_window_tx);
	grass_tx.loadFromFile("textures/grass.bmp");
	grass.setTexture(grass_tx);
	sand_tx.loadFromFile("textures/sand.bmp");
	sand.setTexture(sand_tx);
	rock_tx.loadFromFile("textures/rock.bmp");
	rock.setTexture(rock_tx);
	wooden_wall_tx.loadFromFile("textures/wooden_wall.png");
	wooden_wall.setTexture(wooden_wall_tx);
	brick_wall_tx.loadFromFile("textures/brick_wall.png");
	brick_wall.setTexture(brick_wall_tx);
	wooden_floor_tx.loadFromFile("textures/wooden_floor.png");
	wooden_floor.setTexture(wooden_floor_tx);
	bar_tx.loadFromFile("textures/bar.png");
	bar.setTexture(bar_tx);
	door_tx.loadFromFile("textures/door.png");
	door.setTexture(door_tx);
	window_tx.loadFromFile("textures/window.png");
	window_s.setTexture(window_tx);
	table_tx.loadFromFile("textures/table.png");
	table.setTexture(table_tx);
	bush_tx.loadFromFile("textures/bush.png");
	bush.setTexture(bush_tx);
	barrel_tx.loadFromFile("textures/barrel.png");
	barrel.setTexture(barrel_tx);
	background_tx.loadFromFile("textures/background.png");
	background.setTexture(background_tx);
	enemy_tx.loadFromFile("textures/enemy.png");
	enemy.setTexture(enemy_tx);
	enemy_big_tx.loadFromFile("textures/enemy_big.png");
	enemy_big.setTexture(enemy_big_tx);
	dead_enemy_tx.loadFromFile("textures/dead_enemy.png");
	dead_enemy.setTexture(dead_enemy_tx);
	enemy_boss_tx.loadFromFile("textures/enemy_boss.png");
	enemy_boss.setTexture(enemy_boss_tx);
	enemy_big_boss_tx.loadFromFile("textures/enemy_big_boss.png");
	enemy_big_boss.setTexture(enemy_big_boss_tx);
	dead_enemy_boss_tx.loadFromFile("textures/dead_enemy_boss.png");
	dead_enemy_boss.setTexture(dead_enemy_boss_tx);
	HP_tx.loadFromFile("textures/HP.png");
	HP.setTexture(HP_tx);
	sword_tx.loadFromFile("textures/sword.png");
	sword.setTexture(sword_tx);
	coin_tx.loadFromFile("textures/coin.png");
	coin.setTexture(coin_tx);
	game_over_background_tx.loadFromFile("textures/game_over_background.png");
	game_over_background.setTexture(game_over_background_tx);
	stat_window_tx.loadFromFile("textures/stat_window.png");
	stat_window.setTexture(stat_window_tx);
	log_window_tx.loadFromFile("textures/log_window.png");
	log_window.setTexture(log_window_tx);
	fight_background_tx.loadFromFile("textures/fight_background.png");
	fight_background.setTexture(fight_background_tx);
	font.loadFromFile("font/times-new-roman.ttf");
}

void Engine::make_dialogue(std::string greeting, std::string basic_replay, std::string farewell,
	std::string dialogue_opt_1, std::string dialogue_opt_2, std::string dialogue_opt_3, std::string answer1,
	std::string answer2, std::string answer3) {
	tab_npc = new std::string * [3];
	for (unsigned i = 0; i < 3; ++i) {
		tab_npc[i] = new std::string[3];
	}
	tab_npc[0][0] = greeting;
	tab_npc[0][1] = basic_replay;
	tab_npc[0][2] = farewell;
	tab_npc[1][0] = dialogue_opt_1;
	tab_npc[1][1] = dialogue_opt_2;
	tab_npc[1][2] = dialogue_opt_3;
	tab_npc[2][0] = answer1;
	tab_npc[2][1] = answer2;
	tab_npc[2][2] = answer3;
}

//Blok interakcji

void Engine::fight_handler() {
	if(fight_action != 0) {
		switch (fight_action) {
		case 0:
			break;
		case 1:
			if (is_hit(array_of_enemys[which_enemy].get_enemy_dodge_chance())) {
				array_of_enemys[which_enemy].set_enemy_hit_points(array_of_enemys[which_enemy].get_enemy_hit_points() - player.get_player_weapon_damage());
				player_fight_log = "Player last action:\nLight attack hit and dealt ";
				player_fight_log.append(std::to_string(player.get_player_weapon_damage()));
				player_fight_log.append(" dmg");
			}
			else {
				player_fight_log = "Player last action:\nLight attack missed";
			}
			break;
		case 2:
			if (is_hit(array_of_enemys[which_enemy].get_enemy_dodge_chance() * 2)) {
				array_of_enemys[which_enemy].set_enemy_hit_points(array_of_enemys[which_enemy].get_enemy_hit_points() - player.get_player_weapon_damage() * 2);
				player_fight_log = "Player last action:\nHeavy attack hit and dealt ";
				player_fight_log.append(std::to_string(player.get_player_weapon_damage()*2));
				player_fight_log.append(" dmg");
			}
			else {
				player_fight_log = "Player last action:\nHeavy attack missed";
			}
			break;
		}
		if (array_of_enemys[which_enemy].enemy_is_alive() == false) {
			if (array_of_enemys[which_enemy].get_enemy_type() == Enemy::types::RAT) {
				player.set_player_amount_of_money(player.get_player_amount_of_money() + 2);
			}
			else {
				player.set_player_amount_of_money(player.get_player_amount_of_money() + 4);
			}
			player_fight_log = "";
			if(killed_rats<4){
				killed_rats += 1;
			}
			if (on_quest && killed_rats == 3) {
				player.set_player_amount_of_money(player.get_player_amount_of_money() + 6);
				on_quest = false;
			}
			game_state = old_game_state;
			fight_action = 0;
		}
		if (fight_action != 0) {
			if (is_hit(player.get_player_dodge_chance())) {
				int critical = (rand() % 19) + 1;
				if (critical > 2) {
					player.set_player_hit_points(player.get_player_hit_points() - array_of_enemys[which_enemy].get_enemy_weapon_damage());
					enemy_fight_log = "Enemy last action:\nNormal attack hit and dealt ";
					enemy_fight_log.append(std::to_string(array_of_enemys[which_enemy].get_enemy_weapon_damage()));
					enemy_fight_log.append(" dmg");
				}
				else {
					player.set_player_hit_points(player.get_player_hit_points() - array_of_enemys[which_enemy].get_enemy_weapon_damage() * 2);
					enemy_fight_log = "Enemy last action:\nCRITICAL attack hit and dealt ";
					enemy_fight_log.append(std::to_string(array_of_enemys[which_enemy].get_enemy_weapon_damage() * 2));
					enemy_fight_log.append(" dmg");
				}
			}
			else {
				enemy_fight_log = "Enemy last action:\nAttack missed";
			}
		}
		fight_action = 0;
	}
}

//Blok rysowania

void Engine::draw_grass(int i, int j)
{
	grass.setPosition(i * 35.f + (960 - (SIZE * 35) / 2), j * 35.f + (540 - (SIZE * 35) / 2));
	window->draw(grass);
}

void Engine::draw_sand(int i, int j)
{
	sand.setPosition(i * 35.f + (960 - (SIZE * 35) / 2), j * 35.f + (540 - (SIZE * 35) / 2));
	window->draw(sand);
}

void Engine::draw_rock(int i, int j)
{
	
	rock.setPosition(i * 35.f + (960 - (SIZE*35) / 2), j * 35.f + (540 - (SIZE * 35) / 2));
	window->draw(rock);
}

void Engine::draw_wooden_wall(int i, int j) {

	wooden_wall.setPosition(i * 35.f + (960 - (SIZE * 35) / 2), j * 35.f + (540 - (SIZE * 35) / 2));
	window->draw(wooden_wall);
}

void Engine::draw_wooden_floor(int i, int j) {

	wooden_floor.setPosition(i * 35.f + (960 - (SIZE * 35) / 2), j * 35.f + (540 - (SIZE * 35) / 2));
	window->draw(wooden_floor);
}

void Engine::draw_brick_wall(int i, int j) {
	brick_wall.setPosition(i * 35.f + (960 - (SIZE * 35) / 2), j * 35.f + (540 - (SIZE * 35) / 2));
	window->draw(brick_wall);
}

void Engine::draw_bar(int i, int j) {
	bar.setPosition(i * 35.f + (960 - (SIZE * 35) / 2), j * 35.f + (540 - (SIZE * 35) / 2));
	window->draw(bar);
}

void Engine::draw_door(int i, int j)
{
	door.setPosition(i * 35.f + (960 - (SIZE * 35) / 2), j * 35.f + (540 - (SIZE * 35) / 2));
	window->draw(door);
}

void Engine::draw_table(int i, int j)
{
	table.setPosition(i * 35.f + (960 - (SIZE * 35) / 2), j * 35.f + (540 - (SIZE * 35) / 2));
	window->draw(table);
}

void Engine::draw_bush(int i, int j)
{
	bush.setPosition(i * 35.f + (960 - (SIZE * 35) / 2), j * 35.f + (540 - (SIZE * 35) / 2));
	window->draw(bush);
}


void Engine::draw_window(int i, int j)
{
	window_s.setPosition(i * 35.f + (960 - (SIZE * 35) / 2), j * 35.f + (540 - (SIZE * 35) / 2));
	window->draw(window_s);
}

void Engine::draw_barrel(int i, int j)
{
	barrel.setPosition(i * 35.f + (960 - (SIZE * 35) / 2), j * 35.f + (540 - (SIZE * 35) / 2));
	window->draw(barrel);
}

void Engine::draw_background() {
	background.setPosition(0, 0);
	window->draw(background);
}

void Engine::draw_enemy_rat(int i, int j) {
	enemy.setPosition(i * 35.f + (960 - (SIZE * 35) / 2), j * 35.f + (540 - (SIZE * 35) / 2));
	window->draw(enemy);
}

void Engine::draw_enemy_rat_big(int i, int j) {
	enemy_big.setPosition(i, j);
	window->draw(enemy_big);
}

void Engine::draw_dead_enemy_rat(int i, int j) {
	dead_enemy.setPosition(i * 35.f + (960 - (SIZE * 35) / 2), j * 35.f + (540 - (SIZE * 35) / 2));
	window->draw(dead_enemy);
}

void Engine::draw_enemy_boss(int i, int j) {
	enemy_boss.setPosition(i * 35.f + (960 - (SIZE * 35) / 2), j * 35.f + (540 - (SIZE * 35) / 2));
	window->draw(enemy_boss);
}

void Engine::draw_enemy_boss_big(int i, int j) {
	enemy_big_boss.setPosition(i, j);
	window->draw(enemy_big_boss);
}

void Engine::draw_dead_enemy_boss(int i, int j) {
	dead_enemy_boss.setPosition(i * 35.f + (960 - (SIZE * 35) / 2), j * 35.f + (540 - (SIZE * 35) / 2));
	window->draw(dead_enemy_boss);
}

void Engine::draw_game_over_background() {
	game_over_background.setPosition(0,0);
	window->draw(game_over_background);
}

void Engine::draw_controls(int i, int j) {
	std::string controls = "1. Postacia poruszamy za\npomoca klawiszy WSAD\n\n2. Akcje(czyli rozmowa\nz NPC, walka czy\nprzechodzenie przez drzwi)\nwykonujemy za pomoca\nspacji (nie mozesz stac \nna ukos od celu)";
	controls_t.setString(controls);
	controls_t.setFont(font);
	controls_t.setCharacterSize(16);
	controls_t.setOutlineColor(sf::Color::Black);
	controls_t.setOutlineThickness(2);
	controls_t.setFillColor(sf::Color::White);
	controls_t.setStyle(sf::Text::Bold);
	controls_t.setPosition(i+10, j+10);
	draw_controls_window(i, j);
	window->draw(controls_t);
}

void Engine::draw_player_statistics(int i, int j)
{
	std::string hp_s = std::to_string(player.get_player_hit_points());
	player_hp_t.setString(hp_s);
	std::string weapon_damage_s = std::to_string(player.get_player_weapon_damage());
	player_weapon_damage_t.setString(weapon_damage_s);
	std::string money_s = std::to_string(player.get_player_amount_of_money());
	money_t.setString(money_s);
	player_name.setString("Player:");
	player_name.setFont(font);
	player_name.setCharacterSize(24);
	player_name.setOutlineColor(sf::Color::Black);
	player_name.setFillColor(sf::Color::White);
	player_name.setOutlineThickness(2);
	player_name.setStyle(sf::Text::Bold);
	player_hp_t.setFont(font);
	player_hp_t.setCharacterSize(50);
	player_hp_t.setOutlineColor(sf::Color::Black);
	player_hp_t.setOutlineThickness(2);
	player_hp_t.setFillColor(sf::Color::White);
	player_hp_t.setStyle(sf::Text::Bold);
	player_weapon_damage_t.setFont(font);
	player_weapon_damage_t.setCharacterSize(50);
	player_weapon_damage_t.setOutlineColor(sf::Color::Black);
	player_weapon_damage_t.setFillColor(sf::Color::White);
	player_weapon_damage_t.setOutlineThickness(2);
	player_weapon_damage_t.setStyle(sf::Text::Bold);
	money_t.setFont(font);
	money_t.setCharacterSize(50);
	money_t.setOutlineColor(sf::Color::Black);
	money_t.setFillColor(sf::Color::White);
	money_t.setOutlineThickness(2);
	money_t.setStyle(sf::Text::Bold);
	player_name.setPosition(i + 10, j + 10);
	player_hp_t.setPosition(i+70, j + 30);
	player_weapon_damage_t.setPosition(i+70, j + 95);
	money_t.setPosition(i+70, j + 160);
	HP.setPosition(i+10, j+35);
	sword.setPosition(i+10, j+100);
	coin.setPosition(i+10, j+165);
	draw_stat_window(i, j);
	window->draw(player_name);
	window->draw(HP);
	window->draw(sword);
	window->draw(coin);
	window->draw(player_hp_t);
	window->draw(player_weapon_damage_t);
	window->draw(money_t);
}

void Engine::draw_enemy_statistics(int i, int j, Enemy enemy) {
	std::string hp_s = std::to_string(enemy.get_enemy_hit_points());
	enemy_hp_t.setString(hp_s);
	std::string weapon_damage_s = std::to_string(enemy.get_enemy_weapon_damage());
	enemy_weapon_damage_t.setString(weapon_damage_s);
	std::string text_dodge_chance = "Dodge: ";
	std::string dodge_chance_s = text_dodge_chance.append(std::to_string(enemy.get_enemy_dodge_chance()));
	enemy_name.setString(enemy.get_enemy_name());
	enemy_name.setFont(font);
	enemy_name.setCharacterSize(24);
	enemy_name.setOutlineColor(sf::Color::Black);
	enemy_name.setFillColor(sf::Color::White);
	enemy_name.setOutlineThickness(2);
	enemy_name.setStyle(sf::Text::Bold);
	enemy_type_t.setString(enemy.get_enemy_type(true));
	enemy_hp_t.setFont(font);
	enemy_hp_t.setCharacterSize(50);
	enemy_hp_t.setOutlineColor(sf::Color::Black);
	enemy_hp_t.setOutlineThickness(2);
	enemy_hp_t.setFillColor(sf::Color::White);
	enemy_hp_t.setStyle(sf::Text::Bold);
	enemy_weapon_damage_t.setFont(font);
	enemy_weapon_damage_t.setCharacterSize(50);
	enemy_weapon_damage_t.setOutlineColor(sf::Color::Black);
	enemy_weapon_damage_t.setFillColor(sf::Color::White);
	enemy_weapon_damage_t.setOutlineThickness(2);
	enemy_weapon_damage_t.setStyle(sf::Text::Bold);
	enemy_dodge_chance_t.setFont(font);
	enemy_dodge_chance_t.setCharacterSize(24);
	enemy_dodge_chance_t.setOutlineColor(sf::Color::Black);
	enemy_dodge_chance_t.setFillColor(sf::Color::White);
	enemy_dodge_chance_t.setOutlineThickness(2);
	enemy_dodge_chance_t.setStyle(sf::Text::Bold);
	enemy_dodge_chance_t.setString(dodge_chance_s);
	enemy_type_t.setFont(font);
	enemy_type_t.setCharacterSize(24);
	enemy_type_t.setOutlineColor(sf::Color::Black);
	enemy_type_t.setFillColor(sf::Color::White);
	enemy_type_t.setOutlineThickness(2);
	enemy_type_t.setStyle(sf::Text::Bold);
	enemy_name.setPosition(i + 10, j + 10);
	enemy_hp_t.setPosition(i + 70, j + 35);
	enemy_weapon_damage_t.setPosition(i + 70, j + 100);
	enemy_dodge_chance_t.setPosition(i + 10, j + 165);
	enemy_type_t.setPosition(i + 10, j + 195);
	HP.setPosition(i + 10, j + 40);
	sword.setPosition(i + 10, j + 105);
	draw_stat_window(i, j);
	window->draw(HP);
	window->draw(sword);
	window->draw(enemy_name);
	window->draw(enemy_hp_t);
	window->draw(enemy_weapon_damage_t);
	window->draw(enemy_dodge_chance_t);
	window->draw(enemy_type_t);
}

void Engine::draw_player(int i, int j) {
	switch (direction) {
	case 0:
		player0.setPosition(i * 35.f + (960 - (SIZE * 35) / 2), j * 35.f + (540 - (SIZE * 35) / 2));
		window->draw(player0);
		break;
	case 1:
		player1.setPosition(i * 35.f + (960 - (SIZE * 35) / 2), j * 35.f + (540 - (SIZE * 35) / 2));
		window->draw(player1);
		break;
	case 2:
		player2.setPosition(i * 35.f + (960 - (SIZE * 35) / 2), j * 35.f + (540 - (SIZE * 35) / 2));
		window->draw(player2);
		break;
	}
}

void Engine::draw_player_big(int i, int j)
{
	player_big.setPosition(i, j);
	window->draw(player_big);
}

void Engine::draw_npc(int i, int j)
{
	npc.setPosition(i * 35.f + (960 - (SIZE * 35) / 2), j * 35.f + (540 - (SIZE * 35) / 2));
	window->draw(npc);
}

void Engine::draw_dialogue_window() {
	dialogue_window1.setPosition(0, 780);
	window->draw(dialogue_window1);
}

void Engine::draw_dialogue() {
	if (first_contact) {
		npc_response_t.setString(npc1.get_dialogue(0, 0));
	}
	else {
		basic_npc_response_t.setString(npc1.get_dialogue(0, 1));
	}
	switch (which_response) {
	case 1:
		npc_response_t.setString(npc1.get_dialogue(2, 0));
		break;
	case 2:
		npc_response_t.setString(npc1.get_dialogue(2, 1));
		break;
	case 3:
		npc_response_t.setString(npc1.get_dialogue(2, 2));
		break;
	}
	player_response_t.setString(combine_string(npc1.get_dialogue(1, 0), npc1.get_dialogue(1, 1), npc1.get_dialogue(1, 2)));
	npc_response_t.setFont(font);
	npc_response_t.setCharacterSize(24);
	npc_response_t.setFillColor(sf::Color::White);
	npc_response_t.setOutlineColor(sf::Color::Black);
	npc_response_t.setOutlineThickness(2);
	npc_response_t.setStyle(sf::Text::Bold);
	npc_response_t.setPosition(10, 785);
	window->draw(npc_response_t);
	basic_npc_response_t.setFont(font);
	basic_npc_response_t.setCharacterSize(24);
	basic_npc_response_t.setFillColor(sf::Color::White);
	basic_npc_response_t.setOutlineColor(sf::Color::Black);
	basic_npc_response_t.setOutlineThickness(2);
	basic_npc_response_t.setStyle(sf::Text::Bold);
	basic_npc_response_t.setPosition(10, 810);
	window->draw(basic_npc_response_t);
	player_response_t.setFont(font);
	player_response_t.setCharacterSize(24);
	player_response_t.setFillColor(sf::Color::White);
	player_response_t.setOutlineColor(sf::Color::Black);
	player_response_t.setOutlineThickness(2);
	player_response_t.setStyle(sf::Text::Bold);
	player_response_t.setPosition(10, 900);
	window->draw(player_response_t);
}

void Engine::draw_action_window(int i, int j) {
	list_of_actions.setString("1. Fast attack \n2. Heavy attack");
	list_of_actions.setFont(font);
	list_of_actions.setCharacterSize(24);
	list_of_actions.setOutlineColor(sf::Color::Black);
	list_of_actions.setOutlineThickness(2);
	list_of_actions.setFillColor(sf::Color::White);
	list_of_actions.setStyle(sf::Text::Bold);
	list_of_actions.setPosition(i+10, j+10);
	draw_stat_window(i, j);
	window->draw(list_of_actions);
}

void Engine::draw_log_window(int i, int j) {
	player_last_action_t.setString(player_fight_log);
	enemy_last_action_t.setString(enemy_fight_log);
	player_last_action_t.setFont(font);
	player_last_action_t.setCharacterSize(24);
	player_last_action_t.setOutlineColor(sf::Color::Black);
	player_last_action_t.setOutlineThickness(2);
	player_last_action_t.setFillColor(sf::Color::Green);
	player_last_action_t.setStyle(sf::Text::Bold);
	enemy_last_action_t.setFont(font);
	enemy_last_action_t.setCharacterSize(24);
	enemy_last_action_t.setOutlineColor(sf::Color::Black);
	enemy_last_action_t.setOutlineThickness(2);
	enemy_last_action_t.setFillColor(sf::Color::Red);
	enemy_last_action_t.setStyle(sf::Text::Bold);
	player_last_action_t.setPosition(i + 10, j + 10);
	enemy_last_action_t.setPosition(i + 10, j + 68);
	log_window.setPosition(i, j);
	window->draw(log_window);
	window->draw(player_last_action_t);
	window->draw(enemy_last_action_t);
}

void Engine::draw_stat_window(int i, int j) {
	stat_window.setPosition(i, j);
	window->draw(stat_window);
}

void Engine::draw_controls_window(int i, int j) {
	control_window.setPosition(i, j);
	window->draw(control_window);
}

void Engine::draw_quest_window(int i, int j) {
	quest_window.setPosition(i, j);
	window->draw(quest_window);
}

void Engine::draw_quest(int i, int j)
{
	quest_string = "Killed rats(" + std::to_string(killed_rats)  + "/3)";
	quest_text.setString(quest_string);
	quest_text.setFont(font);
	quest_text.setCharacterSize(16);
	quest_text.setOutlineColor(sf::Color::Black);
	quest_text.setOutlineThickness(2);
	quest_text.setFillColor(sf::Color::White);
	quest_text.setStyle(sf::Text::Bold);
	quest_name.setString("Quest:");
	quest_name.setFont(font);
	quest_name.setCharacterSize(16);
	quest_name.setOutlineColor(sf::Color::Black);
	quest_name.setOutlineThickness(2);
	quest_name.setFillColor(sf::Color::White);
	quest_name.setStyle(sf::Text::Bold);
	quest_name.setPosition(i + 10, j + 10);
	quest_text.setPosition(i + 10, j + 40);
	draw_quest_window(i, j);
	if (on_quest) {
		window->draw(quest_text);
	}
	window->draw(quest_name);
}

void Engine::draw_fight_background() {
	fight_background.setPosition(0, 0);
	window->draw(fight_background);
}

//Utylity

void Engine::reset() {
	for (int i = 0; i < NUMBER_OF_ENEMYS; i++) {
		array_of_enemys[i].reset_enemy();
	}
	direction = 0;
	player.reset_player();
	which_enemy = -1;
	which_response = 0;
	killed_rats = 0;
	on_quest = false;
	is_talking = false;
	first_contact = true;
	player_turn = false;
	after_fight = false;
	game_continue = false;
	killed_rats = 0;
}

void Engine::which_button_guard() {
	if (which_button > 2) {
		which_button = 0;
	}
	if (which_button < 0) {
		which_button = 2;
	}
}

bool Engine::is_hit(int dodge_chance) {
	int x = (rand() % 100) + 1 - dodge_chance;
	if (x <= 0) {
		return false;
	}
	else {
		return true;
	}
}

std::string Engine::combine_string(std::string string1, std::string string2, std::string string3) {
	return "1. " + string1 + "\n" + "2. " + string2 + "\n" + "3. " + string3 + "\n" + "Press Q to quit";
}


unsigned** Engine::make_line(float x1, float y1, float x2, float y2) {
	float x, y, dx, dy, step;
	unsigned** tab = 0;
	tab = new unsigned* [30];
	for (unsigned h = 0; h < 30; h++) {
		tab[h] = new unsigned[2];
	}

	dx = abs(x2 - x1);
	dy = abs(y2 - y1);

	if (dx >= dy)
		step = dx;
	else
		step = dy;

	dx = dx / step;
	dy = dy / step;

	x = x1;
	y = y1;

	for (int i = 0; i < 30; i++) {
		x = x + dx;
		y = y + dy;
		unsigned xf = round(x);
		unsigned yf = round(y);
		tab[i][0] = xf;
		tab[i][1] = yf;
	}
	return tab;
}




