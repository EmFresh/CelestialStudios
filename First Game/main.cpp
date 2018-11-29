#define _USE_MATH_DEFINES 
//#define 

#include <GameEmGine.h>
#include <EmGineAudioPlayer.h>
#include <vector>
#include <ctime>
#include "Boss.h"
#include "Player.h"

using namespace std;

float ang = 2;
int numModel = 0;
bool m_left = 0, m_right = 0, m_in = 0, m_out = 0, m_up = 0, m_down = 0,
rotLeft = 0, rotRight = 0, rotUp = 0, rotDown = 0,
movePlayer = true;
Coord2D leftM, rightM;
EmGineAudioPlayer audio;

#define modSize 50 //Number of mod that can exist
GameEmGine game("The Real Game", 1920, 1080, 0, 0, 0, false);
Shader colourProgram, colourProgram2;
std::vector<Model*> mod;

//shader initialization
void shaderInit()
{
	//shader initialization
}

//instance key is pressed
void keyInputPressed(int key, int mod)
{
	mod;
	m_left = (key == 'A' ? true : m_left);
	m_right = (key == 'D' ? true : m_right);
	m_up = (key == 'E' ? true : m_up);
	m_down = (key == 'Q' ? true : m_down);
	m_in = (key == 'W' ? true : m_in);
	m_out = (key == 'S' ? true : m_out);

	rotLeft = (key == GLFW_KEY_LEFT ? true : rotLeft);
	rotRight = (key == GLFW_KEY_RIGHT ? true : rotRight);
	rotUp = (key == GLFW_KEY_UP ? true : rotUp);
	rotDown = (key == GLFW_KEY_DOWN ? true : rotDown);

	printf("key PRESSED code: %d\n\n", key);
}

//instace key is released
void keyInputReleased(int key, int mod)
{
	mod;
	m_left = (key == 'A' ? false : m_left);
	m_right = (key == 'D' ? false : m_right);
	m_in = (key == 'W' ? false : m_in);
	m_out = (key == 'S' ? false : m_out);
	m_up = (key == 'E' ? false : m_up);
	m_down = (key == 'Q' ? false : m_down);

	rotLeft = (key == GLFW_KEY_LEFT ? false : rotLeft);
	rotRight = (key == GLFW_KEY_RIGHT ? false : rotRight);
	rotUp = (key == GLFW_KEY_UP ? false : rotUp);
	rotDown = (key == GLFW_KEY_DOWN ? false : rotDown);

	//changes fps limit
	if (key == GLFW_KEY_KP_6)
		game.setFPSLimit(game.getFPSLimit() + 1);
	if (key == GLFW_KEY_KP_4)
		game.setFPSLimit(game.getFPSLimit() - 1);

	if (key == GLFW_KEY_F) //Toggles Fullscreen
	{
		static bool full;
		game.getWindow()->setFullScreen(full = !full);
		printf("Full Screen: %s\n", full ? "true" : "false");
	}

	if (key == GLFW_KEY_SPACE) //changes the model that is being moved
	{
		static CAMERA_TYPE type = PERSPECTIVE;
		game.setCameraType(type = type == ORTHOGRAPHIC ? PERSPECTIVE : ORTHOGRAPHIC);
	}

	if (key == GLFW_KEY_TAB)
		movePlayer = !movePlayer;

	if (key == GLFW_KEY_F5) //resets the camera
	{
		GameEmGine::m_modelShader->refresh();

		//game.setCameraAngle(0, { 1, 1, 1 });
		//	game.setCameraPosition({0,0,0});
	}

	if (key == 'R')
		game.setCameraAngle(0, { 1,1,1 });

	printf("key RELEASED code: %d\n\n", key);
}

/// - Collision Class - ///
bool collisions(Model *l, Model *k)
{
	//if distance between mod in the x OR z is less than half of both widths combined then collide and don't allow any more movement in that direction.
	Coord3D thing = l->getCenter() - k->getCenter();

	float distanceX = abs(thing.coordX);
	float distanceZ = abs(thing.coordZ);

	float capW = (l->getWidth() + k->getWidth()) / 2;
	float capD = (l->getDepth() + k->getDepth()) / 2;

	if (std::abs(distanceX) <= capW)
		if (std::abs(distanceZ) <= capD)
			return true;

	return false;
}

/// - 3D Collision Function - ///
bool collisions3D(Model *l, Model *k)
{
	//if distance between mod in the x OR z is less than half of both widths combined then collide and don't allow any more movement in that direction.
	Coord3D thing = l->getCenter() - k->getCenter();

	float distanceX = abs(thing.coordX);
	float distanceY = abs(thing.coordY);
	float distanceZ = abs(thing.coordZ);

	float capW = (l->getWidth() + k->getWidth()) / 2;
	float capH = (l->getHeight() + k->getHeight()) / 2;
	float capD = (l->getDepth() + k->getDepth()) / 2;

	if (std::abs(distanceX) <= capW)
		if (std::abs(distanceZ) <= capD)
			if (std::abs(distanceY) <= capH)
				return true;

	return false;
}

//updates within game loop
void update(double dt)
{
	static float  time = 0;
	time += dt;
	static float coolDown = 0;
	static float duration = 0;
	static bool f = true;
	static bool bossActive = true;

	float move = .1f;

	static float pointSize = 50.0f;
	//printf("%f\n", dt);
	static Player* player;

	static vector<Model*> bullets[4];
	static vector<Coord3D> velocity[4];
	static bool makeShitLessCancer[4], makeShitLessCancer2[4];//stops the creation of bullets when trigger is healed down
	static float  curveroni = 0;
	static bool hasTarget = false;
	curveroni += .01;
	if (curveroni >= 1)
	{
		hasTarget = false;
	}
	curveroni = fmodf(curveroni, 1);

	static Coord3D bossTarget;

	//gets a   target for missile (player 1,2,3 or 4) randomly
	if (!hasTarget)
	{

		bossTarget = mod[rand() % 4]->getTransformer().getPosition();
		hasTarget = true;
	}

	if (mod[8])
		if (hasTarget)
		{
			Coord3D
				p1 = mod[8]->getTransformer().getPosition() + Coord3D(0.0f, 5.0f, 1.5f),//start point
				p2 = bossTarget,//end point 
				c1 = p1 - Coord3D{ 0,50,100 },//controle point
				c2 = p2 - Coord3D{ 0,100,100 };//controle point

			Coord3D cat = catmull
			(
				c1,
				p1,
				p2,
				c2,
				curveroni
			);

			Coord3D  pointPosition = cat;
			mod[18]->getTransformer().setPosition(pointPosition.coordX, pointPosition.coordY, pointPosition.coordZ);
		}

	//some function
	//{

	//when function plays, get player locations
	//call 4 missles to shoot at those locations
	//maybe call missles to shoot randomly
	//}


	if (movePlayer)
		for (int a = 0; a < 4; a++)

			if (game.isControllerConnected(a))
			{
				player = (Player*)mod[a];
				Xinput p1 = game.getController(a);
				//Player Collisions with Walls
				//if (player->getTransformer().getPosition().coordX > 20)
				//{
				//	player->getTransformer().setPosition(20, player->getTransformer().getPosition().coordY, player->getTransformer().getPosition().coordZ);
				//}
				static float angle[4] = { 180,180,180,180 };
				if (p1.Coord2D_sticks[RS].x || p1.Coord2D_sticks[RS].y)
				{

					angle[a] = acosf(p1.Coord2D_sticks[RS].x /
						sqrtf(p1.Coord2D_sticks[RS].x*p1.Coord2D_sticks[RS].x
							+ p1.Coord2D_sticks[RS].y*p1.Coord2D_sticks[RS].y)) * (180 / (float)M_PI);
					angle[a] += (p1.Coord2D_sticks[RS].y < 0 ? (180 - angle[a]) * 2 : 0) + 90;//90 represents the start angle
					angle[a] = fmodf(angle[a], 360);
				}

				/// - Missile Collisions with Player - ///
				if (collisions3D(player, mod[18]))
				{
					mod[18]->getTransformer().setPosition(mod[8]->getTransformer().getPosition() + Coord3D(0, 5, 2));
					player->setHealth(player->getHealth() - 50);
					Coord3D test = player->getTransformer().getPosition();
					if (player->getHealth() <= 0)
					{
						//game.removeModel(mod[22 +a]);

						mod[22 + a]->setColour(player->getColour());
						//static Coord3D test = player->getTransformer().getPosition();
						mod[22 + a]->getTransformer().setScale(0.75f, 1, 0.5), mod[22 + a]->getTransformer().setPosition(test), mod[22 + a]->getTransformer().setRotation({ 0.0f,90.0f,0.0f });
						game.addModel(mod[22 + a]);
						game.removeModel(player);
					}
				}

				if (p1.triggers[RT] >= .95 && !makeShitLessCancer[a])
				{
					makeShitLessCancer[a] = true;



					bullets[a].push_back(nullptr);
					game.addModel(bullets[a].back() = new Model(*mod[a]));
					bullets[a].back()->getTransformer().reset();
					Coord3D pos = mod[a]->getTransformer().getPosition();
					bullets[a].back()->getTransformer().setPosition(pos.coordX, pos.coordY + .1, pos.coordZ);
					bullets[a].back()->getTransformer().setScale(0.25);

					bullets[a].back()->getTransformer().setRotation({ 90 , angle[a] ,0 });


					float cosVal = cos((float)(fmodf(angle[a] - 90, 360)*(M_PI / 180)));
					float sinVal = sin((float)(fmodf(angle[a] - 90, 360)*(M_PI / 180)));

					velocity[a].push_back(Coord3D());
					velocity[a].back() = Coord3D(cosVal * move * 2, 0, sinVal * move * 2);
					audio.createStream("pew.wav");
					audio.play();
				}
				else if (p1.triggers[RT] < .95 && makeShitLessCancer[a])
					makeShitLessCancer[a] = false;

				/// - Button Presses on controller - ///
				//Start button quits game
				if (p1.buttonPressed(p1.buttons.START))
				{
					puts("\nExiting Game\n");
					game.exit();
				}
				if (p1.buttonPressed(p1.buttons.X))
				{
					puts("RELOADING!!!\n");
				}
				if (p1.buttonPressed(p1.buttons.Y))
				{
					puts("SPECIAL ABILITY\n");
				}

				/// - Left Trigger to Dash - ///

				if (p1.triggers[LT] >= .95)
				{
					static float coolDown[4];

					//get deltaTime put into duraction variable

					if (time - coolDown[a] >= 3)
					{
						if (f == true)
						{
							duration = time;
							f = false;
						}
						move = 0.5f;
						if (time - 0.1f >= duration)
						{
							move = 0.1f;
							//If triggers up then coolDown = time;
							coolDown[a] = time;
							f = true;
						}
					}

				}

				//Do the same with the LT button, have it so will only work every X seconds.
				else
				{
					move -= .001;
					if (move < .1)
						move = .1;
					makeShitLessCancer2[a] = false;
				}

				/// - Bullet Collisions - ///
				for (unsigned b = 0; b < bullets[a].size(); b++)
					if (bullets[a][b])
					{
						bullets[a][b]->getTransformer().translateBy(velocity[a][b].coordX, velocity[a][b].coordY, velocity[a][b].coordZ);

						if (mod[8])
							if (collisions(bullets[a][b], mod[8]))
							{
								game.removeModel(bullets[a][b]);
								bullets[a].erase(bullets[a].begin() + b);
								velocity[a].erase(velocity[a].begin() + b);
								Boss*CandyMan = (Boss*)mod[8];//Boss a.k.a model 8, is now called CandyMan for teh purposes of functions.
								CandyMan->setHealth(CandyMan->getHealth() - 10);// When hit takes damage
								if (CandyMan->getHealth() <= 0)
								{
									game.removeModel(CandyMan); // If health = 0 then boss dead
									//	mod[8] = nullptr;
									bossActive = false;
									puts("Killed The BOSS\n");
								}
								puts("Hit The BOSS\n");
								break;
							}

						if (bullets[a][b])
							for (int i = 4; i < 7; i++) // Bullet Object number parameter
							{
								bullets[a][b]->getTransformer().translateBy(velocity[a][b].coordX, velocity[a][b].coordY, velocity[a][b].coordZ);

								if (collisions(bullets[a][b], mod[i]))
								{
									game.removeModel(bullets[a][b]);
									bullets[a].erase(bullets[a].begin() + b);
									velocity[a].erase(velocity[a].begin() + b);
									printf("Hit Wall\n\n");
									break;
								}
							}
					}


				mod[a]->getTransformer().setRotation({ 0,angle[a], 0 });
				mod[a]->getTransformer().translateBy(p1.Coord2D_sticks[LS].x * move, 0, p1.Coord2D_sticks[LS].y * move); //move player
				//game.moveCameraPositionBy({ p1.Coord2D_sticks[LS].x * move, 0, p1.Coord2D_sticks[LS].y * move });
			}

	if (!movePlayer)
		if (game.isControllerConnected(0))
		{
			Xinput p1 = game.getController(0);

			p1.numButtons;
			p1.numSticks;
			float angle = 0;
			if (p1.Coord2D_sticks[RS].x || p1.Coord2D_sticks[RS].y)
			{

				angle = acosf(p1.Coord2D_sticks[RS].x /
					sqrt(p1.Coord2D_sticks[RS].x*p1.Coord2D_sticks[RS].x
						+ p1.Coord2D_sticks[RS].y*p1.Coord2D_sticks[RS].y)) * (180 / (float)M_PI);
				angle += (p1.Coord2D_sticks[RS].y < 0 ? (180 - angle) * 2 : 0) + 90;//90 represents the start angle
				angle = fmodf(angle, 360);
			}

			if (Xinput::buttonPressed(p1.buttons.A))
				printf("%d\n", p1.buttons.A);

			//move camera
			move *= 2;

			game.moveCameraPositionBy({ p1.Coord2D_sticks[LS].x * move , 0 * move, p1.Coord2D_sticks[LS].y * move });//move camera
			game.moveCameraAngleBy(ang * (abs(p1.Coord2D_sticks[RS].x) + abs(p1.Coord2D_sticks[RS].y)), { p1.Coord2D_sticks[RS].y  ,p1.Coord2D_sticks[RS].x, 0 });//rotate camera
			//game.getMainCamera()->getTransformer().rotateBy({ ang *p1.Coord2D_sticks[RS].y ,ang *p1.Coord2D_sticks[RS].x ,0}, { p1.Coord2D_sticks[RS].y  ,p1.Coord2D_sticks[RS].x, 0 });
			game.moveCameraPositionBy({ 0 ,p1.triggers[LT] * -move,0 });//move out
			game.moveCameraPositionBy({ 0 ,p1.triggers[RT] * move,0 });//move out
			move /= 2;
		}
}

void mouseButtonReleased(int button, int _mod)
{
	if (button == LEFT_BUTTON)
		leftM = InputManager::getMouseCursorPosition();
	if (button == RIGHT_BUTTON)
		rightM = InputManager::getMouseCursorPosition();
}

void render()
{}

//// We need BogoBogo sort in our game right? NO!
//template<class T>
//bool sorted(T* sort, unsigned size)
//{
//	for(unsigned a = 1; a < size; a++)
//		if(sort[a] < sort[a - 1])
//			return false;
//	return true;
//}
//
//template<class T>
//void randomize(T* sort, unsigned size)
//{
//	for(int a = 0; a < size; a++)
//		swap(sort[a], sort[(rand() % size]);
//}
//
//template<class T>
//void bogo(T* sort, unsigned size)
//{
//	if(sorted(sort, size))
//		return;
//
//	randomize(sort, size);
//	bogo(sort, size);
//}
//
//template<class T>
//void bogobogo(T* sort, unsigned size)
//{
//	T *sort2 = new T[size];
//	memcpy_s(sort2, sizeof(T)*size, sort, sizeof(T)*size);
//	unsigned n = size - 1;
//
//	while(n)
//	{
//		bogo(sort2, n);
//		if()
//	}
//}

int main()
{
	srand(clock());

	/// - Load mod into Scene - ///

	//Players
	mod.push_back(new Player("Models/AssaultModel/AssaultClassModel.obj"));
	game.addModel(mod.back());//0
	mod.push_back(new Player(*mod[0]));
	game.addModel(mod.back());//1
	mod.push_back(new Player(*mod[0]));
	game.addModel(mod.back());//2
	mod.push_back(new Player(*mod[0]));
	game.addModel(mod.back());//3

	//Building 1s
	mod.push_back(new Model("Models/Buildings//Building1/building1.obj"));
	game.addModel(mod.back());//4
	mod.push_back(new Model(*mod[4]));
	game.addModel(mod.back());//5
	mod.push_back(new Model(*mod[4]));
	game.addModel(mod.back());//6

	//Project Nebula Sign
	mod.push_back(new Model("Models/Neon Signs/Project Nebula/signn.obj"));
	game.addModel(mod.back()); //7

	//Boss
	mod.push_back(new Boss("Models/BOSS/BOSS.obj"));
	game.addModel(mod.back()); //8

	//Floor
	mod.push_back(new Model("Models/Floor/Floor.obj"));
	game.addModel(mod.back()); //9

	//Light Posts
	mod.push_back(new Model("Models/Lamp/lampPost.obj"));
	game.addModel(mod.back()); //10
	mod.push_back(new Model(*mod[10]));
	game.addModel(mod.back());//11
	mod.push_back(new Model(*mod[10]));
	game.addModel(mod.back());//12
	mod.push_back(new Model(*mod[10]));
	game.addModel(mod.back());//13
	mod.push_back(new Model(*mod[10]));
	game.addModel(mod.back());//14
	mod.push_back(new Model(*mod[10]));
	game.addModel(mod.back());//15

	//Bench
	mod.push_back(new Model("Models/Bench/Bench.obj"));
	game.addModel(mod.back()); //16
	mod.push_back(new Model(*mod[16]));
	game.addModel(mod.back());//17

	//Missile
	mod.push_back(new Model("Models/Missile/candyMissile.obj"));
	game.addModel(mod.back()); //18

	//Building 2s
	mod.push_back(new Model("Models/Buildings//Building2/building2.obj"));
	game.addModel(mod.back());//19
	mod.push_back(new Model(*mod[19]));
	game.addModel(mod.back());//20
	mod.push_back(new Model(*mod[19]));
	game.addModel(mod.back());//21

	mod.push_back(new Model("Models/RIP/RIP.obj")); //22
	mod.push_back(new Model(*mod[22]));
	mod.push_back(new Model(*mod[22]));
	mod.push_back(new Model(*mod[22]));

	/// - Set Model Transforms - ///
	//Player Transforms
	mod[0]->getTransformer().setScale(1.2f), mod[0]->getTransformer().setPosition(1.0f, 0.0f, -5.0f);
	mod[1]->getTransformer().setScale(1.2f), mod[1]->getTransformer().setPosition(-1.0f, 0.0f, -5.0f);
	mod[2]->getTransformer().setScale(1.2f), mod[2]->getTransformer().setPosition(2.0f, 0.0f, -5.0f);
	mod[3]->getTransformer().setScale(1.2f), mod[3]->getTransformer().setPosition(-2.0f, 0.0f, -5.0f);

	//Building Transforms
	//Building 1s
	mod[4]->getTransformer().setScale(2), mod[4]->getTransformer().setPosition(-16.75f, 0.0f, -2.0f);
	mod[5]->getTransformer().setScale(2), mod[5]->getTransformer().setPosition(16.75f, 0.0f, 10.0f), mod[5]->getTransformer().setRotation({ 0.0f,180.0f,0.0f });
	mod[6]->getTransformer().setScale(2), mod[6]->getTransformer().setPosition(-4.0f, 0.0f, 22.75f), mod[6]->getTransformer().setRotation({ 0.0f,-90.0f,0.0f });
	//Building 2s
	mod[19]->getTransformer().setScale(1.75f), mod[19]->getTransformer().setPosition(-16.4f, 0.0f, 3.0f);
	mod[20]->getTransformer().setScale(1.75f), mod[20]->getTransformer().setPosition(16.4f, 0.0f, 0.0f), mod[20]->getTransformer().setRotation({ 0.0f, 180.0f, 0.0f });;
	mod[21]->getTransformer().setScale(1.75f), mod[21]->getTransformer().setPosition(13.5f, 0.0f, 22.4f), mod[21]->getTransformer().setRotation({ 0.0f, -90.0f, 0.0f });

	//Project Nebula Sign Transforms
	mod[7]->getTransformer().setScale(3), mod[7]->getTransformer().setPosition(9.0f, 1.1f, 21.0f);

	//Boss Transforms
	mod[8]->getTransformer().setScale(4), mod[8]->getTransformer().setPosition(0.0f, 0.0f, 16.0f), mod[8]->getTransformer().setRotation({ 0.0f, 180.0f, 0.0f });
	//Missile
	mod[18]->getTransformer().setPosition(0.0f, 2.0f, 17.0f);

	//Floor Transforms
	mod[9]->getTransformer().setScale(1.875f, 1.0f, 1.5f), mod[9]->getTransformer().setPosition(0.0f, 0.0f, 5.0f);

	//Street Light Transforms
	mod[10]->getTransformer().setScale(0.5f), mod[10]->getTransformer().setPosition(13.0f, 0.0f, -1.0f);
	mod[11]->getTransformer().setScale(0.5f), mod[11]->getTransformer().setPosition(13.0f, 0.0f, 7.0f);
	mod[12]->getTransformer().setScale(0.5f), mod[12]->getTransformer().setPosition(13.0f, 0.0f, 15.0f);
	mod[13]->getTransformer().setScale(0.5f), mod[13]->getTransformer().setPosition(-13.0f, 0.0f, -1.0f), mod[13]->getTransformer().setRotation({ 0.0f,180.0f,0.0f });
	mod[14]->getTransformer().setScale(0.5f), mod[14]->getTransformer().setPosition(-13.0f, 0.0f, 7.0f), mod[14]->getTransformer().setRotation({ 0.0f,180.0f,0.0f });
	mod[15]->getTransformer().setScale(0.5f), mod[15]->getTransformer().setPosition(-13.0f, 0.0f, 15.0f), mod[15]->getTransformer().setRotation({ 0.0f,180.0f,0.0f });

	//Bench Transforms
	mod[16]->getTransformer().setPosition(-13.0f, 0.0f, 3.0f);
	mod[17]->getTransformer().setPosition(13.0f, 0.0f, 3.0f), mod[17]->getTransformer().setRotation({ 0.0f,180.0f,0.0f });


	/// - Set Model Colour - ///
	//Players
	mod[0]->setColour(1, 0, 0);
	mod[1]->setColour(0, 0, 1);
	mod[2]->setColour(0, 1, 0);
	mod[3]->setColour(1, 1, 0);

	LightSource::setLightAmount(11);
	for (int a = 0; a < 6; a++)
	{
		mod[10 + a]->boundingBoxUpdate();
		LightSource::setLightType(LIGHT_TYPE::DIRECTIONAL, a);
		LightSource::setParent(mod[10 + a], a);
		LightSource::setPosition({ -5.0f,4.5,0.0f }, a);
		LightSource::setDirection({ 0.0f,-1.0f,0.0f }, a);

	}

	LightSource::setLightType(LIGHT_TYPE::POINT, 6);
	LightSource::setParent(mod[18], 6);
	LightSource::setDiffuse({ 255,100,0,100 }, 6);
	LightSource::setAttenuationQuadratic(0.04f, 6.0f);

	LightSource::setLightType(LIGHT_TYPE::POINT, 7);
	LightSource::setParent(mod[0], 7);
	LightSource::setPosition({ 0, -0.75f, 0	}, 7);
	LightSource::setDiffuse({ 255,0,0,100 }, 7);
	LightSource::setAttenuationQuadratic(1.f, 7);
	LightSource::setLightType(LIGHT_TYPE::POINT, 8);
	LightSource::setParent(mod[1], 8);
	LightSource::setPosition({ 0, -0.75f, 0 }, 8);
	LightSource::setDiffuse({ 0,0,255,100 }, 8);
	LightSource::setAttenuationQuadratic(1.f, 8);
	LightSource::setLightType(LIGHT_TYPE::POINT, 9);
	LightSource::setParent(mod[2], 9);
	LightSource::setPosition({ 0, -0.75f, 0 }, 9);
	LightSource::setDiffuse({ 0,255,0,100 }, 9);
	LightSource::setAttenuationQuadratic(1.f, 9);
	LightSource::setLightType(LIGHT_TYPE::POINT, 10);
	LightSource::setParent(mod[3], 10);
	LightSource::setPosition({ 0, -0.75f, 0 }, 10);
	LightSource::setDiffuse({ 255,255,0,100 }, 10);
	LightSource::setAttenuationQuadratic(1.f, 10);

	LightSource::setSceneAmbient({ 60,60,60,255 });
	/// - Set Camera - ///

	game.setCameraPosition({ 0,15,-10 });
	game.setCameraAngle(-45, { 1,0,0 });

	audio.createStream("Game Jam(Full).wav");

	audio.play(true);

	//engine stuff
	//game.setFPSLimit(60);
	game.setBackgroundColour(0.05f, 0.0f, 0.1f);
	game.keyPressed(keyInputPressed);
	game.keyReleased(keyInputReleased);
	game.mouseButtonReleased(mouseButtonReleased);
	game.gameLoopUpdate(update);
	game.run();//this one is pretty important
	//the game ended... why are you here?... leave
	//Or run it again... ;)
	return 0;
}