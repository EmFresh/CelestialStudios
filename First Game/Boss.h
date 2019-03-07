#pragma once
#include <GameEmGine.h>
#include "Player.h"


class Boss: public Model
{
public:
	Boss();
	Boss(Model& model);
	Boss(const char* path);
	~Boss();

	void setPlayers(Player players[4]);

	bool randAttacks();
	int getHealth();
	void setHealth(int v);
	void update();

	static Model* missles[4];
	
private:
	int health = 1000;
	int attackDamage;
	float attackCooldown;
	Player* targets;
};

