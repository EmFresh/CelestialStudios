#pragma once
#include <GameEmGine.h>
#include <memory>
#include <list>
#include "Player.h"
#include "Minion.h"

class Boss : public Model
{
public:
	void init();
	Boss();
	Boss(Model& model);
	Boss(const char* path);
	~Boss();

	void setPlayers(std::list<Player >& players);

	template<class T>
	void insertionSort(std::vector<T> vec)
	{
		for(int a = 1; a < vec.size(); a++)
			for(int b = a - 1; b >= 0; b--)
			{
				if(vec[b] > vec[a])
				{
					vec.insert(vec.begin() + b, vec[a]);
					break;
				}
			}
	}

	bool randAttacks();
	float getHealth();
	void setHealth(float v);
	const std::list<std::shared_ptr<Model>>& getMissials();
	void update(float dt);
	void setActive(bool active);
	bool isActive();

	bool hitByEnemy(Model* mod, float damage = 10);

	std::list<Light> lights;
	std::list<Minion*> minions;
	std::list<std::shared_ptr<Model>> missiles;
	Model* lazer;
	bool bossFlash;
private:
	void shootLazer(int playerIndex);
	//boss's blood bar
	Model* m_lifeBar;
	Model* m_baseBar;
	float 	m_time, m_bossFlashTime;
	bool m_active = true, m_dead = false;
	float m_initialHealth, m_health;

	static Model* minion;
	int attackDamage;
	float attackCooldown;
	Player* targets[4];
};