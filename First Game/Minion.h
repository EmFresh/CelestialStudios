#pragma once
#include "player.h"
#include <Model.h>

class Minion : public Model
{
public:
	Minion();
	Minion(Model& model);
	Minion(const char* path);
	~Minion();

	float getHealth();
	void setHealth(float v);
	float getTimeSinceLastSpawn();
	void setTimeSinceLastSpawn(float v);
	static void addTarget(Player* mod);
	static void clearTargets();
	bool hitByEnemy(Model*, float damage = 35.f);
	void update(float dt);

	void move(bool move);

	friend bool operator>(Minion a, Minion b);
	Coord3D<> moveTo;
private:
	bool m_move = true;
	static std::vector<Player*> targets;
	float m_health = 30;
	float m_moveSpeed = .05f;
	float m_timeSinceLastSpawn = 0;
};

