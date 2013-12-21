/*
 * Contains the interface for the HUD
 */
 
#ifndef HUD_H
#define HUD_H
 
#include <iostream>
#include <string>
#include <utility>
#include <fstream>
#include <sstream>
#include <map>
#include "text.h"
#include "drawable.h"
#include "graphicEngine.h"
#include "hero.h"

class Weapon;
class Hero;

class HUD
{
	public:
	GraphicEngine * ge;
	Drawable backGround;
	map<string, vector<string> > configurationElements;
	map<string, Drawable> hudElements;
	map<string, AnimatedDrawable> hudAnimatedElements;

	//Text * scoreHUD;
	map<int, float> quarkIndex;
	Drawable health, quarkB, quarkT, quarkU, quarkD, quarkC, quarkS;
	Drawable weaponLoad, file, brightDot;
	AnimatedDrawable electron, hadron, baryon, plasma;
	vector<Drawable> lives;
	vector<Drawable> score;

	HUD();
	HUD(GraphicEngine * ge);
	void loadHUDElements(string path);
	void displayUI(Hero * hero);
	void displayHealth(int life);
	void displayLife(int nbLife);
	void displayScore(int score);
	//void displayMassPotential(float massPo);
	//void displayRadioPotential(float radioPo);
	void displayQuarkLevels(map<int, int> quarkLevels);
	void displayAQuarkLevel(Drawable * aQuark, float anIndex, int aLevel, int aQuarkName);
	void displayWeapons(Hero * hero);
};

enum {QB, QT, QU, QD, QC, QS};
#endif

