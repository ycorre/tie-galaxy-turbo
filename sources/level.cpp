/*
 * Contains the level superclass functions
 */

#include "level.h"

void Level::loadLevel(Hero * aHero)
{

}

void Level::loadObject()
{
	loadBackGround();
	loadTextures();
}

//Load all textures used for the level at the beginning
//Texture and objects are specified in the level configuration file
void Level::loadTextures()
{
	for (map<string, vector<string> >::iterator anElement = configurationElements.begin(); anElement != configurationElements.end(); ++anElement)
	{
		ge->textures.insert(make_pair(anElement->first, ge->loadTexture((anElement->second).back())));
	}
}

void Level::loadBackGround()
{
	bigStars.getTexture("bigStars");
	bigStars.width = SCREEN_WIDTH;
	bigStars.height = SCREEN_HEIGHT;
	bigStars.posX = 0;
	bigStars.posY = 0;
	bigStars.state = 0;
	bigStars.animX = 0;
	bigStars.animY = 0;

	smallStars.getTexture("smallStars");
	smallStars.width = SCREEN_WIDTH;
	smallStars.height = SCREEN_HEIGHT;
	smallStars.posX = 0;
	smallStars.posY = 0;
	smallStars.state = 0;
	smallStars.animX = 0;
	smallStars.animY = 0;

	nebulae.getTexture("nebulae");
	nebulae.width = SCREEN_WIDTH;
	nebulae.height = SCREEN_HEIGHT;
	nebulae.posX = 0;
	nebulae.posY = 0;
	nebulae.state = 0;
	nebulae.animX = 0;
	nebulae.animY = 0;
}

void Level::moveBackGround()
{
	//Move the backgrounds
	nebulae.animX = nebulae.animX + 0.5;
	bigStars.animX = bigStars.animX + 1.5;
	smallStars.animX = smallStars.animX + 1;

	//Set the blending parameters
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
	//glBlendEquation(GL_FUNC_ADD);

	//Draw the textures
	ge->draw(&nebulae);
	ge->draw(&bigStars);
	ge->draw(&smallStars);

	//Restore the blending parameters
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Level::drawLevel()
{
	checkEvent();

	pe->stayOnScreen(hero, make_pair(SCREEN_WIDTH, GAMEZONE_HEIGHT));

	for (std::list<Drawable *>::iterator anElement = activeElements.begin() ; anElement != activeElements.end(); ++anElement)
	{
		(*anElement)->animate();
		if((*anElement)->display)
		{
			ge->toDisplay.push_back(*anElement);
		}
	}
	hero->animate();
	bg.animX = bg.animX + cameraSpeed;
}


//function that handle the events (enemies apparitions, collision checks,  etc...)
void Level::checkEvent()
{

	for (std::list<Drawable *>::iterator anElement = activeElements.begin() ; anElement != activeElements.end(); ++anElement)
	{
		if((*anElement)->toRemove)
		{
			activeElements.erase(anElement++);
		}
		else
		{
			if((*anElement)->isEnemy())
			{
				checkEnemyCollision(*anElement);
				Enemy * anEnemy = static_cast<Enemy *>(*anElement);
				anEnemy->fire();
			}
			if((*anElement)->isBonus() ||(*anElement)->isLaser())
			{
				checkCollision(*anElement);
			}
		}
	}
}

int Level::checkEnemyCollision(Drawable * anElement)
{
	if(pe->collisionDetection(hero, anElement))
	{
		anElement->processCollisionWith(hero);
		hero->processCollisionWith(anElement);
		return 1;
	}

	for (list<Laser*>::iterator aLaser = hero->lasers.begin(); aLaser != hero->lasers.end(); ++aLaser)
	{
		Laser * aL = *aLaser;
		if(pe->collisionDetection(aL, anElement))
		{
			anElement->processCollisionWith(aL);
			aL->toRemove = TRUE;
			return 1;
		}
	}
	return 0;
}

int Level::checkCollision(Drawable * anElement)
{
	if(pe->collisionDetection(hero, anElement))
	{
		anElement->processCollisionWith(hero);
		hero->processCollisionWith(anElement);
		return 1;
	}
	return 0;
}

void Level::createExplosion(int x, int y, int type)
{
	activeElements.push_back(new Explosion(x, y, type));
}

void Level::createBonus(int x, int y, int type)
{
	activeElements.push_back(new Bonus(x, y, type));
}

//Load all the configuration elements from a text file
//Format: elementName;width;height;listOfAnimationStatus$nbOfFramesInAnAnimation;pathToTextureFile
void Level::loadConf()
{
	ifstream file;
	string line;
	string fileName = "conf/l1.conf";
	string token;
	string type;
	vector<string> confElements;
	string confElement;

	file.open(fileName.c_str());
	while(getline(file, line))
	{
		if(line.size()!=0) //Ignore empty lines
		{
			confElements.clear();
			istringstream myLine(line);
			getline(myLine, type, ';');

			while(getline(myLine, token, ';'))
			{
				confElements.push_back(token);
			}

			configurationElements.insert(make_pair(type, confElements));
		}
	}
}

void Level::heroLoseLife()
{
	hero->loseLife();
}

int Level::isOnScreen(Drawable * aDrawable)
{
	return pe->isOnScreen(aDrawable);
}

void Level::endLevel()
{
	activeElements.clear();
	levelState = LEVEL_WON;
}

void Level::finishLevel()
{

}
