#pragma once
#define _USE_MATH_DEFINES
#include <GameEmGine.h>
#include <EmGineAudioPlayer.h>
#include <vector>
#include "Player.h"
#include "Boss.h"
#include "Minion.h"
#include "Assault.h"
#include "Specialist.h"
#include "Medic.h"
#include "Tank.h"

typedef EmGineAudioPlayer AudioPlayer;
typedef GameEmGine GAME;

using std::vector;

class Game:public Scene
{
public:
	/// - Collision Class - ///

	bool collision(Model* l, Model* k)
	{
		//if distance between mod in the x OR z is less than half of both widths combined then collide and don't allow any more movement in that direction.
		Coord3D thing = l->getCenter() - k->getCenter();

		float distanceX = abs(thing.x);
		float distanceZ = abs(thing.z);

		float capW = (l->getWidth() + k->getWidth()) / 2;
		float capD = (l->getDepth() + k->getDepth()) / 2;

		if(std::abs(distanceX) <= capW)
			if(std::abs(distanceZ) <= capD)
				return true;

		return false;
	}


	///~ 3D Collision Function ~///

	bool collision3D(Model * l, Model * k)
	{
		//if distance between mod in the x OR z is less than half of both widths combined then collide and don't allow any more movement in that direction.
		Coord3D thing = l->getCenter() - k->getCenter();

		float distanceX = abs(thing.x);
		float distanceY = abs(thing.y);
		float distanceZ = abs(thing.z);

		float capW = (l->getWidth() + k->getWidth()) / 2;
		float capH = (l->getHeight() + k->getHeight()) / 2;
		float capD = (l->getDepth() + k->getDepth()) / 2;

		if(std::abs(distanceX) <= abs(capW))
			if(std::abs(distanceZ) <= abs(capD))
				if(std::abs(distanceY) <= abs(capH))
					return true;

		return false;
	}

	//instance key is pressed
	void keyInputPressed(int key, int modifier)
	{
		modifier;
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

	//instance key is released
	void keyInputReleased(int key, int modifier)
	{
		modifier;
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
		if(key == GLFW_KEY_KP_6)
			GAME::setFPSLimit(GAME::getFPSLimit() + 1);
		if(key == GLFW_KEY_KP_4)
			GAME::setFPSLimit(GAME::getFPSLimit() - 1);

		if(key == GLFW_KEY_F) //Toggles Fullscreen
		{
			static bool full;
			GAME::getWindow()->setFullScreen(full = !full);
			printf("Full Screen: %s\n", full ? "true" : "false");
		}

		if(key == GLFW_KEY_SPACE) //changes the model that is being moved
		{
			static CAMERA_TYPE type = PERSPECTIVE;
			GAME::setCameraType(type = type == ORTHOGRAPHIC ? PERSPECTIVE : ORTHOGRAPHIC);
		}

		if(key == GLFW_KEY_TAB)
			movePlayer = !movePlayer;

		if(key == GLFW_KEY_F5) //resets the camera
		{
			GAME::m_modelShader->refresh();
			//			GAME::m_grayScalePost->refresh();
						//GAME::setCameraAngle(0, { 1, 1, 1 });
						//	GAME::setCameraPosition({0,0,0});
		}

		if(key == 'R')
			GAME::setCameraAngle(0, {1,1,1});

		printf("key RELEASED code: %d\n\n", key);
	}

	void mouseButtonReleased(int button, int _mod)
	{
		_mod;
		if(button == LEFT_BUTTON)
			leftM = InputManager::getMouseCursorPosition();
		if(button == RIGHT_BUTTON)
			rightM = InputManager::getMouseCursorPosition();
	}

	void playerTypes(vector<Player*> & playerType)
	{
		mod.insert(mod.begin(), playerType.begin(), playerType.end());
	}

	// Set game screen
	void init()
	{
		mod.resize(93);//sets the initial size of the vector (if u add any more models, increase this number)



		/// - Set Camera  - ///
		GAME::setCameraType(PERSPECTIVE);
		GAME::setCameraPosition({0,18.5f,-14});
		GAME::setCameraAngle(-35, {1,0,0});


		//GAME::setFPSLimit(60);
		/// - Load mod into Scene - ///

		for(auto& a : mod)
			if(a)
				GAME::addModel(a);
			else
				break;

		static Animation walk[4], idle[4];

		for(int a = 0; a < 4; a++)
		{
			walk[a].addDir("Models/AssaultModel/Walk/");
			idle[a].addDir("Models/AssaultModel/Idle/");
			mod[a]->addAnimation("walk", &walk[a]);
			mod[a]->addAnimation("idle", &idle[a]);
			mod[a]->setAnimation("walk");

			walk[a].setAnimationSpeed(.25);
			walk[a].repeat(true);
			idle[a].stop();

			mod[a]->addAnimation("walk", &walk[a]);
			mod[a]->addAnimation("idle", &idle[a]);
			mod[a]->setAnimation("walk");
		}

		//Building 1s
		mod[4] = (new Model("Models/Buildings/CashCorp/CashcorpBuildingWIP.obj"));
		GAME::addModel(mod[4]);//4
		mod[5] = (new Model(*mod[4]));
		GAME::addModel(mod[5]);//5
		mod[6] = (new Model(*mod[4]));
		GAME::addModel(mod[6]);//6

		mod[6]->setToRender(false);

		//Project Nebula Sign
		mod[7] = (new Model("Models/Neon Signs/Project Nebula/signn.obj"));
		GAME::addModel(mod[7]); //7

		//Boss
		mod[8] = (new Boss("Models/BOSS/robotBOI.obj"));
		GAME::addModel(mod[8]); //8
		//mod[8]->enableBoundingBox(true);

		//Floor
		mod[9] = (new Model("Models/Floor/Floor.obj"));
		GAME::addModel(mod[9]); //9

		//Light Posts
		mod[10] = (new Model("Models/Lamp/lampPost.obj"));
		GAME::addModel(mod[10]); //10
		mod[11] = (new Model(*mod[10]));
		GAME::addModel(mod[11]);//11
		mod[12] = (new Model(*mod[10]));
		GAME::addModel(mod[12]);//12
		mod[13] = (new Model(*mod[10]));
		GAME::addModel(mod[13]);//13
		mod[14] = (new Model(*mod[10]));
		GAME::addModel(mod[14]);//14
		mod[15] = (new Model(*mod[10]));
		GAME::addModel(mod[15]);//15

		//Bench
		mod[16] = (new Model("Models/Bench/Bench.obj"));
		GAME::addModel(mod[16]); //16
		mod[17] = (new Model(*mod[16]));
		GAME::addModel(mod[17]);//17

		//Planet
		mod[18] = (new Model("Models/Planet/planet.obj"));
		GAME::addModel(mod[18]); //18
		mod[18]->setToRender(false);

		//Building 2s
		mod[19] = (new Model("Models/Buildings/Tunnel/tunnelWIP.obj"));
		GAME::addModel(mod[19]);//19
		mod[20] = (new Model(*mod[19]));
		GAME::addModel(mod[20]);//20
		mod[21] = (new Model(*mod[19]));
		GAME::addModel(mod[21]);//21
		mod[21]->setToRender(false);

		//GraveStones
		mod[22] = (new Model("Models/RIP/Rip Ani/RIP1.obj")); //22
		mod[23] = (new Model("Models/RIP/Rip Ani/RIP1.obj"));//23
		mod[24] = (new Model("Models/RIP/Rip Ani/RIP1.obj"));//24
		mod[25] = (new Model("Models/RIP/Rip Ani/RIP1.obj"));//25

		//Coloured ring "IDs"
		mod[26] = (new Model("Models/ID/Identifier.obj"));//26
		GAME::addModel(mod[26]);
		mod[27] = (new Model(*mod[26]));//27
		GAME::addModel(mod[27]);
		mod[28] = (new Model(*mod[27]));//28
		GAME::addModel(mod[28]);
		mod[29] = (new Model(*mod[28]));//29
		GAME::addModel(mod[29]);

		//Building 3s
		mod[30] = (new Model("Models/Buildings/Building3/House.obj"));
		GAME::addModel(mod[30]);//30
		mod[31] = (new Model(*mod[30]));
		GAME::addModel(mod[31]);//31
		mod[32] = (new Model(*mod[30]));
		GAME::addModel(mod[32]);//32

		//Building 4s
		mod[33] = (new Model("Models/Buildings/Building4/tallBuilding.obj"));
		GAME::addModel(mod[33]);//33
		mod[34] = (new Model(*mod[33]));
		GAME::addModel(mod[34]);//34

		//Trees
		mod[35] = (new Model("Models/DiedTree/tree.obj"));
		GAME::addModel(mod[35]);//35
		mod[36] = (new Model(*mod[35]));
		GAME::addModel(mod[36]);//36
		mod[37] = (new Model(*mod[35]));
		GAME::addModel(mod[37]);//37
		mod[38] = (new Model(*mod[35]));
		GAME::addModel(mod[38]);//38

		//Building 5s
		mod[39] = (new Model("Models/Buildings/Building5/smallShop.obj"));
		GAME::addModel(mod[39]);//39
		mod[40] = (new Model(*mod[39]));
		GAME::addModel(mod[40]);//40
		mod[41] = (new Model(*mod[39]));
		GAME::addModel(mod[41]);//41

		mod[41]->setToRender(false);

		mod[42] = (new Model("Models/Buildings/Building6/Building6.obj"));
		GAME::addModel(mod[42]);//42
		mod[43] = (new Model(*mod[42]));
		GAME::addModel(mod[43]);//43

		mod[42]->setToRender(false);
		mod[43]->setToRender(false);

		

		mod[48] = (new Model("Models/Bullet/bullet.obj"));//48

		mod[49] = (new Model("Models/Trash/TrashCan.obj"));
		GAME::addModel(mod[49]); //49
		mod[50] = (new Model(*mod[49]));
		GAME::addModel(mod[50]);//50

		mod[49]->setToRender(false);
		mod[50]->setToRender(false);

		mod[51] = (new Model("Models/Picnic/PicnicTable.obj"));
		GAME::addModel(mod[51]); //51
		mod[52] = (new Model(*mod[51]));
		GAME::addModel(mod[52]);//52

		mod[51]->setToRender(false);
		mod[52]->setToRender(false);

		mod[53] = (new Model("Models/PizzaSign/PIZZA.obj"));
		GAME::addModel(mod[53]); //53

		mod[54] = (new Model("Models/AssaultModel/Weapon/AssaultClassGun.obj"));
		GAME::addModel(mod[54]); //54
		mod[55] = (new Model(*mod[54]));
		GAME::addModel(mod[55]); //55
		mod[56] = (new Model(*mod[54]));
		GAME::addModel(mod[56]); //56
		mod[57] = (new Model(*mod[54]));
		GAME::addModel(mod[57]); //57

		mod[58] = (new Model("Models/Planet/Planet2/planet.obj"));
		GAME::addModel(mod[58]); //58

		mod[58]->setToRender(false);
		//Pause Menu
		//mod[] = (new Model("Models/Pause Menu/Pause Menu.obj"));//33

		//collision floor
		mod[59] = (new Model("Models/Floor/Floor2.obj"));//59
		GAME::addModel(mod[59]);

		mod[59]->setToRender(false);
		mod[59]->getTransformer().setScale(1.f, 1.0f, 1.5f), mod[59]->getTransformer().setPosition(0.0f, 0.15f, 5.0f);

		//missile hit boxes
		mod[60] = (new Model(*((Boss*)mod[8])->getMissial(0)));//60
		GAME::addModel(mod[60]); //
		mod[60]->setToRender(false);
		mod[60]->getTransformer().setScale(6, 1, 1);
		mod[61] = (new Model(*mod[60]));//61
		GAME::addModel(mod[61]);//
		mod[62] = (new Model(*mod[60]));//62
		GAME::addModel(mod[62]);//
		mod[63] = (new Model(*mod[60]));//63
		GAME::addModel(mod[63]);//

		((Boss*)mod[8])->getMissial(0)->addChild(mod[60]);
		((Boss*)mod[8])->getMissial(1)->addChild(mod[61]);
		((Boss*)mod[8])->getMissial(2)->addChild(mod[62]);
		((Boss*)mod[8])->getMissial(3)->addChild(mod[63]);

		//player's blood bar
		mod[64] = (new Model("Models/BloodBar/RedBar/blood.obj"));//64
		GAME::addModel(mod[64]);
		mod[65] = (new Model("Models/BloodBar/BlueBar/blood.obj"));//65
		GAME::addModel(mod[65]);
		mod[66] = (new Model("Models/BloodBar/GreenBar/blood.obj"));//66
		GAME::addModel(mod[66]);
		mod[67] = (new Model("Models/BloodBar/YellowBar/blood.obj"));//67
		GAME::addModel(mod[67]);
		mod[68] = (new Model("Models/BloodBar/RedBarLighter/blood.obj"));//68
		GAME::addModel(mod[68]);
		mod[69] = (new Model("Models/BloodBar/BlueBarLighter/blood.obj"));//69
		GAME::addModel(mod[69]);
		mod[70] = (new Model("Models/BloodBar/GreenBarLighter/blood.obj"));//70
		GAME::addModel(mod[70]);
		mod[71] = (new Model("Models/BloodBar/YellowBarLighter/blood.obj"));//71
		GAME::addModel(mod[71]);

		//boss's blood bar
		mod[72] = (new Model("Models/BloodBar/PinkBar/blood.obj"));//72
		GAME::addModel(mod[72]);
		mod[73] = (new Model("Models/BloodBar/PinkBarLighter/blood.obj"));//73
		GAME::addModel(mod[73]);

		//bullet's circle
		mod[74] = (new Model("Models/BulletCircle/BulletCircle.obj"));//74
		GAME::addModel(mod[74]);
		mod[75] = (new Model(*mod[74]));//75
		GAME::addModel(mod[75]);
		mod[76] = (new Model(*mod[75]));//76
		GAME::addModel(mod[76]);
		mod[77] = (new Model(*mod[76]));//77
		GAME::addModel(mod[77]);

		//Minions
		mod[78] = (new Minion("Models/Minion/SmallRobot/SmallRobot.obj"));//78
		GAME::addModel(mod[78]);
		mod[78]->setToRender(false);

		//TRAIN
		mod[79] = (new Model("Models/Train/Head/trainhead.obj"));//79
		GAME::addModel(mod[79]);
		mod[80] = (new Model("Models/Train/Body/trainbodyblend.obj"));//80
		GAME::addModel(mod[80]);
		mod[81] = (new Model(*mod[80]));//81
		GAME::addModel(mod[81]);
		mod[82] = (new Model(*mod[81]));//82
		GAME::addModel(mod[82]);
		mod[83] = (new Model(*mod[82]));//83
		GAME::addModel(mod[83]);
		mod[84] = (new Model(*mod[83]));//84
		GAME::addModel(mod[84]);
		mod[85] = (new Model("Models/Train/Head/trainhead.obj"));//85
		GAME::addModel(mod[85]);
		mod[79]->getTransformer().setPosition(-14.45f, 0.3f, 8.0f);
		mod[80]->getTransformer().setPosition(-9.2f, 0.3f, 8.0f);
		mod[81]->getTransformer().setPosition(-4.6f, 0.3f, 8.0f);
		mod[82]->getTransformer().setPosition(0.0f, 0.3f, 8.0f);
		mod[83]->getTransformer().setPosition(4.6f, 0.3f, 8.0f);
		mod[84]->getTransformer().setPosition(9.2f, 0.3f, 8.0f);
		mod[85]->getTransformer().setPosition(14.45f, 0.3f, 8.0f), mod[85]->getTransformer().setRotation(Coord3D(0, 180, 0));

		//RAIL
		mod[86] = (new Model("Models/Rail/rail.obj"));//86
		GAME::addModel(mod[86]);
		mod[87] = (new Model(*mod[86]));//87
		GAME::addModel(mod[87]);
		mod[88] = (new Model(*mod[87]));//88
		GAME::addModel(mod[88]);
		mod[89] = (new Model(*mod[88]));//89
		GAME::addModel(mod[89]);
		mod[90] = (new Model(*mod[89]));//90
		GAME::addModel(mod[90]);
		mod[91] = (new Model(*mod[90]));//91
		GAME::addModel(mod[91]);
		mod[92] = (new Model(*mod[91]));//92
		GAME::addModel(mod[92]);



		mod[86]->getTransformer().setScale(0.7f), mod[86]->getTransformer().setPosition(-18.0f, 0.0f, 8.0f), mod[86]->getTransformer().setRotation(Coord3D(0, 90, 0));
		mod[87]->getTransformer().setScale(0.7f), mod[87]->getTransformer().setPosition(-12.0f, 0.0f, 8.0f), mod[87]->getTransformer().setRotation(Coord3D(0, 90, 0));
		mod[88]->getTransformer().setScale(0.7f), mod[88]->getTransformer().setPosition(-6.0f, 0.0f, 8.0f), mod[88]->getTransformer().setRotation(Coord3D(0, 90, 0));
		mod[89]->getTransformer().setScale(0.7f), mod[89]->getTransformer().setPosition(0.0f, 0.0f, 8.0f), mod[89]->getTransformer().setRotation(Coord3D(0, 90, 0));
		mod[90]->getTransformer().setScale(0.7f), mod[90]->getTransformer().setPosition(6.0f, 0.0f, 8.0f), mod[90]->getTransformer().setRotation(Coord3D(0, 90, 0));
		mod[91]->getTransformer().setScale(0.7f), mod[91]->getTransformer().setPosition(12.0f, 0.0f, 8.0f), mod[91]->getTransformer().setRotation(Coord3D(0, 90, 0));
		mod[92]->getTransformer().setScale(0.7f), mod[92]->getTransformer().setPosition(18.0f, 0.0f, 8.0f), mod[92]->getTransformer().setRotation(Coord3D(0, 90, 0));

		////Medic Healing Ring
		//mod[] = (new Model("Models/TrainGrayBox.obj"));//86
		//GAME::addModel(mod[]);


		/// - Set Model Transforms - ///
		//Player Transforms
		mod[0]->getTransformer().setScale(1.35f), mod[0]->getTransformer().setPosition(1.0f, 0.0f, -5.0f);
		mod[1]->getTransformer().setScale(1.35f), mod[1]->getTransformer().setPosition(-1.0f, 0.0f, -5.0f);
		mod[2]->getTransformer().setScale(1.35f), mod[2]->getTransformer().setPosition(2.0f, 0.0f, -5.0f);
		mod[3]->getTransformer().setScale(1.35f), mod[3]->getTransformer().setPosition(-2.0f, 0.0f, -5.0f);
		mod[0]->getTransformer().setRotation(Coord3D(0, 180, 0));
		mod[1]->getTransformer().setRotation(Coord3D(0, 180, 0));
		mod[2]->getTransformer().setRotation(Coord3D(0, 180, 0));
		mod[3]->getTransformer().setRotation(Coord3D(0, 180, 0));

		//Building Transforms
		//Building 1s
		mod[4]->getTransformer().setScale(1), mod[4]->getTransformer().setPosition(-15.175f, 0.0f, -2.0f), mod[4]->getTransformer().setRotation({0.0f,90.0f,0.0f});;
		mod[5]->getTransformer().setScale(1), mod[5]->getTransformer().setPosition(6.0f, 0.0f, 29.0f), mod[5]->getTransformer().setRotation({0.0f,-90.0f,0.0f});
		//mod[6]->getTransformer().setScale(2), mod[6]->getTransformer().setPosition(-4.0f, 0.0f, 22.75f), mod[6]->getTransformer().setRotation({0.0f,-90.0f,0.0f});

		//Building 2s
		mod[19]->getTransformer().setScale(0.85f), mod[19]->getTransformer().setPosition(-16.8f, 0.0f, 7.8f), mod[19]->getTransformer().setRotation({0.0f, 90.0f,0.0f});
		mod[20]->getTransformer().setScale(0.85f), mod[20]->getTransformer().setPosition(16.8f, 0.0f, 8.0f), mod[20]->getTransformer().setRotation({0.0f, -90.0f, 0.0f});;
		//mod[21]->getTransformer().setScale(1.75f), mod[21]->getTransformer().setPosition(13.5f, 0.0f, 22.4f), mod[21]->getTransformer().setRotation({0.0f, -90.0f, 0.0f});

		//Buildings 3s
		mod[30]->getTransformer().setPosition(10.5f, 0.0f, 23.6f);
		mod[31]->getTransformer().setPosition(19.5f, 0.0f, 3.75f), mod[31]->getTransformer().setRotation({0,180,0});
		mod[32]->getTransformer().setPosition(-9.3f, 0.0f, 25.35f), mod[32]->getTransformer().setRotation({0,-90,0});
		//Building 4s
		mod[33]->getTransformer().setPosition(17.35f, 0.0f, 20.9f), mod[33]->getTransformer().setRotation({0,-90,0});
		mod[34]->getTransformer().setPosition(-14.25f, 0.0f, 24.30f), mod[34]->getTransformer().setRotation({0,180,0});
		//Building 5s
		mod[39]->getTransformer().setScale(1.0f, 1.0f, 1.05f), mod[39]->getTransformer().setPosition(19.6f, 0.0f, 16.5f), mod[39]->getTransformer().setRotation({0,180,0});
		mod[40]->getTransformer().setScale(1.25f, 1.0f, 1.0f), mod[40]->getTransformer().setPosition(-16.9f, 0.0f, 16.35f), mod[40]->getTransformer().setRotation({0,90,0});
		//mod[41]->getTransformer().setScale(1.0f, 1.3f, 1.6f), mod[41]->getTransformer().setPosition(1.0f, 0.0f, 25.5f), mod[41]->getTransformer().setRotation({0,-90,0});
		//Building 6s
		//mod[42]->getTransformer().setScale(1.0f, 1.5f, 1.8f), mod[42]->getTransformer().setPosition(-14.2f, 0.0f, 22.9f), mod[42]->getTransformer().setRotation({0,-90,0});
		//mod[43]->getTransformer().setPosition(16.65f, 0.0f, -3.15f), mod[43]->getTransformer().setRotation({0,180,0});

		//Project Nebula Sign Transforms
		mod[7]->getTransformer().setScale(3), mod[7]->getTransformer().setPosition(9.5f, 5.34f, 22.5f);

		//Boss Trarrnsforms
		mod[8]->getTransformer().setScale(1.3f), mod[8]->getTransformer().setPosition(0.0f, 0.0f, 19.0f), mod[8]->getTransformer().setRotation({0.0f, 0.0f, 0.0f});

		//Floor Transforms
		mod[9]->getTransformer().setScale(2.25f, 1.0f, 5.0f), mod[9]->getTransformer().setPosition(0.0f, 0.0f, 5.0f);

		//Street Light Transforms
		mod[10]->getTransformer().setScale(0.5f, 0.8f, 0.5f), mod[10]->getTransformer().setPosition(13.0f, 0.0f, -1.0f);
		mod[11]->getTransformer().setScale(0.5f, 0.8f, 0.5f), mod[11]->getTransformer().setPosition(13.0f, 0.0f, 7.0f);
		mod[12]->getTransformer().setScale(0.5f, 0.8f, 0.5f), mod[12]->getTransformer().setPosition(13.0f, 0.0f, 15.0f);
		mod[13]->getTransformer().setScale(0.5f, 0.8f, 0.5f), mod[13]->getTransformer().setPosition(-13.0f, 0.0f, -1.0f), mod[13]->getTransformer().setRotation({0.0f,180.0f,0.0f});
		mod[14]->getTransformer().setScale(0.5f, 0.8f, 0.5f), mod[14]->getTransformer().setPosition(-13.0f, 0.0f, 7.0f), mod[14]->getTransformer().setRotation({0.0f,180.0f,0.0f});
		mod[15]->getTransformer().setScale(0.5f, 0.8f, 0.5f), mod[15]->getTransformer().setPosition(-13.0f, 0.0f, 15.0f), mod[15]->getTransformer().setRotation({0.0f,180.0f,0.0f});

		//Bench Transforms
		mod[16]->getTransformer().setPosition(-13.0f, 0.0f, 3.0f);
		mod[17]->getTransformer().setPosition(13.0f, 0.0f, 3.0f), mod[17]->getTransformer().setRotation({0.0f,180.0f,0.0f});

		//Planet Transforms
		mod[18]->getTransformer().setPosition(9.0f, 17.0f, 36.0f);
		//mod[58]->getTransformer().setPosition(-10.0f, 11.0f, 25.0f);

		//ID rings?
		mod[26]->setColour({255,0,0});
		mod[26]->getTransformer().setScale(0.65f), mod[26]->getTransformer().setPosition(0.0f, 0.1f, 0.0f), mod[26]->getTransformer().setRotation({0,-90,0});

		mod[27]->setColour({0,0,255});
		mod[27]->getTransformer().setScale(0.65f), mod[27]->getTransformer().setPosition(0.0f, 0.1f, 0.0f), mod[27]->getTransformer().setRotation({0,-90,0});

		mod[28]->setColour({0,255,0});
		mod[28]->getTransformer().setScale(0.65f), mod[28]->getTransformer().setPosition(0.0f, 0.1f, 0.0f), mod[28]->getTransformer().setRotation({0,-90,0});

		mod[29]->setColour({255,255,0});
		mod[29]->getTransformer().setScale(0.65f), mod[29]->getTransformer().setPosition(0.0f, 0.1f, 0.0f), mod[29]->getTransformer().setRotation({0,-90,0});

		//Trees
		mod[35]->getTransformer().setScale(0.3f), mod[35]->getTransformer().setPosition(13.0f, 0.0f, -3.0f), mod[35]->getTransformer().setRotation({0,-0,0});
		mod[36]->getTransformer().setScale(0.3f), mod[36]->getTransformer().setPosition(-13.0f, 0.0f, -3.0f), mod[36]->getTransformer().setRotation({0,-0,0});
		mod[37]->getTransformer().setScale(0.3f), mod[37]->getTransformer().setPosition(13.0f, 0.0f, 11.0f), mod[37]->getTransformer().setRotation({0,-0,0});
		mod[38]->getTransformer().setScale(0.3f), mod[38]->getTransformer().setPosition(-13.0f, 0.0f, 11.0f), mod[38]->getTransformer().setRotation({0,-0,0});

		//Pizza Sign
		mod[53]->getTransformer().setScale(1.5f), mod[53]->getTransformer().setPosition(-10.25f, 5.4f, 22.3f);

		//Assault Weapons
		mod[54]->getTransformer().setScale(0.075f), mod[54]->getTransformer().setPosition(-0.1f, 0.65f, -0.15f), mod[54]->getTransformer().setRotation({0.0f,0.0f,0.0f});
		mod[55]->getTransformer().setScale(0.075f), mod[55]->getTransformer().setPosition(-0.1f, 0.65f, -0.15f), mod[55]->getTransformer().setRotation({0.0f,0.0f,0.0f});
		mod[56]->getTransformer().setScale(0.075f), mod[56]->getTransformer().setPosition(-0.1f, 0.65f, -0.15f), mod[56]->getTransformer().setRotation({0.0f,0.0f,0.0f});
		mod[57]->getTransformer().setScale(0.075f), mod[57]->getTransformer().setPosition(-0.1f, 0.65f, -0.15f), mod[57]->getTransformer().setRotation({0.0f,0.0f,0.0f});

		//Player Blood Bar
		for(int i = 0; i < 4; i++)
		{
			mod[i + 64]->getTransformer().setPosition(mod[i]->getTransformer().getPosition() + Coord3D{0.35f,1.4f,0.0f});
			mod[i + 64]->getTransformer().setRotation(Coord3D(0, 90, 0));
			mod[i + 64]->getTransformer().setScale(0.08f, 0.08f, 0.065f);
			mod[i + 68]->getTransformer().setPosition(mod[i + 64]->getTransformer().getPosition());
			mod[i + 68]->getTransformer().setRotation(Coord3D(0, 90, 0));
			mod[i + 68]->getTransformer().setScale(0.08f, 0.08f, 0.065f);
		}

		//Boss Blood Bar
		mod[72]->getTransformer().setPosition(mod[8]->getTransformer().getPosition() + Coord3D{13.0f,18.5f,0.0f});
		mod[72]->getTransformer().setRotation(Coord3D(0, 90, 0));
		mod[72]->getTransformer().setScale(0.8f, 0.8f, 2.5f);
		mod[73]->getTransformer().setPosition(mod[72]->getTransformer().getPosition());
		mod[73]->getTransformer().setRotation(Coord3D(0, 90, 0));
		mod[73]->getTransformer().setScale(0.8f, 0.8f, 2.5f);

		//Bullet Circle
		mod[74]->setColour({255,0,0,150});
		mod[74]->getTransformer().setScale(0.65f), mod[74]->getTransformer().setPosition(0.0f, 0.05f, 0.0f), mod[74]->getTransformer().setRotation({0,-90,0});

		mod[75]->setColour({0,0,255,150});
		mod[75]->getTransformer().setScale(0.65f), mod[75]->getTransformer().setPosition(0.0f, 0.05f, 0.0f), mod[75]->getTransformer().setRotation({0,-90,0});

		mod[76]->setColour({0,255,0,150});
		mod[76]->getTransformer().setScale(0.65f), mod[76]->getTransformer().setPosition(0.0f, 0.05f, 0.0f), mod[76]->getTransformer().setRotation({0,-90,0});

		mod[77]->setColour({255,255,0,150});
		mod[77]->getTransformer().setScale(0.65f), mod[77]->getTransformer().setPosition(0.0f, 0.05f, 0.0f), mod[77]->getTransformer().setRotation({0,-90,0});



		/// - Set Model Colour - ///
		//Players colors and children
		mod[0]->setColour(1, 0.5, 0.5);
		mod[1]->setColour(0.5, 0.5, 1);
		mod[2]->setColour(0.5, 1, 0.5);
		mod[3]->setColour(1, 1, 0.5);
		mod[0]->addChild(mod[26]);
		mod[1]->addChild(mod[27]);
		mod[2]->addChild(mod[28]);
		mod[3]->addChild(mod[29]);
		mod[0]->addChild(mod[54]);
		mod[1]->addChild(mod[55]);
		mod[2]->addChild(mod[56]);
		mod[3]->addChild(mod[57]);
		mod[0]->addChild(mod[74]);
		mod[1]->addChild(mod[75]);
		mod[2]->addChild(mod[76]);
		mod[3]->addChild(mod[77]);

		LightSource::setLightAmount(14);
		for(int a = 0; a < 6; a++)
		{
			//mod[10 + a]->boundingBoxUpdate();
			LightSource::setLightType(LIGHT_TYPE::DIRECTIONAL, a);
			LightSource::setParent(mod[10 + a], a);
			LightSource::setPosition({-5.0f,4.5,0.0f}, a);
			LightSource::setDirection({0.0f,-1.0f,0.0f}, a);
			//LightSource::setDiffuse({ 255,100,0,100 }, 6);
			//LightSource::setAttenuationQuadratic(0.06f, 6);
		}

		LightSource::setLightType(LIGHT_TYPE::POINT, 6);
		LightSource::setParent(mod[44], 6);
		LightSource::setDiffuse({255,100,0,100}, 6);
		LightSource::setAttenuationQuadratic(0.06f, 6);

		LightSource::setLightType(LIGHT_TYPE::POINT, 7);
		LightSource::setParent(mod[0], 7);
		LightSource::setPosition({0, -0.75f, 0}, 7);
		LightSource::setDiffuse({255,0,0,100}, 7);
		LightSource::setAttenuationQuadratic(1.f, 7);

		LightSource::setLightType(LIGHT_TYPE::POINT, 8);
		LightSource::setParent(mod[1], 8);
		LightSource::setPosition({0, -0.75f, 0}, 8);
		LightSource::setDiffuse({0,0,255,100}, 8);
		LightSource::setAttenuationQuadratic(1.f, 8);

		LightSource::setLightType(LIGHT_TYPE::POINT, 9);
		LightSource::setParent(mod[2], 9);
		LightSource::setPosition({0, -0.75f, 0}, 9);
		LightSource::setDiffuse({0,255,0,100}, 9);
		LightSource::setAttenuationQuadratic(1.f, 9);

		LightSource::setLightType(LIGHT_TYPE::POINT, 10);
		LightSource::setParent(mod[3], 10);
		LightSource::setPosition({0, -0.75f, 0}, 10);
		LightSource::setDiffuse({255,255,0,100}, 10);
		LightSource::setAttenuationQuadratic(1.f, 10);

		LightSource::setLightType(LIGHT_TYPE::POINT, 11);
		LightSource::setParent(mod[45], 11);
		LightSource::setDiffuse({255,100,0,100}, 11);
		LightSource::setAttenuationQuadratic(0.06f, 11);

		LightSource::setLightType(LIGHT_TYPE::POINT, 12);
		LightSource::setParent(mod[46], 12);
		LightSource::setDiffuse({255,100,0,100}, 12);
		LightSource::setAttenuationQuadratic(0.06f, 12);

		LightSource::setLightType(LIGHT_TYPE::POINT, 13);
		LightSource::setParent(mod[47], 13);
		LightSource::setDiffuse({255,100,0,100}, 13);
		LightSource::setAttenuationQuadratic(0.06f, 13);

		LightSource::setSceneAmbient({255,255,255,255});

		/// - Set Camera  - ///

		GAME::setCameraPosition({0,15.5f,-17.5});
		GAME::setCameraAngle(-25, {1,0,0});

		/// key/mouse input ///
		keyPressed = [&](int a, int b) {keyInputPressed(a, b); };
		keyReleased = [&](int a, int b) {keyInputReleased(a, b); };
		mouseReleased = [&](int a, int b) {mouseButtonReleased(a, b); };

		AudioPlayer::init();

		audio.createAudioStream("Audio/Game Jam(Full).wav");

		audio.play(true);
	}


	/// - The Update Loop - ///

	void update(double dt)
	{
		//Time
		static float  time = 0;
		time += (float)dt; //Add Delta Time to Time

		//static float coolDown = 0;
		static float duration = 0;
		static bool f = true;

		float move = .1f;

		static float pointSize = 50.0f;
		static Player* player;

		// Boss Variables
		static Boss* CandyMan;
		CandyMan = (Boss*)mod[8]; //Set model 8 as Boss called "CandyMan"
		static bool bossActive = true; //

		//Minion Variables
		static vector<Minion*>minions;//Vector of minions
		static int minionCounter = 0;
		static float minionTimer;
		static float minionSpeed = .05f;
		static int maxMinionsOnScreen = 20;

		//Train Variables
		static float trainTimer = 0; //Determines when train comes and goes

		static vector<float> timer[4];
		static vector<Model*> pMissiles[4];
		static vector<Coord3D> missileVelocity[4];
		static vector<Model*> bullets[4];
		static vector<Coord3D> velocity[4];
		static bool gunControlLaw[4], dashControl[4];//stops the creation of bullets when trigger is healed down

		CandyMan->setPlayers((Player**)mod.data());
		CandyMan->update();

		//static bool dead[4];

		static float lastTime = (float)clock() / CLOCKS_PER_SEC;

		static Animation squash[4];

		static bool init = false;

		static float angle[4] = {180,180,180,180};

		static vector<Boss*> nimimis;


		/// - Math for the Catmull curves for the Boss - ///

		CandyMan->update();


		/// - Tombstone Animations - ///

		if(!init)
		{

			for(int a = 0; a < 4; a++)
			{
				squash[a].addDir("Models/RIP/Rip Ani/");
				squash[a].setAnimationSpeed(.2f);
				//	squash[a].repeat(true);
			}
			init = true;
		}

		bool youDead = true;
		static float deathCounter = 0;
		if(init)

			/// - If game mode (NOT CAMERA MODE) - ///
			if(movePlayer)
			{
				for(int a = 0; a < 4; a++)
				{
					player = (Player*)mod[a];
					if(GAME::isControllerConnected(a))
					{
						Xinput p1 = GAME::getController(a);
						if(p1.buttonPressed(p1.buttons.SELECT))
						{
							for(int b = 0; b < 4; b++)
								player->dead = false;
							GAME::setScene(new Game);
						}
						//Start button quits game
						if(p1.buttonPressed(p1.buttons.START))
						{
							puts("\nExiting Game\n");
							GAME::exit();
						}

						if(!player->dead)
						{
							deathCounter = 0;
							youDead = !true;


							if(p1.sticks[RS].x || p1.sticks[RS].y)
							{

								angle[a] = acosf(p1.sticks[RS].x /
									sqrtf(p1.sticks[RS].x * p1.sticks[RS].x
										+ p1.sticks[RS].y * p1.sticks[RS].y)) * (180 / (float)M_PI);
								angle[a] += (p1.sticks[RS].y < 0 ? (180 - angle[a]) * 2 : 0) + 90;//90 represents the start angle
								angle[a] = fmodf(angle[a], 360);
							}

							/// - Missile Collisions with Player - ///
							for(int b = 0; b < 4; b++)
							{
								bool collision = collision3D(player, mod[60 + b]);
								if(collision)
								{
									//curveroni[a] = 1;
									CandyMan->getMissial(a)->getTransformer().setPosition(mod[8]->getCenter());
									player->setHealth(player->getHealth() - 35);
									//Coord3D test = ;

								}
							}
							//Player comes near Boss, gets teleported backwards
							if(collision(player, CandyMan))
							{
								player->getTransformer().setPosition(player->getTransformer().getPosition().x, player->getTransformer().getPosition().y, player->getTransformer().getPosition().z - 15);
								player->setHealth(player->getHealth() - 35);
							}
							//If player dies
							if(player->getHealth() <= 0)
							{
								player->dead = true;
								mod[22 + a]->setColour(player->getColour());
								mod[22 + a]->getTransformer().setScale(0.75f * 2, 1 * 2, 0.5 * 2), mod[22 + a]->getTransformer().setPosition(player->getTransformer().getPosition()), mod[22 + a]->getTransformer().setRotation({0.0f,270.0f,0.0f});
								GAME::addModel(mod[22 + a]);
								mod[22 + a]->addAnimation("squash", &squash[a]);

								mod[22 + a]->setAnimation("squash");
								GAME::removeModel(player);
								GAME::removeModel(mod[44] + a);
							}
							/// - Player Shooting - ///
							if(p1.triggers[RT] >= .95 && !gunControlLaw[a])
							{
								if(player->getBulletCount() > 0)
								{
									gunControlLaw[a] = true; //gun Control Law makes it so the guns function "manualy" instead of "fully automatic"

									bullets[a].push_back(nullptr);
									GAME::addModel(bullets[a].back() = new Model(*mod[48]));
									bullets[a].back()->getTransformer().reset();
									bullets[a].back()->setColour(player->getColour());//bullet color = player color
									Coord3D pos = mod[a]->getTransformer().getPosition();
									bullets[a].back()->getTransformer().setPosition(pos.x, pos.y + .1f, pos.z);
									bullets[a].back()->getTransformer().setScale(0.13f);
									bullets[a].back()->getTransformer().setRotation({90 , angle[a] ,0});

									float cosVal = cos((float)(fmodf(angle[a] - 90, 360) * (M_PI / 180)));
									float sinVal = sin((float)(fmodf(angle[a] - 90, 360) * (M_PI / 180)));

									velocity[a].push_back(Coord3D());
									velocity[a].back() = Coord3D(cosVal * move * 3, 0, sinVal * move * 3);

									timer[a].push_back(0);
									audio.createAudioStream("Audio/pew.wav");
									audio.play();
									player->setBulletCount(player->getBulletCount() - 1);
								}
							}
							else if(p1.triggers[RT] < .95 && gunControlLaw[a])
								gunControlLaw[a] = false;

							/// - Button Presses on controller - ///
							if(p1.buttonPressed(p1.buttons.X))
							{
								player->setBulletCount(30);
								puts("RELOADING!!!\n");
							}
							if(p1.buttonPressed(p1.buttons.Y))
							{
								if(time - player->getTimeSinceLastMissile() >= 3)
								{
									pMissiles[a].push_back(nullptr);
									GAME::addModel(pMissiles[a].back() = new Model(*mod[44]));
									pMissiles[a].back()->getTransformer().reset();
									pMissiles[a].back()->setColour(player->getColour());
									Coord3D pos = mod[a]->getTransformer().getPosition();
									pMissiles[a].back()->getTransformer().setPosition(pos.x, pos.y + .1f, pos.z);
									pMissiles[a].back()->getTransformer().setScale(0.4f);

									pMissiles[a].back()->getTransformer().setRotation({0 , angle[a] ,0});

									float cosVal = cos((float)(fmodf(angle[a] - 90, 360) * (M_PI / 180)));
									float sinVal = sin((float)(fmodf(angle[a] - 90, 360) * (M_PI / 180)));

									missileVelocity[a].push_back(Coord3D());
									missileVelocity[a].back() = Coord3D(cosVal * move * 6, 0, sinVal * move * 6);
									player->setTimeSinceLastMissile(time);

									timer[a].push_back(0);
									audio.createAudioStream("pew.wav");
									audio.play();
									puts("SPECIAL ABILITY\n");
								}
							}

							/// - Boss Spawns Minions - ///
							//TODO: More Minions, Have boss spawn minions, Make minions have unifrom move speed. Lerp between colours??
							if(minionCounter < 10)
							{

								minions.push_back(nullptr);
								GAME::addModel(minions.back() = new Minion(*mod[78]));
								minions.back()->setToRender(true);
								minions.back()->getTransformer().reset();
								minions.back()->setColour(200, 100, 50);
								minions.back()->getTransformer().getPosition();
								minions.back()->getTransformer().setPosition(float(rand() % 4 + 7), float(rand() % 1 + 2), -float(rand() % 3) + 2); // Random spawns in bottom right of screen
								minions.back()->getTransformer().setScale(0.4f, 0.6f, 0.4f);

								minionCounter += 1;
							}

							/// - Minions Movement Towards Players - ///
							for(unsigned int m = 0; m < minions.size(); m++)
							{
								Coord3D norm = player->getTransformer().getPosition() - minions[m]->getTransformer().getPosition();
								norm.normalize();

								minions[m]->getTransformer().translateBy(norm * .001f);
								if(collision(minions[m], minions[m]))// Might have to change one of the m values??
								{
									// TODO:  Minions collide with each other so they arent a blob.
								}
							}

							/// - Left Trigger to Dash - ///

							if(p1.triggers[LT] >= .95)
							{
								static float coolDown[4];

								//get deltaTime put into duraction variable

								if(time - coolDown[a] >= 3)
								{
									if(f == true)
									{
										duration = time;
										f = false;
									}
									move = 0.5f;
									if(time - 0.1f >= duration)
									{
										move = 0.1f;
										//If triggers up then coolDown = time;
										coolDown[a] = time;
										f = true;
									}
								}

							}

							else//Do the same with the LT button, have it so will only work every X seconds.
							{
								move -= .001f;
								if(move <= .1f)
									move = .1f;
								//f = false;
								dashControl[a] = false;
							}

							mod[a]->getTransformer().setRotation({0,angle[a], 0});
							mod[a]->getTransformer().translateBy(p1.sticks[LS].x * move, 0, p1.sticks[LS].y * move); //move player
							float speed = p1.sticks[LS].x * p1.sticks[LS].x + p1.sticks[LS].y * p1.sticks[LS].y;


							if(!collision(mod[a], mod[59]))
								mod[a]->getTransformer().setPosition(
									abs(mod[a]->getTransformer().getPosition().x) > mod[59]->getWidth() / 2 ? mod[a]->getTransformer().getPosition().x < 0 ? -mod[59]->getWidth() / 2 : mod[59]->getWidth() / 2 : mod[a]->getTransformer().getPosition().x,
									0,
									abs(mod[a]->getTransformer().getPosition().z) > mod[59]->getDepth() / 2 ? mod[a]->getTransformer().getPosition().z < 0 ? -mod[59]->getDepth() / 2 : mod[59]->getDepth() / 2 : mod[a]->getTransformer().getPosition().z);

							//Player Animations
							if(!speed)
								mod[a]->getAnimation("walk")->pause();
							else
							{
								mod[a]->getAnimation("walk")->play();
								mod[a]->setAnimation("walk");
								mod[a]->getAnimation("walk")->setAnimationSpeed(.25f / speed);
							}

							//Update each player's Blood Bar
							mod[a + 64]->getTransformer().setPosition(mod[a]->getTransformer().getPosition() + Coord3D{0.35f,1.6f,0.0f});
							mod[a + 64]->getTransformer().setScale(0.08f, 0.08f, 0.065f * (player->getHealth() / player->getInitialHealth()));
							mod[a + 68]->getTransformer().setPosition(mod[a + 64]->getTransformer().getPosition());
							if(player->dead)
							{
								GAME::removeModel(mod[a + 64]);
								GAME::removeModel(mod[a + 68]);

								GAME::removeModel(mod[a + 26]);
								GAME::removeModel(mod[a + 54]);
								GAME::removeModel(mod[a + 74]);
							}

							//Update each player's Bullet Circle
							mod[a + 74]->getTransformer().setScale(0.65f * (player->getBulletCount() / 30.0f));
						}

						///~ Bullet Collisions ~///
						for(unsigned b = 0; b < bullets[a].size(); b++)
							if(bullets[a][b])
							{
								timer[a][b] += (float)clock() / CLOCKS_PER_SEC - lastTime;
								bullets[a][b]->getTransformer().translateBy(velocity[a][b].x, velocity[a][b].y, velocity[a][b].z);

								if(timer[a][b] >= 1)
								{
									GAME::removeModel(bullets[a][b]);
									bullets[a].erase(bullets[a].begin() + b);
									velocity[a].erase(velocity[a].begin() + b);
									timer[a].erase(timer[a].begin() + b);
									break;
								}

								bool bulletHit = false;
								/// Bullet Collisions with Train
								for(int t = 0; t < 7; t++)
								{
									if(collision(bullets[a][b], mod[79 + t]))
									{
										GAME::removeModel(bullets[a][b]);
										bulletHit = true;
									}
								}
								if(mod[8])
									if(collision(bullets[a][b], mod[8]))
									{
										GAME::removeModel(bullets[a][b]);
										bullets[a].erase(bullets[a].begin() + b);
										velocity[a].erase(velocity[a].begin() + b);
										timer[a].erase(timer[a].begin() + b);
										//Boss a.k.a model 8, is now called "CandyMan" for the purposes of functions.
										CandyMan->setHealth(CandyMan->getHealth() - 10);// When hit takes damage
										if(CandyMan->getHealth() <= 0)
										{
											GAME::removeModel(CandyMan); // If health = 0 then boss dead
											//	mod[8] = nullptr;
											bossActive = false;
											puts("Killed The BOSS\n");
										}
										puts("Hit The BOSS\n");
										break;
									}

								for(auto& minion : minions)
								{
									if(collision(bullets[a][b], minion))
									{
										minion->setHealth(minion->getHealth() - 10);
										GAME::removeModel(bullets[a][b]);
										bulletHit = true;
										if(minion->getHealth() <= 0)
										{
											GAME::removeModel(minion);
											minions.erase(std::find(minions.begin(), minions.end(), minion));
											puts("Killed a boi\n");
											minionCounter -= 1;
											break;
										}

									}
									//TODO: Make Minions collide with player and do damage. Make Minions move at set speed
									//if (collision(minion, player))
									//{
									//	player->setHealth(player->getHealth() - 10);
									//}
								}

								if(bulletHit)
									bullets[a].erase(bullets[a].begin() + b);

							}

						for(unsigned b = 0; b < pMissiles[a].size(); b++)
							if(pMissiles[a][b])
							{
								timer[a][b] += (float)clock() / CLOCKS_PER_SEC - lastTime;
								pMissiles[a][b]->getTransformer().translateBy(missileVelocity[a][b].x, missileVelocity[a][b].y, missileVelocity[a][b].z);

								if(timer[a][b] >= 1)
								{
									GAME::removeModel(pMissiles[a][b]);
									pMissiles[a].erase(pMissiles[a].begin() + b);
									missileVelocity[a].erase(missileVelocity[a].begin() + b);
									timer[a].erase(timer[a].begin() + b);
									break;
								}

								/// - Missile Collisions with Train - ///
								for(int t = 0; t < 7; t++)
								{
									if(collision(pMissiles[a][b], mod[79 + t]))
									{
										GAME::removeModel(pMissiles[a][b]);
										pMissiles[a].erase(pMissiles[a].begin() + b);
									}
								}

								/// - If Boss Alive - ///
								if(mod[8])
									/// - If Missiles collide with Boss -///
									if(collision(pMissiles[a][b], mod[8]))
									{
										GAME::removeModel(pMissiles[a][b]);
										pMissiles[a].erase(pMissiles[a].begin() + b);
										missileVelocity[a].erase(missileVelocity[a].begin() + b);
										timer[a].erase(timer[a].begin() + b);
										//Boss a.k.a model 8, is now called CandyMan for the purposes of functions.
										CandyMan->setHealth(CandyMan->getHealth() - 50);// When hit takes damage
										/// - If Boss Dies - ///
										if(CandyMan->getHealth() <= 0)
										{
											GAME::removeModel(CandyMan); // If health = 0 then boss dead
											bossActive = false;
											puts("Killed The BOSS\n");
										}
										puts("Hit The BOSS\n");
										break;
									}
							}

						if(bossActive == false)
						{
							GAME::removeModel(mod[72]);
							GAME::removeModel(mod[73]);
							mod[44]->getTransformer().setPosition(1000.0f, 1000.0f, 1000.0f);
							mod[45]->getTransformer().setPosition(1000.0f, 1000.0f, 1000.0f);
							mod[46]->getTransformer().setPosition(1000.0f, 1000.0f, 1000.0f);
							mod[47]->getTransformer().setPosition(1000.0f, 1000.0f, 1000.0f);
						}
					}
					mod[a]->getTransformer().setScale(0.85f, 1.5f, 1.0f);
				}
			}
			else
			{
				//	mod[a]->getAnimation("squash")->update(mod[a]->getShader(),mod[a]->getMesh());
			}


		/// - Train Car Movement - ///
		for(int a = 0; a < 4; a++)
		{
			player = (Player*)mod[a];

			//Train Sits in middle of map
			if(0 <= (time - trainTimer) && 10 > (time - trainTimer))
			{
				for(int t = 0; t < 7; t++)
				{
					if(collision(mod[79 + t], player))
					{
						if(player->getTransformer().getPosition().z < mod[79 + t]->getTransformer().getPosition().z)
							player->getTransformer().setPosition(player->getTransformer().getPosition() + Coord3D(0.0f, 0.f, -0.1f));
						if(player->getTransformer().getPosition().z > mod[79 + t]->getTransformer().getPosition().z)
							player->getTransformer().setPosition(player->getTransformer().getPosition() + Coord3D(0.0f, 0.f, 0.1f));
					}
				}

			}
			//Train Moves off map
			if(10 <= (time - trainTimer) && 20 > (time - trainTimer))
			{
				for(int t = 0; t < 7; t++)
				{
					mod[79 + t]->getTransformer().translateBy(Coord3D{0.05f, 0.f, 0.f});//Move train cars right
					if(collision(mod[79 + t], player))
					{
						player->setHealth(player->getHealth() - 10);
						if(player->getTransformer().getPosition().z < mod[79 + t]->getTransformer().getPosition().z)
							player->getTransformer().setPosition(player->getTransformer().getPosition() + Coord3D(0.0f, 0.f, -0.8f));
						if(player->getTransformer().getPosition().z > mod[79 + t]->getTransformer().getPosition().z)
							player->getTransformer().setPosition(player->getTransformer().getPosition() + Coord3D(0.0f, 0.f, 0.8f));
					}
				}
			}
			//Train stops
			else if(20 <= (time - trainTimer) && 30 > (time - trainTimer))
			{
				for(int t = 0; t < 7; t++)
				{
					mod[79 + t]->getTransformer().translateBy(Coord3D{0.0f, 0.f, 0.f});//Stop Train cars
				}
			}
			//Train moves back onto map
			else if(30 <= (time - trainTimer) && 40 > (time - trainTimer))
			{
				for(int t = 0; t < 7; t++)
				{
					mod[79 + t]->getTransformer().translateBy(Coord3D{-0.05f, 0.f, 0.f});//Move train cars back to the right
					if(collision(mod[79 + t], player))
					{
						player->setHealth(player->getHealth() - 10);
						if(player->getTransformer().getPosition().z < mod[79 + t]->getTransformer().getPosition().z)
							player->getTransformer().setPosition(player->getTransformer().getPosition() + Coord3D(0.0f, 0.f, -0.8f));
						if(player->getTransformer().getPosition().z > mod[79 + t]->getTransformer().getPosition().z)
							player->getTransformer().setPosition(player->getTransformer().getPosition() + Coord3D(0.0f, 0.f, 0.8f));
						//if (player->getTransformer().getPosition().x < mod[85]->getTransformer().getPosition().x)
						//	player->getTransformer().setPosition(player->getTransformer().getPosition() + Coord3D(0.8f, 0.f, 0.0f));
					}
				}
			}
			//Train stops on map
			else if(40 <= (time - trainTimer) && 50 > (time - trainTimer))
			{
				for(int t = 0; t < 7; t++)
				{
					mod[79 + t]->getTransformer().setPosition(mod[79 + t]->getTransformer().getPosition() + Coord3D{0.00f, 0.f, 0.f});//Stop Train cars on map
					if(collision(mod[79 + t], player))
					{
						if(player->getTransformer().getPosition().z < mod[79 + t]->getTransformer().getPosition().z)
							player->getTransformer().setPosition(player->getTransformer().getPosition() + Coord3D(0.0f, 0.f, -0.1f));
						if(player->getTransformer().getPosition().z > mod[79 + t]->getTransformer().getPosition().z)
							player->getTransformer().setPosition(player->getTransformer().getPosition() + Coord3D(0.0f, 0.f, 0.1f));
					}
				}
				trainTimer += time; //Reset Train timer so it all starts again.
			}
		}

		//Boss health bar calculation
		mod[72]->getTransformer().setScale(0.8f, 0.8f, 2.5f * (CandyMan->getHealth() / 1000.0f));

		if(youDead)
		{
			//TODO: do something when the party is dead, game over screen with "Main Menu" "Quit" options

		}
		else
		{

		}

		lastTime = (float)clock() / CLOCKS_PER_SEC;

		/// - If game not active and Camera is active (Move camera mode) - ///
		if(!movePlayer)
			if(GAME::isControllerConnected(0))
			{
				Xinput p1 = GAME::getController(0);
				deathCounter = 0;

				//move camera
				move *= 2;

				GAME::moveCameraPositionBy({p1.sticks[LS].x * move , 0 * move, p1.sticks[LS].y * move});//move camera
				GAME::moveCameraAngleBy(ang * (abs(p1.sticks[RS].x) + abs(p1.sticks[RS].y)), {p1.sticks[RS].y  ,p1.sticks[RS].x, 0});//rotate camera
				//GAME::getMainCamera()->getTransformer().rotateBy({ ang *p1.sticks[RS].y ,ang *p1.sticks[RS].x ,0}, { p1.sticks[RS].y  ,p1.sticks[RS].x, 0 });
				GAME::moveCameraPositionBy({0 ,p1.triggers[LT] * -move,0});//move out
				GAME::moveCameraPositionBy({0 ,p1.triggers[RT] * move,0});//move out
				move /= 2;
			}

		GAME::m_grayScalePost->enable();
		glUniform1f(GAME::m_grayScalePost->getUniformLocation("uTime"), deathCounter);
		GAME::m_grayScalePost->disable();

		deathCounter += .007f;
		deathCounter = deathCounter <= 1 ? deathCounter : 1;
	}

private:
	std::vector<Model*> mod;
	bool fadein = true;
	bool fadeout = false;
	float splashT = 0;
	GLubyte splashAmbient = 0;

	float ang = 2;
	int numModel = 0;
	bool m_left = 0, m_right = 0, m_in = 0, m_out = 0, m_up = 0, m_down = 0,
		rotLeft = 0, rotRight = 0, rotUp = 0, rotDown = 0,
		movePlayer = true;
	Coord2D leftM, rightM;
	AudioPlayer audio;
	bool pause = true;
};
