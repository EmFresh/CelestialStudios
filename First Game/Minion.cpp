#include "Minion.h"
std::vector<Player*> Minion::targets;


Minion::Minion():Model()
{

}

Minion::Minion(Model& model) : Model(model)
{

}

Minion::Minion(const char* path) : Model(path)
{

}


Minion::~Minion()
{
}

void Minion::setHealth(float v)
{
	m_health = v;
}

float Minion::getTimeSinceLastSpawn()
{
	return m_timeSinceLastSpawn;
}

void Minion::setTimeSinceLastSpawn(float v)
{
	m_timeSinceLastSpawn = v;
}

void Minion::addTarget(Player* mod)
{
	targets.push_back(mod);
}

void Minion::clearTargets()
{
	targets.clear();
}

bool Minion::hitByEnemy(Model* enemy, float damage)
{
	if(enemy)
	{
		if(collision2D(enemy, {0,1,0}))
		{
			setHealth(getHealth() - damage);
			return true;
		}
	}
	return false;
}

void Minion::update(float dt)
{
	dt;
	/// - Minions Movement Towards Players - ///
	if(m_move)
	{
		float distance, lastDistance = 0;
		moveTo = targets[0]->getLocalPosition();

		if(!targets.empty())
			lastDistance = (getLocalPosition() - targets[0]->getLocalPosition()).length();

		for(auto& a : targets)
		{
			distance = (getLocalPosition() - a->getLocalPosition()).length();
			if(distance < lastDistance)
				moveTo = a->getLocalPosition() - getLocalPosition(),
				lastDistance = (getLocalPosition() - a->getLocalPosition()).length();
		}

		moveTo.normalize();
		translateBy(moveTo * m_moveSpeed);
	}
	
}

void Minion::move(bool move)
{
	m_move = move;
}

float Minion::getHealth()
{
	return m_health;
}

bool operator>(Minion a, Minion b)
{
	return 1;
}
