#pragma once
#include <EmGineAudioPlayer.h>
#include <GameEmGine.h>
#include <SceneManager.h> 
#include <vector>
#include <list>
#include "Player.h"
#include "Boss.h"
#include "Minion.h"
#include "Assault.h"
#include "Specialist.h"
#include "Medic.h"
#include "Tank.h"

using std::vector;
using std::list;
using std::next;

class Game :public Scene
{
public:
	void onSceneExit() {}

	//instance key is pressed
	void keyInputPressed(int key, int modifier)
	{
		modifier;
		m_left = (key == 'A' ? true : m_left);
		m_right = (key == 'D' ? true : m_right);
		m_up = (key == 'E' ? true : m_up);
		m_down = (key == 'Q' ? true : m_down);
		m_fwd = (key == 'W' ? true : m_fwd);
		m_back = (key == 'S' ? true : m_back);

		rotLeft = (key == GLFW_KEY_LEFT ? true : rotLeft);
		rotRight = (key == GLFW_KEY_RIGHT ? true : rotRight);
		rotUp = (key == GLFW_KEY_UP ? true : rotUp);
		rotDown = (key == GLFW_KEY_DOWN ? true : rotDown);

		//printf("key PRESSED code: %d\n\n", key);
	}

	ColourRGBA lastColour;

	//instance key is released
	void keyInputReleased(int key, int modifier)
	{
		modifier;
		m_left = (key == 'A' ? false : m_left);
		m_right = (key == 'D' ? false : m_right);
		m_fwd = (key == 'W' ? false : m_fwd);
		m_back = (key == 'S' ? false : m_back);
		m_up = (key == 'E' ? false : m_up);
		m_down = (key == 'Q' ? false : m_down);

		rotLeft = (key == GLFW_KEY_LEFT ? false : rotLeft);
		rotRight = (key == GLFW_KEY_RIGHT ? false : rotRight);
		rotUp = (key == GLFW_KEY_UP ? false : rotUp);
		rotDown = (key == GLFW_KEY_DOWN ? false : rotDown);

		if(key == GLFW_KEY_TAB)
			++(*(char*)&currentState) %= 3;//don't ask

		switch(currentState)
		{
		case CONTROL_STATE::CAMERA:
			modeStr = "Camera";
			break;
		case CONTROL_STATE::LEVEL:
			modeStr = "Level Editing";
			break;
		}

		if(key == 'S' && modifier == GLFW_MOD_CONTROL)
		{
			//deselect
			if(curModel)
				curModel->setColour(lastColour);
			curModel = nullptr;

			vector<Model*>tmp;
			for(auto& a : mod)
				tmp.push_back(&a);
			SceneManager::saveScene("GameScene.scene", tmp);
		}
		if(key == 'L' && modifier == GLFW_MOD_CONTROL)
		{
			mod.clear();
			SceneManager::loadScene("gamescene.scene", mod);

			GameEmGine::clearObjectList();
			for(auto& a : mod)
				GameEmGine::addModel(&a);
		}

		//changes fps limit
		if(key == GLFW_KEY_KP_6)
			GameEmGine::setFPSLimit(util::clamp<short>(0, 60, GameEmGine::getFPSLimit() + 5));
		if(key == GLFW_KEY_KP_4)
			GameEmGine::setFPSLimit(util::clamp<short>(0, 60, GameEmGine::getFPSLimit() - 5));

		if(key == GLFW_KEY_F) //Toggles Full-screen
		{
			static bool full;
			GameEmGine::getWindow()->setFullScreen(full = !full);
			printf("Full Screen: %s\n", full ? "true" : "false");
		}

		if(key == GLFW_KEY_SPACE) //Changes the camera mode
		{
			static Camera::CAM_TYPE type = Camera::FRUSTUM;
			GameEmGine::setCameraType(type = type == Camera::ORTHOGRAPHIC ? Camera::FRUSTUM : Camera::ORTHOGRAPHIC);
		}

		if(key == GLFW_KEY_F5) //refreshes the shaders
			Shader::refresh();


		//if(key == GLFW_KEY_Q)
		//	GameEmGine::lutActive = (GameEmGine::lutActive == false) ? true : false;
		//if(key == GLFW_KEY_T)
		//	GameEmGine::toonActive = (GameEmGine::toonActive == false) ? true : false;
		//if(key == 'R')
		//	GameEmGine::setCameraRotation({0,0,0});

		//printf("key RELEASED code: %d\n\n", key);
	}

	//instance mouse is released
	void mouseButtonReleased(int button, int a_mod)
	{
		a_mod;
		//if(button == MOUSE_LEFT_BUTTON)
		//	leftM = InputManager::getMousePosition();
		//if(button == MOUSE_RIGHT_BUTTON)
		//	rightM = InputManager::getMousePosition();

		if(button == MOUSE_LEFT_BUTTON)
		{
			if(curModel)
				curModel->setColour(lastColour);

			curModel = GameEmGine::getMouseCollisionObject();

			if(curModel)
			{
				if(curModel)
					lastColour = curModel->getColour();

				int count = 0;
				for(auto beg = mod.begin(); beg != mod.end(); beg++, count++)
					if(*beg == *curModel)
						break;
				printf("Object#: %d\n", count);
				curModel->print();

				curModel->setColour(0, .65f, 0);
			}
		}
		if(button == MOUSE_RIGHT_BUTTON)
		{
			//deselect
			if(curModel)
				curModel->setColour(lastColour);
			curModel = nullptr;
		}
	}

	void playerTypes(vector<Player*>& playerType)
	{
		for(Player* a : playerType)
			mod.push_back(*a);
	}

	// Set game screen
	void init()
	{

		setSkyBox("skyboxes/space2/");
		enableSkyBox(true);

		//GameEmGine::m_modelShader->sendUniform("darken", 1);
		mod.resize(132);//sets the initial size of the vector (if u add any more models, increase this number)

		/// ~ Set Camera  ~ ///
		GameEmGine::setCameraType(Camera::FRUSTUM);
		GameEmGine::getMainCamera()->translate({0,15.5f,-5});
		GameEmGine::getMainCamera()->rotate({-45,0,0});

		/// ~ Post Effects~ ///
		Scene::customPostEffects = [&](FrameBuffer*gBuff, FrameBuffer* post, float )
		{
			gBuff,post;

			static Text mode;

			static FrameBuffer screen(1, "Post Screen");
			screen.initColourTexture(0, post->getColourWidth(0), post->getColourHeight(0));
			screen.resizeColour(0, post->getColourWidth(0), post->getColourHeight(0));

			OrthoPeramiters ortho{0,(float)GameEmGine::getWindowWidth(),(float)GameEmGine::getWindowHeight(),0,0,500};
			Camera cam(&ortho);

			if(!screen.checkFBO())
			{
				puts("FBO failed Creation");
				//system("pause");
				return;
			}

			mode.setText(modeStr.c_str());
			mode.setTextSize(50);
			mode.rotate(180, 0, 0);

			Vec2i offset{-int(mode.getWidth() * 0.1f), int(-GameEmGine::getWindowHeight() + mode.getHeight() * 2)};
			mode.translate(GameEmGine::getWindowSize() - mode.getSize() + offset);

			post->copyColourToBuffer(screen.getColourWidth(0), screen.getColourHeight(0), &screen);


			screen.enable();
			mode.render(&cam);
			screen.disable();

		//	screen.copyColourToBuffer(post->getColourWidth(0), post->getColourHeight(0), post);

		};

		//GAME::setFPSLimit(60);
		/// ~ Load mod into Scene ~ ///
		int numPlayers = 0;
		for(auto& a : mod)
			if(a.getMesh(0))
				GameEmGine::addModel(&a),
				Minion::addTarget((Player*)&a),
				numPlayers++;
			else
				break;

		static Transformer uniScaler;
		uniScaler.scale(.5);

#if FALSE
#pragma region BULL SHIT!!!!!

		//Building 1s
		*next(mod.begin(), 4) = (Model("Models/Buildings/CashCorp/CashcorpBuildingWIP.obj"));
		GameEmGine::addModel(&*next(mod.begin(), 4));//4
		*next(mod.begin(), 5) = (Model(*next(mod.begin(), 4)));
		GameEmGine::addModel(&*next(mod.begin(), 5));//5
		*next(mod.begin(), 6) = (Model(*next(mod.begin(), 4)));
		GameEmGine::addModel(&*next(mod.begin(), 6));//6

		(&*next(mod.begin(), 6))->setToRender(false);

		//Project Nebula Sign
		*next(mod.begin(), 7) = Model("Models/Neon Signs/Project Nebula/signn.obj");
		GameEmGine::addModel(&*next(mod.begin(), 7)); //7

		//Boss		
		*next(mod.begin(), 8) = Boss("Models/BOSS/missleShoot/BMS1.obj");
		GameEmGine::addModel(&*next(mod.begin(), 8)); //8



		//Floor
		*next(mod.begin(), 9) = (Model("Models/Floor/Floor.obj"));
		GameEmGine::addModel(&*next(mod.begin(), 9)); //9

		//Light Posts
		*next(mod.begin(), 10) = (Model("Models/Lamp/lampPost.obj"));
		GameEmGine::addModel(&*next(mod.begin(), 10)); //10
		*next(mod.begin(), 11) = (Model(*next(mod.begin(), 10)));
		GameEmGine::addModel(&*next(mod.begin(), 11));//11
		*next(mod.begin(), 12) = (Model(*next(mod.begin(), 10)));
		GameEmGine::addModel(&*next(mod.begin(), 12));//12
		*next(mod.begin(), 13) = (Model(*next(mod.begin(), 10)));
		GameEmGine::addModel(&*next(mod.begin(), 13));//13
		*next(mod.begin(), 14) = (Model(*next(mod.begin(), 10)));
		GameEmGine::addModel(&*next(mod.begin(), 14));//14
		*next(mod.begin(), 15) = (Model(*next(mod.begin(), 10)));
		GameEmGine::addModel(&*next(mod.begin(), 15));//15

		//Bench
		*next(mod.begin(), 16) = (Model("Models/Bench/Bench.obj"));
		GameEmGine::addModel(&*next(mod.begin(), 16)); //16
		*next(mod.begin(), 17) = (Model(*next(mod.begin(), 16)));
		GameEmGine::addModel(&*next(mod.begin(), 17));//17

		//Planet
		*next(mod.begin(), 18) = (Model("Models/Planet/planet.obj"));
		GameEmGine::addModel(&*next(mod.begin(), 18)); //18
		(&*next(mod.begin(), 18))->setToRender(false);

		//Building 2s
		*next(mod.begin(), 19) = (Model("Models/Buildings/Tunnel/Tunnel_Back_Final.obj"));
		GameEmGine::addModel(&*next(mod.begin(), 19));//19
		*next(mod.begin(), 20) = (Model(*next(mod.begin(), 19)));
		GameEmGine::addModel(&*next(mod.begin(), 20));//20
		*next(mod.begin(), 21) = (Model(*next(mod.begin(), 19)));
		GameEmGine::addModel(&*next(mod.begin(), 21));//21
		(&*next(mod.begin(), 21))->setToRender(false);

		//GraveStones
		*next(mod.begin(), 22) = (Model("Models/RIP/Rip Ani/RIP1.obj")); //22
		*next(mod.begin(), 23) = (Model("Models/RIP/Rip Ani/RIP1.obj"));//23
		*next(mod.begin(), 24) = (Model("Models/RIP/Rip Ani/RIP1.obj"));//24
		*next(mod.begin(), 25) = (Model("Models/RIP/Rip Ani/RIP1.obj"));//25

		//Building 3s
		*next(mod.begin(), 30) = (Model("Models/Buildings/Building3/House.obj"));
		GameEmGine::addModel(&*next(mod.begin(), 30));//30
		*next(mod.begin(), 31) = (Model(*next(mod.begin(), 30)));
		GameEmGine::addModel(&*next(mod.begin(), 31));//31
		*next(mod.begin(), 32) = (Model(*next(mod.begin(), 30)));
		GameEmGine::addModel(&*next(mod.begin(), 32));//32

		//Building 4s
		*next(mod.begin(), 33) = (Model("Models/Buildings/Building4/tallBuilding.obj"));
		GameEmGine::addModel(&*next(mod.begin(), 33));//33
		*next(mod.begin(), 34) = (Model(*next(mod.begin(), 33)));
		GameEmGine::addModel(&*next(mod.begin(), 34));//34

		//Trees
		*next(mod.begin(), 35) = (Model("Models/DiedTree/tree.obj"));
		GameEmGine::addModel(&*next(mod.begin(), 35));//35
		*next(mod.begin(), 36) = (Model(*next(mod.begin(), 35)));
		GameEmGine::addModel(&*next(mod.begin(), 36));//36
		*next(mod.begin(), 37) = (Model(*next(mod.begin(), 35)));
		GameEmGine::addModel(&*next(mod.begin(), 37));//37
		*next(mod.begin(), 38) = (Model(*next(mod.begin(), 35)));
		GameEmGine::addModel(&*next(mod.begin(), 38));//38

		//Building 5s
		*next(mod.begin(), 39) = (Model("Models/Buildings/Building5/smallShop.obj"));
		GameEmGine::addModel(&*next(mod.begin(), 39));//39
		*next(mod.begin(), 40) = (Model(*next(mod.begin(), 39)));
		GameEmGine::addModel(&*next(mod.begin(), 40));//40
		*next(mod.begin(), 41) = (Model(*next(mod.begin(), 39)));
		GameEmGine::addModel(&*next(mod.begin(), 41));//41

		(&*next(mod.begin(), 41))->setToRender(false);

		*next(mod.begin(), 42) = (Model("Models/Buildings/Building6/Building6.obj"));
		GameEmGine::addModel(&*next(mod.begin(), 42));//42
		*next(mod.begin(), 43) = (Model(*next(mod.begin(), 42)));
		GameEmGine::addModel(&*next(mod.begin(), 43));//43

		(&*next(mod.begin(), 42))->setToRender(false);
		(&*next(mod.begin(), 43))->setToRender(false);

		*next(mod.begin(), 44) = Model("Models/missile/candyMissile.obj");

		*next(mod.begin(), 48) = (Model("Models/Bullet/bullet.obj"));//48

		*next(mod.begin(), 49) = (Model("Models/Trash/TrashCan.obj"));
		GameEmGine::addModel(&*next(mod.begin(), 49)); //49
		*next(mod.begin(), 50) = (Model(*next(mod.begin(), 49)));
		GameEmGine::addModel(&*next(mod.begin(), 50));//50

		(&*next(mod.begin(), 49))->setToRender(false);
		(&*next(mod.begin(), 50))->setToRender(false);

		*next(mod.begin(), 51) = (Model("Models/Picnic/PicnicTable.obj"));
		GameEmGine::addModel(&*next(mod.begin(), 51)); //51
		*next(mod.begin(), 52) = (Model(*next(mod.begin(), 51)));
		GameEmGine::addModel(&*next(mod.begin(), 52));//52

		(&*next(mod.begin(), 51))->setToRender(false);
		(&*next(mod.begin(), 52))->setToRender(false);

		*next(mod.begin(), 53) = (Model("Models/PizzaSign/PIZZA.obj"));
		GameEmGine::addModel(&*next(mod.begin(), 53)); //53

		*next(mod.begin(), 54) = (Model("Models/AssaultModel/Weapon/AssaultClassGun.obj"));
		GameEmGine::addModel(&*next(mod.begin(), 54)); //54
		*next(mod.begin(), 55) = (Model(*next(mod.begin(), 54)));
		GameEmGine::addModel(&*next(mod.begin(), 55)); //55
		*next(mod.begin(), 56) = (Model(*next(mod.begin(), 54)));
		GameEmGine::addModel(&*next(mod.begin(), 56)); //56
		*next(mod.begin(), 57) = (Model(*next(mod.begin(), 54)));
		GameEmGine::addModel(&*next(mod.begin(), 57)); //57
		(&*next(mod.begin(), 54))->setToRender(false);
		(&*next(mod.begin(), 55))->setToRender(false);
		(&*next(mod.begin(), 56))->setToRender(false);
		(&*next(mod.begin(), 57))->setToRender(false);

		*next(mod.begin(), 58) = (Model("Models/Planet/Planet2/planet.obj"));
		GameEmGine::addModel(&*next(mod.begin(), 58)); //58

		(&*next(mod.begin(), 58))->setToRender(false);
		//Pause Menu
		//&*next(mod.begin(),) = (Model("Models/Pause Menu/Pause Menu.obj"));//33

		//collision2D floor
		*next(mod.begin(), 59) = (Model("Models/Floor/Floor2.obj"));//59
		GameEmGine::addModel(&*next(mod.begin(), 59));

		(&*next(mod.begin(), 59))->setToRender(false);
		(&*next(mod.begin(), 59))->scale(1.f, 1.0f, 1.f), (&*next(mod.begin(), 59))->translate(0.0f, 0.15f, 5.0f);

		//TRAIN
		*next(mod.begin(), 79) = (Model("Models/Train/Head/trainhead.obj"));//79
		GameEmGine::addModel(&*next(mod.begin(), 79));
		*next(mod.begin(), 80) = (Model("Models/Train/Body/trainbodyblend.obj"));//80
		GameEmGine::addModel(&*next(mod.begin(), 80));
		*next(mod.begin(), 81) = (Model(*next(mod.begin(), 80)));//81
		GameEmGine::addModel(&*next(mod.begin(), 81));
		*next(mod.begin(), 82) = (Model(*next(mod.begin(), 81)));//82
		GameEmGine::addModel(&*next(mod.begin(), 82));
		*next(mod.begin(), 83) = (Model(*next(mod.begin(), 82)));//83
		GameEmGine::addModel(&*next(mod.begin(), 83));
		*next(mod.begin(), 84) = (Model(*next(mod.begin(), 83)));//84
		GameEmGine::addModel(&*next(mod.begin(), 84));
		*next(mod.begin(), 85) = (Model("Models/Train/Head/trainhead.obj"));//85
		GameEmGine::addModel(&*next(mod.begin(), 85));
		(&*next(mod.begin(), 79))->translate(-14.45f, 0.3f, 8.0f);
		(&*next(mod.begin(), 80))->translate(-9.2f, 0.3f, 8.0f);
		(&*next(mod.begin(), 81))->translate(-4.6f, 0.3f, 8.0f);
		(&*next(mod.begin(), 82))->translate(0.0f, 0.3f, 8.0f);
		(&*next(mod.begin(), 83))->translate(4.6f, 0.3f, 8.0f);
		(&*next(mod.begin(), 84))->translate(9.2f, 0.3f, 8.0f);
		(&*next(mod.begin(), 85))->translate(14.45f, 0.3f, 8.0f), (&*next(mod.begin(), 85))->rotate(Coord3D<>(0, 180, 0));

		//Rail
		*next(mod.begin(), 86) = (Model("Models/Rail/rail.obj"));//86
		GameEmGine::addModel(&*next(mod.begin(), 86));
		*next(mod.begin(), 87) = (Model(*next(mod.begin(), 86)));//87
		GameEmGine::addModel(&*next(mod.begin(), 87));
		*next(mod.begin(), 88) = (Model(*next(mod.begin(), 87)));//88
		GameEmGine::addModel(&*next(mod.begin(), 88));
		*next(mod.begin(), 89) = (Model(*next(mod.begin(), 88)));//89
		GameEmGine::addModel(&*next(mod.begin(), 89));
		*next(mod.begin(), 90) = (Model(*next(mod.begin(), 89)));//90
		GameEmGine::addModel(&*next(mod.begin(), 90));
		*next(mod.begin(), 91) = (Model(*next(mod.begin(), 90)));//91
		GameEmGine::addModel(&*next(mod.begin(), 91));
		*next(mod.begin(), 92) = (Model(*next(mod.begin(), 91)));//92
		GameEmGine::addModel(&*next(mod.begin(), 92));

		(&*next(mod.begin(), 86))->scale(0.7f), (&*next(mod.begin(), 86))->translate(-18.0f, 0.0f, 8.0f), (&*next(mod.begin(), 86))->rotate(Coord3D<>(0, 90, 0));
		(&*next(mod.begin(), 87))->scale(0.7f), (&*next(mod.begin(), 87))->translate(-12.0f, 0.0f, 8.0f), (&*next(mod.begin(), 87))->rotate(Coord3D<>(0, 90, 0));
		(&*next(mod.begin(), 88))->scale(0.7f), (&*next(mod.begin(), 88))->translate(-6.0f, 0.0f, 8.0f), (&*next(mod.begin(), 88))->rotate(Coord3D<>(0, 90, 0));
		(&*next(mod.begin(), 89))->scale(0.7f), (&*next(mod.begin(), 89))->translate(0.0f, 0.0f, 8.0f), (&*next(mod.begin(), 89))->rotate(Coord3D<>(0, 90, 0));
		(&*next(mod.begin(), 90))->scale(0.7f), (&*next(mod.begin(), 90))->translate(6.0f, 0.0f, 8.0f), (&*next(mod.begin(), 90))->rotate(Coord3D<>(0, 90, 0));
		(&*next(mod.begin(), 91))->scale(0.7f), (&*next(mod.begin(), 91))->translate(12.0f, 0.0f, 8.0f), (&*next(mod.begin(), 91))->rotate(Coord3D<>(0, 90, 0));
		(&*next(mod.begin(), 92))->scale(0.7f), (&*next(mod.begin(), 92))->translate(18.0f, 0.0f, 8.0f), (&*next(mod.begin(), 92))->rotate(Coord3D<>(0, 90, 0));


		//Escape pods
		*next(mod.begin(), 94) = (Model("Models/TrainGrayBox.obj"));//94
		GameEmGine::addModel(&*next(mod.begin(), 94));
		*next(mod.begin(), 95) = (Model(*next(mod.begin(), 94)));//95
		GameEmGine::addModel(&*next(mod.begin(), 95));
		*next(mod.begin(), 96) = (Model(*next(mod.begin(), 94)));//96
		GameEmGine::addModel(&*next(mod.begin(), 96));
		*next(mod.begin(), 97) = (Model(*next(mod.begin(), 94)));//97
		GameEmGine::addModel(&*next(mod.begin(), 97));

		//yeah we arent using these anytime soon so :) 
		(&*next(mod.begin(), 94))->setToRender(false);
		(&*next(mod.begin(), 95))->setToRender(false);
		(&*next(mod.begin(), 96))->setToRender(false);
		(&*next(mod.begin(), 97))->setToRender(false);

		//RailLight
		*next(mod.begin(), 99) = (Model("Models/Rail/railLight.obj"));//99
		GameEmGine::addModel(&*next(mod.begin(), 99));
		*next(mod.begin(), 100) = (Model(*next(mod.begin(), 99)));//100
		GameEmGine::addModel(&*next(mod.begin(), 100));
		*next(mod.begin(), 101) = (Model(*next(mod.begin(), 100)));//101
		GameEmGine::addModel(&*next(mod.begin(), 101));
		*next(mod.begin(), 102) = (Model(*next(mod.begin(), 101)));//102
		GameEmGine::addModel(&*next(mod.begin(), 102));
		*next(mod.begin(), 103) = (Model(*next(mod.begin(), 102)));//103
		GameEmGine::addModel(&*next(mod.begin(), 103));
		*next(mod.begin(), 104) = (Model(*next(mod.begin(), 103)));//104
		GameEmGine::addModel(&*next(mod.begin(), 104));
		*next(mod.begin(), 105) = (Model(*next(mod.begin(), 104)));//105
		GameEmGine::addModel(&*next(mod.begin(), 105));

		////Background
		//*next(mod.begin(), 106) = (Model("Models/BackgroundSky/sky.obj"));//106
		//GameEmGine::addModel(&*next(mod.begin(), 106));
		//(&*next(mod.begin(), 106))->scale(8.0f, 8.0f, 5.0f), (&*next(mod.begin(), 106))->translate(1.0f, 4.0f, 40.0f), (&*next(mod.begin(), 106))->rotate({90.0f,0.0f,0.0f});

		//Add more buildings in the back
		*next(mod.begin(), 107) = (Model("Models/Buildings/Building7/PharmacureBuilding.obj"));//107
		GameEmGine::addModel(&*next(mod.begin(), 107));

		(&*next(mod.begin(), 107))->scale(1.5f, 1.5f, 1.0f), (&*next(mod.begin(), 107))->translate(2.5f, 0.0f, 30.0f);

		*next(mod.begin(), 108) = (Model("Models/Buildings/Building1/building1.obj"));//108
		GameEmGine::addModel(&*next(mod.begin(), 108));
		*next(mod.begin(), 109) = (Model(*next(mod.begin(), 108)));//109
		GameEmGine::addModel(&*next(mod.begin(), 109));

		(&*next(mod.begin(), 108))->scale(2.0f, 3.5f, 2.5f), (&*next(mod.begin(), 108))->translate(-6.0f, 0.0f, 37.0f), (&*next(mod.begin(), 108))->rotate({0.0f, -90.0f, 0.0f});
		(&*next(mod.begin(), 109))->scale(2.0f, 3.5f, 2.5f), (&*next(mod.begin(), 109))->translate(25.2f, 0.0f, 18.0f), (&*next(mod.begin(), 109))->rotate({0.0f, 180.0f, 0.0f});

		*next(mod.begin(), 110) = (Model("Models/Buildings/Building2/building2.obj"));//110
		GameEmGine::addModel(&*next(mod.begin(), 110));
		*next(mod.begin(), 111) = (Model(*next(mod.begin(), 110)));//111
		GameEmGine::addModel(&*next(mod.begin(), 111));

		(&*next(mod.begin(), 110))->scale(2.0f, 3.5f, 2.5f), (&*next(mod.begin(), 110))->translate(-22.0f, 0.0f, 15.0f), (&*next(mod.begin(), 110))->rotate({0.0f, 0.0f, 0.0f});
		(&*next(mod.begin(), 111))->scale(1.0f, 3.5f, 2.5f), (&*next(mod.begin(), 111))->translate(5.0f, 0.0f, 37.0f), (&*next(mod.begin(), 111))->rotate({0.0f, -90.0f, 0.0f});

		*next(mod.begin(), 112) = (Model("Models/Buildings/Building8/Pharmacure_Model.obj"));//112
		GameEmGine::addModel(&*next(mod.begin(), 112));
		*next(mod.begin(), 113) = (Model(*next(mod.begin(), 112)));//113
		GameEmGine::addModel(&*next(mod.begin(), 113));

		(&*next(mod.begin(), 112))->scale(1.0f, 1.0f, 1.0f), (&*next(mod.begin(), 112))->translate(17.0f, 0.0f, 22.0f), (&*next(mod.begin(), 112))->rotate({0.0f, -90.0f, 0.0f});
		(&*next(mod.begin(), 113))->scale(2.0f, 2.0f, 2.0f), (&*next(mod.begin(), 113))->translate(-25.0f, 0.0f, 25.0f), (&*next(mod.begin(), 113))->rotate({0.0f, 90.0f, 0.0f});

		*next(mod.begin(), 114) = (Model("Models/Buildings/Building3/House.obj"));//114
		GameEmGine::addModel(&*next(mod.begin(), 114));

		(&*next(mod.begin(), 114))->scale(2.0f, 2.0f, 2.0f), (&*next(mod.begin(), 114))->translate(25.0f, 0.0f, 10.0f), (&*next(mod.begin(), 114))->rotate({0.0f, 90.0f, -90.0f});

		*next(mod.begin(), 115) = (Model("Models/Buildings/Building9/cyber1.obj"));//115
		GameEmGine::addModel(&*next(mod.begin(), 115));
		*next(mod.begin(), 116) = (Model(*next(mod.begin(), 115)));//116
		GameEmGine::addModel(&*next(mod.begin(), 116));

		(&*next(mod.begin(), 115))->scale(3.0f, 3.0f, 3.0f), (&*next(mod.begin(), 115))->translate(-22.0f, 0.0f, 35.0f), (&*next(mod.begin(), 115))->rotate({0.0f, 45.0f, 0.0f});
		(&*next(mod.begin(), 116))->scale(3.0f, 3.0f, 3.0f), (&*next(mod.begin(), 116))->translate(13.5f, 0.0f, 35.0f), (&*next(mod.begin(), 116))->rotate({0.0f, 0.0f, 0.0f});

		*next(mod.begin(), 117) = (Model("Models/Buildings/Building10/cyber2.obj"));//117
		GameEmGine::addModel(&*next(mod.begin(), 117));
		*next(mod.begin(), 118) = (Model(*next(mod.begin(), 117)));//118
		GameEmGine::addModel(&*next(mod.begin(), 118));

		(&*next(mod.begin(), 117))->scale(3.0f, 3.0f, 3.0f), (&*next(mod.begin(), 117))->translate(21.0f, 0.0f, 27.0f), (&*next(mod.begin(), 117))->rotate({0.0f, 90.0f, 0.0f});
		(&*next(mod.begin(), 118))->scale(3.0f, 3.0f, 3.0f), (&*next(mod.begin(), 118))->translate(-0.5f, 2.0f, 36.0f), (&*next(mod.begin(), 118))->rotate({0.0f, 90.0f, 0.0f});

		*next(mod.begin(), 119) = (Model("Models/Buildings/Building7/PharmacureBuilding.obj"));//119
		GameEmGine::addModel(&*next(mod.begin(), 119));
		(&*next(mod.begin(), 119))->scale(1.0f, 3.0f, 1.0f), (&*next(mod.begin(), 119))->translate(17.f, 0.0f, 30.0f), (&*next(mod.begin(), 119))->rotate({0.0f, -90.0f, 0.0f});

		*next(mod.begin(), 120) = (Model("Models/Buildings/Building11/cyber3.obj"));//120
		GameEmGine::addModel(&*next(mod.begin(), 120));
		*next(mod.begin(), 121) = (Model(*next(mod.begin(), 120)));//121
		GameEmGine::addModel(&*next(mod.begin(), 121));
		(&*next(mod.begin(), 120))->scale(2.0f, 3.0f, 3.0f), (&*next(mod.begin(), 120))->translate(-17.f, -5.0f, 24.0f), (&*next(mod.begin(), 120))->rotate({0.0f, 0.0f, 0.0f});
		(&*next(mod.begin(), 121))->scale(2.0f, 2.0f, 2.0f), (&*next(mod.begin(), 121))->translate(-4.2f, -5.0f, 29.7f), (&*next(mod.begin(), 121))->rotate({0.0f, -90.0f, 0.0f});

		*next(mod.begin(), 122) = (Model("Models/Buildings/Building5/smallShop.obj"));//122
		GameEmGine::addModel(&*next(mod.begin(), 122));
		(&*next(mod.begin(), 122))->scale(1.2f, 1.2f, 1.2f), (&*next(mod.begin(), 122))->translate(-8.0f, 0.0f, 27.0f), (&*next(mod.begin(), 122))->rotate({0.0f, -90.0f, 0.0f});

		//Building 2s
		*next(mod.begin(), 123) = (Model("Models/Buildings/Tunnel/Tunnel_Front_Blue.obj")); //123
		GameEmGine::addModel(&*next(mod.begin(), 123));
		*next(mod.begin(), 124) = (Model(*next(mod.begin(), 123)));
		GameEmGine::addModel(&*next(mod.begin(), 124));//124
		(&*next(mod.begin(), 123))->setColour({0,255,255});
		(&*next(mod.begin(), 124))->setColour({0,255,255});

		//boss portrait beside its health bar 
		(*next(mod.begin(), 125)) = (Model("Models/BOSS/bossPORTRAIT.obj")); //125
		(&*next(mod.begin(), 125))->translate(10.2f, 18.2f, 19.2f);
		(&*next(mod.begin(), 125))->scale(2.0f, 2.0f, 2.0f);
		(&*next(mod.begin(), 125))->rotate({0, 0, 0});
		(&*next(mod.begin(), 125))->setTransparent(true);
		GameEmGine::addModel(&*next(mod.begin(), 125));

		//pause screen
		(*next(mod.begin(), 126)) = (Model("Models/Scene/Pause/pausedScreen.obj")); //125
		(&*next(mod.begin(), 126))->translate(-0.8f, 10.0f, -8.0f);
		(&*next(mod.begin(), 126))->scale(0.25f, 0.45f, 0.25f);
		GameEmGine::addModel(&*next(mod.begin(), 126));
		(&*next(mod.begin(), 126))->setToRender(false);
		(&*next(mod.begin(), 126))->setTransparent(true);

		//Tutorial Sign
		*next(mod.begin(), 127) = (Model("Models/Sign/tallSign/tallSign.obj"));
		GameEmGine::addModel(&*next(mod.begin(), 127));
		(&*next(mod.begin(), 127))->translate(-14.0f, 0.0f, 36.0f), (&*next(mod.begin(), 127))->scale(1.5f, 1.5f, 1.5f);
		(&*next(mod.begin(), 127))->setColour({0,255,255});

		*next(mod.begin(), 128) = (Model("Models/Sign/shortSign/shortSign.obj"));
		GameEmGine::addModel(&*next(mod.begin(), 128));
		(&*next(mod.begin(), 128))->translate(-16.9f, 0.0f, 17.0f), (&*next(mod.begin(), 128))->rotate({0, 90, 0}), (&*next(mod.begin(), 128))->scale(0.8f, 1.0f, 1.0f);
		(&*next(mod.begin(), 128))->setColour({0,255,255});

		*next(mod.begin(), 129) = (Model("Models/Sign/sideSign/sideSign.obj"));
		GameEmGine::addModel(&*next(mod.begin(), 129));
		(&*next(mod.begin(), 129))->scale(1.0f, 3.0f, 1.0f), (&*next(mod.begin(), 129))->translate(16.8f, 0.0f, 29.5f), (&*next(mod.begin(), 129))->rotate({0.0f, -90.0f, 0.0f});

		(*next(mod.begin(), 130)) = (Model("Models/Scene/GameOver/gameOver.obj")); //125
		(&*next(mod.begin(), 130))->translate(-0.8f, 10.0f, -8.0f);
		(&*next(mod.begin(), 130))->scale(0.25f, 0.45f, 0.25f);
		GameEmGine::addModel(&*next(mod.begin(), 130));
		(&*next(mod.begin(), 130))->setToRender(false);
		(&*next(mod.begin(), 130))->setTransparent(true);

		(*next(mod.begin(), 131)) = (Model("Models/Scene/Win/win.obj")); //125
		(&*next(mod.begin(), 131))->translate(-0.8f, 10.0f, -8.0f);
		(&*next(mod.begin(), 131))->scale(0.25f, 0.45f, 0.25f);
		GameEmGine::addModel(&*next(mod.begin(), 131));
		(&*next(mod.begin(), 131))->setToRender(false);
		(&*next(mod.begin(), 131))->setTransparent(true);

		/// - Set Model Transforms - ///
		//Player Transforms
		(&*next(mod.begin(), 0))->scale(1.2f, 1.4f, 1.2f), (&*next(mod.begin(), 0))->translate(1.0f, 0.0f, -5.0f);
		(&*next(mod.begin(), 1))->scale(1.2f, 1.4f, 1.2f), (&*next(mod.begin(), 1))->translate(-1.0f, 0.0f, -5.0f);
		(&*next(mod.begin(), 2))->scale(1.2f, 1.4f, 1.2f), (&*next(mod.begin(), 2))->translate(2.0f, 0.0f, -5.0f);
		(&*next(mod.begin(), 3))->scale(1.2f, 1.4f, 1.2f), (&*next(mod.begin(), 3))->translate(-2.0f, 0.0f, -5.0f);
		(&*next(mod.begin(), 0))->rotate(Coord3D<>(0, 180, 0));
		(&*next(mod.begin(), 1))->rotate(Coord3D<>(0, 180, 0));
		(&*next(mod.begin(), 2))->rotate(Coord3D<>(0, 180, 0));
		(&*next(mod.begin(), 3))->rotate(Coord3D<>(0, 180, 0));

		(&*next(mod.begin(), 19))->addChild(&*next(mod.begin(), 123));
		(&*next(mod.begin(), 20))->addChild(&*next(mod.begin(), 124));

		//Building Transforms
		//Building 1s
		(&*next(mod.begin(), 4))->scale(1), (&*next(mod.begin(), 4))->translate(-15.175f, 0.0f, -2.0f), (&*next(mod.begin(), 4))->rotate({0.0f,90.0f,0.0f});;
		(&*next(mod.begin(), 5))->scale(1), (&*next(mod.begin(), 5))->translate(6.0f, 0.0f, 29.0f), (&*next(mod.begin(), 5))->rotate({0.0f,-90.0f,0.0f});
		//&*next(mod.begin(),6)->scale(2), &*next(mod.begin(),6)->translate(-4.0f, 0.0f, 22.75f), &*next(mod.begin(),6)->rotate({0.0f,-90.0f,0.0f});

		//Building 2s
		(&*next(mod.begin(), 19))->scale(0.85f), (&*next(mod.begin(), 19))->translate(-18.0f, 0.0f, 6.4f), (&*next(mod.begin(), 19))->rotate({0.0f, 90.0f,0.0f}); //left 
		(&*next(mod.begin(), 20))->scale(0.85f), (&*next(mod.begin(), 20))->translate(18.0f, 0.0f, 9.5f), (&*next(mod.begin(), 20))->rotate({0.0f, -90.0f, 0.0f}); //right 

		//Buildings 3s
		(&*next(mod.begin(), 30))->translate(10.5f, 0.0f, 23.6f);
		(&*next(mod.begin(), 31))->translate(19.5f, 0.0f, 3.75f), (&*next(mod.begin(), 31))->rotate({0,180,0});
		(&*next(mod.begin(), 32))->translate(-12.0f, 0.0f, 25.35f), (&*next(mod.begin(), 32))->rotate({0,-90,0});

		//Building 4s //Lillian's building, moved back
		(&*next(mod.begin(), 33))->translate(27.0f, 0.0f, 26.0f), (&*next(mod.begin(), 33))->rotate({0,45,0}); //right
		(&*next(mod.begin(), 34))->translate(-14.0f, 0.0f, 36.0f), (&*next(mod.begin(), 34))->scale(1.5f, 1.5f, 1.5f), (&*next(mod.begin(), 34))->rotate({0,180,0}); //left
			//Building 5s
		(&*next(mod.begin(), 39))->scale(1.0f, 1.0f, 1.05f), (&*next(mod.begin(), 39))->translate(19.6f, 0.0f, 16.5f), (&*next(mod.begin(), 39))->rotate({0,180,0});
		(&*next(mod.begin(), 40))->scale(1.25f, 1.0f, 1.0f), (&*next(mod.begin(), 40))->translate(-16.9f, 0.0f, 16.35f), (&*next(mod.begin(), 40))->rotate({0,90,0});

		//Project Nebula Sign Transforms
		(&*next(mod.begin(), 7))->scale(3), (&*next(mod.begin(), 7))->translate(9.5f, 5.34f, 22.5f);

		//Boss Trarrnsforms
		(&*next(mod.begin(), 8))->scale(2.0f), (&*next(mod.begin(), 8))->translate(0.0f, 0.0f, 23.0f), (&*next(mod.begin(), 8))->rotate({0.0f, 0.0f, 0.0f});

		//Floor Transforms
		(&*next(mod.begin(), 9))->scale(2.25f, 1.0f, 5.0f), (&*next(mod.begin(), 9))->translate(0.0f, 0.0f, 5.0f);

		//Street Light Transforms
		(&*next(mod.begin(), 10))->scale(0.5f, 0.8f, 0.5f), (&*next(mod.begin(), 10))->translate(13.0f, 0.0f, -1.0f);
		(&*next(mod.begin(), 11))->scale(0.5f, 0.8f, 0.5f), (&*next(mod.begin(), 11))->translate(13.0f, 0.0f, 6.0f);
		(&*next(mod.begin(), 12))->scale(0.5f, 0.8f, 0.5f), (&*next(mod.begin(), 12))->translate(13.0f, 0.0f, 15.0f);
		(&*next(mod.begin(), 13))->scale(0.5f, 0.8f, 0.5f), (&*next(mod.begin(), 13))->translate(-13.0f, 0.0f, -1.0f), (&*next(mod.begin(), 13))->rotate({0.0f,180.0f,0.0f});
		(&*next(mod.begin(), 14))->scale(0.5f, 0.8f, 0.5f), (&*next(mod.begin(), 14))->translate(-13.0f, 0.0f, 6.0f), (&*next(mod.begin(), 14))->rotate({0.0f,180.0f,0.0f});
		(&*next(mod.begin(), 15))->scale(0.5f, 0.8f, 0.5f), (&*next(mod.begin(), 15))->translate(-13.0f, 0.0f, 15.0f), (&*next(mod.begin(), 15))->rotate({0.0f,180.0f,0.0f});

		//Bench Transforms
		(&*next(mod.begin(), 16))->translate(-13.0f, 0.0f, 3.0f);
		(&*next(mod.begin(), 17))->translate(13.0f, 0.0f, 3.0f), (&*next(mod.begin(), 17))->rotate({0.0f,180.0f,0.0f});

		//Planet Transforms
		(&*next(mod.begin(), 18))->translate(9.0f, 17.0f, 36.0f);
		//&*next(mod.begin(),58)->translate(-10.0f, 11.0f, 25.0f);

		//Trees
		(&*next(mod.begin(), 35))->scale(0.3f), (&*next(mod.begin(), 35))->translate(13.0f, 0.0f, -3.0f), (&*next(mod.begin(), 35))->rotate({0,-0,0});
		(&*next(mod.begin(), 36))->scale(0.3f), (&*next(mod.begin(), 36))->translate(-13.0f, 0.0f, -3.0f), (&*next(mod.begin(), 36))->rotate({0,-0,0});
		(&*next(mod.begin(), 37))->scale(0.3f), (&*next(mod.begin(), 37))->translate(13.0f, 0.0f, 11.0f), (&*next(mod.begin(), 37))->rotate({0,-0,0});
		(&*next(mod.begin(), 38))->scale(0.3f), (&*next(mod.begin(), 38))->translate(-13.0f, 0.0f, 11.0f), (&*next(mod.begin(), 38))->rotate({0,-0,0});

		//Pizza Sign
		(&*next(mod.begin(), 53))->scale(1.5f), (&*next(mod.begin(), 53))->translate(-13.0f, 5.4f, 22.3f);

		//Train
		(&*next(mod.begin(), 79))->translate(-14.45f, 0.3f, 8.0f);
		(&*next(mod.begin(), 80))->translate(-9.2f, 0.3f, 8.0f);
		(&*next(mod.begin(), 81))->translate(-4.6f, 0.3f, 8.0f);
		(&*next(mod.begin(), 82))->translate(0.0f, 0.3f, 8.0f);
		(&*next(mod.begin(), 83))->translate(4.6f, 0.3f, 8.0f);
		(&*next(mod.begin(), 84))->translate(9.2f, 0.3f, 8.0f);
		(&*next(mod.begin(), 85))->translate(14.45f, 0.3f, 8.0f), (&*next(mod.begin(), 85))->rotate(Coord3D<>(0, 180, 0));

		//Rail
		(&*next(mod.begin(), 86))->scale(0.7f), (&*next(mod.begin(), 86))->translate(-18.0f, 0.0f, 8.0f), (&*next(mod.begin(), 86))->rotate(Coord3D<>(0, 90, 0));
		(&*next(mod.begin(), 87))->scale(0.7f), (&*next(mod.begin(), 87))->translate(-12.0f, 0.0f, 8.0f), (&*next(mod.begin(), 87))->rotate(Coord3D<>(0, 90, 0));
		(&*next(mod.begin(), 88))->scale(0.7f), (&*next(mod.begin(), 88))->translate(-6.0f, 0.0f, 8.0f), (&*next(mod.begin(), 88))->rotate(Coord3D<>(0, 90, 0));
		(&*next(mod.begin(), 89))->scale(0.7f), (&*next(mod.begin(), 89))->translate(0.0f, 0.0f, 8.0f), (&*next(mod.begin(), 89))->rotate(Coord3D<>(0, 90, 0));
		(&*next(mod.begin(), 90))->scale(0.7f), (&*next(mod.begin(), 90))->translate(6.0f, 0.0f, 8.0f), (&*next(mod.begin(), 90))->rotate(Coord3D<>(0, 90, 0));
		(&*next(mod.begin(), 91))->scale(0.7f), (&*next(mod.begin(), 91))->translate(12.0f, 0.0f, 8.0f), (&*next(mod.begin(), 91))->rotate(Coord3D<>(0, 90, 0));
		(&*next(mod.begin(), 92))->scale(0.7f), (&*next(mod.begin(), 92))->translate(18.0f, 0.0f, 8.0f), (&*next(mod.begin(), 92))->rotate(Coord3D<>(0, 90, 0));

		//RialLight
		(&*next(mod.begin(), 99))->scale(0.7f), (&*next(mod.begin(), 99))->translate(-18.0f, 0.03f, 8.0f), (&*next(mod.begin(), 99))->rotate(Coord3D<>(0, 90, 0));
		(&*next(mod.begin(), 100))->scale(0.7f), (&*next(mod.begin(), 100))->translate(-12.0f, 0.03f, 8.0f), (&*next(mod.begin(), 100))->rotate(Coord3D<>(0, 90, 0));
		(&*next(mod.begin(), 101))->scale(0.7f), (&*next(mod.begin(), 101))->translate(-6.0f, 0.03f, 8.0f), (&*next(mod.begin(), 101))->rotate(Coord3D<>(0, 90, 0));
		(&*next(mod.begin(), 102))->scale(0.7f), (&*next(mod.begin(), 102))->translate(0.0f, 0.03f, 8.0f), (&*next(mod.begin(), 102))->rotate(Coord3D<>(0, 90, 0));
		(&*next(mod.begin(), 103))->scale(0.7f), (&*next(mod.begin(), 103))->translate(6.0f, 0.03f, 8.0f), (&*next(mod.begin(), 103))->rotate(Coord3D<>(0, 90, 0));
		(&*next(mod.begin(), 104))->scale(0.7f), (&*next(mod.begin(), 104))->translate(12.0f, 0.03f, 8.0f), (&*next(mod.begin(), 104))->rotate(Coord3D<>(0, 90, 0));
		(&*next(mod.begin(), 105))->scale(0.7f), (&*next(mod.begin(), 105))->translate(18.0f, 0.03f, 8.0f), (&*next(mod.begin(), 105))->rotate(Coord3D<>(0, 90, 0));
#pragma endregion

#else

		//	std::copy(mod.begin(), next(mod.begin(), numPlayers), players);
			////	players.resize(numPlayers);
		for(int a = numPlayers; a > 0; --a)
			players.push_back(*(Player*)(&*next(mod.begin(), numPlayers - a)));



		SceneManager::loadScene("gamescene.scene", mod);

		Minion::clearTargets();
		for(int a = numPlayers; a > 0; --a)
			Minion::addTarget((Player*)&*next(mod.begin(), numPlayers - a)),
			* next(mod.begin(), numPlayers - a) = (*next(players.begin(), numPlayers - a));

		//todo: delete fake players here

		//re-add all models (just in case)
		GameEmGine::clearObjectList();
		for(auto& a : mod)
			GameEmGine::addModel(&a);

		//re-initializing Boss (constructor not called otherwise)
		Boss tmpBoss = Boss();//8
		tmpBoss = *next(mod.begin(), 8);
		*next(mod.begin(), 8) = tmpBoss;

#endif // FALSE

		//apply uniform transform
		for(auto& a : mod)
			uniScaler.addChild(&a);

		//lights.resize(14);
		for(int a = 0; a < 6; a++)
		{
			lights.push_back(Light());
			//&*next(mod.begin(),10 + a)->boundingBoxUpdate();
			(*next(lights.begin(), a)).setLightType(Light::POINT);
			(*next(lights.begin(), a)).setParent(&*next(mod.begin(), 10 + a));
			(*next(lights.begin(), a)).translate({-0.5f,4.5,0.0f});
			//(&*next(lights.begin(),a).setDiffuse({ 255,100,0,100 }, 6);
			//(&*next(lights.begin(),a).setAttenuationQuadratic(0.06f, 6);
		}

		for(int a = 0; a < 4; a++)
		{
			lights.push_back(Light());
			(*next(lights.begin(), a + 6)).setLightType(Light::POINT);
			(*next(lights.begin(), a + 6)).setParent(&*next(mod.begin(), a));
			(*next(lights.begin(), a + 6)).translate({0, -0.75f, 0});
			(*next(lights.begin(), a + 6)).setAttenuationQuadratic(1.f);
		}

		(*next(lights.begin(), 6)).setDiffuse({255, 0, 0});
		(*next(lights.begin(), 7)).setDiffuse({0, 0, 255});
		(*next(lights.begin(), 8)).setDiffuse({0, 255, 0});
		(*next(lights.begin(), 9)).setDiffuse({255,255,0});




		//(&*next(lights.begin(),a).setSceneAmbient({255,255,255,255});

		/// - Set Camera  - ///
		//*next(mod.begin(),130)->translate(-0.8f, 10.0f, -8.0f);
		GameEmGine::getMainCamera()->translate({0,15.5f,-17.5});
		GameEmGine::getMainCamera()->rotate({-25,0,0});

		/// key/mouse input ///
		keyPressed = [&](int a, int b) { keyInputPressed(a, b); };
		keyReleased = [&](int a, int b) { keyInputReleased(a, b); };
		mouseReleased = [&](int a, int b) { mouseButtonReleased(a, b); };

		AudioPlayer::init();

		//audio.createAudioStream("Audio/potential mix (with beat).wav", "BG Music");

		//audio.setVolume(0.6f);
		//audio.play(true);
		LightManager::enableShadows(true);
		auto light = new Light();
		light->setLightType(Light::TYPE::DIRECTIONAL);
		light->rotate(-45, 0, 0);
	//	light->setDiffuse({110,110,110});
		light->enableAmbiant(false);
		LightManager::addLight(light);

		light = new Light();
		light->setLightType(Light::TYPE::POINT);
		LightManager::addLight(light);

		std::next(mod.begin(), 9)->setCastShadow(false);

	}

	void insertionSort(list<Minion*>& arr, Model* checker)
	{
		int i, j;
		float key;
		for(i = 1; i < (int)arr.size(); i++)
		{
			key = Coord3D<>::distance((*next(arr.begin(), i))->getLocalPosition(), checker->getLocalPosition());
			Minion* tmp = (*next(arr.begin(), i));
			j = i - 1;

			/*
				Move elements of (*next(arr.begin(),0..i-1], that are
				greater than key, to one position ahead
				of their current position
			*/
			while(j >= 0 && Coord3D<>::distance((*next(arr.begin(), j))->getLocalPosition(), checker->getLocalPosition()) > key)
			{
				(*next(arr.begin(), j + 1)) = (*next(arr.begin(), j));
				j = j - 1;
			}
			(*next(arr.begin(), j + 1)) = tmp;
		}
	}

	/// - The Update Loop - ///

	void update(double dt)
	{
		switch(currentState)
		{
		case CONTROL_STATE::CAMERA:
		case CONTROL_STATE::LEVEL:
			objectMovement(dt);
			break;
		case CONTROL_STATE::GAME:
			//	gameUpdate(dt);
			break;
		}
	}



	void objectMovement(double dt)
	{
		float moveSpd = 15 * (float)dt;
		moveSpd;
		if(m_left)
			switch(currentState)
			{
			case CONTROL_STATE::GAME:
				break;
			case CONTROL_STATE::LEVEL:
				GameEmGine::getMainCamera()->enableFPSMode(false); if(!curModel)return;
				curModel->translateBy(-moveSpd, 0, 0);
				break;
			case CONTROL_STATE::CAMERA:
				GameEmGine::getMainCamera()->enableFPSMode();
				GameEmGine::getMainCamera()->translateBy(-moveSpd, 0, 0);
				break;
			}
		if(m_right)
			switch(currentState)
			{
			case CONTROL_STATE::GAME:
				break;
			case CONTROL_STATE::LEVEL:
				GameEmGine::getMainCamera()->enableFPSMode(false); if(!curModel)return;
				curModel->translateBy(moveSpd, 0, 0);
				break;
			case CONTROL_STATE::CAMERA:
				GameEmGine::getMainCamera()->enableFPSMode();
				GameEmGine::getMainCamera()->translateBy(moveSpd, 0, 0);
				break;
			}
		if(m_fwd)
			switch(currentState)
			{
			case CONTROL_STATE::GAME:
				break;
			case CONTROL_STATE::LEVEL:
				GameEmGine::getMainCamera()->enableFPSMode(false); if(!curModel)return;
				curModel->translateBy(0, 0, moveSpd);
				break;
			case CONTROL_STATE::CAMERA:
				GameEmGine::getMainCamera()->enableFPSMode();
				GameEmGine::getMainCamera()->translateBy(0, 0, moveSpd);
				break;
			}
		if(m_back)
			switch(currentState)
			{
			case CONTROL_STATE::GAME:
				break;
			case CONTROL_STATE::LEVEL:
				GameEmGine::getMainCamera()->enableFPSMode(false); if(!curModel)return;
				curModel->translateBy(0, 0, -moveSpd);
				break;
			case CONTROL_STATE::CAMERA:
				GameEmGine::getMainCamera()->enableFPSMode();
				GameEmGine::getMainCamera()->translateBy(0, 0, -moveSpd);
				break;
			}
		if(m_up)
			switch(currentState)
			{
			case CONTROL_STATE::GAME:
				break;
			case CONTROL_STATE::LEVEL:
				GameEmGine::getMainCamera()->enableFPSMode(false); if(!curModel)return;
				curModel->translateBy(0, moveSpd, 0);
				break;
			case CONTROL_STATE::CAMERA:
				GameEmGine::getMainCamera()->enableFPSMode();
				GameEmGine::getMainCamera()->translateBy(0, moveSpd, 0);
				break;
			}
		if(m_down)
			switch(currentState)
			{
			case CONTROL_STATE::GAME:
				break;
			case CONTROL_STATE::LEVEL:
				GameEmGine::getMainCamera()->enableFPSMode(false); if(!curModel)return;
				curModel->translateBy(0, -moveSpd, 0);
				break;
			case CONTROL_STATE::CAMERA:
				GameEmGine::getMainCamera()->enableFPSMode();
				GameEmGine::getMainCamera()->translateBy(0, -moveSpd, 0);
				break;
			}

		if(rotLeft)
			switch(currentState)
			{
			case CONTROL_STATE::GAME:
				break;
			case CONTROL_STATE::LEVEL:
				GameEmGine::getMainCamera()->enableFPSMode(false); if(!curModel)return;
				curModel->rotateBy(0, -moveSpd, 0);
				break;
			case CONTROL_STATE::CAMERA:
				GameEmGine::getMainCamera()->enableFPSMode();
				GameEmGine::getMainCamera()->rotateBy(0, -moveSpd, 0);
				break;
			}
		if(rotRight)
			switch(currentState)
			{
			case CONTROL_STATE::GAME:
				break;
			case CONTROL_STATE::LEVEL:
				GameEmGine::getMainCamera()->enableFPSMode(false); if(!curModel)return;
				curModel->rotateBy(0, moveSpd, 0);
				break;
			case CONTROL_STATE::CAMERA:
				GameEmGine::getMainCamera()->enableFPSMode();
				GameEmGine::getMainCamera()->rotateBy(0, moveSpd, 0);
				break;
			}
		if(rotUp)
			switch(currentState)
			{
			case CONTROL_STATE::GAME:
				break;
			case CONTROL_STATE::LEVEL:
				//this is correct
				GameEmGine::getMainCamera()->enableFPSMode(false); if(!curModel)return;
				curModel->rotateBy(moveSpd, 0, 0);
				break;
			case CONTROL_STATE::CAMERA:
				//this is correct
				GameEmGine::getMainCamera()->enableFPSMode();
				GameEmGine::getMainCamera()->rotateBy(moveSpd, 0, 0);
				break;
			}
		if(rotDown)
			switch(currentState)
			{
			case CONTROL_STATE::GAME:
				break;
			case CONTROL_STATE::LEVEL:
				GameEmGine::getMainCamera()->enableFPSMode(false); if(!curModel)return;
				curModel->rotateBy(-moveSpd, 0, 0);
				break;
			case CONTROL_STATE::CAMERA:
				GameEmGine::getMainCamera()->enableFPSMode();
				GameEmGine::getMainCamera()->rotateBy(-moveSpd, 0, 0);
				break;
			}

	}

	void gameUpdate(double dt)
	{
		//(*next(lights.begin(),a].setParent(((Boss*)&(*next(mod.begin(),8]).getMissials()[0], 10);
		//(*next(lights.begin(),a].setParent(((Boss*)&(*next(mod.begin(),8]).getMissials()[1], 11);
		//(*next(lights.begin(),a].setParent(((Boss*)&(*next(mod.begin(),8]).getMissials()[2], 12);
		//(*next(lights.begin(),a].setParent(((Boss*)&(*next(mod.begin(),8]).getMissials()[3], 13);


		time += (float)dt; //Add Delta Time to Time

		float move = .1f;
		Player* player;

		// Boss Variables
		Boss* CandyMan = (Boss*)&(*next(mod.begin(), 8)); //Set model 8 as Boss called "CandyMan"


		Model* count;


		///~ countdown stuff ~///
		if(countdown)
		{
			CandyMan->setActive(false);
			for(int a = 0; a < 4; a++)
			{
				player = (Player*)&(*next(mod.begin(), a));
				player->setActive(false);
			}

			count = &nums[countdownCounter];

			if(!m_init)
			{
				countdownTimer = time;
				count->rotate({-25, 0, 0});
				count->setTransparent(true);
				GameEmGine::addModel(count);
				m_init = true;

			}

			count->translate(lerp(GameEmGine::getMainCamera()->getLocalPosition() + Coord3D<>{0, -5, 7}, GameEmGine::getMainCamera()->getLocalPosition() - Coord3D<>{0, 0, 0}, (time - countdownTimer) / 3.5f));

			if(int((time - countdownTimer) / 2))
			{
				GameEmGine::removeModel(count);
				m_init = false;
				countdownCounter++;
				if(countdownCounter == 3)
				{
					countdown = false;
					CandyMan->setActive(true);
					for(int a = 0; a < 4; a++)
					{
						player = (Player*)&(*next(mod.begin(), a));
						player->setActive(true);
					}
				}
			}
		}


		//static list<Player*> players;
		//if(players.size() != 4)
		//	for(int a = 0; a < 4; ++a)
		//		players.push_back((Player*)&(*next(mod.begin(), a)));

		CandyMan->setPlayers(players);
		CandyMan->update((float)dt);

		//add mod for pause screen here but set render to false 
		for(int a = 0; a < 4; a++)
		{

			player = (Player*)&(*next(mod.begin(), a));
			player->setPlayerIndex(a);

			static bool pausedAgain[4] = {0,0,0,0};

			if(GameEmGine::isControllerConnected(a))
			{
				/// - Game Over - ///
				if(gameOver)
				{
					//pausedAgain[a] = true;
					for(int b = 0; b < 4; b++)
						(*(Player*)&(*next(mod.begin(), b))).setActive(pause);

					//rn the music gets quieter during the pause screen 
					if(!pause)
						EmGineAudioPlayer::setVolume(.3f, 0);

					else
						EmGineAudioPlayer::setVolume(0.6f, 0);

					(*next(mod.begin(), 130)).rotate(GameEmGine::getMainCamera()->getLocalRotation() * Coord3D { -1.f, 1.f, 1.f }); //should be parallel to camera hopefully 
					(*next(mod.begin(), 130)).translate(GameEmGine::getMainCamera()->getLocalPosition() + Coord3D{-0.8f, -5.5f, 8.5f}); //should be parallel to camera hopefully 

					(*next(mod.begin(), 130)).setToRender(!pause);
					CandyMan->setActive(pause);

					if(fadeout)
					{
						splashT += 0.01f;
						splashT = splashT > 1 ? 1 : splashT;
						splashAmbient = (GLubyte)lerp(255, 0, splashT);
						(*next(lights.begin(), a)).setAmbient({splashAmbient,splashAmbient,splashAmbient,splashAmbient});
						if(splashAmbient <= 2)
						{
							fadein = true;
							fadeout = false;
							splashT = 0;
							splashAmbient = 255;

							GameEmGine::setScene((getParent()->getParent()->getParent())); // Can't set new scene from game for some reason?
						}
					}
					if(GameEmGine::getController(a)->isButtonStroked(CONTROLLER_A))
					{
						fadeout = true;
					}
					if(GameEmGine::getController(a)->isButtonStroked(CONTROLLER_B))
					{
						GameEmGine::exit();
					}
				}

				/// - GAme Win - ///
				if(gameWin)
				{
					//pausedAgain[a] = true;
					for(int b = 0; b < 4; b++)
						(*(Player*)&(*next(mod.begin(), b))).setActive(pause);

					//rn the music gets quieter during the pause screen 
					if(!pause)
						EmGineAudioPlayer::setVolume(.3f, 0);

					else
						EmGineAudioPlayer::setVolume(0.6f, 0);

					(*next(mod.begin(), 131)).rotate(GameEmGine::getMainCamera()->getLocalRotation()); //should be parallel to camera hopefully 
					(*next(mod.begin(), 131)).setToRender(!pause);
					CandyMan->setActive(pause);

					if(GameEmGine::getController(a)->isButtonStroked(CONTROLLER_A))
					{
						splashT += 0.01f;
						splashT = splashT > 1 ? 1 : splashT;
						splashAmbient = (GLubyte)lerp(255, 0, splashT);
						(*next(lights.begin(), a)).setAmbient({splashAmbient,splashAmbient,splashAmbient,splashAmbient});
						if(splashAmbient <= 5)
						{
							fadein = true;
							fadeout = false;
							splashT = 0;
							splashAmbient = 255;

							//GamePlayInit();
							//GameEmGine::setScene(new Controls); // Can't set new scene from game for some reason?
						}
					}
					if(GameEmGine::getController(a)->isButtonStroked(CONTROLLER_B))
					{
						GameEmGine::exit();
					}
				}

				//music should slow down in the pause menu!!!!
				if(GameEmGine::getController(a)->isButtonPressed(CONTROLLER_START) && !pausedAgain[a])
				{
					pausedAgain[a] = true;
					for(int b = 0; b < 4; b++)
						(*(Player*)&(*next(mod.begin(), b))).setActive(pause);

					//rn the music gets quieter during the pause screen 
					if(!pause)
						EmGineAudioPlayer::setVolume(.3f, 0);
					else
						EmGineAudioPlayer::setVolume(0.6f, 0);

					(*next(mod.begin(), 126)).rotate(GameEmGine::getMainCamera()->getLocalRotation()); //should be parallel to camera hopefully 
					(*next(mod.begin(), 126)).setToRender(!pause);
					CandyMan->setActive(pause);
					//music should slow down in the pause menu!!!!
					pause = !pause;
					screenPause = !screenPause;

				}
				else if(GameEmGine::getController(a)->isButtonReleased(CONTROLLER_START))
				{
					pausedAgain[a] = false;
					//pauseScreen[a] = false; 
					//(*next(mod.begin(),126].setToRender(false);
				}

				if(!player->dead)
					deathCounter = 0;
			}
			else
			{
				(*(Player*)&(*next(mod.begin(), a))).setActive(false);
			}
			player->update((float)dt);

			player->onPlayArea(&(*next(mod.begin(), 59)));

			//bullet collisions with boss
			if(player->bulletCollisions(CandyMan))
			{
				CandyMan->setHealth(CandyMan->getHealth() - 10);

				CandyMan->bossFlash = true;

			}
			if(CandyMan->getHealth() <= 0)
			{
				gameWin = true;
			}

			//bullet collision with minions
			for(auto& minion : CandyMan->minions)
			{
				if(player->bulletCollisions(minion))
					minion->setHealth(minion->getHealth() - 10);
				if(player->collision2D(minion, {0,1,0}))
				{
					player->hitByEnemy(minion, 5);
					player->translateBy(minion->moveTo * 3);
				}
			}

			if(player->type == specialist)
				if(!((Specialist*)player)->pTurrets.empty())
					insertionSort(CandyMan->minions, ((Specialist*)player)->pTurrets[0]);



			for(int b = 0; b < 7; b++)
			{
				player->bulletCollisions(&(*next(mod.begin(), 79 + b)));
				if(player->collision2D(&(*next(mod.begin(), 79 + b)), {0,1,0}))
				{

					player->translateBy(((XinputController*)GameEmGine::getController(a))->getSticks()[LS].x * -move * 1.1f, 0,
						((XinputController*)GameEmGine::getController(a))->getSticks()[LS].y * move *
						player->getLocalPosition().z < (*next(mod.begin(), 79 + b)).getLocalPosition().z ? -1 : 1 * 1.1f); //move player back

				}

			}

			switch(player->type)
			{
			case assault:


				for(int b = 0; b < (int)(*(Assault*)player).pMissiles.size(); b++)
				{
					CandyMan->hitByEnemy((*(Assault*)player).pMissiles[b], 50);
				}
				(*(Assault*)player).missileCollision(CandyMan);

				for(int t = 79; t < 79 + 7; t++)
				{
					(*(Assault*)player).missileCollision(&(*next(mod.begin(), t)));
				}



				break;
			case tank:
				//nothing special needed
				break;
			case medic:
				for(int b = 0; b < 4; b++)
					if((*(Medic*)player).isHealing)
						(*(Medic*)player).getHealing((Player*)&(*next(mod.begin(), b)));
				break;
			case specialist:
				for(auto& b : (*(Specialist*)player).pTurrets)
				{
					b->setMinions(CandyMan->minions);
				}

				if((*(Specialist*)player).hitTurret(CandyMan))
				{
					//do something?
				}

				//bullet collision with minions
				for(auto& minion : CandyMan->minions)
				{
					if((*(Specialist*)player).hitTurret(minion))
					{
						//do something?

					}
				}

				for(auto& missile : CandyMan->getMissials())
					if((*(Specialist*)player).hitTurret(missile.get()))
					{
						//do something?
					}

				break;
			}

		}




		/// - Train Car Movement - ///
		/*
		{
			(*next(mod.begin(), 123)).setColour({255, 0, 0});
			(*next(mod.begin(), 124)).setColour({255, 0, 0});
			bool isMoving = false;

			//Train Sits in middle of map
			if((trainTimer) < 10)
			{
				//do nothing
			}
			//Train warning
			else if(trainTimer < 13)
			{
				//enter code here
			}
			//train leaves
			else if(trainTimer < 20)
			{
				isMoving = true;
				for(int t = 0; t < 7; t++)
				{
					auto train = &(*next(mod.begin(), 79 + t));
					train->translate(
						util::lerp(0.f, 20.f, util::catmull(0.f, 1.f, -.5f, .5f, 1.f - ((30 - trainTimer) / (30 - 20)))),
						train->getLocalPosition().y,
						train->getLocalPosition().z);
				}
			}
			//train stays off screen
			else if(trainTimer < 30)
			{

			}
			//train comes back
			else if(trainTimer < 39)
			{
				isMoving = true;

			}
			else
				trainTimer = 0;



			for(int t = 0; t < 7; t++)
			{
				if(Model::collision2D(&(*next(mod.begin(), 79 + t)), player, {false,true,false}))
				{
					if(isMoving)
						//damage the player and push them back
						for(int a = 0; a < 4; a++)
						{
							player = (Player*)&(*next(mod.begin(), a));
							if(Model::collision2D(&(*next(mod.begin(), 79 + t)), player, {false,true,false}))
							{
								player->setHealth(player->getHealth() - 10);
								player->translate(
									abs(player->getLocalPosition().x) > (*next(mod.begin(), 79 + t)).getWidth() / 2 ? player->getLocalPosition().x < 0 ? -(*next(mod.begin(), 79 + t)).getWidth() / 2 : (*next(mod.begin(), 79 + t)).getWidth() / 2 : player->getLocalPosition().x,
									0,
									abs(player->getLocalPosition().z) >(*next(mod.begin(), 79 + t)).getDepth() / 2 ? player->getLocalPosition().z < 0 ? -(*next(mod.begin(), 79 + t)).getDepth() / 2 : (*next(mod.begin(), 79 + t)).getDepth() / 2 : player->getLocalPosition().z);
							}
						}
					else
						//stop the player from walking through
						player->translate(
							abs(player->getLocalPosition().x) > (*next(mod.begin(), 79 + t)).getWidth() / 2 ? player->getLocalPosition().x < 0 ? -(*next(mod.begin(), 79 + t)).getWidth() / 2 : (*next(mod.begin(), 79 + t)).getWidth() / 2 : player->getLocalPosition().x,
							0,
							abs(player->getLocalPosition().z) >(*next(mod.begin(), 79 + t)).getDepth() / 2 ? player->getLocalPosition().z < 0 ? -(*next(mod.begin(), 79 + t)).getDepth() / 2 : (*next(mod.begin(), 79 + t)).getDepth() / 2 : player->getLocalPosition().z);

				}
			}
			trainInit = false;
		}
		*/

		if(!pause)
		{
			//increase train counter
			if(countdownCounter == 3)
				trainTimer += (float)dt;
			//keep boss active
			CandyMan->setActive(true);

			//keep players active
			for(int a = 0; a < 4; a++)
				(*(Player*)&(*next(mod.begin(), a))).setActive(true);
		}

		/// - If game not active and Camera is active (Move camera mode) - ///
		if(!movePlayer)
		{
			for(int a = 0; a < 4; a++)
				(*(Player*)&(*next(mod.begin(), a))).setActive(false);

			CandyMan->setActive(false);
			deathCounter = 0;

			if(GameEmGine::isControllerConnected(0))
			{

				XinputController* p1 = (XinputController*)GameEmGine::getController(0);

				//move camera
				move *= 2;

				GameEmGine::getMainCamera()->enableFPSMode();
				GameEmGine::translateCameraBy({p1->getSticks()[LS].x * move , 0 * move, p1->getSticks()[LS].y * move});//move camera
				GameEmGine::rotateCameraBy(ang * (abs(p1->getSticks()[RS].x) + abs(p1->getSticks()[RS].y)) * Coord3D<>{p1->getSticks()[RS].y, p1->getSticks()[RS].x, 0});//rotate camera
				//GAME::getMainCamera().rotateBy({ ang *p1->getSticks()[RS].y ,ang *p1->getSticks()[RS].x ,0}, { p1->getSticks()[RS].y  ,p1->getSticks()[RS].x, 0 });
				GameEmGine::translateCameraBy({0 ,p1->getTriggers().LT * -move,0});//move out
				GameEmGine::translateCameraBy({0 ,p1->getTriggers().RT * move,0});//move out
				move /= 2;
			}
		}
		//GameEmGine::m_grayScalePost.enable();
		//glUniform1f(GameEmGine::m_grayScalePost.getUniformLocation("uTime"), deathCounter);
		//GameEmGine::m_grayScalePost.disable();

		if(deathCounter)
			gameOver = true;
		deathCounter += .01f;
		deathCounter = deathCounter <= 1 ? deathCounter : 1;
	}

private:
	bool m_init = false;

	enum class CONTROL_STATE :char
	{
		CAMERA,
		LEVEL,
		GAME,
	}currentState;
	std::string modeStr = "Camera";

	list<Player>players;
	list<Model> testSceneLoad;
	list<Model> mod;
	list<Light> lights;
	bool fadein = true;
	bool fadeout = false;
	float splashT = 0;
	GLubyte splashAmbient = 0;

	float ang = 2;
	Model* curModel = nullptr;
	bool m_left = 0, m_right = 0, m_fwd = 0, m_back = 0, m_up = 0, m_down = 0,
		rotLeft = 0, rotRight = 0, rotUp = 0, rotDown = 0, movePlayer = true;

	Coord2D<> leftM, rightM;
	AudioPlayer audio;

	bool gameOver = false;
	bool gameWin = false;
	bool screenPause = false;
	bool pause = false;

	//Time
	float  time = 0;
	float deathCounter = 0;

	//Train Variables
	float trainTimer = 0; //Determines when train comes and goes
	vector<float> timer[4];

	//Countdown Variables
	Model nums[3]{Model("Models/COUNTDOWN/3cd.obj"), Model("Models/COUNTDOWN/2cd.obj"), Model("Models/COUNTDOWN/1cd.obj")};
	float countdownTimer = 0;
	int countdownCounter = 0;
	bool countdown = true;

	bool trainInit = false;
};
