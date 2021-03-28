#include "Enemy.h"

Enemy::Enemy(int x, int y, std::string name, int game_state, int type) {
	this->enemy_x = x;
	this->enemy_y = y;
	this->enemy_name = name;
	this->game_state = game_state;
	this->type = type;
	if (type == RAT) {
		enemy_dodge_chance = 10;
		enemy_hit_points = 5;
		enemy_weapon_damage = 1;
	}
	if (type == BOSS){
		enemy_dodge_chance = 20;
		enemy_hit_points = 10;
		enemy_weapon_damage = 2;
	}
	enemy_start_hit_points = enemy_hit_points;
}

Enemy::Enemy() {

}

int Enemy::get_enemy_x() {
	return enemy_x;
}

int Enemy::get_enemy_y() {
	return enemy_y;
}


int Enemy::get_enemy_game_state() {
	return game_state;
}

std::string Enemy::get_enemy_name() {
	return enemy_name;
}

int Enemy::get_enemy_dodge_chance() {
	return enemy_dodge_chance;
}

int Enemy::get_enemy_weapon_damage() {
	return enemy_weapon_damage;
}

int Enemy::get_enemy_hit_points() {
	return enemy_hit_points;
}

void Enemy::set_enemy_hit_points(int hit_points) {
	this->enemy_hit_points = hit_points;
}

int Enemy::get_enemy_type() {
	return type;
}

std::string Enemy::get_enemy_type(bool overloaded_function_return_string) {
	switch (type) {
	case 0:
		return "Type: Rat";
	case 1:
		return "Type: Boss";
	}
	return "Blad";
}

bool Enemy::enemy_is_alive() {
	if (enemy_hit_points <= 0){
		this->enemy_alive = false;
	}
	return this->enemy_alive;
}

void Enemy::reset_enemy() {
	enemy_hit_points = enemy_start_hit_points;
	this->enemy_alive = true;
}