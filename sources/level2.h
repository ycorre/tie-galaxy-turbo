/*
 * Contains level 2 description
 */
 
#ifndef LEVEL2_H
#define LEVEL2_H
 
#include <iostream>
#include <string>
#include <utility>
#include <fstream>
#include <sstream>
#include "level.h"
#include "asteroid.h"

class Level2:  public Level
{
	public:
	//Frequency at which asteroid are generated
	int bombGenerationRate;
	//Keep track of the last asteroid generated
	unsigned int lastTimeBomb;

	Level2();
	virtual void loadLevel(Hero * aHero);
	void loadObject();
	virtual void drawLevel();
	void checkEvent();
	void createWave1();
	int checkCollision(Drawable * anEnemy);
	int checkEnemyCollision(Drawable * anElement);
	int checkBonusCollision(Drawable * anElement);
	int generateBomb();
	virtual void finishLevel();
};

#endif
