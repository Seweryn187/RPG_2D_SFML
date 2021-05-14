#include <iostream>
#include "Player.h"

Player::Player(int hit_points, int weapon_damage, int amount_of_money, int dodge_chance) {
	this->player_hit_points = hit_points;
	this->weapon_damage = weapon_damage;
	this->amount_of_money = amount_of_money;
	this->player_dodge_chance = dodge_chance;
	player_start_hit_points = hit_points;
	player_start_amount_of_money = amount_of_money;
	player_start_weapon_damage = weapon_damage;
}

int Player::get_player_hit_points() {
	return player_hit_points;
}

void Player::set_player_hit_points(int hit_points) {
	this->player_hit_points = hit_points;
}

int Player::get_player_weapon_damage() {
	return weapon_damage;
}

void Player::set_player_weapon_damage(int weapon_damage) {
	this->weapon_damage = weapon_damage;
}

int Player::get_player_amount_of_money() {
	return amount_of_money;
}

int Player::get_player_dodge_chance() {
	return player_dodge_chance;
}

void Player::set_player_amount_of_money(int amount_of_money) {
	this->amount_of_money = amount_of_money;
}

bool Player::is_alive() {
	if (player_hit_points <= 0) {
		alive = false;
	}
	return alive;
}

void Player::reset_player() {
	player_hit_points = player_start_hit_points;
	amount_of_money = player_start_amount_of_money;
	weapon_damage = player_start_weapon_damage;
	alive = true;
}

std::string Player::get_player_name()
{
	return player_name;
}

void Player::set_player_name(std::string name)
{
	player_name = name;
}
