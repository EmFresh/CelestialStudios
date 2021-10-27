#define _USE_MATH_DEFINES 
#pragma once

#include <GameEmGine.h>
#include <Model.h>
#include <EmGineAudioPlayer.h>
typedef EmGineAudioPlayer AudioPlayer;

using namespace util;

enum PlayerType
{
	assault,
	tank,
	medic,
	specialist
};

class Player : public Model
{
public:
	void init(int index);

	Player(int index = 0);
	Player(Player& model);
	Player(const Player& model);
	Player(const char* path);

	virtual ~Player();
	void setPlayerIndex(int index);
	virtual float getHealth();
	virtual void setHealth(float v);
	virtual float getTimeSinceLastMissile();
	virtual void setTimeSinceLastMissile(float v);
	void hitByEnemy(Model* mod, float damage = 35.f);
	int getBulletCount();
	void setBulletCount(int v);
	bool bulletCollisions(Model* mod);
	void onPlayArea(Model* mod);
	virtual void update(float dt);
	void setActive(bool active);
	bool isActive();
	float getInitialHealth();

	//Player Ability Variables
	float reloadTimer = 0;
	bool reloading = false;
	float cooldown = false;
	bool f = false;
	PlayerType type;

	float shotBuzzTimer;
	float shotBuzzDir = .2f;
	float onHitShakeTimer = 0;
	float onHitShakeDir = 0.6f;

	bool dead = false;
protected:
	float m_initialHealth = 100;
	int m_index;
	float angle;
	float duration = 0;
	bool m_active = true;
	Animation* squash;
	Model* graveStone, //22
		* bulletCircle,//74
		* ringID,//26
		* gun;//54


	std::vector<float> timer;
	std::vector<Model*> bullets;
	std::vector<Coord3D<>> velocity;
	float move = .1f;


	Model* m_baseBar = nullptr, * m_lifeBar = nullptr;

	bool gunControlLaw = false, dashControl = false;
	float time = 0;

	float m_health = 100;
	int m_bulletCount = 30;
	float m_timeSinceLastMissile = 0;
};
