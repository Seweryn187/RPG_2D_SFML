#include "Engine.h"

// Zmienne globalne
int which_enemy = -1;
int which_response = 0;
int killed_rats = 0;
int which_apperance = 0;
bool on_quest = false;
bool done_quest = false;
bool is_talking = false;
bool first_contact = true;
bool player_turn = false;
bool after_fight = false;
bool game_continue = false;
bool show_help_window = false;
bool english_language = true;
float SIZE = 30;
int direction = 0;
const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;
const float scale = 0.8;
const float invScale = 1 / scale; 
const int NUMBER_OF_ENEMYS = 3;


Map map(SIZE);
Npc npc1 = Npc(24, 5, "Innkeeper", 1);
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
	auto image = sf::Image{};
	if (!image.loadFromFile("textures/ico.png"))
	{
		std::cout << "Can't find icon";
	}
	window->setIcon(image.getSize().x, image.getSize().y, image.getPixelsPtr());
	map.create();
	make_dialogue("Innkeeper: Welcome stranger", "Innkeeper: ...", "Innkeeper: Goodbye.", "Serve me beer", "Do you want some help?", "I will do it",
			"Innkeeper: Ok, here you are.", "Innkeeper: Yes, I have problem with rats in the basement.", "Innkeeper: Good luck, take my old sword.");
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
				if (show_help_window) {
					draw_help(880, 400);
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
				if (show_help_window) {
					draw_help(880, 400);
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
				if (show_help_window) {
					draw_help(880, 400);
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
			if (killed_rats == 3) {
				after_quest();
			}
			break;
		case 200:
			player_input = "";
			player_text.setString("");
			while (game_state == 200) {
				print_menu();
				handle_events();
				window->display();
			}
			break;
		case 201:
			while (game_state == 201) {
				print_credits();
			}
			break;
		case 202:
			while (game_state == 202) {
				draw_background();
 				handle_events();
 				draw_selection_menu();
				window->display();
			}
			break;
		case 203:
			while (game_state == 203) {
				draw_name_select();
				handle_events();
				window->draw(player_text);
				window->display();
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
			which_button = 0;
			change_level(200);
		}
		if (((event.type == sf::Event::KeyPressed) && ((event.key.code == sf::Keyboard::A) 
			|| (event.key.code == sf::Keyboard::Left)) && (is_talking == false) && (show_help_window == false)) && game_state < 100) {
			movement(0);
		}
		if (((event.type == sf::Event::KeyPressed) && ((event.key.code == sf::Keyboard::W) 
			|| (event.key.code == sf::Keyboard::Up)) && (is_talking == false) && (show_help_window == false)) && game_state < 100) {
			movement(1);
		}
		if (((event.type == sf::Event::KeyPressed) && ((event.key.code == sf::Keyboard::S) 
			|| (event.key.code == sf::Keyboard::Down)) && (is_talking == false) && (show_help_window == false)) && game_state < 100) {
			movement(2);
		}
		if (((event.type == sf::Event::KeyPressed) && ((event.key.code == sf::Keyboard::D) 
			|| (event.key.code == sf::Keyboard::Right)) && (is_talking == false) && (show_help_window == false)) && game_state < 100) {
			movement(3);
		}
		if ((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::Space)) {
			player_action();
		}
		if ((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::H)) {
			if (show_help_window == false) {
				show_help_window = true;
			}
			else {
				show_help_window = false;
			}
		}
		if ((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::Q)) {
			is_talking = false;
			which_response = 0;
		}
		if ((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::Num1) && (is_talking == true)) {
			if (player.get_player_amount_of_money() > 0 && player.get_player_hit_points() < 10 && !done_quest) {
				player.set_player_amount_of_money(player.get_player_amount_of_money() - 2);
				player.set_player_hit_points(player.get_player_hit_points() + 1);
			}
			else if (player.get_player_hit_points() < 10 && done_quest) {
				player.set_player_hit_points(player.get_player_hit_points() + 1);
			}
			which_response = 1;
			first_contact = false;
		}
		if ((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::Num2) && (is_talking == true)) {
			which_response = 2;
			if (killed_rats == 3) {
				on_quest = false;
				done_quest = true;
			}

			first_contact = false;
		}
		if ((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::Num3) && (is_talking == true)) {
			if (player.get_player_weapon_damage() < 2) {
				player.set_player_weapon_damage(player.get_player_weapon_damage() + 1);
			}
			which_response = 3;
			if (!done_quest) {
				on_quest = true;
			}
			first_contact = false;
		}
		if ((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::Num1) && (game_state>=100) && (game_state < 200)) {
			fight_action = 1;
		}
		if ((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::Num2) && (game_state >= 100) && (game_state < 200)) {
			fight_action = 2;
		}
		if ((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::Up) && (game_state == 200)) {
			which_button -= 1;
			which_button_guard();
		}
		if ((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::Down) && (game_state == 200)) {
			which_button += 1;
			which_button_guard();
		}
		if ((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::Right) && (game_state == 202)) {
			which_button += 1;
			which_button_guard();
		}
		if ((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::Left) && (game_state == 202)) {
			which_button -= 1;
			which_button_guard();
		}

		if ((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::Enter) && (game_state == 200)) {
			switch (which_button) {
			case 0:
				reset();
				change_level(202);
				which_button = 0;
				break;
			case 1:
				if (english_language == true) {
					english_language = false;
				}
				else {
					english_language = true;
				}
				if (english_language && !done_quest) {
					make_dialogue("Innkeeper: Welcome stranger", "Innkeeper:", "Innkeeper: Goodbye.", "Give me beer", "Do you want some help?", "I will do it",
						"Innkeeper: Ok, here you are.", "Innkeeper: Yes, I have problem with rats in the basement.", "Innkeeper: Good luck, take my old sword.");
				}
				else{
					make_dialogue("Karczmarz: Witaj nieznajomy", "Karczmarz:", "Karczmarz: Zegnaj.", "Daj mi piwo", "Potrzebujesz pomocy?", "Zrobie to",
						"Karczmarz: Ok, prosze bardzo.", "Karczmarz: Tak, mam problem z szczurami w piwnicy.", "Karczmarz: Zycze powodzenia, wez moj stary miecz.");
				}
				for (int i = 0; i < 3; i++) {
					for (int j = 0; j < 3; j++) {
						npc1.set_dialogue(tab_npc[i][j], i, j);
					}
				}
				break;
			case 2:
				change_level(201);
				break;
			case 3:
				window->close();
				break;
			}
		}
		if ((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::Enter) && (game_state == 202)) {
			switch (which_button) {
			case 0:
				which_apperance += 1;
				which_apperance_guard();
				break;
			case 1:
				change_level(203);
				break;
			}
		}
		if (event.type == sf::Event::TextEntered && (game_state == 203)) {
			if (event.text.unicode < 128)
			{
				if (event.text.unicode == '\b') {
					player_input.erase(player_input.getSize() - 1, 1);
				}
				else if (event.text.unicode == 27 || player_input.getSize() > 12) {}
				else {
					player_input += event.text.unicode;
				}
				player_text.setString(player_input);
			}
		}
		if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Enter) && (game_state == 203)) {
			change_level(0);
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
	if ((map(player_x+1,player_y).get_state() == Field::states::DOORFORWARD || map(player_x + 1, player_y + 1).get_state() == Field::states::DOORFORWARD 
		|| map(player_x, player_y+1).get_state() == Field::states::DOORFORWARD || map(player_x - 1, player_y+1).get_state() == Field::states::DOORFORWARD
		|| map(player_x-1, player_y).get_state() == Field::states::DOORFORWARD || map(player_x - 1, player_y-1).get_state() == Field::states::DOORFORWARD
		|| map(player_x, player_y-1).get_state() == Field::states::DOORFORWARD || map(player_x + 1, player_y-1).get_state() == Field::states::DOORFORWARD)) {
		door_forward_used = true;
		change_level(game_state + 1);
	}
	if ((map(player_x + 1, player_y).get_state() == Field::states::DOORBACK|| map(player_x + 1, player_y + 1).get_state() == Field::states::DOORBACK
		|| map(player_x, player_y + 1).get_state() == Field::states::DOORBACK || map(player_x - 1, player_y + 1).get_state() == Field::states::DOORBACK
		|| map(player_x - 1, player_y).get_state() == Field::states::DOORBACK|| map(player_x - 1, player_y - 1).get_state() == Field::states::DOORBACK
		|| map(player_x, player_y - 1).get_state() == Field::states::DOORBACK || map(player_x + 1, player_y - 1).get_state() == Field::states::DOORBACK)) {
		door_back_used = true;
		change_level(game_state - 1);
	}
	if (player_x < 28) {
		if ((map(player_x + 1, player_y).get_state() == Field::states::NPC || map(player_x + 1, player_y + 1).get_state() == Field::states::NPC
			|| map(player_x, player_y + 1).get_state() == Field::states::NPC || map(player_x - 1, player_y + 1).get_state() == Field::states::NPC
			|| map(player_x - 1, player_y).get_state() == Field::states::NPC || map(player_x - 1, player_y - 1).get_state() == Field::states::NPC
			|| map(player_x, player_y - 1).get_state() == Field::states::NPC || map(player_x + 1, player_y - 1).get_state() == Field::states::NPC)
			|| map(player_x + 2, player_y).get_state() == Field::states::NPC || map(player_x + 2, player_y + 1).get_state() == Field::states::NPC
			|| map(player_x + 2, player_y - 1).get_state() == Field::states::NPC) {
			is_talking = true;
		}
	}
	if ((map(player_x + 1, player_y).get_state() == Field::states::ENEMY || map(player_x + 1, player_y + 1).get_state() == Field::states::ENEMY
		|| map(player_x, player_y + 1).get_state() == Field::states::ENEMY || map(player_x - 1, player_y + 1).get_state() == Field::states::ENEMY
		|| map(player_x - 1, player_y).get_state() == Field::states::ENEMY || map(player_x - 1, player_y - 1).get_state() == Field::states::ENEMY
		|| map(player_x, player_y - 1).get_state() == Field::states::ENEMY || map(player_x + 1, player_y - 1).get_state() == Field::states::ENEMY)) {
		for (int i = 0; i < NUMBER_OF_ENEMYS; i++) {
			if ((player_x + 1== array_of_enemys[i].get_enemy_x() && player_y == array_of_enemys[i].get_enemy_y() || player_x + 1 == array_of_enemys[i].get_enemy_x()
				&& player_y + 1 == array_of_enemys[i].get_enemy_y() || player_x == array_of_enemys[i].get_enemy_x() && player_y +1 == array_of_enemys[i].get_enemy_y()
				|| player_x - 1 == array_of_enemys[i].get_enemy_x() && player_y + 1 == array_of_enemys[i].get_enemy_y() || player_x - 1 == array_of_enemys[i].get_enemy_x()
				&& player_y == array_of_enemys[i].get_enemy_y() || player_x - 1 == array_of_enemys[i].get_enemy_x() && player_y - 1 == array_of_enemys[i].get_enemy_y()
				|| player_x == array_of_enemys[i].get_enemy_x() && player_y - 1 == array_of_enemys[i].get_enemy_y() || player_x + 1 == array_of_enemys[i].get_enemy_x() && player_y -1 == array_of_enemys[i].get_enemy_y())) {
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
	draw_menu_background();
	switch (which_button) {
	case 0:
		print_title();
		if (english_language == true) {
			print_button("START", 750, 250, 80, true);
			print_button("LANGUAGE:EN", 750, 450, -35, false);
			print_button("CREDITS", 750, 650, 40, false);
			print_button("EXIT", 750, 850, 95, false);
		}
		else {
			print_button("START", 700, 250, 120, true);
			print_button("JEZYK:PL", 700, 450, 85, false);
			print_button("NAPISY KONCOWE", 700, 650, -30, false);
			print_button("WYJSCIE", 700, 850, 90, false);
		}
		break;
	case 1:
		print_title();
		if (english_language == true) {
			print_button("START", 750, 250, 80, false);
			print_button("LANGUAGE:EN", 750, 450, -35, true);
			print_button("CREDITS", 750, 650, 40, false);
			print_button("EXIT", 750, 850, 95, false);
		}
		else {
			print_button("START", 700, 250, 120, false);
			print_button("JEZYK:PL", 700, 450, 85, true);
			print_button("NAPISY KONCOWE", 700, 650, -30, false);
			print_button("WYJSCIE", 700, 850, 90, false);
		}
		break;
	case 2:
		print_title();
		if (english_language == true) {
			print_button("START", 750, 250, 80, false);
			print_button("LANGUAGE:EN", 750, 450, -35, false);
			print_button("CREDITS", 750, 650, 40, true);
			print_button("EXIT", 750, 850, 95, false);
		}
		else {
			print_button("START", 700, 250, 120, false);
			print_button("JEZYK:PL", 700, 450, 85, false);
			print_button("NAPISY KONCOWE", 700, 650, -30, true);
			print_button("WYJSCIE", 700, 850, 90, false);
		}
		break;
	case 3:
		print_title();
		if (english_language == true) {
			print_button("START", 750, 250, 80, false);
			print_button("LANGUAGE:EN", 750, 450, -35, false);
			print_button("CREDITS", 750, 650, 40, false);
			print_button("EXIT", 750, 850, 95, true);
		}
		else {
			print_button("START", 700, 250, 120, false);
			print_button("JEZYK:PL", 700, 450, 85, false);
			print_button("NAPISY KONCOWE", 700, 650, -30, false);
			print_button("WYJSCIE", 700, 850, 90, true);
		}
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
	if (english_language) {
		title.setPosition(500, 80);
		title.setString("Inns and rats: Roleplaying game");
	}
	else {
		title.setPosition(550, 80);
		title.setString("Tawerny i szczury: gra RPG");
	}
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
	if (english_language) {
		button.setSize(sf::Vector2f(400, 100));
	}
	else {
		button.setSize(sf::Vector2f(500, 100));
	}
	button.setPosition(float(i), float(j));
	button_name.setFont(font);
	button_name.setCharacterSize(50);
	button_name.setOutlineColor(sf::Color::Black);
	button_name.setOutlineThickness(5);
	button_name.setFillColor(sf::Color::White);
	button_name.setStyle(sf::Text::Bold);
	button_name.setPosition(float(i+50+offset), float(j+20));
	button_name.setString(name);
	window->draw(button);
	window->draw(button_name);
}

void Engine::print_credits() {
	credits_view.setSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	credits_view.setCenter(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
	credits_view.setViewport(sf::FloatRect(0, (1 - scale) / 2, scale, scale));

	sf::Text titleCredit("Credits", font, SCREEN_HEIGHT / 12);
	if (!english_language) {
		titleCredit.setString("Napisy Koncowe");
	}
	titleCredit.setPosition((SCREEN_WIDTH - titleCredit.getGlobalBounds().width) / 2, 0);
	titleCredit.setFillColor(sf::Color::White);

	sf::Text creditText("Creators and Lead Developers:", font, SCREEN_WIDTH / 24);
	if (!english_language) {
		creditText.setString("Tworcy i glowni programisci");
	}
	creditText.setPosition((SCREEN_WIDTH * invScale - creditText.getGlobalBounds().width) / 2, SCREEN_HEIGHT);
	creditText.setFillColor(sf::Color::White);

	sf::Text creditText2("Seweryn Cholewa and Dariusz Stefanski", font, SCREEN_WIDTH / 24);
	if (!english_language) {
		creditText2.setString("Seweryn Cholewa i Dariusz Stefanski");
	}
	creditText2.setPosition((SCREEN_WIDTH * invScale - creditText2.getGlobalBounds().width) / 2, creditText.getGlobalBounds().top + creditText.getGlobalBounds().height);
	creditText2.setFillColor(sf::Color::Red);

	sf::Text creditText3("Graphic Designers:", font, SCREEN_WIDTH / 24);
	if (!english_language) {
		creditText3.setString("Graficy:");
	}
	creditText3.setPosition((SCREEN_WIDTH * invScale - creditText3.getGlobalBounds().width) / 2, creditText2.getGlobalBounds().top + creditText2.getGlobalBounds().height);
	creditText3.setFillColor(sf::Color::White);

	sf::Text creditText4("Seweryn Cholewa and Dariusz Stefanski", font, SCREEN_WIDTH / 24);
	if (!english_language) {
		creditText4.setString("Seweryn Cholewa i Dariusz Stefanski");
	}
	creditText4.setPosition((SCREEN_WIDTH * invScale - creditText4.getGlobalBounds().width) / 2, creditText3.getGlobalBounds().top + creditText3.getGlobalBounds().height);
	creditText4.setFillColor(sf::Color::Red);

	sf::Text creditText5("Story Writers:", font, SCREEN_WIDTH / 24);
	if (!english_language) {
		creditText5.setString("Tworcy fabuly:");
	}
	creditText5.setPosition((SCREEN_WIDTH * invScale - creditText5.getGlobalBounds().width) / 2, creditText4.getGlobalBounds().top + creditText4.getGlobalBounds().height);
	creditText5.setFillColor(sf::Color::White);

	sf::Text creditText6("Seweryn Cholewa and Dariusz Stefanski", font, SCREEN_WIDTH / 24);
	if (!english_language) {
		creditText6.setString("Seweryn Cholewa i Dariusz Stefanski");
	}
	creditText6.setPosition((SCREEN_WIDTH * invScale - creditText6.getGlobalBounds().width) / 2, creditText5.getGlobalBounds().top + creditText5.getGlobalBounds().height);
	creditText6.setFillColor(sf::Color::Red);

	sf::Text creditText7("Special thanks to :", font, SCREEN_WIDTH / 24);
	if (!english_language) {
		creditText7.setString("Specjalne podziekowania dla:");
	}
	creditText7.setPosition((SCREEN_WIDTH * invScale - creditText7.getGlobalBounds().width) / 2, creditText6.getGlobalBounds().top + creditText6.getGlobalBounds().height);
	creditText7.setFillColor(sf::Color::White);

	sf::Text creditText8("Seweryn Cholewa and Dariusz Stefanski", font, SCREEN_WIDTH / 24);
	if (!english_language) {
		creditText8.setString("Seweryna Cholewy i Dariusza Stefanskiego");
	}
	creditText8.setPosition((SCREEN_WIDTH * invScale - creditText8.getGlobalBounds().width) / 2, creditText7.getGlobalBounds().top + creditText7.getGlobalBounds().height);
	creditText8.setFillColor(sf::Color::Red);

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
			window->draw(creditText3);
			window->draw(creditText4);
			window->draw(creditText5);
			window->draw(creditText6);
			window->draw(creditText7);
			window->draw(creditText8);
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
		for (unsigned i = 0; i < SIZE; i++) {
			for (unsigned j = 0; j < SIZE; j++) {
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
		map(25, 4).set_state(Field::states::DECORATION);
		map(3, 25).set_state(Field::states::DECORATION);
		map(10, 20).set_state(Field::states::DECORATION);
		map(15, 10).set_state(Field::states::DECORATION);
		map(3, 3).set_state(Field::states::DECORATION);
		map(20, 25).set_state(Field::states::DECORATION);
		break;
	case 1:
		for (unsigned i = 0; i < SIZE; i++) {
			for (unsigned j = 0; j < SIZE; j++) {
				map(i, j).set_state(Field::states::SAND);
			}
		}
		place_npcs();
		map(5, 10).set_state(Field::states::DECORATION);
		map(6, 10).set_state(Field::states::DECORATION);
		map(4, 10).set_state(Field::states::DECORATION);
		map(5, 9).set_state(Field::states::DECORATION);
		map(5, 11).set_state(Field::states::DECORATION);

		map(10, 10).set_state(Field::states::DECORATION);
		map(9, 10).set_state(Field::states::DECORATION);
		map(11, 10).set_state(Field::states::DECORATION);
		map(10, 9).set_state(Field::states::DECORATION);
		map(10, 11).set_state(Field::states::DECORATION);

		map(5, 20).set_state(Field::states::DECORATION);
		map(4, 20).set_state(Field::states::DECORATION);
		map(6, 20).set_state(Field::states::DECORATION);
		map(5, 19).set_state(Field::states::DECORATION);
		map(5, 21).set_state(Field::states::DECORATION);

		map(10, 20).set_state(Field::states::DECORATION);
		map(9, 20).set_state(Field::states::DECORATION);
		map(11, 20).set_state(Field::states::DECORATION);
		map(10, 19).set_state(Field::states::DECORATION);
		map(10, 21).set_state(Field::states::DECORATION);

		for (int k = 1; k < 11; k++) {
			map(23, k).set_state(Field::states::DECORATION);
		}
		break;
	case 2:
		for (unsigned i = 0; i < SIZE; i++) {
			for (unsigned j = 0; j < SIZE; j++) {
				map(i, j).set_state(Field::states::SAND);
			}
		}
		map(15, 15).set_state(Field::states::DECORATION);
		map(5, 25).set_state(Field::states::DECORATION);
		map(3, 4).set_state(Field::states::DECORATION);
		map(10, 10).set_state(Field::states::DECORATION);
		map(20, 8).set_state(Field::states::DECORATION);
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
	draw_quest(200, 150);
	draw_player_statistics(1500, 150);
	for (auto i = 0; unsigned(i) < SIZE; i++)
	{
		for (auto j = 0; unsigned(j) < SIZE; j++)
		{

			if (map(unsigned(i), unsigned(j)).get_state() == Field::states::GRASS) {
				draw_grass(float(i), float(j));
			}
			if (map(unsigned(i), unsigned(j)).get_state() == Field::states::SAND) {
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
			if (map(unsigned(i), unsigned(j)).get_state() == Field::states::ROCK) {
				switch (game_state) {
				case 0:
					if (j != 0) {
						draw_grass(float(i), float(j));
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
			if (map(unsigned(i), unsigned(j)).get_state() == Field::states::DOORFORWARD || map(unsigned(i), unsigned(j)).get_state() == Field::states::DOORBACK) {
				draw_door(i, j);
			}
			if (map(unsigned(i), unsigned(j)).get_state() == Field::states::NPC) {
				switch (game_state) {
				case 0:
					draw_grass(float(i), float(j));
					break;
				case 1:
					draw_wooden_floor(i, j);
					break;
				}
				draw_npc(i, j);
			}
			if (map(unsigned(i), unsigned(j)).get_state() == Field::states::ENEMY) {
				switch (game_state) {
				case 0:
					draw_grass(float(i), float(j));
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
					draw_grass(float(i), float(j));
					draw_bush(i, j);
					break;
				case 1:
					draw_wooden_floor(i, j);
					if (i == 23 && j > 0 && j < 11) {
					}
					else if ((i == 5 || i == 10) && (j == 10 || j == 20)) {
						draw_table(i, j);
					}
					else {
						draw_barrel(i,j);
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
	draw_player_big_back(250, 675);
	draw_log_window(1150,675);
}



void Engine::game_over() {
	show_help_window = false;
	game_over_t.setFont(font);
	game_over_t.setCharacterSize(120);
	game_over_t.setOutlineColor(sf::Color::Black);
	game_over_t.setOutlineThickness(5);
	game_over_t.setFillColor(sf::Color::Red);
	game_over_t.setStyle(sf::Text::Bold);

	if(english_language){
		game_over_t.setPosition(500, 250);
		game_over_t.setString("      Game Over\n      ESC to quit\nEnter to go to menu");
	}
	else {
		game_over_t.setPosition(150, 250);
		game_over_t.setString("                Koniec Gry\n     Nacisnij ESC, zeby wyjsc\nNacisnij Enter, zeby kontynuowac");
	}
	draw_fight_background();
	draw_skeleton(500, 650);
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
	player_0_green_tx.loadFromFile("textures/player_0_green.png");
	player_0_green.setTexture(player_0_green_tx);
	player_1_green_tx.loadFromFile("textures/player_1_green.png");
	player_1_green.setTexture(player_1_green_tx);
	player_2_green_tx.loadFromFile("textures/player_2_green.png");
	player_2_green.setTexture(player_2_green_tx);
	player_big_back_green_tx.loadFromFile("textures/player_big_back_green.png");
	player_big_back_green.setTexture(player_big_back_green_tx);
	player_big_front_green_tx.loadFromFile("textures/player_big_front_green.png");
	player_big_front_green.setTexture(player_big_front_green_tx);
	player_0_red_tx.loadFromFile("textures/player_0_red.png");
	player_0_red.setTexture(player_0_red_tx);
	player_1_red_tx.loadFromFile("textures/player_1_red.png");
	player_1_red.setTexture(player_1_red_tx);
	player_2_red_tx.loadFromFile("textures/player_2_red.png");
	player_2_red.setTexture(player_2_red_tx);
	player_big_back_red_tx.loadFromFile("textures/player_big_back_red.png");
	player_big_back_red.setTexture(player_big_back_red_tx);
	player_big_front_red_tx.loadFromFile("textures/player_big_front_red.png");
	player_big_front_red.setTexture(player_big_front_red_tx);
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
	menu_background_tx.loadFromFile("textures/menu_background.png");
	menu_background.setTexture(menu_background_tx);
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
	skeleton_tx.loadFromFile("textures/skeleton.png");
	skeleton.setTexture(skeleton_tx);
	stat_window_tx.loadFromFile("textures/stat_window.png");
	stat_window.setTexture(stat_window_tx);
	log_window_tx.loadFromFile("textures/log_window.png");
	log_window.setTexture(log_window_tx);
	fight_background_tx.loadFromFile("textures/fight_background.png");
	fight_background.setTexture(fight_background_tx);
	font.loadFromFile("font/times-new-roman.ttf");
	quest_icon_tx.loadFromFile("textures/quest_icon.png");
	quest_icon.setTexture(quest_icon_tx);
	stat_icon_tx.loadFromFile("textures/stat_icon.png");
	stat_icon.setTexture(stat_icon_tx);
	sword_icon.setTexture(sword_tx);
	help_icon_tx.loadFromFile("textures/help_icon.png");
	help_icon.setTexture(help_icon_tx);
	enemy_icon_tx.loadFromFile("textures/enemy_icon.png");
	enemy_icon.setTexture(enemy_icon_tx);

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
	show_help_window = false;
	if(fight_action != 0) {
		switch (fight_action) {
		case 0:
			break;
		case 1:
			if (is_hit(array_of_enemys[which_enemy].get_enemy_dodge_chance())) {
				array_of_enemys[which_enemy].set_enemy_hit_points(array_of_enemys[which_enemy].get_enemy_hit_points() - player.get_player_weapon_damage());
				if (english_language) {
					player_fight_log = player.get_player_name() + " last action:\nLight attack hit and dealt ";
				}
				else {
					player_fight_log = "Ostatnia akcja " + player.get_player_name() + "\nLekki atak uderzyl i zadal  ";
				}
				player_fight_log.append(std::to_string(player.get_player_weapon_damage()));
				if (english_language) {
					player_fight_log.append(" dmg");
				}
				else {
					if (player.get_player_weapon_damage() > 1) {
						player_fight_log.append(" punkty obrazen");
					}
					else {
						player_fight_log.append(" punkt obrazen");
					}
				}
			}
			else {
				if (english_language) {
					player_fight_log = player.get_player_name() + " last action:\nLight attack missed";
				}
				else {
					player_fight_log = "Ostatnia akcja " + player.get_player_name() + ":\nLekki atak nie trafil";
				}
			}
			break;
		case 2:
			if (is_hit(array_of_enemys[which_enemy].get_enemy_dodge_chance() * 2)) {
				array_of_enemys[which_enemy].set_enemy_hit_points(array_of_enemys[which_enemy].get_enemy_hit_points() - player.get_player_weapon_damage() * 2);
				if (english_language) {
					player_fight_log = player.get_player_name() +" last action:\nHeavy attack hit and dealt ";
				}
				else {
					player_fight_log = "Ostatnia akcja " + player.get_player_name() + ":\nCiezki atak trafil i zadal ";
				}
				player_fight_log.append(std::to_string(player.get_player_weapon_damage()*2));
				if (english_language) {
					player_fight_log.append(" dmg");
				}
				else {
					player_fight_log.append(" punkty obrazen");
				}
			}
			else {
				if (english_language) {
					player_fight_log = player.get_player_name() + " last action:\nLight attack missed";
				}
				else {
					player_fight_log = "Ostatnia akcja " + player.get_player_name() + ":\nCiezki atak nie trafil";
				}
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
			}
			game_state = old_game_state;
			fight_action = 0;
		}
		if (fight_action != 0) {
			if (is_hit(player.get_player_dodge_chance())) {
				int critical = (rand() % 19) + 1;
				if (critical > 2) {
					player.set_player_hit_points(player.get_player_hit_points() - array_of_enemys[which_enemy].get_enemy_weapon_damage());
					if (english_language) {
						enemy_fight_log = "Enemy last action:\nLight attack hit and dealt ";
					}
					else {
						enemy_fight_log = "Ostatnia akcja przeciwnika:\nLekki atak trafil i zadal ";
					}
					enemy_fight_log.append(std::to_string(array_of_enemys[which_enemy].get_enemy_weapon_damage()));
					if (english_language) {
						enemy_fight_log.append(" dmg");
					}
					else {
						if (array_of_enemys[which_enemy].get_enemy_weapon_damage() > 1) {
							enemy_fight_log.append(" punkty obrazen");
						}
						else {
							enemy_fight_log.append(" punkt obrazen");
						}
					}
				}
				else {
					player.set_player_hit_points(player.get_player_hit_points() - array_of_enemys[which_enemy].get_enemy_weapon_damage() * 2);
					if (english_language) {
						enemy_fight_log = "Enemy last action:\nCRITICAL attack hit and dealt ";
					}
					else {
						enemy_fight_log = "Ostatnia akcja przeciwnika:\nUderzenie krytyczne trafilo i zadalo\n";
					}
					enemy_fight_log.append(std::to_string(array_of_enemys[which_enemy].get_enemy_weapon_damage() * 2));
					if (english_language) {
						enemy_fight_log.append(" dmg");
					}
					else {
						enemy_fight_log.append(" punkty obrazen");
					}
				}
			}
			else {
				if (english_language) {
					enemy_fight_log = "Enemy last action:\nAttack missed";
				}
				else {
					enemy_fight_log = "Ostatnia akcja przeciwnika:\nAtak nie trafil";
				}
				
			}
		}
		fight_action = 0;
	}
}

//Blok rysowania

void Engine::draw_grass(float i, float j)
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

void Engine::draw_menu_background() {
	menu_background.setPosition(0, 0);
	window->draw(menu_background);
}

void Engine::draw_enemy_rat(int i, int j) {
	enemy.setPosition(i * 35.f + (960 - (SIZE * 35) / 2), j * 35.f + (540 - (SIZE * 35) / 2));
	window->draw(enemy);
}

void Engine::draw_enemy_rat_big(int i, int j) {
	enemy_big.setPosition(float(i), float(j));
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
	enemy_big_boss.setPosition(float(i), float(j));
	window->draw(enemy_big_boss);
}

void Engine::draw_dead_enemy_boss(int i, int j) {
	dead_enemy_boss.setPosition(i * 35.f + (960 - (SIZE * 35) / 2), j * 35.f + (540 - (SIZE * 35) / 2));
	window->draw(dead_enemy_boss);
}

void Engine::draw_help(float i, float j) {
	std::string controls;
	if (english_language) {
		controls = "Help: \n1.You can move your\ncharacter by using\narrows and WSAD keys.\n2.You can undertake\nthe action (talk, fight \nor using doors) by\nusing space bar key.\n3.If you have problems\nit is worth to talk\nwith innkeeper."; 
	}
	else {
		controls = "Pomoc: \n1.Postacia poruszamy\nza pomoca strzalek\ni klawiszy WSAD.\n2.Akcje(czyli rozmowa\n,walka czy uzywanie\ndrzwi)wykonujemy za\npomoca spacji.\n3.W razie klopotow\n warto porozmawiac\nz karczmarzem.";
	}
	controls_t.setString(controls);
	controls_t.setFont(font);
	controls_t.setCharacterSize(18);
	controls_t.setOutlineColor(sf::Color::Black);
	controls_t.setOutlineThickness(2);
	controls_t.setFillColor(sf::Color::White);
	controls_t.setStyle(sf::Text::Bold);
	controls_t.setPosition(i+10.f, j+10.f);
	help_icon.setPosition(i+150.f, j+225.f);
	draw_controls_window(i, j);
	window->draw(help_icon);
	window->draw(controls_t);
}

void Engine::draw_player_statistics(float i, float j)
{
	std::string hp_s = std::to_string(player.get_player_hit_points());
	player_hp_t.setString(hp_s);
	std::string weapon_damage_s = std::to_string(player.get_player_weapon_damage());
	player_weapon_damage_t.setString(weapon_damage_s);
	std::string money_s = std::to_string(player.get_player_amount_of_money());
	money_t.setString(money_s);
	player_name.setString(player.get_player_name() + ":");
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
	stat_icon.setPosition(i+145,j+230);
	draw_stat_window(i, j);
	window->draw(stat_icon);
	window->draw(player_name);
	window->draw(HP);
	window->draw(sword);
	window->draw(coin);
	window->draw(player_hp_t);
	window->draw(player_weapon_damage_t);
	window->draw(money_t);
}

void Engine::draw_enemy_statistics(float i, float j, Enemy enemy) {
	std::string hp_s = std::to_string(enemy.get_enemy_hit_points());
	enemy_hp_t.setString(hp_s);
	std::string weapon_damage_s = std::to_string(enemy.get_enemy_weapon_damage());
	enemy_weapon_damage_t.setString(weapon_damage_s);
	std::string text_dodge_chance;
	if (english_language) {
		text_dodge_chance = "Dodge: ";
	}
	else {
		text_dodge_chance = "Unik: ";
	}
	std::string dodge_chance_s = text_dodge_chance.append(std::to_string(enemy.get_enemy_dodge_chance()));
	if (english_language) {
		enemy_name.setString(enemy.get_enemy_name());
	}
	else {
		if (enemy.get_enemy_name() == "Basement rat") {
			enemy_name.setString("Piwniczny szczur");
		}
		else {
			enemy_name.setString("Krol szczurow");
		}
	}
	enemy_name.setFont(font);
	enemy_name.setCharacterSize(24);
	enemy_name.setOutlineColor(sf::Color::Black);
	enemy_name.setFillColor(sf::Color::White);
	enemy_name.setOutlineThickness(2);
	enemy_name.setStyle(sf::Text::Bold);
	if (english_language) {
		enemy_type_t.setString(enemy.get_enemy_type(true));
	}
	else {
		if (enemy.get_enemy_type() == 0) {
			enemy_type_t.setString("Typ: Szczur");
		}
		else {
			enemy_type_t.setString("Typ: Boss");
		}
	}
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
	enemy_icon.setPosition(i + 140, j + 230);
	draw_stat_window(i, j);
	window->draw(enemy_icon);
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
		if (which_apperance == 0) {
			player_0_green.setPosition(i * 35.f + (960 - (SIZE * 35) / 2), j * 35.f + (540 - (SIZE * 35) / 2));
			window->draw(player_0_green);
		}
		else {
			player_0_red.setPosition(i * 35.f + (960 - (SIZE * 35) / 2), j * 35.f + (540 - (SIZE * 35) / 2));
			window->draw(player_0_red);
		}
		break;
	case 1:
		if (which_apperance == 0) {
			player_1_green.setPosition(i * 35.f + (960 - (SIZE * 35) / 2), j * 35.f + (540 - (SIZE * 35) / 2));
			window->draw(player_1_green);
		}
		else {
			player_1_red.setPosition(i * 35.f + (960 - (SIZE * 35) / 2), j * 35.f + (540 - (SIZE * 35) / 2));
			window->draw(player_1_red);
		}
		break;
	case 2:
		if (which_apperance == 0) {
			player_2_green.setPosition(i * 35.f + (960 - (SIZE * 35) / 2), j * 35.f + (540 - (SIZE * 35) / 2));
			window->draw(player_2_green);
		}
		else {
			player_2_red.setPosition(i * 35.f + (960 - (SIZE * 35) / 2), j * 35.f + (540 - (SIZE * 35) / 2));
			window->draw(player_2_red);
		}
		break;
	}
}

void Engine::draw_player_big_back(float i, float j)
{
	if (which_apperance == 0) {
		player_big_back_green.setPosition(i, j);
		window->draw(player_big_back_green);
	}
	else {
		player_big_back_red.setPosition(i, j);
		window->draw(player_big_back_red);
	}
}

void Engine::draw_player_big_front(float i, float j)
{
	if (which_apperance == 0) {
		player_big_front_green.setPosition(i, j);
		window->draw(player_big_front_green);
	}
	else {
		player_big_front_red.setPosition(i, j);
		window->draw(player_big_front_red);
	}
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

void Engine::draw_skeleton(float i, float j) {
	skeleton.setPosition(i, j);
	window->draw(skeleton);
}

void Engine::draw_dialogue() {
	switch (which_response) {
	case 0:
		if (first_contact) {
			npc_response_t.setString(npc1.get_dialogue(0, 0));
		}
		else {
			npc_response_t.setString(npc1.get_dialogue(0, 1));
		}
		player_response_t2.setString(player.get_player_name() + ":");
		break;
	case 1:
		npc_response_t.setString(npc1.get_dialogue(2, 0));
		if (english_language) {
			player_response_t2.setString(player.get_player_name() + ": " + npc1.get_dialogue(1, 0));
		}
		else {
			player_response_t2.setString(player.get_player_name() + ": " + npc1.get_dialogue(1, 0));
		}
		break;
	case 2:
		npc_response_t.setString(npc1.get_dialogue(2, 1));
		if (english_language) {
			player_response_t2.setString(player.get_player_name() + ": " + npc1.get_dialogue(1, 1));
		}
		else {
			player_response_t2.setString(player.get_player_name() + ": " + npc1.get_dialogue(1, 1));
		}
		break;
	case 3:
		npc_response_t.setString(npc1.get_dialogue(2, 2));
		if (english_language) {
			player_response_t2.setString(player.get_player_name() + ": " + npc1.get_dialogue(1, 2));
		}
		else {
			player_response_t2.setString(player.get_player_name() + ": " + npc1.get_dialogue(1, 2));
		}
		break;
	}
	player_response_t.setString(combine_string(npc1.get_dialogue(1, 0), npc1.get_dialogue(1, 1), npc1.get_dialogue(1, 2)));
	player_response_t2.setFont(font);
	player_response_t2.setCharacterSize(24);
	player_response_t2.setFillColor(sf::Color::White);
	player_response_t2.setOutlineColor(sf::Color::Black);
	player_response_t2.setOutlineThickness(2);
	player_response_t2.setStyle(sf::Text::Bold);
	player_response_t2.setPosition(10, 790);
	npc_response_t.setFont(font);
	npc_response_t.setCharacterSize(24);
	npc_response_t.setFillColor(sf::Color::White);
	npc_response_t.setOutlineColor(sf::Color::Black);
	npc_response_t.setOutlineThickness(2);
	npc_response_t.setStyle(sf::Text::Bold);
	npc_response_t.setPosition(10, 820);
	player_response_t.setFont(font);
	player_response_t.setCharacterSize(24);
	player_response_t.setFillColor(sf::Color::White);
	player_response_t.setOutlineColor(sf::Color::Black);
	player_response_t.setOutlineThickness(2);
	player_response_t.setStyle(sf::Text::Bold);
	player_response_t.setPosition(10, 900);
	window->draw(player_response_t2);
	window->draw(npc_response_t);
	window->draw(player_response_t);
}

void Engine::draw_action_window(float i, float j) {
	if (english_language) {
		list_of_actions.setString("Actions:\n1.Light attack\n(better chance to\nhit,but lower dmg).\n\n2.Heavy attack\n(lower chance to\nhit,but higher dmg).");
	}
	else {
		list_of_actions.setString("Akcje:\n1.Lekki atak\n(wieksza szansa na\ntrafienie,ale mniejsze\nobrazenia).\n\n2.Ciezki atak\n(mniejsza szansa na\ntrafienie,ale wieksze\nobrazenia).");
	}
	list_of_actions.setFont(font);
	list_of_actions.setCharacterSize(20);
	list_of_actions.setOutlineColor(sf::Color::Black);
	list_of_actions.setOutlineThickness(2);
	list_of_actions.setFillColor(sf::Color::White);
	list_of_actions.setStyle(sf::Text::Bold);
	list_of_actions.setPosition(i+10, j+10);
	sword_icon.setPosition(i+145,j+230);
	draw_stat_window(i, j);
	window->draw(sword_icon);
	window->draw(list_of_actions);
}

void Engine::draw_log_window(float i, float j) {
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

void Engine::draw_stat_window(float i, float j) {
	stat_window.setPosition(i, j);
	window->draw(stat_window);
}

void Engine::draw_controls_window(float i, float j) {
	control_window.setPosition(i, j);
	window->draw(control_window);
}

void Engine::draw_quest_window(float i, float j) {
	quest_window.setPosition(i, j);
	window->draw(quest_window);
}

void Engine::draw_quest(float i, float j)
{
	if (english_language) {
		quest_string = "Deratization:\nKilled rats(" + std::to_string(killed_rats) + "/3)";
		quest_name.setString("Quests:");
	}
	else {
		quest_string = "Deratyzacja:\nZabite szczury(" + std::to_string(killed_rats) + "/3)";
		quest_name.setString("Zadania:");
	}
	quest_text.setString(quest_string);
	quest_text.setFont(font);
	quest_text.setCharacterSize(20);
	quest_text.setOutlineColor(sf::Color::Black);
	quest_text.setOutlineThickness(2);
	quest_text.setFillColor(sf::Color::White);
	quest_text.setStyle(sf::Text::Bold);
	quest_name.setFont(font);
	quest_name.setCharacterSize(24);
	quest_name.setOutlineColor(sf::Color::Black);
	quest_name.setOutlineThickness(2);
	quest_name.setFillColor(sf::Color::White);
	quest_name.setStyle(sf::Text::Bold);
	quest_name.setPosition(i + 10, j + 10);
	quest_text.setPosition(i + 10, j + 40);
	quest_icon.setPosition(i + 150, j + 225);
	draw_quest_window(i, j);
	if (on_quest) {
		window->draw(quest_text);
	}
	window->draw(quest_icon);
	window->draw(quest_name);
}

void Engine::draw_fight_background() {
	fight_background.setPosition(0, 0);
	window->draw(fight_background);
}

void Engine::draw_selection_menu() {
	draw_menu_background();
	selection_menu_text1.setFont(font);
	selection_menu_text1.setCharacterSize(70);
	selection_menu_text1.setOutlineColor(sf::Color::Black);
	selection_menu_text1.setOutlineThickness(5);
	selection_menu_text1.setFillColor(sf::Color::White);
	selection_menu_text1.setStyle(sf::Text::Bold);

	if (english_language) {
		selection_menu_text1.setPosition(600, 100);
		selection_menu_text1.setString("Choose your appearance: ");
	}
	else {
		selection_menu_text1.setPosition(650, 100);
		selection_menu_text1.setString("Wybierz swoj wyglad: ");
	}

	draw_player_big_front(500, 300);
	draw_player_big_back(1100, 300);

	switch (which_button) {
	case 0:
		if (english_language == true) {
			print_button("CONFIRM", 1050, 850, 25, false);
			print_button("NEXT", 450, 850, 80, true);
		}
		else {
			print_button("ZATWIERDZ", 1050, 850, 50, false);
			print_button("KOLEJNY", 350, 850, 90, true);
		}
		break;
	case 1:
		if (english_language == true) {
			print_button("CONFIRM", 1050, 850, 25, true);
			print_button("NEXT", 450, 850, 80, false);
		}
		else {
			print_button("ZATWIERDZ", 1050, 850, 50, true);
			print_button("KOLEJNY", 350, 850, 90, false);
		}
		break;
	}


	window->draw(selection_menu_text1);

}

void Engine::draw_name_select() {
	draw_menu_background();
	name_select_text.setFont(font);
	name_select_text.setCharacterSize(70);
	name_select_text.setOutlineColor(sf::Color::Black);
	name_select_text.setOutlineThickness(5);
	name_select_text.setFillColor(sf::Color::White);
	name_select_text.setStyle(sf::Text::Bold);
	player_text.setFont(font);
	player_text.setCharacterSize(70);
	player_text.setOutlineColor(sf::Color::Black);
	player_text.setOutlineThickness(5);
	player_text.setFillColor(sf::Color::Red);
	player_text.setStyle(sf::Text::Bold);
	if (english_language) {
		name_select_text.setPosition(700, 480);
		name_select_text.setString("Enter player name: ");
		player_text.setPosition(700, 550);
	}
	else {
		name_select_text.setPosition(700, 480);
		name_select_text.setString("Podaj  nazwe gracza:");
		player_text.setPosition(700, 580);
	}
	if (player_input.getSize() > 0) {
		player.set_player_name(player_input);
	}
	else {
		if (english_language) {
			player.set_player_name("Nameless");
		}
		else {
			player.set_player_name("Bezimienny");
		}
	}
	window->draw(name_select_text);
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
	show_help_window = false;
	done_quest = false;
	which_apperance = 0;
}

void Engine::after_quest() {
		if (english_language) {
			make_dialogue("Innkeeper: Welcome stranger", "Innkeeper: ...", "Innkeeper: Goodbye.", "Serve me beer", "I killed all rats in your basement.", "How is the inn going?",
				"Innkeeper: Ok, here you are.", "Innkeeper: Much obliged.In return you can drink beer in my inn for free.", "Innkeeper: Too bad, due to the increase in beer taxes, my inn is losing a lot of money");
		}
		else {
			make_dialogue("Karczmarz: Witaj nieznajomy", "Karczmarz: ...", "Karczmarz: Zegnaj.", "Podaj mi piwo", "Zabilem wszystkie szczury z twojej piwnicy", "Jak idzie handel?",
				"Karczmarz: Ok, prosze bardzo.", "Karczmarz: Jestem bardzo wdziêczny. W zamian za twoj¹ pomoc mo¿esz piæ piwo w mojej karczmie za darmo", "Karczmarz: Kiepsko, przez wzrost podatkow od piwa moja karczma traci wiele pieniedzy");
		}
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				npc1.set_dialogue(tab_npc[i][j], i, j);
			}
		}
}

void Engine::which_button_guard() {
	if (game_state == 200) {
		if (which_button > 3) {
			which_button = 0;
		}
		if (which_button < 0) {
			which_button = 3;
		}
	}
	else if(game_state == 202){
		if (which_button > 1) {
			which_button = 0;
		}
		if (which_button < 0) {
			which_button = 1;
		}
	}
}

void Engine::which_apperance_guard() {
	if (which_apperance > 1) {
		which_apperance = 0;
	}
	if (which_apperance < 0) {
		which_apperance = 1;
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
	if (english_language) {
		return "1. " + string1 + "\n" + "2. " + string2 + "\n" + "3. " + string3 + "\n" + "Press Q to quit";
	}
	else {
		return "1. " + string1 + "\n" + "2. " + string2 + "\n" + "3. " + string3 + "\n" + "Kliknij Q, aby wyjsc";
	}
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




