
#include <Model.h>
class Player : public Model
{
public:
	Player();
	Player(Model & model);
	Player(const char * path);
	~Player();
	int getHealth();
	void setHealth(int v);
	int getBulletCount();
	void setBulletCount(int v);

private:
	int health = 100;
	int bulletCount = 30;
};
