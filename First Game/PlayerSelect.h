#pragma once
#include <GameEmGine.h>
#include "Game.h"

class PlayerSelect:public Scene
{
public:
	// Set menu screen
	void init()
	{
		mod.push_back(new Model("Models/Screen/PlayerSelect/PlayerSelect.obj"));
		GameEmGine::addModel(mod.back()); //Mod 0 
		mod.push_back(new Model("Models/Screen/Menu/Start.obj"));
		GameEmGine::addModel(mod.back()); //Mod 1
		mod[1]->setToRender(false);



		mod.push_back(new Model("Models/ClassPH/Assault/assaultPH.obj")); // I wrote them like this because they will all be different models eventually I think.
	//	GameEmGine::addModel(mod.back()); // mod 2
		mod[2]->getTransformer().setRotation({0,270,180}), mod[2]->getTransformer().setScale(10);
		//	mod[2]->setToRender(false);

		mod.push_back(new Model("Models/ClassPH/Tank/tankPH.obj"));
		//	GameEmGine::addModel(mod.back()); // mod 3
		mod[3]->getTransformer().setRotation({0,270,180}), mod[3]->getTransformer().setScale(10);
		//	mod[3]->setToRender(false);

		mod.push_back(new Model("Models/ClassPH/Medic/medicPH.obj"));
		//	GameEmGine::addModel(mod.back()); // mod 4
		mod[4]->getTransformer().setRotation({0,270,180}), mod[4]->getTransformer().setScale(10);
		//	mod[4]->setToRender(false);

		mod.push_back(new Model("Models/ClassPH/Specialist/specPH.obj"));
		//	GameEmGine::addModel(mod.back()); // mod 5
		mod[5]->getTransformer().setRotation({0,270,180}), mod[5]->getTransformer().setScale(10);
		//	mod[5]->setToRender(false);


			//TODO: Add back button and more flashy start button and "Press A to ready" buttons
		float extra=0;
		for(int a = 0; a < 4; a++)
		{
			if(a == 2)
				extra = .7f;
			else if(a == 3)
				extra = 2;
			mod.push_back(new Model(*classes[0]));
			mod[6 + a]->getTransformer().setPosition(float(-42.2 + a * 27.5 + extra), -25, 0), mod[6 + a]->getTransformer().setRotation({0,270,0}), mod[6 + a]->getTransformer().setScale(1, 15, 7);
			mod[6 + a]->setToRender(true);
		GAME::addModel(mod.back()); //6, 7, 8, 9
		}



		//See GDD for general layout of this screen.

		mod[0]->addChild(mod[1]);

		LightSource::setSceneAmbient({0,0,0,255});

		mod[0]->getTransformer().setScale(0.85f, 1.5f, 1.0f);
		mod[1]->getTransformer().setRotation({90,0,0});
		mod[1]->getTransformer().setScale(15.0f);
		mod[1]->getTransformer().setPosition({mod[0]->getWidth() - mod[1]->getWidth() - 200, -9.f * 1 + 15,0});

		LightSource::setSceneAmbient({0,0,0,255});

		keyPressed = [=](int a, int b) {keyInputPressed(a, b);  };
	}

	void keyInputPressed(int key, int modfier)
	{
		modfier;
		if(key == 'B' && !fadeout)
		{
			for(int i = 0; i < 4; i++)
				switch(option[i])
				{
				case 0:

					players.push_back(new Assault("Models/AssaultModel/Idle/ACM1.obj"));
					break;
				case 1:
					players.push_back(new Tank("Models/AssaultModel/Idle/ACM1.obj"));
					break;
				case 2:
					players.push_back(new Medic("Models/AssaultModel/Idle/ACM1.obj"));
					break;
				case 3:
					players.push_back(new Specialist("Models/AssaultModel/Idle/ACM1.obj"));
					break;
				default:
					break;
				}
			fadeout = true;
		}
	}

	// doing the update for menu screenb
	void updateMenu()
	{

		static bool menuMoved[] = {false,false,false,false};

		if(fadein)
		{
			splashT += 0.01f;
			splashAmbient = (GLubyte)lerp(0, 255, splashT);
			LightSource::setSceneAmbient({splashAmbient,splashAmbient,splashAmbient,splashAmbient});
			if(splashAmbient >= 250)
			{
				fadein = false;
				splashT = 0;
				splashAmbient = 255;
				LightSource::setSceneAmbient({splashAmbient,splashAmbient,splashAmbient,splashAmbient});
			}
		}

		static Coord3D tmp = Coord3D(20.0f);
		float extra = 0;
		for(int a = 0; a < 4; a++)
		{
			if(GameEmGine::isControllerConnected(a))
			{
				if(abs(GameEmGine::getController(a).sticks[LS].x) >= 0.8)
				{
					if(!menuMoved[a])
					{

						option[a] += GameEmGine::getController(a).sticks[LS].x < 0 ? 1 : -1;

						option[a] = option[a] > 3 ? 0 : option[a] < 0 ? 3 : option[a];

						lerpParam = 0;
						if(a == 2)
							extra = .7f;
						else if(a == 3)
							extra = 2;
						GameEmGine::removeModel(mod[6 + a]);
						mod[6 + a]->setColour({255,255,255});
						*mod[6 + a] = *classes[option[a]];
						mod[6 + a]->getTransformer().setPosition(float(-42.2 + a * 27.5), -25, 0), mod[6 + a]->getTransformer().setRotation({0,270,0}), mod[6 + a]->getTransformer().setScale(1, 15, 7);
						GameEmGine::addModel(mod[6 + a]);

						//tmp = mod[option]->getTransformer().getScale();
						menuMoved[a] = true;
					}
				}
				else

					if(abs(GameEmGine::getController(a).sticks[LS].x) < .3f)
						menuMoved[a] = false;

				static bool fixthisnow = true;
				if(Xinput::buttonPressed(GameEmGine::getController(a).buttons.A) && fixthisnow)
				{
					fixthisnow = false;
					for(int i = 0; i < 4; i++)
						switch(option[i])
						{
						case 0:

							players.push_back(new Assault("Models/AssaultModel/Idle/ACM1.obj"));
							break;
						case 1:
							players.push_back(new Tank("Models/AssaultModel/Idle/ACM1.obj"));
							break;
						case 2:
							players.push_back(new Medic("Models/AssaultModel/Idle/ACM1.obj"));
							break;
						case 3:
							players.push_back(new Specialist("Models/AssaultModel/Idle/ACM1.obj"));
							break;
						default:
							break;
						}
					fadeout = true;
					break;
				}
			}
		}
		//TODO: Set this to change a picture instead of this
		//mod[option]->getTransformer().setScale(lerp(tmp, Coord3D(12.0f), lerpParam));
		//mod[option]->setColour(lerp(ColourRGBA{ 255,255,255 }, ColourRGBA{ 0,255,255 }, lerpParam));
		//lerpParam += .1f;

		if(lerpParam >= 1)
		{
			lerpParam = 1;
		}

		if(fadeout)
		{
			splashT += 0.01f;
			splashT = splashT > 1 ? 1 : splashT;
			splashAmbient = (GLubyte)lerp(255, 0, splashT);
			LightSource::setSceneAmbient({splashAmbient,splashAmbient,splashAmbient,splashAmbient});
			if(splashAmbient <= 5)
			{
				fadein = true;
				fadeout = false;
				splashT = 0;
				splashAmbient = 255;

				//GamePlayInit();
				Game* game = new Game;
				game->playerTypes(players);
				GameEmGine::setScene(game);
			}
		}
	}

	//updates within game loop
	void update(double dt)
	{
		dt;
		updateMenu();
	}

private:
	Model *classes[4]
	{new Assault("Models/ClassPH/Assault/assaultPH.obj"),new Tank("Models/ClassPH/Tank/tankPH.obj"),
		new  Medic("Models/ClassPH/Medic/medicPH.obj"),new Specialist("Models/ClassPH/Specialist/specPH.obj")};
	std::vector<Player*>players;
	std::vector<Model*> mod;
	bool fadein = true;
	bool fadeout = false;
	float splashT = 0;
	GLubyte splashAmbient = 0;
	float lerpParam = 1;
	int option[4] = {0,0,0,0};
};