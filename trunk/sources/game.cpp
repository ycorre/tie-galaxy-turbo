/*
 * Main class of the game
 *
 * Part of this code was created by Jeff Molofee '99
 * (ported to Linux/SDL by Ti Leggett '01)
 *
 * If you've found this code useful, please let me know.
 *
 * Visit Jeff at http://nehe.gamedev.net/
 *
 * or for port-specific comments, questions, bugreports etc.
 * email to leggett@eecs.tulane.edu
 */

#include "game.h"

//TODO ENhance the loading config files system
//Additionnal audio support

//Timers: useful when pausing the game and for potential timing of the player
//Used as global variables and declared in common.h
Uint32 ProgramTimer;
Uint32 GameTimer;

Uint32 NextLoop, Interval, FPS = 60;
Uint32 Score = 0;

Game::Game()
{
	initSDL();
	initGame();
}

// Function to release/destroy the resources and restore to the desktop
void quit(int code)
{
    SDL_Quit();

    exit(code);
}


int main(int argc, char **argv)
{
    Game * game = new Game();

    //set the random generator seed
    srand(time(NULL));

    //Init loop timer
    NextLoop = 0;

    //Init interval in ms, i.e. the length of a loop
    //given by dividing 1 second by the number of targeted FPS
    Interval = 1000/FPS;

    //Launch the game
    game->mainLoop();
    
    return 1;
}

int Game::mainLoop()
{
	int done = FALSE;
	SDL_Event event;

    //Init the program timer
    ProgramTimer = 0;
    GameTimer = 0;

    gameState = MENU;

    //Main loop
    while (!done)
	{
	    //Clear the elements to be displayed at the beginning of the loop
    	graphicEngine.toDisplay.clear();

    	//Process SDL events
	    while (SDL_PollEvent(&event))
		{
		    switch(event.type)
			{
		    	case SDL_KEYDOWN:
					//Handle key presses
					if(gameState == MENU)
					{
						keyboard->handleKeyPressMenu(&event.key.keysym, menu);
					}
					break;
			    
		    	case SDL_KEYUP:
					//Handle key presses
					if(gameState == INGAME || gameState == PAUSE)
					{
						keyboard->handleKeyUpHero(&event.key.keysym, currentLevel->hero);
					}
					break;

				case SDL_QUIT:
					//Handle quit requests
					done = TRUE;
					break;

				default:
					break;
			}
		}

	    keyboard->processKeyState();
	    keyboard->processKeyPress();

	    switch(gameState)
	    {
	    	case MENU:
	    		menu->displayMenu();
	    		break;

	    	case INGAME:
	    		keyboard->processeyInGame(currentLevel->hero);
				currentLevel->drawLevel();
				if (currentLevel->levelState == LEVEL_WON)
				{
					launchNextLevel();
				}
				if (currentLevel->levelState == LEVEL_LOST)
				{
					menu->currentMenu = MENU_GAMEOVER;
					graphicEngine.fadeOut(1);
					gameState = MENU;
				}
				break;

	    	default:
	    		break;

	    }

	    //Display the frame
		graphicEngine.drawFrame();
		
		//Control the speed of the game
		controlFPS();
		updateTimers();
	}

    //exit
    quit(0);

    //Should never get here
    return 0;
}

//Initialization of the program
int Game::initGame()
{
	loadConf();
	graphicEngine.initGe();
	graphicEngine.addFont("res/Arial.ttf");
	graphicEngine.initColors();
	keyboard = new Keyboard();
	keyboard->game = this;
	physicEngine = *(new Pe());
	soundEngine = *(new SoundEngine());
	menu = new Menu(&graphicEngine, &soundEngine);

    Level1 * l1 = new Level1();
    Level2 * l2 = new Level2();
    Level3 * l3 = new Level3();

    l1->name = "level1";
    l2->name = "level2";
    l3->name = "level3";

    levels.insert(make_pair("level1", l1));
    levels.insert(make_pair("level2", l2));
    levels.insert(make_pair("level3", l3));

    //Init the menu
    menu->introLength = 2500;
    menu->loadIntro();
    menu->loadMenu();
    menu->game = this;

	return 1;
}

//Initialization of SDL
int Game::initSDL()
{
	// Flags to pass to SDL_SetVideoMode
    int videoFlags;

    //This holds some info about our display
    const SDL_VideoInfo *videoInfo;
	
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) < 0)
	{
	    cerr <<  "Video initialization failed: " << SDL_GetError() << endl;
	    quit(1);
	}

    // Fetch the video info
    videoInfo = SDL_GetVideoInfo();

    if (!videoInfo)
	{
	    cerr << "Video query failed: " << SDL_GetError() << endl;
	    quit(1);
	}

    //The flags to pass to SDL_SetVideoMode
    videoFlags = SDL_DOUBLEBUF; 	   // Enable double buffering
    videoFlags |= SDL_HWPALETTE;       // Store the palette in hardware

    // This checks to see if surfaces can be stored in memory
    if (videoInfo->hw_available)
    	videoFlags |= SDL_HWSURFACE;
    else
    	videoFlags |= SDL_SWSURFACE;

    // This checks if hardware blits can be done
    if (videoInfo->blit_hw)
    	videoFlags |= SDL_HWACCEL;

    // get a SDL surface from screen
    graphicEngine.screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, videoFlags);

    // Verify there is a surface
    if (!graphicEngine.screen)
	{
	    cerr <<  "Video mode set failed: << " << SDL_GetError() << endl;
	    quit(1);
	}
    
    //Init font usage
    if (TTF_Init() != 0)
    {
    	cerr << "TTF_Init() Failed: " << TTF_GetError() << endl;
    	quit(1);
    }
    
    //Init Audio
    if(Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 8192 ) == -1 )
	{
    	cout << "Warning: Audio_Init() Failed: " << SDL_GetError() << endl;
    	quit(1);
	}

    //Enable repetition of keyboard events
    SDL_EnableKeyRepeat(1, 250);//SDL_DEFAULT_REPEAT_INTERVAL);
    
    Drawable::ge = &graphicEngine;

    return 0;
}

void Game::pause()
{
	//If we are not in pause
	if (gameState != PAUSE)
	{
		//Then save current game state
		previousGameState = gameState;
		//and pause
		gameState = PAUSE;
	}
	else
	{
		//else revert to previous game state
		gameState = previousGameState;
	}
}

//Start a new game (beginning with level 1)
void Game::newGame()
{
	currentLevel = levels.at("level1");
	currentLevel->ge = &graphicEngine;
	currentLevel->pe = &physicEngine;
	currentLevel->soundEngine = &soundEngine;
	Drawable::lev = currentLevel;
	hero = new Hero();
    GameTimer = 0;
    Score = 0;

	currentLevel->loadLevel(hero);
	gameState = INGAME;
}

//Used to launch a specific level
//Should be used only in debug
void Game::launchLevel(string aLevel)
{
	currentLevel = levels.at(aLevel);
	currentLevel->ge = &graphicEngine;
	currentLevel->pe = &physicEngine;
	currentLevel->soundEngine = &soundEngine;
	Drawable::lev = currentLevel;
	hero = new Hero();
    GameTimer = 0;
    Score = 0;

	currentLevel->loadLevel(hero);
	gameState = INGAME;
}

//Launch the next level after a level has ended
void Game::launchNextLevel()
{
	unsigned int newLevel = 0;
	for (vector<string>::iterator aLevel = levelOrder.begin(); aLevel != levelOrder.end(); ++aLevel)
	{
		newLevel++;
		if (currentLevel->name == *aLevel)
			break;
	}

	if (newLevel >= levelOrder.size())
	{
		cout<<"Game won!!!!!!!!!!!\n";
		menu->currentMenu = MENU_SUCCESS;
		gameState = MENU;
		return;
	}

	currentLevel = levels.at(levelOrder.at(newLevel));

	currentLevel->ge = &graphicEngine;
	currentLevel->pe = &physicEngine;
	currentLevel->soundEngine = &soundEngine;
	Drawable::lev = currentLevel;

	currentLevel->loadLevel(hero);
	gameState = INGAME;
}

//Update Timers
void Game::updateTimers()
{
	ProgramTimer = ProgramTimer + Interval;

	if (gameState == INGAME)
		GameTimer = GameTimer + Interval;

	return;
}

//Control game speed
//If necessary, wait the required time until the next loop
void controlFPS()
{
	if(NextLoop > SDL_GetTicks())
		SDL_Delay(NextLoop - SDL_GetTicks());

	NextLoop = SDL_GetTicks() + Interval ;

	return;
}

//Load the game configuration file
void Game::loadConf()
{
	ifstream file;
	string line;
	string fileName = "conf/game.conf";
	string token;
	string anElement;
	vector<string> confElements;
	string confElement;

	file.open(fileName.c_str());
	while(getline(file, line))
	{
		if(line.size()!=0) //Ignore empty lines
		{
			confElements.clear();
			istringstream myLine(line);

			while(getline(myLine, token, ';'))
			{
				levelOrder.push_back(token);
			}
		}
	}
}