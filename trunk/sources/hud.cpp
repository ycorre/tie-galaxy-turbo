/*
 * Contains the menu function
 */

#include "hud.h"

HUD::HUD(GraphicEngine * aGraphicEngine)
{
	int i;
	ge = aGraphicEngine;
	loadHUDElements("conf/hud.json");

	backGround = hudElements.at("hud_bkg");
	backGround.width = SCREEN_WIDTH;
	backGround.height = SCREEN_HEIGHT - GAMEZONE_HEIGHT;
	backGround.setAnimX(0);
	backGround.setAnimY(0);

	health.copyFrom(&(hudAnimatedElements.at("abeSpVie")));
	quarkB.copyFrom(&(hudAnimatedElements.at("abeSpQB")));
	quarkT.copyFrom(&(hudAnimatedElements.at("abeSpQT")));
	quarkU.copyFrom(&(hudAnimatedElements.at("abeSpQU")));
	quarkD.copyFrom(&(hudAnimatedElements.at("abeSpQD")));
	quarkC.copyFrom(&(hudAnimatedElements.at("abeSpQC")));
	quarkS.copyFrom(&(hudAnimatedElements.at("abeSpQS")));

	file = hudElements.at("abeDossiers");
	brightDot = hudElements.at("abeTypeLed");
	weaponLoad.copyFrom(&(hudAnimatedElements.at("abeSpVumetre")));

	for(i = 0; i < 6; i++)
	{
		Drawable anotherLife = hudElements.at("abeVie");
		anotherLife.posX = anotherLife.posX - (i * 30);
		lives.push_back(anotherLife);
	}

	for(i = 0; i < 16; i++)
	{
		Drawable aScoreNumber = hudElements.at("abeNumbers");
		aScoreNumber.posX = aScoreNumber.posX - (i * 15);
		score.push_back(aScoreNumber);
	}

	quarks[QuarkB] = &quarkB;
	quarks[QuarkT] = &quarkT;
	quarks[QuarkU] = &quarkU;
	quarks[QuarkD] = &quarkD;
	quarks[QuarkC] = &quarkC;
	quarks[QuarkS] = &quarkS;

	quarkIndex[QuarkB] = 0;
	quarkIndex[QuarkT] = 0;
	quarkIndex[QuarkU] = 0;
	quarkIndex[QuarkD] = 0;
	quarkIndex[QuarkC] = 0;
	quarkIndex[QuarkS] = 0;

	electron = hudAnimatedElements.at("abeElectr");
	hadron =  hudAnimatedElements.at("abeHadron");
	baryon =  hudAnimatedElements.at("abeBaryon");
	plasma =  hudAnimatedElements.at("abePlasma");
}

void HUD::loadHUDElements(string path)
{
	unsigned int index;
	Json::Value root;
	Json::Reader reader;
	ifstream file;
	file.open(path.c_str());

	int parsingSuccessful = reader.parse(file, root);
	if (!parsingSuccessful)
	{
	    //report to the user the failure and their locations in the document.
	    cout << "Failed to parse configuration:" << endl << reader.getFormattedErrorMessages();
	}

	//instantiate the corresponding drawable elements
	const Json::Value drawable = root["drawable"];
	for (index = 0; index < drawable.size(); ++index)
	{
		Drawable tmp = Drawable(drawable[index]);
		tmp.posY = GAMEZONE_HEIGHT + tmp.posY;
		hudElements.insert(make_pair(tmp.name, tmp));
	}

	const Json::Value animDrawable = root["animatedDrawable"];
	for (index = 0; index < animDrawable.size(); ++index)
	{
		AnimatedDrawable tmp = AnimatedDrawable(animDrawable[index]);
		tmp.posY = GAMEZONE_HEIGHT + tmp.posY;
		hudAnimatedElements.insert(make_pair(tmp.name, tmp));
	}
}

void HUD::displayUI(Hero * anHero)
{
	backGround.processDisplay();
	file.processDisplay();
	displayHealth(anHero->health);
	displayLife(anHero->nbLife);
	displayQuarkLevels(anHero->quarkLevels);
	displayWeapons(anHero);
	displayScore(Score);
}

//Display the current health of the player
void HUD::displayHealth(int healthValue)
{
	static float i = 0.0;

	if(i < healthValue * 3.6)
	{
		i = i + 0.25;
	}
	if (i > healthValue * 3.6)
	{
		i = i - 0.25;
	}

	health.currentAnimation->setFrameTo((int)(i-1));
	//setAnimX((int)(i) * health.width);
	health.processDisplay();
}

void HUD::displayScore(int aScore)
{
	int i = 0;

	while (aScore != 0)
	{
		int currentNumber = aScore % 10;
		aScore = aScore / 10;

		score.at(i).setAnimX(currentNumber * score.at(i).width);
		score.at(i).processDisplay();
		i++;
	}
}

/*
void HUD::displayMassPotential(float massPo)
{
	static float i = 0.0;

	if(i < massPo - 1)
	{
		i = i + 0.3;
	}
	else if (i > massPo)
	{
		i = massPo; //i - 0.3;
	}
	massLoad->setAnimX((int)(i) * massLoad->width);

	massLoad->processDisplay();
}

void HUD::displayRadioPotential(float radioPo)
{
	static float i = 0.0;

	if(i < radioPo)
	{
		i = i + 0.3;
	}
	if (i > radioPo)
	{
		i = i - 0.3;
	}
	radioLoad->setAnimX((int)(i) * radioLoad->width);

	radioLoad->processDisplay();
}
*/

void HUD::displayQuarkLevels(map<int, int> quarkLevels)
{
	for(map<int, int>::iterator aQuark = quarkLevels.begin(); aQuark != quarkLevels.end(); ++aQuark)
	{
		int aQuarkName = (*aQuark).first;
		float anIndex  = quarkIndex.at(aQuarkName);
		int aLevel = quarkLevels.at(aQuarkName);

		if(anIndex < aLevel)
		{
			quarkIndex.at(aQuarkName) = anIndex + 0.25;
		}
		if (anIndex > aLevel)
		{
			quarkIndex.at(aQuarkName) = anIndex - 0.25;
		}

		quarks.at(aQuarkName)->currentAnimation->setFrameTo((int)(quarkIndex.at(aQuarkName)));
		quarks.at(aQuarkName)->processDisplay();
	}
}

void HUD::displayWeapons(Hero * hero)
{
	electron.setAnimation("l1");
	if(hero->currentWeapon->name.compare("electronGun") == 0)
		electron.setAnimation("l2");
	electron.processDisplay();

	hadron.setAnimation("l1");
	if(hero->currentWeapon->name.compare("hadronGun") == 0)
		hadron.setAnimation("l2");
	hadron.processDisplay();

	baryon.setAnimation("l1");
	if(hero->currentWeapon->name.compare("baryonGun") == 0)
		baryon.setAnimation("l2");
	baryon.processDisplay();


	plasma.setAnimation("l1");
	if(hero->currentWeapon->name.compare("plasmaGun") == 0)
		plasma.setAnimation("l2");
	plasma.processDisplay();

	if (hero->currentWeapon->load == -1)
	{
		weaponLoad.currentAnimation->setFrameTo(0);
	}
	else
	{
		weaponLoad.currentAnimation->setFrameTo(1);
	}
	weaponLoad.processDisplay();

	switch(hero->currentWeapon->powerLevel)
	{
		case WEAPON_STANDARD:
			brightDot.posY = 50 + GAMEZONE_HEIGHT;
			break;

		case WEAPON_SUPERIOR:
			brightDot.posY = 38 + GAMEZONE_HEIGHT;
			break;

		case WEAPON_FURIOUS:
			brightDot.posY = 26 + GAMEZONE_HEIGHT;
			break;

		default:
			break;
	}

	brightDot.processDisplay();
}

void HUD::displayLife(int nbLife)
{
	int i;
	if (nbLife > 6)
	{
		nbLife = 6;
	}
	for(i = 0; i < nbLife; i++)
	{
		lives.at(i).processDisplay();
	}
}



