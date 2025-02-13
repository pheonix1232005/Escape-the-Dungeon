#include <windows.h>	// Header file for the Windows Library
#include <gl/gl.h>	// Header file for the OpenGL32 Library
#include <gl/glu.h>	// Header file for the GlU32 Library
#include <gl/glut.h>	// Header file for the Glut32 Library
#include <math.h>	// Header file for the Math Library
#include <iostream>
#include <chrono>
#include <thread>
#include <irrKlang.h>
#include <time.h>
#include <string>

using namespace irrklang;
using namespace std;
//These lines above link the code to the openGL libraries.
#pragma comment(lib, "opengl32.lib")	// import gl library
#pragma comment(lib, "glu32.lib")	// import glu library
#pragma comment(lib, "glut32.lib")	// import glut library


//GLOBAL VARIABLES


float  pPosx = 0, pPosy = 0, sPosx = 0, sPosy = 0, bPosx = 0, bPosy = 0, pillarAlp = 0.4, lsPosx = 0;

bool swordFlag = 0, hitsoundFlag = 1, enemy1HP[3][3] = { {0,1,0}, {0,1,0},{0,1,0} }, swordInEnemy = 0,
enemy1Life = 1, swordWithdrawn = 1, playerLife = 1, playerHP[5][3] = { {1,0,0}, {1,0,0}, {1,0,0}, {1,0,0}, {1,0,0} }, bossLife = 1,
bossHP[10][3] = { {0,1,0}, {0,1,0}, {0,1,0}, {0,1,0}, {0,1,0}, {0,1,0}, {0,1,0}, {0,1,0}, {0,1,0}, {0,1,0} }, swordInBoss = 1,
musicOn = 0, isJumping = 0, shouldDescend = 0, power_upChoosen = 0, pillarFlag = 0, healthGlitch = 0, warningFlag = 0, pillarHit = 0,
pillarHitUpdate = 0, swordCycle = 0, bossSwordsHit = 0, inGame = 1, yaay = 0;

int randomNum, enemy1HitCount = 0, startTime = 0, elapsedTime = 0, startPillarTime = 0, elapsedPillarTime = 0, playerHitCount = 0, hits = 0, roomsCount = 1, roomsPassed = 0,
enemiesCount = 0, enemyDrawn = 0, bossHitCount = 0, jumpSpeed = 2, jumpPos = 0, jumpHeight = 60, immuneCount = 0;

enum GameState { MAIN_MENU, START_GAME, INSTRUCTIONS, GAME_OVER, WIN };
GameState currentState = MAIN_MENU;

// Power-up variables
bool powerUpActive = false;
int selectedPowerUp = 0;
float originalSpeed = 0.5; // Original movement speed
//float speedMultiplier = 1.3; // 30% increase speed
float healthRestore = 0.1; // 10% health restore
//end



//FUNCTIONS DEFITINITIONS

/**
Creates the main window, registers event handlers, and
initializes OpenGL stuff.
*/
void InitGraphics(int argc, char* argv[]);

/**
Sets the logical coordinate system we will use to specify
our drawings.
*/
void SetTransformations();

/**
Handles the paint event. This event is triggered whenever
our displayed graphics are lost or out-of-date.
ALL rendering code should be written here.
*/
void OnDisplay();

//void manageSoundTracks(int value);
void manageSoundTracks(int value);

void drawFilledCircle(GLfloat a, GLfloat b, GLfloat radius);

void healthGlitchFix(int value);

void manageDrawings(int value);

void drawHealth();

void drawPlayer();

void drawSword();

void drawback_ground();

void drawLightSwords();

void moveBullet(int value);

void drawBullet(int value);

void drawEnemy();

void drawDABOSS();

void draw_back_ground_Menu();

void bossDamage(int value);

void enemyDamage(int value);

void specialKeyPress(int key, int x, int y);

void playerDamage(int value);

void updateJump(int value);

void resetValues();

void keyPress(unsigned char key, int x, int y);

void mousePress(int button, int state, int mPosx, int mPosy);

void displayPowerUps(int enemiescount);

void applyPowerUp(int value);

void Playerimmunitymessage();

void fullHealthWarning();

void drawPillars();

void bossAttackPatterns(int value);

void drawMainMenu();

void win();

void displayInstructions();

void stateControl();

void gameOver();

void startTimer() {
	startTime = clock();
}// update time 

void updateTimer() {
	elapsedTime = clock() - startTime;
	//// display timer 
}

void displayTimer();

void displayRoomNo();

irrklang::ISoundEngine* soundEffects = irrklang::createIrrKlangDevice();
irrklang::ISoundEngine* bossTheme = irrklang::createIrrKlangDevice();
irrklang::ISoundEngine* mainTheme = irrklang::createIrrKlangDevice();

int main(int argc, char* argv[]) {
	InitGraphics(argc, argv);

	return 0;
}

/**
Creates the main window, registers event handlers, and
initializes OpenGL stuff.
Initialization is done once at the start of the program.
Rendering is done each time the window needs redrawing
*/
void InitGraphics(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	//Create an 800x600 window with its top-left corner at pixel (100, 100)
	glutInitWindowPosition(-1, -1); //pass (-1, -1) for Window-Manager defaults  glutInitWindowPosition(X, Y);
	glutInitWindowSize(1320, 742);  //glutInitWindowSize(width, hight);
	glutCreateWindow("Escape the Dungeon");
	//OnDisplay will handle the paint event
	glutTimerFunc(125, enemyDamage, 0);        //These			||		  =====	    ||
	glutTimerFunc(25, moveBullet, 0);          //are			||		||     ||   ||
	glutTimerFunc(25, playerDamage, 0);        //piling			||		||	   ||	||
	glutTimerFunc(25, updateJump, 0);		   //up             ||      ||     ||   ||  
	glutTimerFunc(25, bossDamage, 0);          //really			||      ||     ||   ||
	glutTimerFunc(25, manageSoundTracks, 0); //really			||      ||     ||   ||
	glutTimerFunc(25, applyPowerUp, 0);        //really         ||      ||     ||   ||
	glutTimerFunc(25, bossAttackPatterns, 0);  //stupidly       ||      ||     ||   ||
	glutTimerFunc(10, healthGlitchFix, 0);     //fast            =====    =====      =====
	glutDisplayFunc(OnDisplay);
	glutIdleFunc(OnDisplay);
	glutKeyboardFunc(keyPress);
	glutSpecialFunc(specialKeyPress);
	glutMouseFunc(mousePress);
	glutDisplayFunc(stateControl);
	glutIdleFunc(stateControl);
	currentState = MAIN_MENU;
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glutMainLoop(); //Well glutMainLoop is the main function the keeps calling and calling the displaying functions and which also keeps your window actually opened
}

/**
Sets the logical coordinate system we will use to specify
our drawings.
*/
void SetTransformations() {
	//set up the logical coordinate system of the window: [-100, 100] x [-100, 100]
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//define a 2-D orthographic projection matrix
	//parameters: left, right, bottom, top
	gluOrtho2D(-100, 100, -100, 100); //gluOrtho2D(GLdouble left,GLdouble right,GLdouble bottom,GLdouble top)
	glMatrixMode(GL_MODELVIEW);
}

/**
Handles the paint event. This event is triggered whenever
our displayed graphics are lost or out-of-date.
ALL rendering code should be written here.
*/
void OnDisplay() {
	//set the background color to white
	glClearColor(0, 0, 0, 1);
	//fill the whole color buffer with the clear color
	glClear(GL_COLOR_BUFFER_BIT);
	SetTransformations();
	glEnable(GL_LINE_SMOOTH);

	//ALL drawing code goes here
	drawback_ground();
	drawHealth();
	updateTimer();
	displayTimer();
	displayPowerUps(enemiesCount);
	fullHealthWarning();
	Playerimmunitymessage();
	displayRoomNo();
	manageDrawings(0);
}

void manageDrawings(int value)
{
	if (inGame == 1) {
		drawPlayer();
		drawSword();
		if (roomsCount < 5 && playerLife == 1)
		{
			if (enemy1Life == 1)
			{
				drawBullet(0);
				drawEnemy();
			}
			else if (enemy1Life == 0 && enemiesCount == 1)//draw the OTHER enemy (after butchering the first one)
			{
				enemy1Life = 1;
				enemy1HitCount = 0;
				for (int i = 0; i < 3; i++)
				{
					for (int j = 0; j < 3; j++)
					{
						if (j == 1)
							enemy1HP[i][j] = 1;
						else
							enemy1HP[i][j] = 0;
					}
				}
				drawBullet(0);
				drawEnemy();
			}
			else if (roomsPassed == roomsCount && roomsCount < 5)// draw enemies in the next room
			{
				roomsCount++;
				enemyDrawn = 0;
				enemiesCount = 0;
				enemy1Life = 1;
				enemy1HitCount = 0;
				for (int i = 0; i < 3; i++)
				{
					for (int j = 0; j < 3; j++)
					{
						if (j == 1)
							enemy1HP[i][j] = 1;
						else
							enemy1HP[i][j] = 0;
					}
				}
				drawBullet(0);
				drawEnemy();
				std::cout << "roomsCount: " << roomsCount << "\n";
			}
		}
		else if (roomsCount == 5 && bossLife == 1)
		{
			drawDABOSS();
			drawPillars();
			drawLightSwords();
		}

	}
}

void drawHealth()
{
	if (inGame == 1) {
		//HEALTHBAR
		glBegin(GL_QUADS);
		glColor3f(playerHP[0][0], playerHP[0][1], playerHP[0][2]);
		glVertex2f(-95, 75);
		glVertex2f(-95, 70);
		glVertex2f(-90, 70);
		glVertex2f(-90, 75);
		glEnd();

		glBegin(GL_QUADS);
		glColor3f(playerHP[1][0], playerHP[1][1], playerHP[1][2]);
		glVertex2f(-90, 75);
		glVertex2f(-90, 70);
		glVertex2f(-85, 70);
		glVertex2f(-85, 75);
		glEnd();

		glBegin(GL_QUADS);
		glColor3f(playerHP[2][0], playerHP[2][1], playerHP[2][2]);
		glVertex2f(-85, 75);
		glVertex2f(-85, 70);
		glVertex2f(-80, 70);
		glVertex2f(-80, 75);
		glEnd();

		glBegin(GL_QUADS);
		glColor3f(playerHP[3][0], playerHP[3][1], playerHP[3][2]);
		glVertex2f(-80, 75);
		glVertex2f(-80, 70);
		glVertex2f(-75, 70);
		glVertex2f(-75, 75);
		glEnd();

		glBegin(GL_QUADS);
		glColor3f(playerHP[4][0], playerHP[4][1], playerHP[4][2]);
		glVertex2f(-75, 75);
		glVertex2f(-75, 70);
		glVertex2f(-70, 70);
		glVertex2f(-70, 75);
		glEnd();
		glPopMatrix();
	}
}

void bossDamage(int value)
{
	if (inGame == 1) {
		if (bossLife == 1 && roomsCount == 5)
		{
			if ((sPosx + 23) >= 100 && swordFlag == 1 && swordInBoss == 0)
			{
				swordInBoss = 1;
				if ((sPosx + 30) >= 100 && swordFlag == 1 && swordInBoss == 1)
				{
					bossHitCount++;
					soundEffects->play2D("media/audio/sword-cutting-flesh.mp3");
				}
				if (bossHitCount > 0 && swordInBoss == 1)
				{
					if (bossHitCount <= 9)
					{
						bossHP[bossHitCount - 1][0] = 1;
						bossHP[bossHitCount - 1][1] = 0;
						bossHP[bossHitCount - 1][2] = 0;
					}
					else
					{
						bossLife = 0;
					}
				}
			}
		}
		else if (bossLife == 0)
			win();


		glutTimerFunc(25, bossDamage, 0);
	}

}

void enemyDamage(int value)
{
	if (inGame == 1) {
		if ((sPosx + 23) >= randomNum && (sPosy + 25) <= 30 && swordFlag == 1 && enemy1HitCount < 3 && swordInEnemy == 0)
		{
			swordInEnemy = 1;
			if (enemy1HitCount == 0 && swordInEnemy == 1)
			{
				enemy1HP[2][0] = 1;
				enemy1HP[2][1] = 0;
				enemy1HP[2][2] = 0;
				soundEffects->play2D("media/audio/sword-cutting-flesh.mp3", false);
				enemy1HitCount++;
			}
			else if (enemy1HitCount == 1 && swordInEnemy == 1)
			{
				enemy1HP[1][0] = 1;
				enemy1HP[1][1] = 0;
				enemy1HP[1][2] = 0;
				soundEffects->play2D("media/audio/sword-cutting-flesh.mp3", false);
				enemy1HitCount++;
			}
			else
			{
				enemy1Life = 0;
				enemiesCount++;
				std::cout << "enemiesCount = " << enemiesCount << '\n';
				soundEffects->play2D("media/audio/sword-cutting-flesh.mp3", false);
				enemy1HitCount++;

			}
		}
		glutTimerFunc(25, enemyDamage, 0);
	}
}

void drawBullet(int value)
{
	if (inGame == 1)
	{
		if (enemy1Life)
		{
			glPushMatrix();
			glTranslatef(randomNum, 0, 0);
			glTranslatef(bPosx, bPosy, 0);
			drawFilledCircle(-92.5, -75, 2.5);
			glBegin(GL_POLYGON);
			glColor3f(1, 0, 0);
			glVertex2f(-92.5, -72.5); //-72.5
			glVertex2f(-92.5, -77.5); //-77.5
			glVertex2f(-87.5, -77.5); //-77.5
			glVertex2f(-87.5, -72.5); //-72.5
			glEnd();
			glPopMatrix();
		}
	}
}

void resetValues() {
	playerHitCount = hits = 0, roomsCount = 1, roomsPassed = 0, pPosx = 0, pPosy = 0, sPosx = 0, sPosy = 0, bPosx = 0, bPosy = 0,
		pillarAlp = 0.4, lsPosx = 0,
		swordFlag = 0, hitsoundFlag = 1, swordInEnemy = 0, enemy1Life = 1, swordWithdrawn = 1, playerLife = 1, bossLife = 1,
		swordInBoss = 1, musicOn = 0, isJumping = 0, shouldDescend = 0, power_upChoosen = 0, pillarFlag = 0, healthGlitch = 0, warningFlag = 0, pillarHit = 0,
		pillarHitUpdate = 0, swordCycle = 0, bossSwordsHit = 0, inGame = 1, randomNum, enemy1HitCount = 0, startTime = 0, elapsedTime = 0, startPillarTime = 0, elapsedPillarTime = 0, playerHitCount = 0, hits = 0, roomsCount = 1, roomsPassed = 0,
		enemiesCount = 0, enemyDrawn = 0, bossHitCount = 0, jumpSpeed = 2, jumpPos = 0, jumpHeight = 60, immuneCount = 0;
	powerUpActive = false, selectedPowerUp = 0, originalSpeed = 0.5, healthRestore = 0.1, yaay = 0;
}

void keyPress(unsigned char key, int x, int y)
{
	if (inGame == 1) {
		if (currentState == MAIN_MENU) {
			switch (key) {
			case '1':
				currentState = START_GAME;
				break;
			case '2':
				currentState = INSTRUCTIONS;
				break;
			case '3':
				exit(0);
				break;
			default:
				cout << "Invalid choice. Please try again." << endl;
				drawMainMenu();
				break;
			}
		}
		else if (currentState == START_GAME) {
			switch (key)
			{
			case 'd':
			case 'D':
			case '39':
				if (pPosx >= 200 && enemiesCount == 2 && power_upChoosen)
				{
					pPosx = 0;
					sPosx = 0;
					roomsPassed++;
					power_upChoosen = 0;
					warningFlag = 0;
				}
				else if (pPosx >= 190 && enemiesCount != 2 || pPosx >= 190 && power_upChoosen == 0)
				{
					pPosx = 190;
					sPosx = 190;
				}
				else
				{
					pPosx += originalSpeed;
					sPosx += originalSpeed;
				}

				break;
			case 'a':
			case 'A':
			case '37':
				if (pPosx >= 0)
				{
					pPosx -= originalSpeed;
					sPosx -= originalSpeed;
				}
				break;
			case ' ':
				if (!isJumping) {// Only jump if not already jumping
					isJumping = true;
					jumpPos = 0.0f;  // Reset velocity for a fresh jump
				}
				break;
			case 'q':
				if (enemiesCount == 2)
				{
					selectedPowerUp = 0; // Speed
					powerUpActive = true;
				}
				break;
			case 'e':
				if (enemiesCount == 2)
				{
					if (playerHitCount != 0)
					{
						selectedPowerUp = 1; // Health
						powerUpActive = true;
					}
					else
						warningFlag = 1;
				}
				break;
			case 'r':
				if (enemiesCount == 2)
				{
					/*if (powerUpActive == false)
					{*/
					selectedPowerUp = 2; // IMMUNE
					powerUpActive = true;
					immuneCount += 1;
					glutSwapBuffers();
					//}
				}
				break;
			}
		}
		else if (currentState == GAME_OVER) {
			switch (key)
			{
			case'm':
			case'M':
				if (roomsCount == 5)
				{
					bossTheme->drop();
					irrklang::ISoundEngine* bossTheme = irrklang::createIrrKlangDevice();
					irrklang::ISoundEngine* mainTheme = irrklang::createIrrKlangDevice();
				}
				else
				{
					mainTheme->drop();
					bossTheme->drop();
					irrklang::ISoundEngine* bossTheme = irrklang::createIrrKlangDevice();
					irrklang::ISoundEngine* mainTheme = irrklang::createIrrKlangDevice();
				}
				resetValues();
				for (int i = 0; i < 5; i++)
				{
					for (int j = 0; j < 3; j++)
					{
						if (j == 0)
							playerHP[i][j] = 1;
						else
							playerHP[i][j] = 0;
					}
				}
				for (int i = 0; i < 10; i++)
				{
					for (int j = 0; j < 3; j++)
					{
						if (j == 1)
							bossHP[i][j] = 1;
						else
							bossHP[i][j] = 0;
					}
				}
				currentState = MAIN_MENU;
				break;
			}
		}
		else if (currentState == INSTRUCTIONS) {
			switch (key)
			{
			case'm':
			case'M':
			case'109':
				resetValues();
				mainTheme->drop();
				bossTheme->drop();
				irrklang::ISoundEngine* bossTheme = irrklang::createIrrKlangDevice();
				irrklang::ISoundEngine* mainTheme = irrklang::createIrrKlangDevice();
				for (int i = 0; i < 5; i++)
				{
					for (int j = 0; j < 3; j++)
					{
						if (j == 0)
							playerHP[i][j] = 1;
						else
							playerHP[i][j] = 0;
					}
				}
				for (int i = 0; i < 10; i++)
				{
					for (int j = 0; j < 3; j++)
					{
						if (j == 1)
							bossHP[i][j] = 1;
						else
							bossHP[i][j] = 0;
					}
				}
				currentState = MAIN_MENU;
				break;
			}
		}
		else if (currentState == WIN) {
			
			switch (key)
			{
			case'm':
			case'M':
			case'109':
				bossTheme->drop();
				irrklang::ISoundEngine* bossTheme = irrklang::createIrrKlangDevice();
				irrklang::ISoundEngine* mainTheme = irrklang::createIrrKlangDevice();
				resetValues();
				for (int i = 0; i < 5; i++)
				{
					for (int j = 0; j < 3; j++)
					{
						if (j == 0)
							playerHP[i][j] = 1;
						else
							playerHP[i][j] = 0;
					}
				}
				for (int i = 0; i < 10; i++)
				{
					for (int j = 0; j < 3; j++)
					{
						if (j == 1)
							bossHP[i][j] = 1;
						else
							bossHP[i][j] = 0;
					}
				}
				currentState = MAIN_MENU;
				break;
			}
		}
	}
}

void specialKeyPress(int key, int x, int y)
{
	if (inGame == 1) {
		switch (key)
		{
		case GLUT_KEY_RIGHT:
			if (pPosx == 200)
			{
				pPosx = 0;
				sPosx = 0;
			}
			else
			{
				pPosx += originalSpeed;
				sPosx += originalSpeed;
			}
			break;

		case GLUT_KEY_LEFT:
			if (pPosx >= 0)
			{
				pPosx -= originalSpeed;
				sPosx -= originalSpeed;
			}
			break;
		}
	}
}

void moveBullet(int value)
{
	if (inGame == 1)
	{
		if (enemy1Life == 1 && playerLife == 1 && roomsCount < 5) //
		{
			if (randomNum - -1 * bPosx > (pPosx + 5) || (pPosx + 4) <= randomNum - (-1 * bPosx) <= (pPosx + 5) && pPosy >= 29 || (pPosx - 6) - (randomNum - (-1 * bPosx)) >= 5 && pPosy < 29) //
			{
				bPosx -= 0.5;
			}
			else if ((pPosx + 4) <= randomNum - (-1 * bPosx) <= (pPosx + 5) && pPosy < 29)
			{
				if (immuneCount != 0)
				{
					bPosx = 0;
					immuneCount--;
				}
				else
				{
					playerHitCount++;
					bPosx = 0;
				}
			}
			if (-1 * bPosx > randomNum + 10)
			{
				bPosx = 0;
			}
		}
		glutTimerFunc(25, moveBullet, 0);
	}
}

void playerDamage(int value)
{
	if (inGame == 1)
	{
		if (playerLife == 0) {
			currentState = GAME_OVER;
		}
		if (playerLife == 1)
		{
			//calculating damage from normal DA BOSS
			if (roomsCount == 5 && bossLife == 1)
			{
				if ((playerHitCount - hits == 2) && playerHitCount < 5 && pillarHitUpdate == 0)
				{
					hits = playerHitCount;
					playerHP[5 - hits + 1][1] = 1;
					playerHP[5 - hits + 1][2] = 1;
					playerHP[5 - hits][1] = 1;
					playerHP[5 - hits][2] = 1;
					pillarHitUpdate = 1;
				}
				else if (playerHitCount >= 5)
				{
					playerHitCount = 5;
					hits = playerHitCount;
					hits = playerHitCount;
					playerHP[5 - hits + 1][1] = 1;
					playerHP[5 - hits + 1][2] = 1;
					playerHP[5 - hits][1] = 1;
					playerHP[5 - hits][2] = 1;
					if (playerHP[3][1] == 1 && playerHP[3][2] == 1 && playerHP[4][1] == 1 && playerHP[4][2] == 1)
						playerLife = 0;
				}
			}
			else
			{
				if (playerHitCount > hits && playerHitCount <= 5)
				{
					hits = playerHitCount;
					std::cout << "hits = " << hits << "\n";
					playerHP[5 - hits][1] = 1;
					playerHP[5 - hits][2] = 1;
				}
				else if (hits == 5)
					playerLife = 0;
			}
			//calculating damage from normal enemies
		}
		glutTimerFunc(25, playerDamage, 0);
	}
}

void mousePress(int button, int state, int mPosx, int mPosy)
{
	if (inGame == 1)
	{
		switch (button)
		{
		case GLUT_LEFT_BUTTON:
			if (state == GLUT_DOWN && swordFlag == 0)
			{
				if (swordInEnemy == 0)
				{
					soundEffects->play2D("media/audio/swordWhoosh.mp3");
				}
				swordFlag = 1;
			}
			else if (state == GLUT_UP && swordFlag == 1)
			{
				swordFlag = 0;
				if (swordInEnemy == 1)
					swordInEnemy = 0;
				if (swordInBoss == 1)
					swordInBoss = 0;
			}
		}
	}
}

void manageSoundTracks(int value)
{
	if (inGame == 1)
	{
		if (roomsCount < 5)
		{
			if (musicOn == 0)
			{
				mainTheme->play2D("media/audio/Hornet.mp3", true);
				musicOn = 1;
			}
		}
		else
		{
			if (musicOn == 1)
			{
				std::cout << "im in the music = 1" << "\n";
				mainTheme->drop();
				bossTheme->play2D("media/audio/Mantis Lords.mp3", true);
				musicOn = 0;
			}

		}
		glutTimerFunc(25, manageSoundTracks, 0);
	}
}

void displayTimer()
{
	if (inGame == 1)
	{
		// Convert elapsed time to seconds
		float seconds = elapsedTime / (float)CLOCKS_PER_SEC;

		// Calculate minutes and seconds
		int minutes = static_cast<int>(seconds) / 60;
		int remainingSeconds = static_cast<int>(seconds) % 60;

		// Format the time as "MM:SS"
		string timeStr1 = "Time: " + std::to_string(minutes) + ":" + (remainingSeconds < 10 ? "0" : "") + std::to_string(remainingSeconds);

		// Render the timer text to the screen using OpenGL's text rendering functions
		glColor3f(1.0f, 1.0f, 1.0f); // text color
		glRasterPos2f(-95, 90);
		for (char c : timeStr1)
		{
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
		}
	}
}

void displayRoomNo()
{
	if (inGame == 1)
	{
		glColor3f(1.0f, 1.0f, 1.0f); //  text color
		glRasterPos2f(-5, 90);
		std::string roomNum = "Room " + to_string(roomsCount);
		for (char c : roomNum)
		{
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
		}
	}
}

void updateJump(int value) {
	if (inGame == 1)
	{
		if (isJumping) {
			if (jumpPos <= jumpHeight && shouldDescend == 0) {
				if (jumpPos == jumpHeight)
					shouldDescend = 1;
				jumpPos += jumpSpeed; // to move the player
				pPosy += jumpSpeed;  // To measure how far up we moved
				sPosy += jumpSpeed;
			}
			else if (shouldDescend == 1 && jumpPos > 0) {
				pPosy -= jumpSpeed; //again to move the player but down this time
				sPosy -= jumpSpeed;
				jumpPos -= jumpSpeed;// to descend after the peak 
			}
			if (jumpPos == 0) {
				isJumping = 0; //so we can jump again in the key press
				shouldDescend = 0; //to ensure jump again since we need to ascend 
				jumpPos = 0; // Ensure it stops on the ground
			}
		}
		glutTimerFunc(25, updateJump, 0);
	}
}

void drawFilledCircle(GLfloat a, GLfloat b, GLfloat radius)
{
	if (inGame == 1)
	{
		float PI = 3.14;
		int lineAmount = 100; //# of Lines used to draw circle

		GLfloat twicePi = 2.0f * PI;
		//glEnable(GL_LINE_SMOOTH);
		//glLineWidth(0.5f);
		glBegin(GL_TRIANGLE_FAN);
		//glVertex2f(a, b); // not correct why ?
		glColor3f(1, 0, 0);

		for (int i = 0; i <= lineAmount; i++) {

			float theta = i * (twicePi / lineAmount);

			glVertex2f(
				a + (radius * cos(theta)),
				b + (radius * sin(theta))
			);
		}
		glEnd();
	}
}

void healthGlitchFix(int value)
{
	if (inGame == 1) {
		if (playerLife == 1 && healthGlitch == 0 && elapsedTime <= 1000)
		{
			if (playerHitCount == 1)
			{
				playerHP[5 - hits][1] = 0; // Restore health
				playerHP[5 - hits][2] = 0;
				hits--;
				playerHitCount--;
			}
		}
		glutTimerFunc(10, healthGlitchFix, 0);
	}
}

void displayPowerUps(int enemiesCount) {
	if (inGame == 1)
	{
		if (enemiesCount == 2 && power_upChoosen == 0) {  // Only run when enemy count is 2
			glColor3f(1, 1, 1);
			glRasterPos2f(-95, 50);
			std::string powerUpText = "Select a Power-Up:";
			for (char c : powerUpText) {
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
			}

			// Draw Options
			glRasterPos2f(-95, 30);
			std::string option1 = "1. Increase Movement Speed (q)";
			for (char c : option1) {
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
			}

			glRasterPos2f(-95, 10);
			std::string option2 = "2. Restore Health (e)";
			for (char c : option2) {
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
			}

			glRasterPos2f(-95, -10);
			std::string option3 = "3. Immunity for one Bullet (r)";
			for (char c : option3) {
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
			}
		}
	}
}

void applyPowerUp(int value) {
	if (inGame == 1)
	{
		if (powerUpActive && power_upChoosen == 0)
		{
			if (selectedPowerUp == 0)
			{
				originalSpeed += 0.5;
				power_upChoosen = 1;
			}
			else if (selectedPowerUp == 1)
			{
				if (playerHitCount > 0)
				{
					if (playerLife == 1 && hits < 5) {
						playerHP[5 - hits][1] = 0; // Restore health
						playerHP[5 - hits][2] = 0;
						hits--;
						playerHitCount--;
						power_upChoosen = 1;
					}
				}

			}
			else if (selectedPowerUp == 2)
			{
				power_upChoosen = 1;
			}
			powerUpActive = false;
		}
		glutTimerFunc(25, applyPowerUp, 0);
	}
}

void fullHealthWarning()
{
	if (inGame == 1)
	{
		if (warningFlag == 1 && enemiesCount == 2 && power_upChoosen == 0)
		{
			glColor3f(1.0f, 0, 0); // text color
			glRasterPos2f(-68, 50);
			string words = "YOU'RE ALREADY AT FULL HEALTH!!!";
			for (char c : words)
			{
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
			}
		}
	}
}

void drawPillars()
{
	if (inGame == 1)
	{
		if (bossLife == 1 && roomsCount == 5)
		{
			//pillars of light
			glBegin(GL_QUADS);
			glColor4f(1, 1, 1, pillarAlp);
			glVertex2f(-80, -100);
			glVertex2f(-70, -100);
			glVertex2f(-70, 80);
			glVertex2f(-80, 80);
			glEnd();
			glBegin(GL_QUADS);
			glColor4f(1, 1, 1, pillarAlp);
			glVertex2f(-50, -100);
			glVertex2f(-40, -100);
			glVertex2f(-40, 80);
			glVertex2f(-50, 80);
			glEnd();
			glBegin(GL_QUADS);
			glColor4f(1, 1, 1, pillarAlp);
			glVertex2f(-20, -100);
			glVertex2f(-10, -100);
			glVertex2f(-10, 80);
			glVertex2f(-20, 80);
			glEnd();
		}
	}
}

void bossAttackPatterns(int value)
{
	if (inGame == 1)
	{
		if (bossLife == 1 && roomsCount == 5)
		{
			//this part is where the pillars change color
			int seconds = elapsedTime / (float)CLOCKS_PER_SEC;
			if (seconds % 3 == 0)
			{
				pillarFlag = 1;
				if (swordCycle == 0)
					swordCycle = 1;
			}
			else if (seconds % 3 != 0)
			{
				pillarFlag = 0;
				if (swordCycle == 1)
					swordCycle = 0;
			}
			if (pillarFlag == 1)
			{
				pillarAlp = 1.0f;
			}
			else if (pillarFlag == 0)
			{
				pillarAlp = 0.4f;

				if (pillarHitUpdate == 1)
					pillarHitUpdate = 0;
			}

			//this part is where the player take damage from pillars
			if (pillarFlag == 1 && roomsCount == 5)
			{
				if (((20 <= (pPosx + 9) && (pPosx + 9) <= 30) || (50 < (pPosx + 9) && (pPosx + 9) < 60) || (80 < (pPosx + 9) && (pPosx + 9) < 90)) && pillarHitUpdate == 0)
				{
					if (immuneCount != 0)
					{
						immuneCount--;
					}
					else if (immuneCount == 0)
					{
						std::cout << "I GOT HIT FROM THE FRONT!!!";
						playerHitCount += 2;
					}
				}
				else if (((20 <= pPosx && pPosx <= 30) || (50 < pPosx && pPosx < 60) || (80 < pPosx && pPosx < 90)) && pillarHitUpdate == 0)
				{
					if (immuneCount != 0)
					{
						immuneCount--;
					}
					else if (immuneCount == 0)
					{
						std::cout << "I GOT HIT FROM BEHIND!!!";
						playerHitCount += 2;
					}
				}
			}

			//this part is moving the boss' swords
			if ((0 <= (pPosx + 9) - (110 - (-1 * lsPosx)) && (pPosx + 9) - (110 - (-1 * lsPosx)) <= 2) && pPosy < 45 ||
				110 - (-1 * lsPosx) <= pPosx && pPosx <= (130 - (-1 * lsPosx) && pPosy < 60))
			{
				if (bossSwordsHit == 0)
				{
					if (immuneCount != 0)
					{
						immuneCount--;
					}
					else {
						playerHitCount += 2;
						std::cout << "I GOT HIT BY THE SWORDS!!!!!!!!!!!!" << "\n";
						bossSwordsHit = 1;
					}
				}
			}
			if (-1 * lsPosx <= 130 && swordCycle == 0)
			{
				lsPosx -= 1;

			}
			else if (-1 * lsPosx >= 130)
			{
				lsPosx = 0;
				if (bossSwordsHit == 1)
					bossSwordsHit = 0;
			}

		}
		glutTimerFunc(25, bossAttackPatterns, 0);
	}
}

void drawLightSwords()
{
	if (inGame == 1)
	{
		if (bossLife == 1 && roomsCount == 5)
		{
			glPushMatrix();
			glTranslatef(lsPosx, 0, 0);

			//bottom swords
			glBegin(GL_POLYGON);
			glColor3f(0, 1, 1);
			glVertex2f(0, -67.5);
			glVertex2f(5, -70);
			glVertex2f(20, -70);
			glVertex2f(20, -65);
			glVertex2f(5, -65);
			glEnd();

			glBegin(GL_POLYGON);
			glColor3f(0, 1, 1);
			glVertex2f(0, -57.5);
			glVertex2f(5, -60);
			glVertex2f(20, -60);
			glVertex2f(20, -55);
			glVertex2f(5, -55);
			glEnd();

			glPopMatrix();
		}
	}
}

void drawEnemy()
{
	if (inGame == 1)
	{
		if (enemy1Life == 1)
		{
			if (enemyDrawn == 0)
			{
				srand(time(0));
				randomNum = 50 + (rand() % 70);
				enemyDrawn = 1;
				std::cout << "enemyDrawn from the 1st " << enemyDrawn << '\n';
				bPosx = 0;
			}
			if (enemyDrawn == 1 && enemiesCount == 1)
			{
				srand(time(0));
				randomNum = 120 + (rand() % 60);
				enemyDrawn = 2;
				std::cout << "enemyDrawn from the 2nd " << enemyDrawn << '\n' << '\n';
				bPosx = 0;
			}

			glPushMatrix();
			glTranslatef(randomNum, 0, 0);
#ifndef The HealthBar
			glBegin(GL_QUADS);
			glColor3f(enemy1HP[0][0], enemy1HP[0][1], enemy1HP[0][2]);
			glVertex2f(-96, -46);
			glVertex2f(-90.4, -46);
			glVertex2f(-90.4, -48);
			glVertex2f(-96, -48);
			glEnd();
			glBegin(GL_QUADS);
			glColor3f(enemy1HP[1][0], enemy1HP[1][1], enemy1HP[1][2]);
			glVertex2f(-90.4, -46);
			glVertex2f(-83.8, -46);
			glVertex2f(-83.8, -48);
			glVertex2f(-90.4, -48);
			glEnd();
			glBegin(GL_QUADS);
			glColor3f(enemy1HP[2][0], enemy1HP[2][1], enemy1HP[2][2]);
			glVertex2f(-83.8, -46);
			glVertex2f(-77.2, -46);
			glVertex2f(-77.2, -48);
			glVertex2f(-83.8, -48);
			glEnd();
#endif 
			glTranslatef(-100, -45, 0);
			glScalef(0.09f, 0.23f, 0.0f);
#ifndef Face_Shape
			glColor3f(0.2, 0.22, 0.35);
			glBegin(GL_POLYGON);
			glVertex2f(158.489, -61.819);
			glVertex2f(168.447, -61.404);
			glVertex2f(175.915, -61.819);
			glVertex2f(185.872, -63.064);
			glVertex2f(196.660, -66.383);
			glVertex2f(204.128, -71.362);
			glVertex2f(212.011, -78.830);
			glVertex2f(216.989, -87.543);
			glVertex2f(221.968, -96.670);
			glVertex2f(224.043, -107.872);
			glVertex2f(224.872, -117.830);
			glVertex2f(223.628, -126.543);
			glVertex2f(220.309, -136.500);
			glVertex2f(212.840, -144.383);
			glVertex2f(204.957, -148.117);
			glVertex2f(193.340, -151.851);
			glVertex2f(182.138, -153.096);
			glVertex2f(165.957, -155.170);
			glVertex2f(152.266, -154.755);
			glVertex2f(136.085, -153.511);
			glVertex2f(120.734, -149.777);
			glVertex2f(107.457, -144.798);
			glVertex2f(99.989, -136.915);
			glVertex2f(96.670, -126.543);
			glVertex2f(95.426, -113.266);
			glVertex2f(99.989, -95.840);
			glVertex2f(107.457, -85.053);
			glVertex2f(117.000, -75.926);
			glVertex2f(128.617, -68.872);
			glVertex2f(142.723, -63.064);
			glEnd();
#endif 
#ifndef Left_horn
			glBegin(GL_POLYGON);
			glVertex2f(89.617, -68.043);
			glVertex2f(88.372, -59.745);
			glVertex2f(88.787, -47.713);
			glVertex2f(92.936, -35.266);
			glVertex2f(104.138, -19.500);
			glVertex2f(105.798, -30.702);
			glVertex2f(107.043, -43.564);
			glVertex2f(110.777, -52.277);
			glVertex2f(117.000, -60.574);
			glVertex2f(126.957, -69.287);
			glVertex2f(117.415, -75.096);
			glVertex2f(107.872, -85.468);
			glVertex2f(99.989, -79.660);
			glEnd();
#endif 
#ifndef Right_horn
			glBegin(GL_POLYGON);
			glVertex2f(223.628, -57.670);
			glVertex2f(226.947, -44.809);
			glVertex2f(226.117, -28.628);
			glVertex2f(221.138, -12.447);
			glVertex2f(219.064, -22.819);
			glVertex2f(216.989, -33.191);
			glVertex2f(212.426, -44.809);
			glVertex2f(201.638, -56.011);
			glVertex2f(190.021, -63.894);
			glVertex2f(200.394, -68.872);
			glVertex2f(208.691, -75.096);
			glVertex2f(216.989, -67.213);
			glEnd();
#endif 
#ifndef Body_Shape
			glBegin(GL_POLYGON);
			glVertex2f(160.564, -209.521);
			glVertex2f(170.936, -209.521);
			glVertex2f(181.309, -207.447);
			glVertex2f(180.479, -199.564);
			glVertex2f(184.628, -191.266);
			glVertex2f(192.096, -181.309);
			glVertex2f(192.096, -171.766);
			glVertex2f(186.287, -161.809);
			glVertex2f(182.553, -154.340);
			glVertex2f(169.691, -154.340);
			glVertex2f(154.755, -155.170);
			glVertex2f(140.649, -153.926);
			glVertex2f(134.840, -152.266);
			glVertex2f(133.181, -162.638);
			glVertex2f(129.447, -172.181);
			glVertex2f(129.862, -180.894);
			glVertex2f(134.011, -186.702);
			glVertex2f(138.574, -196.245);
			glVertex2f(143.553, -206.202);
			glVertex2f(150.606, -208.691);
			glEnd();
#endif 
#ifndef Left_finger_1
			glBegin(GL_POLYGON);
			glVertex2f(57.255, -173.426);
			glVertex2f(58.500, -181.309);
			glVertex2f(48.128, -183.798);
			glVertex2f(38.170, -190.851);
			glVertex2f(38.170, -185.872);
			glVertex2f(41.489, -180.894);
			glVertex2f(50.202, -173.426);
			glEnd();
#endif 
#ifndef Left_finger_2
			glBegin(GL_POLYGON);
			glVertex2f(73.021, -185.043);
			glVertex2f(83.809, -182.553);
			glVertex2f(83.809, -189.606);
			glVertex2f(82.564, -197.904);
			glVertex2f(75.511, -204.543);
			glVertex2f(73.021, -197.074);
			glVertex2f(74.681, -189.191);
			glEnd();
#endif 
#ifndef Left_finger_3
			glBegin(GL_POLYGON);
			glVertex2f(58.500, -181.723);
			glVertex2f(67.628, -183.798);
			glVertex2f(53.106, -199.564);
			glVertex2f(53.106, -188.777);
			glEnd();
#endif 
#ifndef Left_palm
			glBegin(GL_POLYGON);
			glVertex2f(84.223, -182.968);
			glVertex2f(79.245, -174.255);
			glVertex2f(73.851, -168.447);
			glVertex2f(67.213, -167.617);
			glVertex2f(58.915, -168.862);
			glVertex2f(56.840, -173.011);
			glVertex2f(58.085, -181.309);
			glVertex2f(67.628, -183.798);
			glVertex2f(73.436, -185.043);
			glEnd();
#endif 
#ifndef Left_arm
			glBegin(GL_POLYGON);
			glVertex2f(102.894, -155.170);
			glVertex2f(114.926, -154.340);
			glVertex2f(127.787, -154.340);
			glVertex2f(135.255, -154.755);
			glVertex2f(133.181, -163.883);
			glVertex2f(120.319, -163.883);
			glVertex2f(107.872, -165.128);
			glVertex2f(99.574, -168.447);
			glVertex2f(89.617, -175.915);
			glVertex2f(84.223, -182.553);
			glVertex2f(78.830, -173.426);
			glVertex2f(85.468, -164.713);
			glEnd();
#endif 
#ifndef Right_hand
			glBegin(GL_POLYGON);
			glVertex2f(251.840, -168.447);
			glVertex2f(242.713, -169.691);
			glVertex2f(238.149, -174.670);
			glVertex2f(233.585, -177.989);
			glVertex2f(235.660, -190.851);
			glVertex2f(241.053, -196.660);
			glVertex2f(245.202, -188.777);
			glVertex2f(256.819, -195.415);
			glVertex2f(265.117, -204.543);
			glVertex2f(267.606, -190.021);
			glVertex2f(261.383, -182.968);
			glVertex2f(263.457, -181.309);
			glVertex2f(280.883, -189.606);
			glVertex2f(276.734, -180.064);
			glVertex2f(265.947, -171.766);
			glEnd();
#endif 
#ifndef Right_arm
			glBegin(GL_POLYGON);
			glVertex2f(220.309, -153.096);
			glVertex2f(230.681, -158.489);
			glVertex2f(238.149, -164.298);
			glVertex2f(243.128, -171.351);
			glVertex2f(233.585, -177.574);
			glVertex2f(228.606, -169.691);
			glVertex2f(221.138, -165.128);
			glVertex2f(214.085, -161.394);
			glVertex2f(203.713, -160.564);
			glVertex2f(186.702, -163.468);
			glVertex2f(182.138, -152.681);
			glVertex2f(199.149, -151.851);
			glVertex2f(212.426, -150.191);
			glEnd();
#endif 
#ifndef Left_leg
			glBegin(GL_POLYGON);
			glVertex2f(117.830, -196.245);
			glVertex2f(123.638, -189.606);
			glVertex2f(131.521, -184.213);
			glVertex2f(144.798, -206.617);
			glVertex2f(136.915, -213.670);
			glVertex2f(136.915, -221.138);
			glVertex2f(142.723, -234.415);
			glVertex2f(134.840, -235.660);
			glVertex2f(129.447, -226.947);
			glVertex2f(119.904, -216.989);
			glVertex2f(115.755, -209.521);
			glVertex2f(114.511, -202.053);
			glEnd();
#endif 
#ifndef Left_foot
			glBegin(GL_POLYGON);
			glVertex2f(116.170, -229.851);
			glVertex2f(140.234, -228.606);
			glVertex2f(142.723, -234.830);
			glVertex2f(125.298, -238.979);
			glVertex2f(99.989, -239.809);
			glVertex2f(106.628, -231.096);
			glEnd();
#endif 
#ifndef Right_leg
			glBegin(GL_POLYGON);
			glVertex2f(197.489, -187.117);
			glVertex2f(190.436, -183.383);
			glVertex2f(180.479, -197.904);
			glVertex2f(180.894, -207.032);
			glVertex2f(189.606, -212.840);
			glVertex2f(192.511, -223.213);
			glVertex2f(202.053, -224.872);
			glVertex2f(207.447, -213.670);
			glVertex2f(207.447, -202.053);
			glVertex2f(204.543, -195.415);
			glEnd();
#endif 
#ifndef Right_foot
			glBegin(GL_POLYGON);
			glVertex2f(190.851, -233.170);
			glVertex2f(192.096, -221.968);
			glVertex2f(202.468, -225.702);
			glVertex2f(214.085, -226.532);
			glVertex2f(225.287, -230.266);
			glVertex2f(226.532, -236.489);
			glEnd();
#endif 
#ifndef Shading_left_leg
			glColor3f(0.49, 0.54, 0.66);
			glBegin(GL_POLYGON);
			glVertex2f(124.468, -190.851);
			glVertex2f(131.521, -185.457);
			glVertex2f(139.404, -199.979);
			glVertex2f(129.862, -207.447);
			glVertex2f(128.617, -215.330);
			glVertex2f(117.830, -209.936);
			glVertex2f(115.755, -206.617);
			glVertex2f(115.755, -198.734);
			glEnd();
#endif 
#ifndef Shading_right_leg
			glBegin(GL_POLYGON);
			glVertex2f(191.266, -182.968);
			glVertex2f(198.319, -187.947);
			glVertex2f(204.957, -197.074);
			glVertex2f(204.957, -208.691);
			glVertex2f(198.319, -213.670);
			glVertex2f(181.723, -198.319);
			glEnd();
#endif 
#ifndef First_Shading
			glColor3f(0.49, 0.54, 0.66);
			glBegin(GL_POLYGON);
			glVertex2f(148.117, -158.074);
			glVertex2f(138.574, -170.936);
			glVertex2f(133.181, -178.404);
			glVertex2f(133.181, -183.798);
			glVertex2f(143.968, -204.543);
			glVertex2f(158.489, -207.032);
			glVertex2f(178.819, -203.298);
			glVertex2f(189.191, -182.968);
			glVertex2f(181.723, -168.032);
			glVertex2f(169.691, -156.000);
			glEnd();
#endif 
#ifndef Second_Shading
			glColor3f(0.71, 0.85, 0.87);
			glBegin(GL_POLYGON);
			glVertex2f(141.894, -172.181);
			glVertex2f(146.872, -165.128);
			glVertex2f(163.053, -164.298);
			glVertex2f(174.255, -164.713);
			glVertex2f(183.798, -171.766);
			glVertex2f(161.394, -200.809);
			glVertex2f(155.585, -201.223);
			glVertex2f(149.777, -198.319);
			glVertex2f(145.628, -187.532);
			glEnd();
#endif
#ifndef Right_eye_color
			glColor3f(0.98, 0.64, 0.2);
			glBegin(GL_POLYGON);
			glVertex2f(192.511, -84.638);
			glVertex2f(202.883, -87.543);
			glVertex2f(210.766, -96.255);
			glVertex2f(214.500, -105.383);
			glVertex2f(214.500, -116.585);
			glVertex2f(209.106, -128.202);
			glVertex2f(197.904, -135.255);
			glVertex2f(184.213, -135.670);
			glVertex2f(174.670, -129.447);
			glVertex2f(168.032, -119.074);
			glVertex2f(167.202, -105.798);
			glVertex2f(170.936, -93.766);
			glVertex2f(180.064, -87.128);
			glEnd();
#endif 
#ifndef Left_eye_color
			glBegin(GL_POLYGON);
			glVertex2f(119.489, -89.617);
			glVertex2f(130.277, -93.351);
			glVertex2f(137.745, -101.234);
			glVertex2f(140.234, -110.362);
			glVertex2f(139.819, -120.734);
			glVertex2f(134.426, -130.691);
			glVertex2f(126.543, -136.500);
			glVertex2f(115.340, -136.915);
			glVertex2f(105.383, -129.032);
			glVertex2f(101.234, -120.319);
			glVertex2f(100.819, -109.117);
			glVertex2f(103.309, -99.574);
			glVertex2f(111.191, -92.521);
			glEnd();
#endif 
#ifndef Mouth
			glColor3f(0, 0, 0);
			glLineWidth(2);
			glBegin(GL_LINE_STRIP);
			glVertex2f(143.798, -138.989);
			glVertex2f(148.777, -137.330);
			glVertex2f(156.660, -136.500);
			glVertex2f(163.713, -136.915);
			glVertex2f(169.936, -138.160);
			glEnd();
#endif 
			//The outlines 
#ifndef Left_horn_outline
			glBegin(GL_LINE_LOOP);
			glVertex2f(89.617, -68.043);
			glVertex2f(88.372, -59.745);
			glVertex2f(88.787, -47.713);
			glVertex2f(92.936, -35.266);
			glVertex2f(104.138, -19.500);
			glVertex2f(105.798, -30.702);
			glVertex2f(107.043, -43.564);
			glVertex2f(110.777, -52.277);
			glVertex2f(117.000, -60.574);
			glVertex2f(126.957, -69.287);
			glVertex2f(117.415, -75.096);
			glVertex2f(107.872, -85.468);
			glVertex2f(99.989, -79.660);
			glEnd();
#endif
#ifndef Right_horn_outline
			glBegin(GL_LINE_LOOP);
			glVertex2f(223.628, -57.670);
			glVertex2f(226.947, -44.809);
			glVertex2f(226.117, -28.628);
			glVertex2f(221.138, -12.447);
			glVertex2f(219.064, -22.819);
			glVertex2f(216.989, -33.191);
			glVertex2f(212.426, -44.809);
			glVertex2f(201.638, -56.011);
			glVertex2f(190.021, -63.894);
			glVertex2f(200.394, -68.872);
			glVertex2f(208.691, -75.096);
			glVertex2f(216.989, -67.213);
			glEnd();
#endif
#ifndef body_outline
			glLineWidth(2);
			glBegin(GL_LINE_LOOP);
			glVertex2f(160.564, -209.521);
			glVertex2f(170.936, -209.521);
			glVertex2f(181.309, -207.447);
			glVertex2f(180.479, -199.564);
			glVertex2f(184.628, -191.266);
			glVertex2f(192.096, -181.309);
			glVertex2f(192.096, -171.766);
			glVertex2f(186.287, -161.809);
			glVertex2f(182.553, -154.340);
			glVertex2f(169.691, -154.340);
			glVertex2f(154.755, -155.170);
			glVertex2f(140.649, -153.926);
			glVertex2f(134.840, -152.266);
			glVertex2f(133.181, -162.638);
			glVertex2f(129.447, -172.181);
			glVertex2f(129.862, -180.894);
			glVertex2f(134.011, -186.702);
			glVertex2f(138.574, -196.245);
			glVertex2f(143.553, -206.202);
			glVertex2f(150.606, -208.691);
			glEnd();
#ifndef Right_eye_color
			glBegin(GL_LINE_LOOP);
			glVertex2f(192.511, -84.638);
			glVertex2f(202.883, -87.543);
			glVertex2f(210.766, -96.255);
			glVertex2f(214.500, -105.383);
			glVertex2f(214.500, -116.585);
			glVertex2f(209.106, -128.202);
			glVertex2f(197.904, -135.255);
			glVertex2f(184.213, -135.670);
			glVertex2f(174.670, -129.447);
			glVertex2f(168.032, -119.074);
			glVertex2f(167.202, -105.798);
			glVertex2f(170.936, -93.766);
			glVertex2f(180.064, -87.128);
			glEnd();
#endif
#ifndef Left_eye_outline
			glBegin(GL_LINE_LOOP);
			glVertex2f(119.489, -89.617);
			glVertex2f(130.277, -93.351);
			glVertex2f(137.745, -101.234);
			glVertex2f(140.234, -110.362);
			glVertex2f(139.819, -120.734);
			glVertex2f(134.426, -130.691);
			glVertex2f(126.543, -136.500);
			glVertex2f(115.340, -136.915);
			glVertex2f(105.383, -129.032);
			glVertex2f(101.234, -120.319);
			glVertex2f(100.819, -109.117);
			glVertex2f(103.309, -99.574);
			glVertex2f(111.191, -92.521);
			glEnd();
#endif
#endif
#ifndef Right_eye_outline
			glBegin(GL_LINE_LOOP);
			glVertex2f(192.511, -84.638);
			glVertex2f(202.883, -87.543);
			glVertex2f(210.766, -96.255);
			glVertex2f(214.500, -105.383);
			glVertex2f(214.500, -116.585);
			glVertex2f(209.106, -128.202);
			glVertex2f(197.904, -135.255);
			glVertex2f(184.213, -135.670);
			glVertex2f(174.670, -129.447);
			glVertex2f(168.032, -119.074);
			glVertex2f(167.202, -105.798);
			glVertex2f(170.936, -93.766);
			glVertex2f(180.064, -87.128);
			glEnd();
#endif 
#ifndef Left_eye_outline
			glBegin(GL_LINE_LOOP);
			glVertex2f(119.489, -89.617);
			glVertex2f(130.277, -93.351);
			glVertex2f(137.745, -101.234);
			glVertex2f(140.234, -110.362);
			glVertex2f(139.819, -120.734);
			glVertex2f(134.426, -130.691);
			glVertex2f(126.543, -136.500);
			glVertex2f(115.340, -136.915);
			glVertex2f(105.383, -129.032);
			glVertex2f(101.234, -120.319);
			glVertex2f(100.819, -109.117);
			glVertex2f(103.309, -99.574);
			glVertex2f(111.191, -92.521);
			glEnd();
#endif 
#ifndef Face_outline
			glBegin(GL_LINE_LOOP);
			glVertex2f(158.489, -61.819);
			glVertex2f(168.447, -61.404);
			glVertex2f(175.915, -61.819);
			glVertex2f(185.872, -63.064);
			glVertex2f(196.660, -66.383);
			glVertex2f(204.128, -71.362);
			glVertex2f(212.011, -78.830);
			glVertex2f(216.989, -87.543);
			glVertex2f(221.968, -96.670);
			glVertex2f(224.043, -107.872);
			glVertex2f(224.872, -117.830);
			glVertex2f(223.628, -126.543);
			glVertex2f(220.309, -136.500);
			glVertex2f(212.840, -144.383);
			glVertex2f(204.957, -148.117);
			glVertex2f(193.340, -151.851);
			glVertex2f(182.138, -153.096);
			glVertex2f(165.957, -155.170);
			glVertex2f(152.266, -154.755);
			glVertex2f(136.085, -153.511);
			glVertex2f(120.734, -149.777);
			glVertex2f(107.457, -144.798);
			glVertex2f(99.989, -136.915);
			glVertex2f(96.670, -126.543);
			glVertex2f(95.426, -113.266);
			glVertex2f(99.989, -95.840);
			glVertex2f(107.457, -85.053);
			glVertex2f(117.000, -75.926);
			glVertex2f(128.617, -68.872);
			glVertex2f(142.723, -63.064);
			glEnd();
#endif 
#ifndef right_leg_outline
			glBegin(GL_LINE_LOOP);
			glVertex2f(191.266, -180.479);
			glVertex2f(199.149, -188.362);
			glVertex2f(205.372, -196.245);
			glVertex2f(206.202, -206.617);
			glVertex2f(204.957, -217.819);
			glVertex2f(201.638, -225.702);
			glVertex2f(214.915, -226.532);
			glVertex2f(224.043, -230.266);
			glVertex2f(226.947, -235.245);
			glVertex2f(190.851, -233.585);
			glVertex2f(192.511, -221.553);
			glVertex2f(189.606, -212.426);
			glVertex2f(180.894, -205.787);
			glVertex2f(182.138, -195.830);
			glEnd();
#endif
#ifndef Left_leg_outline
			glBegin(GL_LINE_LOOP);
			glVertex2f(114.511, -201.638);
			glVertex2f(118.245, -195.000);
			glVertex2f(123.223, -190.436);
			glVertex2f(131.521, -184.213);
			glVertex2f(135.255, -188.362);
			glVertex2f(138.160, -195.415);
			glVertex2f(143.553, -203.713);
			glVertex2f(145.628, -206.617);
			glVertex2f(138.989, -211.596);
			glVertex2f(136.915, -214.915);
			glVertex2f(136.915, -221.138);
			glVertex2f(140.649, -229.436);
			glVertex2f(142.309, -234.830);
			glVertex2f(133.596, -237.319);
			glVertex2f(124.053, -239.394);
			glVertex2f(114.511, -239.394);
			glVertex2f(100.404, -239.394);
			glVertex2f(107.457, -230.681);
			glVertex2f(131.521, -229.436);
			glVertex2f(126.543, -223.213);
			glVertex2f(118.660, -214.500);
			glVertex2f(115.755, -208.691);
			glEnd();
#endif
#ifndef Left_arm_outline
			glBegin(GL_LINE_STRIP);
			glVertex2f(135.255, -155.170);
			glVertex2f(120.734, -153.926);
			glVertex2f(107.457, -153.926);
			glVertex2f(97.915, -157.245);
			glVertex2f(86.298, -163.883);
			glVertex2f(79.245, -173.840);
			glVertex2f(73.851, -169.691);
			glVertex2f(67.628, -167.202);
			glVertex2f(59.745, -168.862);
			glVertex2f(56.840, -173.840);
			glVertex2f(49.787, -174.255);
			glVertex2f(42.319, -179.234);
			glVertex2f(38.170, -185.457);
			glVertex2f(37.755, -190.851);
			glVertex2f(46.883, -184.213);
			glVertex2f(58.085, -181.723);
			glVertex2f(53.106, -189.191);
			glVertex2f(52.277, -200.394);
			glVertex2f(61.819, -190.851);
			glVertex2f(67.213, -183.798);
			glVertex2f(72.606, -184.628);
			glVertex2f(74.681, -188.362);
			glVertex2f(73.436, -196.660);
			glVertex2f(75.511, -204.957);
			glVertex2f(81.734, -198.734);
			glVertex2f(83.809, -189.606);
			glVertex2f(83.809, -182.968);
			glVertex2f(88.787, -176.330);
			glVertex2f(95.840, -170.936);
			glVertex2f(104.138, -165.957);
			glVertex2f(115.340, -164.298);
			glVertex2f(126.957, -164.298);
			glVertex2f(133.181, -164.298);
			glEnd();
#endif 
#ifndef Right_arm_outline
			glBegin(GL_LINE_STRIP);
			glVertex2f(202.883, -151.436);
			glVertex2f(212.840, -150.606);
			glVertex2f(226.947, -155.585);
			glVertex2f(235.660, -161.809);
			glVertex2f(241.883, -169.691);
			glVertex2f(251.426, -168.447);
			glVertex2f(265.117, -172.181);
			glVertex2f(275.489, -179.234);
			glVertex2f(281.298, -190.021);
			glVertex2f(270.096, -185.043);
			glVertex2f(263.043, -181.723);
			glVertex2f(261.798, -183.383);
			glVertex2f(267.606, -190.021);
			glVertex2f(264.702, -203.713);
			glVertex2f(255.574, -194.585);
			glVertex2f(245.617, -189.191);
			glVertex2f(240.638, -196.660);
			glVertex2f(235.660, -191.266);
			glVertex2f(233.585, -177.160);
			glVertex2f(226.117, -168.032);
			glVertex2f(216.160, -161.809);
			glVertex2f(207.447, -160.149);
			glVertex2f(187.117, -162.638);
			glEnd();
#endif 


			glPopMatrix();
		}
	}
}

void drawPlayer()
{
	if (inGame == 1)
	{
		if (playerLife == 1)
		{


			glPushMatrix();
			glTranslatef(pPosx, pPosy, 0);
			glTranslatef(-105, -44, 0);
			glScalef(0.035f, 0.075f, 0.0f);


			// the body 
			glBegin(GL_POLYGON);
			glColor3f(1, 1, 1);
			glVertex2f(447.0f, -496.0f);
			glVertex2f(448.0f, -540.0f);
			glVertex2f(453.0f, -603.0f);
			glVertex2f(453.0f, -648.0f);
			glVertex2f(437.0f, -659.0f);
			glVertex2f(368.0f, -668.0f);
			glVertex2f(305.0f, -669.0f);
			glVertex2f(252.0f, -664.0f);
			glVertex2f(237.0f, -653.0f);
			glVertex2f(229.0f, -637.0f);
			glVertex2f(233.0f, -587.0f);
			glVertex2f(236.0f, -537.0f);
			glVertex2f(241.0f, -487.0f);
			glVertex2f(247.0f, -468.0f);
			glVertex2f(283.0f, -487.0f);
			glVertex2f(341.0f, -507.0f);
			glVertex2f(400.0f, -503.0f);
			glVertex2f(456.0f, -493.0f);
			glEnd();
			// line strip of body 
			glBegin(GL_LINE_STRIP);
			/// it will change after background
			glColor3f(0, 0, 0);
			glVertex2f(345.0f, -129.0f);
			glVertex2f(447.0f, -496.0f);
			glVertex2f(448.0f, -540.0f);
			glVertex2f(453.0f, -603.0f);
			glVertex2f(453.0f, -648.0f);
			glVertex2f(437.0f, -659.0f);
			glVertex2f(368.0f, -668.0f);
			glVertex2f(305.0f, -669.0f);
			glVertex2f(252.0f, -664.0f);
			glVertex2f(237.0f, -653.0f);
			glVertex2f(229.0f, -637.0f);
			glVertex2f(233.0f, -587.0f);
			glVertex2f(236.0f, -537.0f);
			glVertex2f(241.0f, -487.0f);
			glVertex2f(247.0f, -468.0f);
			glVertex2f(283.0f, -487.0f);
			glVertex2f(341.0f, -507.0f);
			glVertex2f(400.0f, -503.0f);
			glVertex2f(456.0f, -493.0f);
			glEnd();

			// inner the body 
			glBegin(GL_POLYGON);
			glColor3f(0.79, 0.75, 0.75);
			glVertex2f(315.0f, -507.0f);
			glVertex2f(304.0f, -547.0f);
			glVertex2f(296.0f, -617.0f);
			glVertex2f(301.0f, -657.0f);
			glVertex2f(301.0f, -665.0f);
			glVertex2f(247.0f, -655.0f);
			glVertex2f(239.0f, -547.0f);
			glVertex2f(248.0f, -480.0f);
			glEnd();

			glBegin(GL_POLYGON);
			glColor3f(0.79, 0.75, 0.75);
			glVertex2f(299.0f, -623.0f);
			glVertex2f(288.0f, -648.0f);
			glVertex2f(293.0f, -659.0f);
			glVertex2f(376.0f, -661.0f);
			glVertex2f(448.0f, -651.0f);
			glVertex2f(448.0f, -623.0f);
			glVertex2f(393.0f, -633.0f);
			glVertex2f(336.0f, -633.0f);
			glEnd();


			// the face 
			glBegin(GL_POLYGON); // Begin drawing a polygon
			glColor3f(1, 1, 1); // Set color to the desired color
			glVertex2f(345.0f, -129.0f);
			glVertex2f(295.0f, -141.0f);
			glVertex2f(231.0f, -165.0f);
			glVertex2f(183.0f, -205.0f);
			glVertex2f(160.0f, -256.0f);
			glVertex2f(160.0f, -301.0f);
			glVertex2f(155.0f, -328.0f);
			glVertex2f(168.0f, -360.0f);
			glVertex2f(192.0f, -401.0f);
			glVertex2f(212.0f, -437.0f);
			glVertex2f(259.0f, -472.0f);
			glVertex2f(336.0f, -499.0f);
			glVertex2f(325.0f, -499.0f);
			glVertex2f(397.0f, -500.0f);
			glVertex2f(452.0f, -487.0f);
			glVertex2f(499.0f, -456.0f);
			glVertex2f(521.0f, -396.0f);
			glVertex2f(527.0f, -347.0f);
			glVertex2f(507.0f, -196.0f);
			glVertex2f(457.0f, -147.0f);
			glVertex2f(379.0f, -121.0f);
			glEnd();
			// the linr of face 
			glLineWidth(5);
			glBegin(GL_LINE_LOOP);
			/// it will change after background
			glColor3f(0, 0, 0);
			glVertex2f(345.0f, -129.0f);
			glVertex2f(295.0f, -141.0f);
			glVertex2f(231.0f, -165.0f);
			glVertex2f(183.0f, -205.0f);
			glVertex2f(160.0f, -256.0f);
			glVertex2f(160.0f, -301.0f);
			glVertex2f(155.0f, -328.0f);
			glVertex2f(168.0f, -360.0f);
			glVertex2f(192.0f, -401.0f);
			glVertex2f(212.0f, -437.0f);
			glVertex2f(259.0f, -472.0f);
			glVertex2f(336.0f, -499.0f);
			glVertex2f(325.0f, -499.0f);
			glVertex2f(397.0f, -500.0f);
			glVertex2f(452.0f, -487.0f);
			glVertex2f(499.0f, -456.0f);
			glVertex2f(521.0f, -396.0f);
			glVertex2f(527.0f, -347.0f);
			glVertex2f(507.0f, -196.0f);
			glVertex2f(457.0f, -147.0f);
			glVertex2f(379.0f, -121.0f);
			glEnd();
			// inner the face 
			glBegin(GL_POLYGON);
			glColor3f(0.79, 0.75, 0.75);
			glVertex2f(236.0f, -352.0f);
			glVertex2f(219.0f, -368.0f);
			glVertex2f(192.0f, -372.0f);
			glVertex2f(191.0f, -392.0f);
			glVertex2f(209.0f, -425.0f);
			glVertex2f(239.0f, -451.0f);
			glVertex2f(279.0f, -475.0f);
			glVertex2f(312.0f, -489.0f);
			glVertex2f(340.0f, -452.0f);
			glVertex2f(307.0f, -435.0f);
			glVertex2f(273.0f, -397.0f);
			glVertex2f(260.0f, -373.0f);
			glVertex2f(255.0f, -355.0f);
			glEnd();


			glBegin(GL_POLYGON);
			glColor3f(0.79, 0.75, 0.75);
			glVertex2f(333.0f, -449.0f);
			glVertex2f(315.0f, -488.0f);
			glVertex2f(365.0f, -496.0f);
			glVertex2f(427.0f, -489.0f);
			glVertex2f(427.0f, -459.0f);
			glVertex2f(404.0f, -463.0f);
			glVertex2f(379.0f, -465.0f);
			glVertex2f(352.0f, -465.0f);
			glEnd();
			glBegin(GL_POLYGON);
			glColor3f(0.79, 0.75, 0.75);
			glVertex2f(416.0f, -461.0f);
			glVertex2f(420.0f, -488.0f);
			glVertex2f(459.0f, -477.0f);
			glVertex2f(487.0f, -451.0f);
			glVertex2f(501.0f, -436.0f);
			glVertex2f(471.0f, -445.0f);
			glVertex2f(447.0f, -455.0f);
			glEnd();

			glBegin(GL_POLYGON);
			glColor3f(0.79, 0.75, 0.75);
			glVertex2f(464.0f, -360.0f);
			glVertex2f(496.0f, -372.0f);
			glVertex2f(508.0f, -387.0f);
			glVertex2f(516.0f, -355.0f);
			glVertex2f(516.0f, -341.0f);

			glEnd();

			// above face  // triangle
			glBegin(GL_TRIANGLE_FAN);
			glColor3f(0.79, 0.75, 0.75);
			glVertex2f(237.0f, -89.0f);
			glVertex2f(240.0f, -168.0f);
			glVertex2f(325.0f, -133.0f);
			glEnd();

			glLineWidth(5);
			glBegin(GL_LINE_LOOP);
			glColor3f(0.06, 0.06, 0.06);
			glVertex2f(237.0f, -89.0f);
			glVertex2f(240.0f, -168.0f);
			glVertex2f(325.0f, -133.0f);
			glEnd();

			///  the above under triangle 
			glBegin(GL_POLYGON);
			glColor3f(0.79, 0.75, 0.75);
			glVertex2f(361.0f, -125.0f);
			glVertex2f(393.0f, -125.0f);
			glVertex2f(432.0f, -135.0f);
			glVertex2f(464.0f, -159.0f);
			glVertex2f(484.0f, -179.0f);
			glVertex2f(413.0f, -193.0f);
			glVertex2f(371.0f, -200.0f);
			glVertex2f(341.0f, -205.0f);
			glVertex2f(331.0f, -131.0f);
			glEnd();


			glBegin(GL_POLYGON);
			glColor3f(0.79, 0.75, 0.75);
			glVertex2f(341.0f, -127.0f);
			glVertex2f(296.0f, -143.0f);
			glVertex2f(253.0f, -157.0f);
			glVertex2f(217.0f, -172.0f);
			glVertex2f(280.0f, -252.0f);
			glVertex2f(313.0f, -228.0f);
			glVertex2f(341.0f, -211.0f);
			glVertex2f(383.0f, -196.0f);
			glEnd();

			glBegin(GL_POLYGON);
			glColor3f(0.79, 0.75, 0.75);
			glVertex2f(233.0f, -168.0f);
			glVertex2f(203.0f, -179.0f);
			glVertex2f(181.0f, -212.0f);
			glVertex2f(165.0f, -248.0f);
			glVertex2f(159.0f, -285.0f);
			glVertex2f(161.0f, -293.0f);
			glVertex2f(192.0f, -288.0f);
			glVertex2f(236.0f, -280.0f);
			glVertex2f(280.0f, -253.0f);
			glVertex2f(299.0f, -231.0f);
			glEnd();

			glBegin(GL_POLYGON);
			glColor3f(0.3, 0.3, 0.3);
			glVertex2f(249.0f, -179.0f);
			glVertex2f(243.0f, -199.0f);
			glVertex2f(253.0f, -207.0f);
			glVertex2f(233.0f, -223.0f);
			glVertex2f(251.0f, -233.0f);
			glColor3f(0.58, 0.56, 0.56);
			glVertex2f(289.0f, -213.0f);
			glVertex2f(295.0f, -231.0f);
			glVertex2f(236.0f, -271.0f);
			glVertex2f(203.0f, -280.0f);
			glColor3f(0.51, 0.49, 0.49);
			glVertex2f(171.0f, -283.0f);
			glVertex2f(180.0f, -239.0f);
			glVertex2f(197.0f, -208.0f);
			glVertex2f(223.0f, -184.0f);
			glEnd();

			glBegin(GL_POLYGON);
			glColor3f(1, 1, 1);
			glVertex2f(283.0f, -164.0f);
			glVertex2f(335.0f, -153.0f);
			glVertex2f(391.0f, -157.0f);
			glVertex2f(411.0f, -179.0f);
			glVertex2f(460.0f, -173.0f);
			glVertex2f(427.0f, -144.0f);
			glVertex2f(379.0f, -135.0f);
			glVertex2f(336.0f, -137.0f);
			glEnd();
			// line strip of under triangle 
			glLineWidth(5);
			glBegin(GL_LINE_STRIP);
			glColor3f(0, 0, 0);
			glVertex2f(363.0f, -125.0f);
			glVertex2f(405.0f, -127.0f);
			glVertex2f(444.0f, -147.0f);
			glVertex2f(475.0f, -173.0f);
			glVertex2f(475.0f, -184.0f);
			glVertex2f(405.0f, -192.0f);
			glVertex2f(356.0f, -204.0f);
			glVertex2f(327.0f, -217.0f);
			glVertex2f(307.0f, -235.0f);
			glVertex2f(284.0f, -249.0f);
			glVertex2f(255.0f, -273.0f);
			glVertex2f(213.0f, -285.0f);
			glVertex2f(183.0f, -288.0f);
			glVertex2f(157.0f, -299.0f);
			glVertex2f(161.0f, -255.0f);
			glVertex2f(180.0f, -215.0f);
			glVertex2f(207.0f, -185.0f);
			glVertex2f(239.0f, -168.0f);
			glVertex2f(279.0f, -149.0f);
			glVertex2f(336.0f, -131.0f);
			glEnd();

			// the tak 
			glBegin(GL_POLYGON);
			glColor3f(0.79, 0.76, 0.76);
			glVertex2f(505.0f, -184.0f);
			glVertex2f(525.0f, -211.0f);
			glVertex2f(544.0f, -251.0f);
			glVertex2f(552.0f, -285.0f);
			glVertex2f(553.0f, -312.0f);
			glVertex2f(535.0f, -328.0f);
			glVertex2f(507.0f, -337.0f);
			glVertex2f(459.0f, -348.0f);
			glColor3f(0.52, 0.52, 0.52);
			glVertex2f(401.0f, -351.0f);
			glVertex2f(329.0f, -349.0f);
			glVertex2f(272.0f, -344.0f);
			glVertex2f(231.0f, -344.0f);
			glVertex2f(231.0f, -287.0f);
			glColor3f(0.46, 0.44, 0.44);
			glVertex2f(265.0f, -263.0f);
			glVertex2f(307.0f, -233.0f);
			glVertex2f(333.0f, -211.0f);
			glVertex2f(372.0f, -200.0f);
			glVertex2f(432.0f, -187.0f);
			glVertex2f(484.0f, -183.0f);
			glEnd();

			// the line strip of tak

			glLineWidth(5);
			glBegin(GL_LINE_STRIP);
			glColor3f(0, 0, 0);
			glVertex2f(505.0f, -184.0f);
			glVertex2f(525.0f, -211.0f);
			glVertex2f(544.0f, -251.0f);
			glVertex2f(552.0f, -285.0f);
			glVertex2f(553.0f, -312.0f);
			glVertex2f(535.0f, -328.0f);
			glVertex2f(507.0f, -337.0f);
			glVertex2f(459.0f, -348.0f);
			glVertex2f(401.0f, -351.0f);
			glVertex2f(329.0f, -349.0f);
			glVertex2f(272.0f, -344.0f);
			glVertex2f(231.0f, -344.0f);
			glVertex2f(231.0f, -287.0f);
			glVertex2f(265.0f, -263.0f);
			glVertex2f(307.0f, -233.0f);
			glVertex2f(333.0f, -211.0f);
			glVertex2f(372.0f, -200.0f);
			glVertex2f(432.0f, -187.0f);
			glVertex2f(484.0f, -183.0f);
			glEnd();
			///  the red part and his line strip
			glBegin(GL_POLYGON);
			glColor3f(1, 0, 0);
			glVertex2f(217.0f, -289.0f);
			glVertex2f(237.0f, -285.0f);
			glVertex2f(252.0f, -295.0f);
			glVertex2f(256.0f, -312.0f);
			glVertex2f(253.0f, -329.0f);
			glVertex2f(240.0f, -341.0f);
			glVertex2f(216.0f, -339.0f);
			glVertex2f(201.0f, -323.0f);
			glVertex2f(201.0f, -303.0f);
			glEnd();

			glLineWidth(5);
			glBegin(GL_LINE_LOOP);
			glColor3f(0, 0, 0);
			glVertex2f(217.0f, -289.0f);
			glVertex2f(237.0f, -285.0f);
			glVertex2f(252.0f, -295.0f);
			glVertex2f(256.0f, -312.0f);
			glVertex2f(253.0f, -329.0f);
			glVertex2f(240.0f, -341.0f);
			glVertex2f(216.0f, -339.0f);
			glVertex2f(201.0f, -323.0f);
			glVertex2f(201.0f, -303.0f);
			glEnd();

			// behind red part 

			glBegin(GL_POLYGON);
			glColor3f(0.73, 0.72, 0.72);
			glVertex2f(203.0f, -295.0f);
			glVertex2f(183.0f, -292.0f);
			glVertex2f(164.0f, -300.0f);
			glVertex2f(155.0f, -317.0f);
			glColor3f(0.61, 0.6, 0.6);
			glVertex2f(155.0f, -339.0f);
			glVertex2f(163.0f, -356.0f);
			glVertex2f(181.0f, -365.0f);
			glColor3f(0.49, 0.49, 0.49);
			glVertex2f(203.0f, -360.0f);
			glVertex2f(225.0f, -345.0f);
			glVertex2f(209.0f, -329.0f);
			glEnd();
			// lie strip of the behind 
			glLineWidth(5);
			glBegin(GL_LINE_LOOP);
			glColor3f(0, 0, 0);
			glVertex2f(203.0f, -295.0f);
			glVertex2f(183.0f, -292.0f);
			glVertex2f(164.0f, -300.0f);
			glVertex2f(155.0f, -317.0f);
			glVertex2f(155.0f, -339.0f);
			glVertex2f(163.0f, -356.0f);
			glVertex2f(181.0f, -365.0f);
			glVertex2f(203.0f, -360.0f);
			glVertex2f(225.0f, -345.0f);
			glVertex2f(209.0f, -329.0f);

			glEnd();


			// legs 
			glBegin(GL_POLYGON);
			glColor3f(1, 1, 1);
			glVertex2f(405.0f, -669.0f);
			glVertex2f(376.0f, -668.0f);
			glVertex2f(384.0f, -733.0f);
			glVertex2f(391.0f, -733.0f);
			glEnd();
			/// line of lef
			glLineWidth(5);
			glBegin(GL_LINE_STRIP);
			glColor3f(0, 0, 0);
			glVertex2f(373.0f, -675.0f);
			glVertex2f(373.0f, -727.0f);
			glVertex2f(384.0f, -745.0f);
			glVertex2f(421.0f, -744.0f);
			glVertex2f(411.0f, -735.0f);
			glVertex2f(396.0f, -735.0f);
			glVertex2f(405.0f, -712.0f);
			glVertex2f(409.0f, -680.0f);
			glVertex2f(412.0f, -660.0f);
			glVertex2f(373.0f, -660.0f);
			glEnd();


			// other leg
			glBegin(GL_POLYGON);
			glColor3f(1, 1, 1);
			glVertex2f(300.0f, -674.0f);
			glVertex2f(271.0f, -673.0f);
			glVertex2f(279.0f, -738.0f);
			glVertex2f(286.0f, -738.0f);
			glEnd();

			// line of leg
			glLineWidth(5);
			glBegin(GL_LINE_STRIP);
			glColor3f(0, 0, 0);
			glVertex2f(266.0f, -677.0f);
			glVertex2f(266.0f, -729.0f);
			glVertex2f(277.0f, -747.0f);
			glVertex2f(314.0f, -746.0f);
			glVertex2f(304.0f, -737.0f);
			glVertex2f(289.0f, -737.0f);
			glVertex2f(298.0f, -714.0f);
			glVertex2f(302.0f, -682.0f);
			glVertex2f(305.0f, -662.0f);
			glVertex2f(266.0f, -662.0f);
			glEnd();

			/// the lines in black color 


			glBegin(GL_POLYGON);
			glColor3f(0.2, 0.2, 0.2);
			glVertex2f(452.0f, -232.0f);
			glVertex2f(463.0f, -237.0f);
			glVertex2f(468.0f, -248.0f);
			glVertex2f(471.0f, -269.0f);
			glVertex2f(472.0f, -296.0f);
			glColor3f(0.06, 0.06, 0.06);
			glVertex2f(469.0f, -312.0f);
			glVertex2f(465.0f, -309.0f);
			glVertex2f(465.0f, -288.0f);
			glVertex2f(461.0f, -264.0f);
			glVertex2f(457.0f, -245.0f);
			glEnd();

			glBegin(GL_POLYGON);
			glColor3f(0.06, 0.06, 0.06);
			glVertex2f(496.0f, -219.0f);
			glVertex2f(507.0f, -231.0f);
			glVertex2f(513.0f, -252.0f);
			glVertex2f(516.0f, -273.0f);
			glVertex2f(520.0f, -293.0f);
			glVertex2f(512.0f, -303.0f);
			glColor3f(0.2, 0.2, 0.2);
			glVertex2f(500.0f, -301.0f);
			glVertex2f(508.0f, -287.0f);
			glVertex2f(503.0f, -252.0f);
			glVertex2f(497.0f, -236.0f);
			glEnd();

			glBegin(GL_POLYGON);
			glColor3f(0.2, 0.2, 0.2);
			glVertex2f(415.0f, -253.0f);
			glVertex2f(423.0f, -271.0f);
			glVertex2f(427.0f, -291.0f);
			glVertex2f(427.0f, -303.0f);
			glVertex2f(420.0f, -316.0f);
			glColor3f(0.06, 0.06, 0.06);
			glVertex2f(408.0f, -316.0f);
			glVertex2f(405.0f, -305.0f);
			glVertex2f(415.0f, -301.0f);
			glVertex2f(416.0f, -285.0f);

			glEnd();
			glBegin(GL_POLYGON);
			glColor3f(0.06, 0.06, 0.06);
			glVertex2f(375.0f, -267.0f);
			glVertex2f(375.0f, -279.0f);
			glVertex2f(372.0f, -293.0f);
			glVertex2f(369.0f, -287.0f);
			glVertex2f(369.0f, -284.0f);
			glColor3f(0.2, 0.2, 0.2);
			glVertex2f(367.0f, -293.0f);
			glVertex2f(371.0f, -299.0f);
			glVertex2f(379.0f, -299.0f);
			glVertex2f(381.0f, -289.0f);
			glVertex2f(381.0f, -277.0f);
			glEnd();

			glBegin(GL_POLYGON);
			glColor3f(0.2, 0.2, 0.2);
			glVertex2f(243.0f, -201.0f);
			glVertex2f(225.0f, -209.0f);
			glVertex2f(207.0f, -220.0f);
			glColor3f(0.06, 0.06, 0.06);
			glVertex2f(212.0f, -232.0f);
			glVertex2f(224.0f, -225.0f);
			glVertex2f(240.0f, -212.0f);
			glEnd();



			glBegin(GL_POLYGON);
			glColor3f(0, 0, 0);
			glVertex2f(241.0f, -232.0f);
			glVertex2f(235.0f, -232.0f);
			glVertex2f(225.0f, -237.0f);
			glColor3f(0.2, 0.2, 0.2);
			glVertex2f(225.0f, -245.0f);
			glEnd();

			////////////////////////////////////////////////
			glTranslatef(-59, -40, 0);
			//glScalef(-1.0f,0.0f, 0.0f);
			 //glRotated(90, 0, 0,0.02);

			 // safety 
			glBegin(GL_POLYGON);
			glColor3f(0.49, 0.48, 0.48);
			glVertex2f(532.0f, -440.0f);
			glVertex2f(535.0f, -484.0f);
			glVertex2f(536.0f, -536.0f);
			glVertex2f(533.0f, -587.0f);
			glVertex2f(527.0f, -629.0f);
			glVertex2f(513.0f, -685.0f);
			glVertex2f(504.0f, -712.0f);
			glColor3f(0.51, 0.51, 0.51);
			glVertex2f(515.0f, -719.0f);
			glVertex2f(545.0f, -705.0f);
			glVertex2f(569.0f, -683.0f);
			glVertex2f(593.0f, -651.0f);
			glVertex2f(612.0f, -612.0f);
			glColor3f(0.38, 0.38, 0.38);
			glVertex2f(615.0f, -572.0f);
			glVertex2f(613.0f, -535.0f);
			glVertex2f(595.0f, -497.0f);
			glVertex2f(575.0f, -467.0f);
			glVertex2f(553.0f, -449.0f);
			glEnd();
			// the line of safety 
			glLineWidth(5);
			glBegin(GL_LINE_LOOP);
			glColor3f(0, 0, 0);
			glVertex2f(532.0f, -440.0f);
			glVertex2f(535.0f, -484.0f);
			glVertex2f(536.0f, -536.0f);
			glVertex2f(533.0f, -587.0f);
			glVertex2f(527.0f, -629.0f);
			glVertex2f(513.0f, -685.0f);
			glVertex2f(504.0f, -712.0f);
			glVertex2f(515.0f, -719.0f);
			glVertex2f(545.0f, -705.0f);
			glVertex2f(569.0f, -683.0f);
			glVertex2f(593.0f, -651.0f);
			glVertex2f(612.0f, -612.0f);
			glVertex2f(615.0f, -572.0f);
			glVertex2f(613.0f, -535.0f);
			glVertex2f(595.0f, -497.0f);
			glVertex2f(575.0f, -467.0f);
			glVertex2f(553.0f, -449.0f);
			glEnd();


			/// line 
			glLineWidth(5);
			glBegin(GL_LINE_LOOP);
			glColor3f(0, 0, 0);
			glVertex2f(548.0f, -456.0f);
			glVertex2f(556.0f, -475.0f);
			glVertex2f(561.0f, -505.0f);
			glVertex2f(561.0f, -533.0f);
			glVertex2f(560.0f, -572.0f);
			glVertex2f(555.0f, -611.0f);
			glVertex2f(549.0f, -645.0f);
			glVertex2f(541.0f, -673.0f);
			glVertex2f(533.0f, -700.0f);
			glVertex2f(519.0f, -712.0f);
			glVertex2f(511.0f, -708.0f);
			glVertex2f(517.0f, -665.0f);
			glVertex2f(529.0f, -596.0f);
			glVertex2f(533.0f, -536.0f);
			glVertex2f(532.0f, -473.0f);
			glVertex2f(533.0f, -445.0f);
			glVertex2f(537.0f, -439.0f);
			glEnd();
			// small part 
			glBegin(GL_POLYGON);
			glColor3f(0.51, 0.51, 0.51);
			glVertex2f(615.0f, -552.0f);
			glVertex2f(615.0f, -603.0f);
			glVertex2f(631.0f, -600.0f);
			glColor3f(0.49, 0.48, 0.48);
			glVertex2f(641.0f, -587.0f);
			glVertex2f(641.0f, -573.0f);
			glVertex2f(635.0f, -563.0f);
			glVertex2f(627.0f, -555.0f);
			glEnd();
			// the line  of small part 
			glLineWidth(5);
			glBegin(GL_LINE_LOOP);
			glColor3f(0, 0, 0);
			glVertex2f(615.0f, -552.0f);
			glVertex2f(615.0f, -603.0f);
			glVertex2f(631.0f, -600.0f);
			glVertex2f(641.0f, -587.0f);
			glVertex2f(641.0f, -573.0f);
			glVertex2f(635.0f, -563.0f);
			glVertex2f(627.0f, -555.0f);

			glEnd();
			/////////////////////////////////////////////////////////////
			glPopMatrix();

		}
	}

}

void drawSword()
{
	if (inGame == 1)
	{
		if (swordFlag == 1 && playerLife == 1)
		{
			glPushMatrix();
			glTranslatef(sPosx, sPosy, 0);
			glTranslatef(-90, -82, 0);
			glScalef(0.02f, 0.02f, 0.0f);



			/// the sword 
			glBegin(GL_POLYGON);
			glColor3f(0.61, 0.19, 0.1);
			glVertex2f(284.0f, -167.0f);
			glVertex2f(268.0f, -172.0f);
			glVertex2f(259.0f, -192.0f);
			glVertex2f(256.0f, -222.0f);
			glVertex2f(257.0f, -270.0f);
			glVertex2f(258.0f, -314.0f);
			glVertex2f(267.0f, -352.0f);
			glVertex2f(282.0f, -371.0f);
			glVertex2f(296.0f, -375.0f);
			glVertex2f(316.0f, -358.0f);
			glVertex2f(321.0f, -318.0f);
			glVertex2f(324.0f, -274.0f);
			glVertex2f(320.0f, -218.0f);
			glVertex2f(312.0f, -180.0f);
			glVertex2f(298.0f, -165.0f);
			glEnd();

			// the line of h
			glLineWidth(5);
			glBegin(GL_LINE_LOOP);
			glColor3f(0, 0, 0);
			glVertex2f(284.0f, -167.0f);
			glVertex2f(268.0f, -172.0f);
			glVertex2f(259.0f, -192.0f);
			glVertex2f(256.0f, -222.0f);
			glVertex2f(257.0f, -270.0f);
			glVertex2f(258.0f, -314.0f);
			glVertex2f(267.0f, -352.0f);
			glVertex2f(282.0f, -371.0f);
			glVertex2f(296.0f, -375.0f);
			glVertex2f(316.0f, -358.0f);
			glVertex2f(321.0f, -318.0f);
			glVertex2f(324.0f, -274.0f);
			glVertex2f(320.0f, -218.0f);
			glVertex2f(312.0f, -180.0f);
			glVertex2f(298.0f, -165.0f);
			glEnd();

			// the hand 
			glBegin(GL_POLYGON);
			glColor3f(0.61, 0.19, 0.1);
			glVertex2f(241.0f, -220.0f);
			glVertex2f(196.0f, -226.0f);
			glVertex2f(152.0f, -234.0f);
			glVertex2f(118.0f, -233.0f);
			glVertex2f(99.0f, -229.0f);
			glVertex2f(84.0f, -238.0f);
			glColor3f(0.89, 0.16, 0.02);
			glVertex2f(70.0f, -250.0f);
			glVertex2f(67.0f, -274.0f);
			glVertex2f(69.0f, -292.0f);
			glVertex2f(80.0f, -306.0f);
			glVertex2f(93.0f, -318.0f);
			glVertex2f(104.0f, -318.0f);
			glColor3f(0.9, 0.35, 0.24);
			glVertex2f(121.0f, -313.0f);
			glVertex2f(149.0f, -313.0f);
			glVertex2f(179.0f, -315.0f);
			glVertex2f(226.0f, -323.0f);
			glVertex2f(250.0f, -323.0f);
			glEnd();
			// the line of hand 
			glLineWidth(5);
			glBegin(GL_LINE_LOOP);
			glColor3f(0, 0, 0);
			glVertex2f(241.0f, -220.0f);
			glVertex2f(196.0f, -226.0f);
			glVertex2f(152.0f, -234.0f);
			glVertex2f(118.0f, -233.0f);
			glVertex2f(99.0f, -229.0f);
			glVertex2f(84.0f, -238.0f);
			glVertex2f(70.0f, -250.0f);
			glVertex2f(67.0f, -274.0f);
			glVertex2f(69.0f, -292.0f);
			glVertex2f(80.0f, -306.0f);
			glVertex2f(93.0f, -318.0f);
			glVertex2f(104.0f, -318.0f);
			glVertex2f(121.0f, -313.0f);
			glVertex2f(149.0f, -313.0f);
			glVertex2f(179.0f, -315.0f);
			glVertex2f(226.0f, -323.0f);
			glVertex2f(250.0f, -323.0f);
			glEnd();



			/// the lines in hand 
			glLineWidth(5);
			glBegin(GL_LINE_LOOP);
			glColor3f(0, 0, 0);
			glVertex2f(145.0f, -239.0f);
			glVertex2f(148.0f, -313.0f);
			glVertex2f(92.0f, -312.0f);
			glVertex2f(76.0f, -294.0f);
			glVertex2f(75.0f, -262.0f);
			glVertex2f(89.0f, -242.0f);
			glVertex2f(120.0f, -238.0f);
			glEnd();
			//
			glLineWidth(5);
			glBegin(GL_LINE_LOOP);
			glColor3f(0, 0, 0);
			glVertex2f(145.0f, -238.0f);
			glVertex2f(149.0f, -304.0f);
			glVertex2f(178.0f, -316.0f);
			glVertex2f(195.0f, -307.0f);
			glVertex2f(194.0f, -236.0f);

			glEnd();//

			///
			glLineWidth(5);
			glBegin(GL_LINE_LOOP);
			glColor3f(0, 0, 0);
			glVertex2f(222.0f, -222.0f);
			glVertex2f(192.0f, -225.0f);
			glVertex2f(196.0f, -307.0f);
			glVertex2f(215.0f, -322.0f);
			glVertex2f(244.0f, -313.0f);
			glVertex2f(245.0f, -225.0f);

			glEnd();
			// the front of sword 
			glBegin(GL_POLYGON);
			glColor3f(0.75, 0.76, 0.75);
			glVertex2f(339.0f, -219.0f);
			glVertex2f(775.0f, -152.0f);
			glVertex2f(976.0f, -265.0f);
			glVertex2f(777.0f, -382.0f);
			glVertex2f(331.0f, -320.0f);
			glVertex2f(328.0f, -218.0f);
			glEnd();

			glLineWidth(5);
			glBegin(GL_LINE_LOOP);
			glColor3f(0, 0, 0);
			glVertex2f(339.0f, -219.0f);
			glVertex2f(775.0f, -152.0f);
			glVertex2f(976.0f, -265.0f);
			glVertex2f(777.0f, -382.0f);
			glVertex2f(331.0f, -320.0f);
			glVertex2f(328.0f, -218.0f);
			glEnd();
			// inter the sword 
			glBegin(GL_POLYGON);
			glColor3f(0.45, 0.46, 0.46);
			glVertex2f(361.0f, -260.0f);
			glColor3f(0.66, 0.67, 0.67);
			glVertex2f(949.0f, -265.0f);
			glColor3f(0.56, 0.57, 0.56);
			glVertex2f(776.0f, -365.0f);
			glColor3f(0.77, 0.78, 0.77);
			glVertex2f(352.0f, -299.0f);
			glColor3f(0.47, 0.47, 0.47);
			glVertex2f(356.0f, -278.0f);

			glEnd();

			// the black part 
			glBegin(GL_POLYGON);
			glColor3f(0.11, 0.11, 0.11);
			glVertex2f(492.0f, -262.0f);
			glColor3f(0.45, 0.46, 0.45);
			glVertex2f(497.0f, -282.0f);
			glColor3f(0.21, 0.21, 0.21);
			glVertex2f(832.0f, -275.0f);
			glColor3f(0.58, 0.58, 0.58);
			glVertex2f(827.0f, -258.0f);

			glEnd();
			glBegin(GL_POLYGON);
			glColor3f(0.21, 0.21, 0.21);
			glVertex2f(345.0f, -250.0f);
			glColor3f(0.45, 0.46, 0.45);
			glVertex2f(345.0f, -271.0f);
			glColor3f(0.11, 0.11, 0.11);
			glVertex2f(395.0f, -270.0f);
			glColor3f(0.58, 0.58, 0.58);
			glVertex2f(396.0f, -261.0f);

			glEnd();

			glPopMatrix();
		}
	}
}

void drawDABOSS()
{
	if (inGame == 1)
	{
		if (bossLife == 1 && roomsCount == 5)
		{
			glPushMatrix();
#ifndef Healthbar
			//HEALTHBAR
			glBegin(GL_QUADS);
			glColor3f(bossHP[9][0], bossHP[9][1], bossHP[9][2]);
			glVertex2f(15, 52.5);
			glVertex2f(15, 47.5);
			glVertex2f(20, 47.5);
			glVertex2f(20, 52.5);
			glEnd();
			glBegin(GL_QUADS);
			glColor3f(bossHP[8][0], bossHP[8][1], bossHP[9][2]);
			glVertex2f(20, 52.5);
			glVertex2f(20, 47.5);
			glVertex2f(25, 47.5);
			glVertex2f(25, 52.5);
			glEnd();
			glBegin(GL_QUADS);
			glColor3f(bossHP[7][0], bossHP[7][1], bossHP[7][2]);
			glVertex2f(25, 52.5);
			glVertex2f(25, 47.5);
			glVertex2f(30, 47.5);
			glVertex2f(30, 52.5);
			glEnd();
			glBegin(GL_QUADS);
			glColor3f(bossHP[6][0], bossHP[6][1], bossHP[6][2]);
			glVertex2f(30, 52.5);
			glVertex2f(30, 47.5);
			glVertex2f(35, 47.5);
			glVertex2f(35, 52.5);
			glEnd();
			glBegin(GL_QUADS);
			glColor3f(bossHP[5][0], bossHP[5][1], bossHP[5][2]);
			glVertex2f(35, 52.5);
			glVertex2f(35, 47.5);
			glVertex2f(40, 47.5);
			glVertex2f(40, 52.5);
			glEnd();
			glBegin(GL_QUADS);
			glColor3f(bossHP[4][0], bossHP[4][1], bossHP[4][2]);
			glVertex2f(40, 52.5);
			glVertex2f(40, 47.5);
			glVertex2f(45, 47.5);
			glVertex2f(45, 52.5);
			glEnd();
			glBegin(GL_QUADS);
			glColor3f(bossHP[3][0], bossHP[3][1], bossHP[3][2]);
			glVertex2f(45, 52.5);
			glVertex2f(45, 47.5);
			glVertex2f(50, 47.5);
			glVertex2f(50, 52.5);
			glEnd();
			glBegin(GL_QUADS);
			glColor3f(bossHP[2][0], bossHP[2][1], bossHP[2][2]);
			glVertex2f(50, 52.5);
			glVertex2f(50, 47.5);
			glVertex2f(55, 47.5);
			glVertex2f(55, 52.5);
			glEnd();
			glBegin(GL_QUADS);
			glColor3f(bossHP[1][0], bossHP[1][1], bossHP[1][2]);
			glVertex2f(55, 52.5);
			glVertex2f(55, 47.5);
			glVertex2f(60, 47.5);
			glVertex2f(60, 52.5);
			glEnd();
			glBegin(GL_QUADS);
			glColor3f(bossHP[0][0], bossHP[0][1], bossHP[0][2]);
			glVertex2f(60, 52.5);
			glVertex2f(60, 47.5);
			glVertex2f(65, 47.5);
			glVertex2f(65, 52.5);
			glEnd();
#endif 
			glTranslatef(-15, 65, 0);
			glScalef(1.0f, 1.3f, 0.0f);
			//THE BOSS SHAPE
#ifndef Left_Horn_shape
			glColor3f(0.92, 0.15, 0.17);
			glBegin(GL_POLYGON);
			glVertex2f(26.680, -37.232);
			glVertex2f(24.091, -31.458);
			glVertex2f(23.096, -26.281);
			glVertex2f(22.698, -20.109);
			glVertex2f(26.281, -12.942);
			glVertex2f(27.476, -18.317);
			glVertex2f(30.463, -25.286);
			glVertex2f(34.644, -30.463);
			glVertex2f(39.621, -34.246);
			glVertex2f(45.793, -39.422);
			glVertex2f(40.816, -41.612);
			glVertex2f(36.635, -45.395);
			glVertex2f(32.852, -42.608);
			glVertex2f(29.268, -39.422);
			glEnd();
#endif 
#ifndef Right_Horn_shape
			glBegin(GL_POLYGON);
			glVertex2f(92.184, -33.250);
			glVertex2f(94.573, -28.870);
			glVertex2f(94.772, -21.702);
			glVertex2f(93.578, -15.729);
			glVertex2f(91.587, -12.344);
			glVertex2f(90.193, -18.516);
			glVertex2f(88.401, -23.693);
			glVertex2f(86.012, -26.481);
			glVertex2f(81.433, -30.861);
			glVertex2f(73.867, -37.232);
			glVertex2f(79.442, -40.418);
			glVertex2f(83.025, -42.409);
			glVertex2f(87.406, -38.825);
			glEnd();
#endif 
#ifndef Left_leg_shape
			glBegin(GL_POLYGON);
			glVertex2f(30.463, -104.528);
			glVertex2f(34.246, -101.144);
			glVertex2f(37.232, -98.954);
			glVertex2f(40.019, -97.560);
			glVertex2f(44.201, -101.542);
			glVertex2f(48.581, -103.732);
			glVertex2f(48.382, -106.918);
			glVertex2f(48.183, -110.103);
			glVertex2f(47.784, -114.484);
			glVertex2f(47.984, -119.859);
			glVertex2f(47.784, -121.850);
			glVertex2f(45.793, -123.045);
			glVertex2f(42.210, -121.054);
			glVertex2f(37.630, -118.665);
			glVertex2f(33.648, -116.873);
			glVertex2f(29.069, -116.873);
			glVertex2f(26.082, -117.271);
			glVertex2f(26.082, -113.289);
			glVertex2f(27.277, -110.302);
			glEnd();
#endif 
#ifndef Right_Leg_shape
			glBegin(GL_POLYGON);
			glVertex2f(74.066, -111.099);
			glVertex2f(73.469, -106.918);
			glVertex2f(72.871, -103.135);
			glVertex2f(75.858, -101.542);
			glVertex2f(79.641, -98.356);
			glVertex2f(83.623, -94.972);
			glVertex2f(86.410, -97.958);
			glVertex2f(89.795, -100.945);
			glVertex2f(92.383, -104.528);
			glVertex2f(94.972, -108.909);
			glVertex2f(96.166, -114.284);
			glVertex2f(96.365, -117.271);
			glVertex2f(92.981, -117.072);
			glVertex2f(88.600, -117.072);
			glVertex2f(83.424, -118.067);
			glVertex2f(78.446, -119.461);
			glVertex2f(75.061, -121.850);
			glVertex2f(74.464, -118.266);
			glVertex2f(74.265, -113.687);
			glEnd();
#endif 
#ifndef Full_body_shape
			glBegin(GL_POLYGON);
			glVertex2f(41.214, -41.413);
			glVertex2f(46.590, -38.825);
			glVertex2f(51.169, -36.436);
			glVertex2f(56.744, -35.241);
			glVertex2f(63.115, -34.644);
			glVertex2f(68.889, -35.639);
			glVertex2f(74.663, -37.033);
			glVertex2f(78.844, -39.820);
			glVertex2f(84.021, -42.807);
			glVertex2f(86.211, -45.793);
			glVertex2f(90.790, -51.766);
			glVertex2f(94.175, -59.531);
			glVertex2f(95.569, -64.509);
			glVertex2f(96.564, -73.070);
			glVertex2f(94.972, -79.641);
			glVertex2f(90.790, -86.410);
			glVertex2f(87.406, -90.990);
			glVertex2f(82.030, -96.763);
			glVertex2f(76.455, -100.945);
			glVertex2f(70.084, -105.126);
			glVertex2f(64.509, -106.519);
			glVertex2f(59.731, -106.719);
			glVertex2f(52.961, -105.922);
			glVertex2f(46.789, -103.334);
			glVertex2f(41.015, -98.555);
			glVertex2f(35.042, -93.379);
			glVertex2f(31.060, -87.605);
			glVertex2f(27.078, -81.034);
			glVertex2f(25.684, -73.469);
			glVertex2f(25.485, -66.898);
			glVertex2f(26.680, -61.921);
			glVertex2f(29.666, -55.151);
			glVertex2f(33.051, -50.174);
			glVertex2f(36.635, -45.196);
			glEnd();
#endif 
#ifndef Lower_body_shape
			glColor3f(0.38, 0.12, 0.3);
			glBegin(GL_POLYGON);
			glVertex2f(58.138, -106.918);
			glVertex2f(50.771, -104.927);
			glVertex2f(44.400, -101.542);
			glVertex2f(38.626, -96.365);
			glVertex2f(33.250, -91.388);
			glVertex2f(30.463, -85.813);
			glVertex2f(26.680, -80.437);
			glVertex2f(25.485, -72.075);
			glVertex2f(29.069, -76.853);
			glVertex2f(33.449, -80.636);
			glVertex2f(38.626, -82.826);
			glVertex2f(44.400, -83.822);
			glVertex2f(48.183, -84.220);
			glVertex2f(52.762, -86.808);
			glVertex2f(58.934, -87.406);
			glVertex2f(65.106, -87.207);
			glVertex2f(70.283, -85.216);
			glVertex2f(75.460, -83.623);
			glVertex2f(81.632, -83.025);
			glVertex2f(87.207, -81.234);
			glVertex2f(92.383, -78.446);
			glVertex2f(95.967, -75.858);
			glVertex2f(94.772, -80.238);
			glVertex2f(92.383, -84.220);
			glVertex2f(88.999, -88.799);
			glVertex2f(85.216, -93.578);
			glVertex2f(80.835, -97.759);
			glVertex2f(74.862, -101.940);
			glVertex2f(71.079, -104.329);
			glVertex2f(65.704, -106.320);
			glEnd();
#endif 
#ifndef Left_foot_shape
			glBegin(GL_POLYGON);
			glVertex2f(25.883, -117.072);
			glVertex2f(34.046, -116.873);
			glVertex2f(39.621, -120.058);
			glVertex2f(45.993, -123.045);
			glVertex2f(42.608, -124.837);
			glVertex2f(37.033, -126.031);
			glVertex2f(33.051, -126.231);
			glVertex2f(27.476, -126.031);
			glVertex2f(23.096, -124.837);
			glVertex2f(22.299, -123.045);
			glVertex2f(23.295, -120.855);
			glVertex2f(25.286, -119.063);
			glEnd();
#endif
#ifndef Right_Foot_shape
			glBegin(GL_POLYGON);
			glVertex2f(84.817, -117.868);
			glVertex2f(88.600, -117.072);
			glVertex2f(96.166, -117.271);
			glVertex2f(96.963, -118.864);
			glVertex2f(100.746, -121.452);
			glVertex2f(100.746, -123.443);
			glVertex2f(97.759, -124.638);
			glVertex2f(93.379, -125.036);
			glVertex2f(89.795, -124.638);
			glVertex2f(88.401, -121.850);
			glVertex2f(90.193, -125.434);
			glVertex2f(86.012, -125.633);
			glVertex2f(82.030, -125.434);
			glVertex2f(78.844, -124.439);
			glVertex2f(75.061, -122.049);
			glVertex2f(78.446, -119.461);
			glEnd();
#endif 
#ifndef Right_arm_shape
			glBegin(GL_POLYGON);
			glVertex2f(113.488, -75.858);
			glVertex2f(113.886, -80.636);
			glVertex2f(113.886, -85.813);
			glVertex2f(112.293, -89.596);
			glVertex2f(108.510, -93.578);
			glVertex2f(104.528, -95.370);
			glVertex2f(105.922, -90.392);
			glVertex2f(105.723, -85.614);
			glVertex2f(103.135, -81.831);
			glVertex2f(98.157, -79.243);
			glVertex2f(95.370, -78.645);
			glVertex2f(96.166, -74.663);
			glVertex2f(96.365, -69.686);
			glVertex2f(95.768, -64.310);
			glVertex2f(94.772, -60.527);
			glVertex2f(100.148, -62.319);
			glVertex2f(105.126, -65.106);
			glVertex2f(109.108, -68.889);
			glVertex2f(112.094, -72.871);
			glEnd();
#endif 
#ifndef Left_Arm_shape
			glBegin(GL_POLYGON);
			glVertex2f(8.561, -84.618);
			glVertex2f(8.561, -80.437);
			glVertex2f(9.358, -77.650);
			glVertex2f(10.951, -74.265);
			glVertex2f(13.937, -70.283);
			glVertex2f(18.716, -66.699);
			glVertex2f(22.499, -64.111);
			glVertex2f(26.082, -63.314);
			glVertex2f(25.485, -68.491);
			glVertex2f(25.485, -73.070);
			glVertex2f(26.680, -80.636);
			glVertex2f(23.295, -80.636);
			glVertex2f(19.910, -82.229);
			glVertex2f(17.123, -85.216);
			glVertex2f(15.928, -89.198);
			glVertex2f(16.924, -94.175);
			glVertex2f(20.507, -98.555);
			glVertex2f(16.525, -97.759);
			glVertex2f(12.145, -93.777);
			glVertex2f(9.756, -88.999);
			glEnd();
#endif 
#ifndef Left_Eye_shape
			glColor3f(0.35, 0.77, 0.8);
			glBegin(GL_POLYGON);
			glVertex2f(34.046, -63.713);
			glVertex2f(34.246, -60.726);
			glVertex2f(35.639, -57.939);
			glVertex2f(38.228, -56.943);
			glVertex2f(41.612, -57.540);
			glVertex2f(44.400, -60.328);
			glVertex2f(47.984, -64.907);
			glVertex2f(51.169, -68.889);
			glVertex2f(53.359, -71.478);
			glVertex2f(51.169, -73.070);
			glVertex2f(47.187, -74.464);
			glVertex2f(44.001, -74.663);
			glVertex2f(40.418, -74.663);
			glVertex2f(37.033, -71.876);
			glVertex2f(34.644, -69.287);
			glEnd();
#endif 
#ifndef Right_Eye_shape
			glBegin(GL_POLYGON);
			glVertex2f(86.211, -62.319);
			glVertex2f(86.808, -67.695);
			glVertex2f(85.415, -71.079);
			glVertex2f(82.030, -74.265);
			glVertex2f(78.247, -75.061);
			glVertex2f(73.867, -75.061);
			glVertex2f(69.487, -74.066);
			glVertex2f(65.704, -71.478);
			glVertex2f(67.695, -68.690);
			glVertex2f(71.677, -64.310);
			glVertex2f(75.260, -59.930);
			glVertex2f(78.645, -56.943);
			glVertex2f(82.428, -56.943);
			glVertex2f(85.016, -58.735);
			glEnd();
#endif  
#ifndef Left_Arm_shading
			glColor3f(0.92, 0.15, 0.17);
			glBegin(GL_POLYGON);
			glVertex2f(14.335, -69.686);
			glVertex2f(18.716, -66.699);
			glVertex2f(22.698, -64.111);
			glVertex2f(26.481, -62.717);
			glVertex2f(25.684, -66.898);
			glVertex2f(25.684, -70.482);
			glVertex2f(21.304, -73.469);
			glVertex2f(16.525, -76.654);
			glVertex2f(13.738, -81.034);
			glVertex2f(12.743, -85.415);
			glVertex2f(11.946, -88.799);
			glVertex2f(9.955, -82.826);
			glVertex2f(9.756, -79.243);
			glVertex2f(10.353, -75.460);
			glVertex2f(12.344, -72.075);
			glEnd();
#endif 
#ifndef Right_Arm_Shading
			glBegin(GL_POLYGON);
			glVertex2f(107.117, -66.898);
			glVertex2f(110.103, -70.681);
			glVertex2f(112.891, -75.659);
			glVertex2f(112.692, -82.030);
			glVertex2f(112.094, -84.419);
			glVertex2f(109.108, -78.645);
			glVertex2f(105.126, -74.265);
			glVertex2f(100.148, -69.686);
			glVertex2f(95.967, -67.894);
			glVertex2f(95.171, -62.916);
			glVertex2f(94.772, -60.129);
			glVertex2f(99.949, -62.916);
			glEnd();
#endif 
#ifndef Lower_body_shading
			glColor3f(0.47, 0.18, 0.51);
			glBegin(GL_POLYGON);
			glVertex2f(54.156, -102.139);
			glVertex2f(59.133, -102.139);
			glVertex2f(64.907, -100.148);
			glVertex2f(72.672, -95.768);
			glVertex2f(80.636, -90.790);
			glVertex2f(88.799, -84.419);
			glVertex2f(91.388, -79.243);
			glVertex2f(85.614, -81.831);
			glVertex2f(80.238, -83.424);
			glVertex2f(74.862, -83.822);
			glVertex2f(67.496, -86.012);
			glVertex2f(65.106, -87.406);
			glVertex2f(57.939, -87.804);
			glVertex2f(51.169, -86.808);
			glVertex2f(48.581, -84.618);
			glVertex2f(42.010, -83.424);
			glVertex2f(35.838, -82.229);
			glVertex2f(31.657, -79.442);
			glVertex2f(25.485, -73.070);
			glVertex2f(26.481, -77.650);
			glVertex2f(27.675, -82.627);
			glVertex2f(30.861, -86.609);
			glVertex2f(34.445, -90.392);
			glVertex2f(39.024, -94.573);
			glVertex2f(44.001, -97.958);
			glVertex2f(47.984, -101.144);
			glEnd();
#endif
			//Starting from here are all outlines and Black
#ifndef Connecting_line
			glColor3f(0, 0, 0);
			glLineWidth(4);
			glBegin(GL_LINE_STRIP);
			glVertex2f(53.160, -71.478);
			glVertex2f(66.301, -71.478);
			glEnd();
#endif 
#ifndef first_lower_line
			glBegin(GL_LINE_STRIP);
			glVertex2f(57.540, -71.677);
			glVertex2f(57.540, -76.853);
			glEnd();
#endif 
#ifndef Second_lower_line
			glBegin(GL_LINE_STRIP);
			glVertex2f(62.319, -71.677);
			glVertex2f(62.319, -77.650);
			glEnd();
#endif 
#ifndef Left_Horn_outline
			glBegin(GL_LINE_LOOP);
			glVertex2f(26.680, -37.232);
			glVertex2f(24.091, -31.458);
			glVertex2f(23.096, -26.281);
			glVertex2f(22.698, -20.109);
			glVertex2f(26.281, -12.942);
			glVertex2f(27.476, -18.317);
			glVertex2f(30.463, -25.286);
			glVertex2f(34.644, -30.463);
			glVertex2f(39.621, -34.246);
			glVertex2f(45.793, -39.422);
			glVertex2f(40.816, -41.612);
			glVertex2f(36.635, -45.395);
			glVertex2f(32.852, -42.608);
			glVertex2f(29.268, -39.422);
			glEnd();
#endif 
#ifndef Right_Horn_outline
			glBegin(GL_LINE_LOOP);
			glVertex2f(92.184, -33.250);
			glVertex2f(94.573, -28.870);
			glVertex2f(94.772, -21.702);
			glVertex2f(93.578, -15.729);
			glVertex2f(91.587, -12.344);
			glVertex2f(90.193, -18.516);
			glVertex2f(88.401, -23.693);
			glVertex2f(86.012, -26.481);
			glVertex2f(81.433, -30.861);
			glVertex2f(73.867, -37.232);
			glVertex2f(79.442, -40.418);
			glVertex2f(83.025, -42.409);
			glVertex2f(87.406, -38.825);
			glEnd();
#endif 
#ifndef Right_arm_outline
			glBegin(GL_LINE_LOOP);
			glVertex2f(113.488, -75.858);
			glVertex2f(113.886, -80.636);
			glVertex2f(113.886, -85.813);
			glVertex2f(112.293, -89.596);
			glVertex2f(108.510, -93.578);
			glVertex2f(104.528, -95.370);
			glVertex2f(105.922, -90.392);
			glVertex2f(105.723, -85.614);
			glVertex2f(103.135, -81.831);
			glVertex2f(98.157, -79.243);
			glVertex2f(95.370, -78.645);
			glVertex2f(96.166, -74.663);
			glVertex2f(96.365, -69.686);
			glVertex2f(95.768, -64.310);
			glVertex2f(94.772, -60.527);
			glVertex2f(100.148, -62.319);
			glVertex2f(105.126, -65.106);
			glVertex2f(109.108, -68.889);
			glVertex2f(112.094, -72.871);
			glEnd();
#endif 
#ifndef Left_Arm_outline
			glBegin(GL_LINE_LOOP);
			glVertex2f(9.358, -77.650);
			glVertex2f(10.951, -74.265);
			glVertex2f(13.937, -70.283);
			glVertex2f(18.716, -66.699);
			glVertex2f(22.499, -64.111);
			glVertex2f(26.082, -63.314);
			glVertex2f(25.485, -68.491);
			glVertex2f(25.485, -73.070);
			glVertex2f(26.680, -80.636);
			glVertex2f(23.295, -80.636);
			glVertex2f(19.910, -82.229);
			glVertex2f(17.123, -85.216);
			glVertex2f(15.928, -89.198);
			glVertex2f(16.924, -94.175);
			glVertex2f(20.507, -98.555);
			glVertex2f(16.525, -97.759);
			glVertex2f(12.145, -93.777);
			glVertex2f(9.756, -88.999);
			glVertex2f(8.561, -84.618);
			glVertex2f(8.561, -80.437);
			glEnd();
#endif 
#ifndef Left_leg_outline
			glBegin(GL_LINE_LOOP);
			glVertex2f(30.463, -104.528);
			glVertex2f(34.246, -101.144);
			glVertex2f(37.232, -98.954);
			glVertex2f(40.019, -97.560);
			glVertex2f(44.201, -101.542);
			glVertex2f(48.581, -103.732);
			glVertex2f(48.382, -106.918);
			glVertex2f(48.183, -110.103);
			glVertex2f(47.784, -114.484);
			glVertex2f(47.984, -119.859);
			glVertex2f(47.784, -121.850);
			glVertex2f(45.793, -123.045);
			glVertex2f(42.210, -121.054);
			glVertex2f(37.630, -118.665);
			glVertex2f(33.648, -116.873);
			glVertex2f(29.069, -116.873);
			glVertex2f(26.082, -117.271);
			glVertex2f(26.082, -113.289);
			glVertex2f(27.277, -110.302);
			glEnd();
#endif
#ifndef Left_foot_outline
			glBegin(GL_LINE_LOOP);
			glVertex2f(25.883, -117.072);
			glVertex2f(34.046, -116.873);
			glVertex2f(39.621, -120.058);
			glVertex2f(45.993, -123.045);
			glVertex2f(42.608, -124.837);
			glVertex2f(37.033, -126.031);
			glVertex2f(33.051, -126.231);
			glVertex2f(27.476, -126.031);
			glVertex2f(23.096, -124.837);
			glVertex2f(22.299, -123.045);
			glVertex2f(23.295, -120.855);
			glVertex2f(25.286, -119.063);
			glEnd();
#endif 
#ifndef Right_Leg_outline
			glBegin(GL_LINE_LOOP);
			glVertex2f(74.066, -111.099);
			glVertex2f(73.469, -106.918);
			glVertex2f(72.871, -103.135);
			glVertex2f(75.858, -101.542);
			glVertex2f(79.641, -98.356);
			glVertex2f(83.623, -94.972);
			glVertex2f(86.410, -97.958);
			glVertex2f(89.795, -100.945);
			glVertex2f(92.383, -104.528);
			glVertex2f(94.972, -108.909);
			glVertex2f(96.166, -114.284);
			glVertex2f(96.365, -117.271);
			glVertex2f(92.981, -117.072);
			glVertex2f(88.600, -117.072);
			glVertex2f(83.424, -118.067);
			glVertex2f(78.446, -119.461);
			glVertex2f(75.061, -121.850);
			glVertex2f(74.464, -118.266);
			glVertex2f(74.265, -113.687);
			glEnd();
#endif 
#ifndef Right_Foot_outline
			glBegin(GL_LINE_LOOP);
			glVertex2f(84.817, -117.868);
			glVertex2f(88.600, -117.072);
			glVertex2f(96.166, -117.271);
			glVertex2f(96.963, -118.864);
			glVertex2f(100.746, -121.452);
			glVertex2f(100.746, -123.443);
			glVertex2f(97.759, -124.638);
			glVertex2f(93.379, -125.036);
			glVertex2f(89.795, -124.638);
			glVertex2f(88.401, -121.850);
			glVertex2f(90.193, -125.434);
			glVertex2f(86.012, -125.633);
			glVertex2f(82.030, -125.434);
			glVertex2f(78.844, -124.439);
			glVertex2f(75.061, -122.049);
			glVertex2f(78.446, -119.461);
			glEnd();
#endif 
#ifndef Full_body_outline
			glBegin(GL_LINE_LOOP);
			glVertex2f(41.214, -41.413);
			glVertex2f(46.590, -38.825);
			glVertex2f(51.169, -36.436);
			glVertex2f(56.744, -35.241);
			glVertex2f(63.115, -34.644);
			glVertex2f(68.889, -35.639);
			glVertex2f(74.663, -37.033);
			glVertex2f(78.844, -39.820);
			glVertex2f(84.021, -42.807);
			glVertex2f(86.211, -45.793);
			glVertex2f(90.790, -51.766);
			glVertex2f(94.175, -59.531);
			glVertex2f(95.569, -64.509);
			glVertex2f(96.564, -73.070);
			glVertex2f(94.972, -79.641);
			glVertex2f(90.790, -86.410);
			glVertex2f(87.406, -90.990);
			glVertex2f(82.030, -96.763);
			glVertex2f(76.455, -100.945);
			glVertex2f(70.084, -105.126);
			glVertex2f(64.509, -106.519);
			glVertex2f(59.731, -106.719);
			glVertex2f(52.961, -105.922);
			glVertex2f(46.789, -103.334);
			glVertex2f(41.015, -98.555);
			glVertex2f(35.042, -93.379);
			glVertex2f(31.060, -87.605);
			glVertex2f(27.078, -81.034);
			glVertex2f(25.684, -73.469);
			glVertex2f(25.485, -66.898);
			glVertex2f(26.680, -61.921);
			glVertex2f(29.666, -55.151);
			glVertex2f(33.051, -50.174);
			glVertex2f(36.635, -45.196);
			glEnd();
#endif 
#ifndef Lower_body_outline
			glBegin(GL_LINE_LOOP);
			glVertex2f(25.485, -72.075);
			glVertex2f(29.069, -76.853);
			glVertex2f(33.449, -80.636);
			glVertex2f(38.626, -82.826);
			glVertex2f(44.400, -83.822);
			glVertex2f(48.183, -84.220);
			glVertex2f(52.762, -86.808);
			glVertex2f(58.934, -87.406);
			glVertex2f(65.106, -87.207);
			glVertex2f(70.283, -85.216);
			glVertex2f(75.460, -83.623);
			glVertex2f(81.632, -83.025);
			glVertex2f(87.207, -81.234);
			glVertex2f(92.383, -78.446);
			glVertex2f(95.967, -75.858);
			glVertex2f(94.772, -80.238);
			glVertex2f(92.383, -84.220);
			glVertex2f(88.999, -88.799);
			glVertex2f(85.216, -93.578);
			glVertex2f(80.835, -97.759);
			glVertex2f(74.862, -101.940);
			glVertex2f(71.079, -104.329);
			glVertex2f(65.704, -106.320);
			glVertex2f(58.138, -106.918);
			glVertex2f(50.771, -104.927);
			glVertex2f(44.400, -101.542);
			glVertex2f(38.626, -96.365);
			glVertex2f(33.250, -91.388);
			glVertex2f(30.463, -85.813);
			glVertex2f(26.680, -80.437);
			glEnd();
#endif 
#ifndef Right_eye_outline
			glBegin(GL_LINE_LOOP);
			glVertex2f(86.211, -62.319);
			glVertex2f(86.808, -67.695);
			glVertex2f(85.415, -71.079);
			glVertex2f(82.030, -74.265);
			glVertex2f(78.247, -75.061);
			glVertex2f(73.867, -75.061);
			glVertex2f(69.487, -74.066);
			glVertex2f(65.704, -71.478);
			glVertex2f(67.695, -68.690);
			glVertex2f(71.677, -64.310);
			glVertex2f(75.260, -59.930);
			glVertex2f(78.645, -56.943);
			glVertex2f(82.428, -56.943);
			glVertex2f(85.016, -58.735);
			glEnd();
#endif 
#ifndef Left_eye_outline
			glBegin(GL_LINE_LOOP);
			glVertex2f(34.046, -63.713);
			glVertex2f(34.246, -60.726);
			glVertex2f(35.639, -57.939);
			glVertex2f(38.228, -56.943);
			glVertex2f(41.612, -57.540);
			glVertex2f(44.400, -60.328);
			glVertex2f(47.984, -64.907);
			glVertex2f(51.169, -68.889);
			glVertex2f(53.359, -71.478);
			glVertex2f(51.169, -73.070);
			glVertex2f(47.187, -74.464);
			glVertex2f(44.001, -74.663);
			glVertex2f(40.418, -74.663);
			glVertex2f(37.033, -71.876);
			glVertex2f(34.644, -69.287);
			glEnd();
#endif

			glPopMatrix();
		}
	}
}

void stateControl() {
	if (currentState == MAIN_MENU) {
		glClear(GL_COLOR_BUFFER_BIT);
		drawMainMenu();
	}
	else if (currentState == START_GAME) {
		glClear(GL_COLOR_BUFFER_BIT);
		OnDisplay();
		glutSwapBuffers();
	}
	else if (currentState == GAME_OVER) {
		glClear(GL_COLOR_BUFFER_BIT);
		gameOver();
		glutSwapBuffers();
	}
	else if (currentState == INSTRUCTIONS) {
		glClear(GL_COLOR_BUFFER_BIT);
		displayInstructions();
		glutSwapBuffers();
	}
	else if (currentState == WIN) {
		glClear(GL_COLOR_BUFFER_BIT);
		win();
		glutSwapBuffers();
	}
}

void displayInstructions() {
	glColor3f(0, 1, 0);
	glRasterPos2f(-90, 90);
	string instructionsText = "Instructions on how to play\n";
	for (char c : instructionsText) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
	}

	glColor3f(1, 1, 1);
	glRasterPos2f(-90, 80);
	string instructionsText1 = "A - moves the player to the left\n";
	for (char c : instructionsText1) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
	}

	glColor3f(1, 1, 1);
	glRasterPos2f(-90, 70);
	string instructionsText2 = "D- moves the player to the right \n";
	for (char c : instructionsText2) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
	}

	glColor3f(1, 1, 1);
	glRasterPos2f(-90, 60);
	string instructionsText3 = "Right and Left buttons - for player movement\n";
	for (char c : instructionsText3) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
	}

	glColor3f(1, 1, 1);
	glRasterPos2f(-90, 50);
	string instructionsText4 = "Spacebar - Used to make the character jump\n";
	for (char c : instructionsText4) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
	}

	glColor3f(1, 1, 1);
	glRasterPos2f(-90, 40);
	string instructionsText5 = "Q, E, R - Used to choose powerups\n";
	for (char c : instructionsText5) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
	}

	glColor3f(1, 1, 1);
	glRasterPos2f(-90, 30);
	string instructionsText6 = "Left click on the mouse - Sword appears\n";
	for (char c : instructionsText6) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
	}

	glColor3f(0, 1, 0);
	glRasterPos2f(-90, 10);
	string instructionsText7 = "There are Five rooms:\n";
	for (char c : instructionsText7) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
	}

	glColor3f(1, 1, 1);
	glRasterPos2f(-90, 0);
	string instructionsText8 = "Each of the first four rooms have 2 enemies of the same strength\n";
	for (char c : instructionsText8) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
	}

	glColor3f(1, 1, 1);
	glRasterPos2f(-90, -10);
	string instructionsText9 = "The fifth room has a boss that needs more hits to die.";
	for (char c : instructionsText9) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
	}
	glColor3f(1, 1, 0);
	glRasterPos2f(-30, -20);
	string message = "press on 'M' to return to main menu";
	for (char c : message) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
	}
}

void drawMainMenu() {
	//background
	glClearColor(0.53, 0.35, 0.25, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	SetTransformations();
	draw_back_ground_Menu();

	//name of the menue
	glColor3f(1, 1, 0);
	glRasterPos2f(-15, 30);
	string title = "Escape the Dungeon";
	for (char c : title) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
	}

	//start game
	glColor3f(0, 1, 0);
	glRasterPos2f(-10, 10);
	string startOption = "1. Start Game";
	for (char c : startOption) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
	}

	//instructions
	glColor3f(0, 1, 0);
	glRasterPos2f(-10, 0);
	string instructionsOption = "2. Instructions";
	for (char c : instructionsOption) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
	}

	//exit
	glColor3f(1, 0, 0);
	glRasterPos2f(-10, -10);
	string exitOption = "3. Exit";
	for (char c : exitOption) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
	}

	glutSwapBuffers();
}

void gameOver() {
	glColor3f(1.0f, 0.0f, 0.0f);
	if (playerLife == 0) {
		glRasterPos2f(-15, 0);
		string message = "GAME OVER";
		for (char c : message) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
		}
	}
	glColor3f(1.0f, 1.0f, 1.0f);
	if (playerLife == 0) {
		glRasterPos2f(-30, -10);
		string message = "press on 'M' to return to main menu";
		for (char c : message) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
		}
	}

}

void Playerimmunitymessage()
{
	if (immuneCount != 0)
	{
		glColor3f(1, 0, 0);
		glRasterPos2f(-68, 50);
		string Immune = "The Player is Immune! Immune Count:" + to_string(immuneCount);
		for (char c : Immune)
		{
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
		}
	}
}

void drawback_ground()

{

	glPushMatrix();

	glTranslatef(-101.5, 110, 0);
	glScalef(0.17f, 0.27f, 0.0f);
	// square
	glBegin(GL_POLYGON);
	glColor3f(0, 0, 0);
	glVertex2f(3.0f, -1.0f);
	glVertex2f(1187.0f, -1.0f);
	glVertex2f(1187.0f, -773.0f);
	glVertex2f(3.0f, -773.0f);
	glEnd();
	//
	glBegin(GL_POLYGON);
	glColor3f(0.23, 0.12, 0.24);
	glVertex2f(3.0f, -28.0f);
	glVertex2f(1184.0f, -28.0f);
	glColor3f(0.15, 0.06, 0.17);
	glColor3f(0.28, 0.17, 0.29);
	glVertex2f(1184.0f, -472.0f);
	glVertex2f(3.0f, -472.0f);
	glEnd();
	//0.13, 0.07, 0.15
	glBegin(GL_POLYGON);
	glColor3f(0.37, 0.22, 0.38);
	glVertex2f(8.0f, -477.0f);
	glVertex2f(1187.0f, -477.0f);
	glColor3f(0.13, 0.07, 0.15);
	glVertex2f(1187.0f, -505.0f);
	glVertex2f(8.0f, -505.0f);
	glEnd();

	// 
	glBegin(GL_POLYGON);
	glColor3f(0.28, 0.17, 0.29);
	glVertex2f(0.0f, -513.0f);
	glVertex2f(1187.0f, -513.0f);
	glColor3f(0.13, 0.07, 0.15);
	glVertex2f(1187.0f, -643.0f);
	glVertex2f(0.0f, -643.0f);
	glEnd();
	// the yellow part 
	glBegin(GL_POLYGON);
	glColor3f(0.84, 0.68, 0.31);
	glVertex2f(0.0f, -644.0f);
	glVertex2f(1187.0f, -644.0f);
	glVertex2f(1187.0f, -776.0f);
	glVertex2f(0.0f, -776.0f);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(0.19, 0.13, 0.06);
	glVertex2f(0.0f, -703.0f);
	glVertex2f(1187.0f, -703.0f);
	glVertex2f(1187.0f, -769.0f);
	glVertex2f(0.0f, -769.0f);
	glEnd();



	// the above 
	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.14, 0.11, 0.09); // Set color to your desired color
	glVertex2f(32.0f, -33.0f);
	glVertex2f(35.0f, -43.0f);
	glColor3f(0.13, 0.06, 0.04);
	glVertex2f(31.0f, -60.0f);
	glVertex2f(24.0f, -72.0f);
	glColor3f(0.26, 0.12, 0.07);
	glVertex2f(23.0f, -93.0f);
	glVertex2f(20.0f, -116.0f);
	glColor3f(0.42, 0.25, 0.2);
	glVertex2f(16.0f, -128.0f);
	glVertex2f(3.0f, -104.0f);
	glVertex2f(3.0f, -27.0f);

	glEnd();
	//////////////////////////////////////////////////////////////////
	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.13, 0.06, 0.04);
	glVertex2f(175.0f, -33.0f);
	glVertex2f(168.0f, -56.0f);
	glColor3f(0.26, 0.12, 0.07);
	glVertex2f(164.0f, -65.0f);
	glColor3f(0.42, 0.25, 0.2);
	glVertex2f(159.0f, -83.0f);
	glColor3f(0.24, 0.15, 0.12);
	glVertex2f(147.0f, -61.0f);
	glVertex2f(139.0f, -29.0f);

	glEnd();
	/////////////////////////////////////////////////////////////////////

	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.13, 0.06, 0.04);
	glVertex2f(81.0f, -29.0f);
	glVertex2f(89.0f, -49.0f);
	glColor3f(0.26, 0.12, 0.07);
	glVertex2f(101.0f, -64.0f);
	glVertex2f(115.0f, -81.0f);
	glVertex2f(120.0f, -99.0f);
	glColor3f(0.42, 0.25, 0.2);
	glVertex2f(125.0f, -101.0f);
	glVertex2f(129.0f, -72.0f);
	glVertex2f(132.0f, -33.0f);

	glEnd();

	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.13, 0.06, 0.04);
	glVertex2f(40.0f, -35.0f);
	glVertex2f(36.0f, -45.0f);
	glColor3f(0.26, 0.12, 0.07);
	glVertex2f(40.0f, -67.0f);
	glVertex2f(51.0f, -89.0f);
	glVertex2f(55.0f, -103.0f);
	glColor3f(0.42, 0.25, 0.2);
	glVertex2f(61.0f, -77.0f);
	glVertex2f(64.0f, -49.0f);
	glVertex2f(72.0f, -35.0f);
	glVertex2f(73.0f, -31.0f);

	glEnd();

	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.13, 0.06, 0.04);
	glVertex2f(179.0f, -33.0f);
	glVertex2f(192.0f, -61.0f);
	glColor3f(0.26, 0.12, 0.07);
	glVertex2f(196.0f, -92.0f);
	glVertex2f(201.0f, -107.0f);
	glVertex2f(212.0f, -127.0f);
	glColor3f(0.42, 0.25, 0.2);
	glVertex2f(221.0f, -109.0f);
	glVertex2f(223.0f, -77.0f);
	glVertex2f(228.0f, -64.0f);
	glVertex2f(236.0f, -37.0f);
	glVertex2f(232.0f, -29.0f);

	glEnd();
	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.13, 0.06, 0.04);
	glVertex2f(237.0f, -33.0f);
	glVertex2f(241.0f, -52.0f);
	glColor3f(0.26, 0.12, 0.07);
	glVertex2f(241.0f, -61.0f);
	glVertex2f(247.0f, -77.0f);
	glColor3f(0.42, 0.25, 0.2);
	glVertex2f(253.0f, -92.0f);
	glVertex2f(260.0f, -77.0f);
	glVertex2f(261.0f, -52.0f);
	glVertex2f(268.0f, -29.0f);

	glEnd();

	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.13, 0.06, 0.04);
	glVertex2f(271.0f, -29.0f);
	glVertex2f(296.0f, -55.0f);
	glColor3f(0.26, 0.12, 0.07);
	glVertex2f(315.0f, -83.0f);
	glVertex2f(320.0f, -105.0f);
	glColor3f(0.42, 0.25, 0.2);
	glVertex2f(325.0f, -105.0f);
	glVertex2f(328.0f, -67.0f);
	glVertex2f(333.0f, -31.0f);
	glEnd();

	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.13, 0.06, 0.04);
	glVertex2f(339.0f, -31.0f);
	glVertex2f(345.0f, -53.0f);
	glColor3f(0.26, 0.12, 0.07);
	glVertex2f(352.0f, -67.0f);
	glVertex2f(355.0f, -80.0f);
	glColor3f(0.42, 0.25, 0.2);
	glVertex2f(365.0f, -57.0f);
	glVertex2f(377.0f, -33.0f);

	glEnd();

	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.13, 0.06, 0.04);
	glVertex2f(540.0f, -30.0f);
	glVertex2f(546.0f, -52.0f);
	glColor3f(0.26, 0.12, 0.07);
	glVertex2f(553.0f, -66.0f);
	glVertex2f(556.0f, -79.0f);
	glColor3f(0.42, 0.25, 0.2);
	glVertex2f(572.0f, -50.0f);
	glVertex2f(578.0f, -32.0f);

	glEnd();


	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.13, 0.06, 0.04);
	glVertex2f(740.0f, -33.0f);
	glVertex2f(746.0f, -55.0f);
	glColor3f(0.26, 0.12, 0.07);
	glVertex2f(753.0f, -69.0f);
	glVertex2f(756.0f, -82.0f);
	glColor3f(0.42, 0.25, 0.2);
	glVertex2f(772.0f, -53.0f);
	glVertex2f(778.0f, -35.0f);

	glEnd();

	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.13, 0.06, 0.04);
	glVertex2f(942.0f, -30.0f);
	glVertex2f(948.0f, -52.0f);
	glColor3f(0.26, 0.12, 0.07);
	glVertex2f(955.0f, -66.0f);
	glVertex2f(958.0f, -79.0f);
	glColor3f(0.42, 0.25, 0.2);
	glVertex2f(974.0f, -50.0f);
	glVertex2f(980.0f, -32.0f);

	glEnd();


	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.13, 0.06, 0.04);
	glVertex2f(1146.0f, -32.0f);
	glVertex2f(1152.0f, -54.0f);
	glColor3f(0.26, 0.12, 0.07);
	glVertex2f(1159.0f, -68.0f);
	glVertex2f(1162.0f, -81.0f);
	glColor3f(0.42, 0.25, 0.2);
	glVertex2f(1178.0f, -52.0f);
	glVertex2f(1184.0f, -34.0f);

	glEnd();

	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.13, 0.06, 0.04);
	glVertex2f(385.0f, -37.0f);
	glVertex2f(393.0f, -64.0f);
	glColor3f(0.26, 0.12, 0.07);
	glVertex2f(399.0f, -83.0f);
	glVertex2f(397.0f, -96.0f);
	glVertex2f(409.0f, -109.0f);
	glColor3f(0.42, 0.25, 0.2);
	glVertex2f(413.0f, -123.0f);
	glVertex2f(424.0f, -104.0f);
	glVertex2f(424.0f, -79.0f);
	glVertex2f(424.0f, -67.0f);
	glVertex2f(432.0f, -55.0f);
	glVertex2f(436.0f, -33.0f);

	glEnd();


	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.13, 0.06, 0.04);
	glVertex2f(585.0f, -36.0f);
	glVertex2f(593.0f, -63.0f);
	glColor3f(0.26, 0.12, 0.07);
	glVertex2f(599.0f, -82.0f);
	glVertex2f(597.0f, -95.0f);
	glVertex2f(609.0f, -108.0f);
	glColor3f(0.42, 0.25, 0.2);
	glVertex2f(613.0f, -122.0f);
	glVertex2f(624.0f, -103.0f);
	glVertex2f(624.0f, -78.0f);
	glVertex2f(624.0f, -66.0f);
	glVertex2f(632.0f, -54.0f);
	glVertex2f(636.0f, -32.0f);

	glEnd();

	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.13, 0.06, 0.04);
	glVertex2f(786.0f, -34.0f);
	glVertex2f(794.0f, -61.0f);
	glVertex2f(800.0f, -80.0f);
	glColor3f(0.26, 0.12, 0.07);
	glVertex2f(798.0f, -93.0f);
	glVertex2f(810.0f, -106.0f);
	glVertex2f(814.0f, -120.0f);
	glColor3f(0.42, 0.25, 0.2);
	glVertex2f(825.0f, -101.0f);
	glVertex2f(825.0f, -76.0f);
	glVertex2f(825.0f, -64.0f);
	glVertex2f(833.0f, -52.0f);
	glVertex2f(837.0f, -30.0f);

	glEnd();

	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.13, 0.06, 0.04);
	glVertex2f(987.0f, -29.0f);
	glVertex2f(995.0f, -56.0f);
	glColor3f(0.26, 0.12, 0.07);
	glVertex2f(1001.0f, -75.0f);
	glVertex2f(999.0f, -88.0f);
	glVertex2f(1011.0f, -101.0f);
	glVertex2f(1015.0f, -115.0f);
	glVertex2f(1026.0f, -96.0f);
	glColor3f(0.42, 0.25, 0.2);
	glVertex2f(1026.0f, -71.0f);
	glVertex2f(1026.0f, -59.0f);
	glVertex2f(1034.0f, -47.0f);
	glVertex2f(1038.0f, -25.0f);

	glEnd();

	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.13, 0.06, 0.04);
	glVertex2f(484.0f, -33.0f);
	glVertex2f(493.0f, -47.0f);
	glColor3f(0.26, 0.12, 0.07);
	glVertex2f(493.0f, -56.0f);
	glVertex2f(504.0f, -67.0f);
	glColor3f(0.42, 0.25, 0.2);
	glVertex2f(512.0f, -77.0f);
	glVertex2f(517.0f, -92.0f);
	glVertex2f(528.0f, -104.0f);
	glVertex2f(528.0f, -77.0f);
	glVertex2f(533.0f, -56.0f);
	glVertex2f(533.0f, -35.0f);
	glVertex2f(539.0f, -35.0f);

	glEnd();

	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.13, 0.06, 0.04);
	glVertex2f(689.0f, -34.0f);
	glVertex2f(698.0f, -48.0f);
	glColor3f(0.26, 0.12, 0.07);
	glVertex2f(698.0f, -57.0f);
	glVertex2f(709.0f, -68.0f);
	glColor3f(0.42, 0.25, 0.2);
	glVertex2f(717.0f, -78.0f);
	glVertex2f(722.0f, -93.0f);
	glVertex2f(733.0f, -105.0f);
	glVertex2f(733.0f, -78.0f);
	glVertex2f(738.0f, -57.0f);
	glVertex2f(738.0f, -36.0f);
	glVertex2f(744.0f, -36.0f);

	glEnd();


	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.13, 0.06, 0.04);
	glVertex2f(889.0f, -29.0f);
	glVertex2f(898.0f, -43.0f);
	glVertex2f(898.0f, -52.0f);
	glColor3f(0.26, 0.12, 0.07);
	glVertex2f(909.0f, -63.0f);
	glVertex2f(917.0f, -73.0f);
	glVertex2f(922.0f, -88.0f);
	glColor3f(0.42, 0.25, 0.2);
	glVertex2f(933.0f, -100.0f);
	glVertex2f(933.0f, -73.0f);
	glVertex2f(938.0f, -52.0f);
	glVertex2f(938.0f, -31.0f);
	glVertex2f(944.0f, -31.0f);

	glEnd();
	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.13, 0.06, 0.04);
	glVertex2f(1087.0f, -28.0f);
	glVertex2f(1096.0f, -42.0f);
	glColor3f(0.26, 0.12, 0.07);
	glVertex2f(1096.0f, -51.0f);
	glVertex2f(1107.0f, -62.0f);
	glVertex2f(1115.0f, -72.0f);
	glColor3f(0.42, 0.25, 0.2);
	glVertex2f(1120.0f, -87.0f);
	glVertex2f(1131.0f, -99.0f);
	glVertex2f(1131.0f, -72.0f);
	glVertex2f(1136.0f, -51.0f);
	glVertex2f(1136.0f, -30.0f);
	glVertex2f(1142.0f, -30.0f);

	glEnd();

	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.13, 0.06, 0.04);
	glVertex2f(845.0f, -31.0f);
	glVertex2f(841.0f, -52.0f);
	glColor3f(0.26, 0.12, 0.07);
	glVertex2f(848.0f, -67.0f);
	glVertex2f(856.0f, -93.0f);
	glColor3f(0.42, 0.25, 0.2);
	glVertex2f(865.0f, -68.0f);
	glVertex2f(871.0f, -32.0f);

	glEnd();

	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.13, 0.06, 0.04);
	glVertex2f(1048.0f, -32.0f);
	glVertex2f(1044.0f, -53.0f);
	glColor3f(0.26, 0.12, 0.07);
	glVertex2f(1051.0f, -68.0f);
	glVertex2f(1059.0f, -94.0f);
	glColor3f(0.42, 0.25, 0.2);
	glVertex2f(1068.0f, -69.0f);
	glVertex2f(1074.0f, -33.0f);

	glEnd();

	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.13, 0.06, 0.04);
	glVertex2f(646.0f, -34.0f);
	glVertex2f(642.0f, -55.0f);
	glColor3f(0.26, 0.12, 0.07);
	glVertex2f(646.0f, -77.0f);
	glVertex2f(657.0f, -96.0f);
	glColor3f(0.42, 0.25, 0.2);
	glVertex2f(666.0f, -71.0f);
	glVertex2f(672.0f, -35.0f);

	glEnd();


	///           ///// 














	// the mon
	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.54, 0.45, 0.38); // Set color to your desired color
	glVertex2f(511.0f, -596.0f);
	glVertex2f(499.0f, -600.0f);
	glVertex2f(492.0f, -611.0f);
	glVertex2f(485.0f, -624.0f);
	glVertex2f(485.0f, -652.0f);
	glColor3f(0.6, 0.5, 0.41);
	glVertex2f(536.0f, -657.0f);
	glVertex2f(552.0f, -653.0f);
	glVertex2f(548.0f, -613.0f);
	glVertex2f(536.0f, -601.0f);

	glEnd();
	///
	glLineWidth(2);
	glBegin(GL_LINE_LOOP);
	glColor3f(0.16, 0.13, 0.1);
	glVertex2f(511.0f, -596.0f);
	glVertex2f(499.0f, -600.0f);
	glVertex2f(492.0f, -611.0f);
	glVertex2f(485.0f, -624.0f);
	glVertex2f(485.0f, -652.0f);
	glVertex2f(536.0f, -657.0f);
	glVertex2f(552.0f, -653.0f);
	glVertex2f(548.0f, -613.0f);
	glVertex2f(536.0f, -601.0f);
	glEnd();
	//

	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.54, 0.45, 0.38);
	glVertex2f(1085.0f, -601.0f);
	glVertex2f(1109.0f, -603.0f);
	glVertex2f(1112.0f, -620.0f);
	glColor3f(0.6, 0.5, 0.41);
	glVertex2f(1115.0f, -655.0f);
	glVertex2f(1076.0f, -655.0f);
	glVertex2f(1072.0f, -624.0f);

	glEnd();
	//
	glLineWidth(2);
	glBegin(GL_LINE_LOOP);
	glColor3f(0.16, 0.13, 0.1);
	glVertex2f(1085.0f, -601.0f);
	glVertex2f(1109.0f, -603.0f);
	glVertex2f(1112.0f, -620.0f);
	glVertex2f(1115.0f, -655.0f);
	glVertex2f(1076.0f, -655.0f);
	glVertex2f(1072.0f, -624.0f);

	glEnd();

	//
	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.54, 0.45, 0.38);
	glVertex2f(509.0f, -593.0f);
	glVertex2f(524.0f, -572.0f);
	glVertex2f(552.0f, -573.0f);
	glColor3f(0.6, 0.5, 0.41);
	glVertex2f(559.0f, -576.0f);
	glVertex2f(540.0f, -581.0f);
	glVertex2f(536.0f, -603.0f);

	glEnd();
	//
	glLineWidth(2);
	glBegin(GL_LINE_LOOP);
	glColor3f(0.16, 0.13, 0.1);
	glVertex2f(509.0f, -593.0f);
	glVertex2f(524.0f, -572.0f);
	glVertex2f(552.0f, -573.0f);
	glVertex2f(559.0f, -576.0f);
	glVertex2f(540.0f, -581.0f);
	glVertex2f(536.0f, -603.0f);

	glEnd();


	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.54, 0.45, 0.38);
	glVertex2f(535.0f, -600.0f);
	glVertex2f(547.0f, -611.0f);
	glVertex2f(547.0f, -628.0f);
	glVertex2f(548.0f, -652.0f);
	glVertex2f(567.0f, -655.0f);
	glVertex2f(583.0f, -647.0f);
	glVertex2f(583.0f, -623.0f);
	glColor3f(0.6, 0.5, 0.41);
	glVertex2f(576.0f, -605.0f);
	glVertex2f(579.0f, -596.0f);
	glVertex2f(572.0f, -585.0f);
	glVertex2f(564.0f, -577.0f);
	glVertex2f(549.0f, -583.0f);
	glVertex2f(549.0f, -579.0f);
	glVertex2f(539.0f, -588.0f);

	glEnd();
	//
	glLineWidth(2);
	glBegin(GL_LINE_LOOP);
	glColor3f(0.16, 0.13, 0.1);
	glVertex2f(535.0f, -600.0f);
	glVertex2f(547.0f, -611.0f);
	glVertex2f(547.0f, -628.0f);
	glVertex2f(548.0f, -652.0f);
	glVertex2f(567.0f, -655.0f);
	glVertex2f(583.0f, -647.0f);
	glVertex2f(583.0f, -623.0f);
	glVertex2f(576.0f, -605.0f);
	glVertex2f(579.0f, -596.0f);
	glVertex2f(572.0f, -585.0f);
	glVertex2f(564.0f, -577.0f);
	glVertex2f(549.0f, -583.0f);
	glVertex2f(549.0f, -579.0f);
	glVertex2f(539.0f, -588.0f);

	glEnd();
	//
	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.54, 0.45, 0.38);
	glVertex2f(581.0f, -623.0f);
	glVertex2f(580.0f, -652.0f);
	glColor3f(0.6, 0.5, 0.41);
	glVertex2f(595.0f, -644.0f);
	glVertex2f(595.0f, -627.0f);

	glEnd();
	//
	glLineWidth(2);
	glBegin(GL_LINE_LOOP);
	glColor3f(0.16, 0.13, 0.1);
	glVertex2f(581.0f, -623.0f);
	glVertex2f(580.0f, -652.0f);
	glVertex2f(595.0f, -644.0f);
	glVertex2f(595.0f, -627.0f);

	glEnd();
	//
	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.54, 0.45, 0.38);
	glVertex2f(499.0f, -599.0f);
	glVertex2f(523.0f, -601.0f);
	glVertex2f(526.0f, -618.0f);
	glColor3f(0.6, 0.5, 0.41);
	glVertex2f(529.0f, -653.0f);
	glVertex2f(490.0f, -653.0f);
	glVertex2f(486.0f, -622.0f);

	glEnd();
	//
	glLineWidth(2);
	glBegin(GL_LINE_LOOP);
	glColor3f(0.16, 0.13, 0.1);
	glVertex2f(499.0f, -599.0f);
	glVertex2f(523.0f, -601.0f);
	glVertex2f(526.0f, -618.0f);
	glVertex2f(529.0f, -653.0f);
	glVertex2f(490.0f, -653.0f);
	glVertex2f(486.0f, -622.0f);

	glEnd();
	//
	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.54, 0.45, 0.38);
	glVertex2f(1094.0f, -596.0f);
	glVertex2f(1082.0f, -600.0f);
	glVertex2f(1075.0f, -611.0f);
	glVertex2f(1068.0f, -624.0f);
	glVertex2f(1068.0f, -652.0f);
	glColor3f(0.6, 0.5, 0.41);
	glVertex2f(1119.0f, -657.0f);
	glVertex2f(1135.0f, -653.0f);
	glVertex2f(1131.0f, -613.0f);
	glVertex2f(1119.0f, -601.0f);

	glEnd();
	//
	glLineWidth(2);
	glBegin(GL_LINE_LOOP);
	glColor3f(0.16, 0.13, 0.1);
	glVertex2f(1094.0f, -596.0f);
	glVertex2f(1082.0f, -600.0f);
	glVertex2f(1075.0f, -611.0f);
	glVertex2f(1068.0f, -624.0f);
	glVertex2f(1068.0f, -652.0f);
	glVertex2f(1119.0f, -657.0f);
	glVertex2f(1135.0f, -653.0f);
	glVertex2f(1131.0f, -613.0f);
	glVertex2f(1119.0f, -601.0f);

	glEnd();
	//

	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.54, 0.45, 0.38);
	glVertex2f(1117.0f, -597.0f);
	glVertex2f(1129.0f, -608.0f);
	glVertex2f(1129.0f, -625.0f);
	glVertex2f(1130.0f, -649.0f);
	glVertex2f(1149.0f, -652.0f);
	glVertex2f(1165.0f, -644.0f);
	glVertex2f(1165.0f, -620.0f);
	glVertex2f(1158.0f, -602.0f);
	glVertex2f(1161.0f, -593.0f);
	glColor3f(0.6, 0.5, 0.41);
	glVertex2f(1154.0f, -582.0f);
	glVertex2f(1146.0f, -574.0f);
	glVertex2f(1131.0f, -580.0f);
	glVertex2f(1131.0f, -576.0f);
	glVertex2f(1121.0f, -585.0f);

	glEnd();
	//
	glLineWidth(2);
	glBegin(GL_LINE_LOOP);
	glColor3f(0.16, 0.13, 0.1);
	glVertex2f(1117.0f, -597.0f);
	glVertex2f(1129.0f, -608.0f);
	glVertex2f(1129.0f, -625.0f);
	glVertex2f(1130.0f, -649.0f);
	glVertex2f(1149.0f, -652.0f);
	glVertex2f(1165.0f, -644.0f);
	glVertex2f(1165.0f, -620.0f);
	glVertex2f(1158.0f, -602.0f);
	glVertex2f(1161.0f, -593.0f);
	glVertex2f(1154.0f, -582.0f);
	glVertex2f(1146.0f, -574.0f);
	glVertex2f(1131.0f, -580.0f);
	glVertex2f(1131.0f, -576.0f);
	glVertex2f(1121.0f, -585.0f);

	glEnd();
	//
	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.54, 0.45, 0.38);
	glVertex2f(1095.0f, -596.0f);
	glVertex2f(1116.0f, -601.0f);
	glVertex2f(1120.0f, -588.0f);
	glVertex2f(1140.0f, -579.0f);
	glColor3f(0.6, 0.5, 0.41);
	glVertex2f(1144.0f, -572.0f);
	glVertex2f(1124.0f, -571.0f);
	glVertex2f(1107.0f, -571.0f);
	glVertex2f(1100.0f, -581.0f);

	glEnd();
	//
	glLineWidth(2);
	glBegin(GL_LINE_LOOP);
	glColor3f(0.16, 0.13, 0.1);
	glVertex2f(1095.0f, -596.0f);
	glVertex2f(1116.0f, -601.0f);
	glVertex2f(1120.0f, -588.0f);
	glVertex2f(1140.0f, -579.0f);
	glVertex2f(1144.0f, -572.0f);
	glVertex2f(1124.0f, -571.0f);
	glVertex2f(1107.0f, -571.0f);
	glVertex2f(1100.0f, -581.0f);

	glEnd();
	//
	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.54, 0.45, 0.38);
	glVertex2f(1165.0f, -621.0f);
	glVertex2f(1161.0f, -651.0f);
	glVertex2f(1176.0f, -647.0f);
	glColor3f(0.6, 0.5, 0.41);
	glVertex2f(1181.0f, -637.0f);
	glVertex2f(1180.0f, -631.0f);
	glEnd();
	//
	glLineWidth(2);
	glBegin(GL_LINE_LOOP);
	glColor3f(0.16, 0.13, 0.1);
	glVertex2f(1165.0f, -621.0f);
	glVertex2f(1161.0f, -651.0f);
	glVertex2f(1176.0f, -647.0f);
	glVertex2f(1181.0f, -637.0f);
	glVertex2f(1180.0f, -631.0f);
	glEnd();
	///////////////////////////////////
//  before bones
	glBegin(GL_POLYGON);
	glColor3f(0.47, 0.27, 0.09);
	glVertex2f(9.0f, -664.0f);
	glVertex2f(13.0f, -665.0f);
	glVertex2f(36.0f, -659.0f);
	glVertex2f(52.0f, -668.0f);
	glVertex2f(77.0f, -665.0f);
	glVertex2f(92.0f, -668.0f);
	glVertex2f(112.0f, -663.0f);
	glColor3f(0.51, 0.3, 0.12);
	glVertex2f(135.0f, -669.0f);
	glVertex2f(157.0f, -665.0f);
	glVertex2f(205.0f, -675.0f);
	glVertex2f(232.0f, -661.0f);
	glVertex2f(260.0f, -673.0f);
	glVertex2f(284.0f, -664.0f);
	glVertex2f(319.0f, -664.0f);
	glColor3f(0.39, 0.2, 0.04);
	glVertex2f(328.0f, -673.0f);
	glVertex2f(364.0f, -672.0f);
	glVertex2f(364.0f, -663.0f);
	glVertex2f(395.0f, -664.0f);
	glVertex2f(403.0f, -672.0f);
	glVertex2f(427.0f, -664.0f);
	glVertex2f(449.0f, -672.0f);
	glColor3f(0.47, 0.27, 0.09);
	glVertex2f(465.0f, -668.0f);
	glVertex2f(499.0f, -668.0f);
	glVertex2f(503.0f, -663.0f);
	glVertex2f(541.0f, -663.0f);
	glVertex2f(564.0f, -665.0f);
	glVertex2f(577.0f, -672.0f);
	glColor3f(0.55, 0.31, 0.11);
	glVertex2f(600.0f, -673.0f);
	glVertex2f(616.0f, -665.0f);
	glVertex2f(644.0f, -660.0f);
	glVertex2f(661.0f, -676.0f);
	glVertex2f(679.0f, -663.0f);
	glVertex2f(708.0f, -671.0f);
	glVertex2f(733.0f, -665.0f);
	glVertex2f(757.0f, -661.0f);
	glColor3f(0.31, 0.18, 0.06);
	glVertex2f(789.0f, -667.0f);
	glVertex2f(808.0f, -667.0f);
	glVertex2f(837.0f, -668.0f);
	glVertex2f(865.0f, -668.0f);
	glVertex2f(861.0f, -677.0f);
	glColor3f(0.53, 0.28, 0.07);
	glVertex2f(896.0f, -668.0f);
	glVertex2f(920.0f, -676.0f);
	glVertex2f(944.0f, -657.0f);
	glVertex2f(943.0f, -660.0f);
	glVertex2f(967.0f, -671.0f);
	glVertex2f(1003.0f, -660.0f);
	glColor3f(0.35, 0.19, 0.06);
	glVertex2f(1027.0f, -669.0f);
	glVertex2f(1053.0f, -663.0f);
	glVertex2f(1076.0f, -669.0f);
	glVertex2f(1103.0f, -665.0f);
	glColor3f(0.4, 0.26, 0.14);
	glVertex2f(1121.0f, -679.0f);
	glVertex2f(1144.0f, -664.0f);
	glVertex2f(1179.0f, -671.0f);
	glVertex2f(1184.0f, -664.0f);
	glVertex2f(1184.0f, -703.0f);
	glVertex2f(13.0f, -707.0f);
	glVertex2f(3.0f, -697.0f);

	glEnd();










	// the broen part s
	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.38, 0.24, 0.19);
	glVertex2f(56.0f, -716.0f);
	glVertex2f(41.0f, -713.0f);
	glVertex2f(28.0f, -716.0f);
	glVertex2f(28.0f, -728.0f);
	glVertex2f(35.0f, -743.0f);
	glVertex2f(45.0f, -745.0f);
	glVertex2f(51.0f, -748.0f);
	glVertex2f(63.0f, -737.0f);

	glEnd();

	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.38, 0.24, 0.19);
	glVertex2f(185.0f, -715.0f);
	glVertex2f(170.0f, -712.0f);
	glVertex2f(157.0f, -715.0f);
	glVertex2f(157.0f, -727.0f);

	glVertex2f(164.0f, -742.0f);
	glVertex2f(174.0f, -744.0f);
	glVertex2f(180.0f, -747.0f);
	glVertex2f(192.0f, -736.0f);

	glEnd();

	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.38, 0.24, 0.19);
	glVertex2f(313.0f, -716.0f);
	glVertex2f(298.0f, -713.0f);
	glVertex2f(285.0f, -716.0f);
	glVertex2f(285.0f, -728.0f);
	glVertex2f(292.0f, -743.0f);
	glColor3f(0.6, 0.5, 0.41);
	glVertex2f(302.0f, -745.0f);
	glVertex2f(308.0f, -748.0f);
	glVertex2f(320.0f, -737.0f);

	glEnd();


	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.38, 0.24, 0.19);
	glVertex2f(438.0f, -714.0f);
	glVertex2f(423.0f, -711.0f);
	glVertex2f(410.0f, -714.0f);
	glVertex2f(410.0f, -726.0f);
	glVertex2f(417.0f, -741.0f);
	glVertex2f(427.0f, -743.0f);
	glVertex2f(433.0f, -746.0f);
	glVertex2f(445.0f, -735.0f);

	glEnd();


	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.38, 0.24, 0.19);
	glVertex2f(566.0f, -715.0f);
	glVertex2f(551.0f, -712.0f);
	glVertex2f(538.0f, -715.0f);
	glVertex2f(538.0f, -727.0f);
	glVertex2f(545.0f, -742.0f);
	glVertex2f(555.0f, -744.0f);
	glVertex2f(561.0f, -747.0f);
	glVertex2f(573.0f, -736.0f);

	glEnd();



	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.38, 0.24, 0.19);
	glVertex2f(692.0f, -716.0f);
	glVertex2f(672.0f, -712.0f);
	glVertex2f(659.0f, -715.0f);
	glVertex2f(659.0f, -727.0f);
	glVertex2f(666.0f, -742.0f);
	glVertex2f(676.0f, -744.0f);
	glVertex2f(682.0f, -747.0f);
	glVertex2f(694.0f, -736.0f);

	glEnd();

	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.38, 0.24, 0.19);
	glVertex2f(825.0f, -718.0f);
	glVertex2f(805.0f, -714.0f);
	glVertex2f(792.0f, -717.0f);
	glVertex2f(792.0f, -729.0f);
	glVertex2f(799.0f, -744.0f);
	glVertex2f(809.0f, -746.0f);
	glVertex2f(815.0f, -749.0f);
	glVertex2f(827.0f, -738.0f);

	glEnd();
	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.38, 0.24, 0.19);
	glVertex2f(946.0f, -717.0f);
	glVertex2f(926.0f, -713.0f);
	glVertex2f(913.0f, -716.0f);
	glVertex2f(913.0f, -728.0f);
	glVertex2f(920.0f, -743.0f);
	glVertex2f(930.0f, -745.0f);
	glVertex2f(936.0f, -748.0f);
	glVertex2f(948.0f, -737.0f);

	glEnd();


	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.38, 0.24, 0.19);
	glVertex2f(1073.0f, -718.0f);
	glVertex2f(1053.0f, -714.0f);
	glVertex2f(1040.0f, -717.0f);
	glVertex2f(1040.0f, -729.0f);
	glVertex2f(1047.0f, -744.0f);
	glVertex2f(1057.0f, -746.0f);
	glVertex2f(1063.0f, -749.0f);
	glVertex2f(1075.0f, -738.0f);

	glEnd();

	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.38, 0.24, 0.19);
	glVertex2f(97.0f, -743.0f);
	glVertex2f(116.0f, -744.0f);
	glVertex2f(136.0f, -748.0f);
	glVertex2f(139.0f, -759.0f);
	glVertex2f(123.0f, -764.0f);
	glVertex2f(104.0f, -769.0f);
	glVertex2f(89.0f, -761.0f);
	glVertex2f(84.0f, -751.0f);

	glEnd();
	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.38, 0.24, 0.19);
	glVertex2f(225.0f, -743.0f);
	glVertex2f(251.0f, -743.0f);
	glVertex2f(264.0f, -748.0f);
	glVertex2f(267.0f, -759.0f);
	glVertex2f(251.0f, -764.0f);
	glVertex2f(232.0f, -769.0f);
	glVertex2f(217.0f, -761.0f);
	glVertex2f(212.0f, -751.0f);

	glEnd();


	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.38, 0.24, 0.19);
	glVertex2f(351.0f, -744.0f);
	glVertex2f(377.0f, -744.0f);
	glVertex2f(390.0f, -749.0f);
	glVertex2f(393.0f, -760.0f);
	glVertex2f(377.0f, -765.0f);
	glVertex2f(358.0f, -770.0f);
	glVertex2f(343.0f, -762.0f);
	glVertex2f(338.0f, -752.0f);

	glEnd();



	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.38, 0.24, 0.19);
	glVertex2f(478.0f, -743.0f);
	glVertex2f(504.0f, -743.0f);
	glVertex2f(517.0f, -748.0f);
	glVertex2f(520.0f, -759.0f);
	glVertex2f(504.0f, -764.0f);
	glVertex2f(485.0f, -769.0f);
	glVertex2f(470.0f, -761.0f);
	glVertex2f(465.0f, -751.0f);

	glEnd();

	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.38, 0.24, 0.19);
	glVertex2f(604.0f, -743.0f);
	glVertex2f(630.0f, -743.0f);
	glVertex2f(643.0f, -748.0f);
	glVertex2f(646.0f, -759.0f);
	glVertex2f(630.0f, -764.0f);
	glVertex2f(611.0f, -769.0f);
	glVertex2f(596.0f, -761.0f);
	glVertex2f(591.0f, -751.0f);

	glEnd();
	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.38, 0.24, 0.19);
	glVertex2f(730.0f, -743.0f);
	glVertex2f(756.0f, -743.0f);
	glVertex2f(769.0f, -748.0f);
	glVertex2f(772.0f, -759.0f);
	glVertex2f(756.0f, -764.0f);
	glVertex2f(737.0f, -769.0f);
	glVertex2f(722.0f, -761.0f);
	glVertex2f(717.0f, -751.0f);

	glEnd();


	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.38, 0.24, 0.19);
	glVertex2f(857.0f, -744.0f);
	glVertex2f(883.0f, -744.0f);
	glVertex2f(896.0f, -749.0f);
	glVertex2f(899.0f, -760.0f);
	glVertex2f(883.0f, -765.0f);
	glVertex2f(864.0f, -770.0f);
	glVertex2f(849.0f, -762.0f);
	glVertex2f(844.0f, -752.0f);

	glEnd();

	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.38, 0.24, 0.19);
	glVertex2f(980.0f, -740.0f);
	glVertex2f(1006.0f, -740.0f);
	glVertex2f(1019.0f, -745.0f);
	glVertex2f(1022.0f, -756.0f);
	glVertex2f(1006.0f, -761.0f);
	glVertex2f(987.0f, -766.0f);
	glVertex2f(972.0f, -758.0f);
	glVertex2f(967.0f, -748.0f);

	glEnd();

	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.38, 0.24, 0.19);
	glVertex2f(1107.0f, -741.0f);
	glVertex2f(1133.0f, -741.0f);
	glVertex2f(1146.0f, -746.0f);
	glVertex2f(1149.0f, -757.0f);
	glVertex2f(1133.0f, -762.0f);
	glVertex2f(1114.0f, -767.0f);
	glVertex2f(1099.0f, -759.0f);
	glVertex2f(1094.0f, -749.0f);

	glEnd();
	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.38, 0.24, 0.19);
	glVertex2f(1107.0f, -741.0f);
	glVertex2f(1133.0f, -741.0f);
	glVertex2f(1146.0f, -746.0f);
	glVertex2f(1149.0f, -757.0f);
	glVertex2f(1133.0f, -762.0f);
	glVertex2f(1114.0f, -767.0f);
	glVertex2f(1099.0f, -759.0f);
	glVertex2f(1094.0f, -749.0f);

	glEnd();

	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.38, 0.24, 0.19);
	glVertex2f(115.0f, -729.0f);
	glVertex2f(125.0f, -724.0f);
	glVertex2f(132.0f, -732.0f);
	glVertex2f(135.0f, -737.0f);
	glVertex2f(131.0f, -739.0f);
	glVertex2f(120.0f, -736.0f);

	glEnd();

	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.38, 0.24, 0.19);
	glVertex2f(196.0f, -732.0f);
	glVertex2f(209.0f, -731.0f);
	glVertex2f(215.0f, -739.0f);
	glVertex2f(214.0f, -742.0f);
	glVertex2f(212.0f, -745.0f);
	glVertex2f(198.0f, -744.0f);

	glEnd();
	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.38, 0.24, 0.19);
	glVertex2f(261.0f, -714.0f);
	glVertex2f(271.0f, -712.0f);
	glVertex2f(277.0f, -720.0f);
	glVertex2f(276.0f, -723.0f);
	glVertex2f(274.0f, -726.0f);
	glVertex2f(261.0f, -724.0f);

	glEnd();


	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.38, 0.24, 0.19);
	glVertex2f(318.0f, -706.0f);
	glVertex2f(328.0f, -704.0f);
	glVertex2f(334.0f, -712.0f);
	glVertex2f(333.0f, -715.0f);
	glVertex2f(331.0f, -718.0f);
	glVertex2f(318.0f, -716.0f);

	glEnd();
	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.38, 0.24, 0.19);
	glVertex2f(392.0f, -733.0f);
	glVertex2f(401.0f, -730.0f);
	glVertex2f(407.0f, -738.0f);
	glVertex2f(406.0f, -741.0f);
	glVertex2f(404.0f, -744.0f);
	glVertex2f(391.0f, -742.0f);

	glEnd();

	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.38, 0.24, 0.19);
	glVertex2f(442.0f, -707.0f);
	glVertex2f(454.0f, -705.0f);
	glVertex2f(461.0f, -711.0f);
	glVertex2f(459.0f, -716.0f);
	glVertex2f(457.0f, -719.0f);
	glVertex2f(444.0f, -717.0f);

	glEnd();
	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.38, 0.24, 0.19);
	glVertex2f(568.0f, -709.0f);
	glVertex2f(582.0f, -706.0f);
	glVertex2f(587.0f, -713.0f);
	glVertex2f(585.0f, -718.0f);
	glVertex2f(581.0f, -720.0f);
	glVertex2f(571.0f, -718.0f);

	glEnd();
	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.38, 0.24, 0.19);
	glVertex2f(510.0f, -716.0f);
	glVertex2f(524.0f, -713.0f);
	glVertex2f(529.0f, -720.0f);
	glVertex2f(527.0f, -725.0f);
	glVertex2f(523.0f, -727.0f);
	glVertex2f(513.0f, -725.0f);

	glEnd();

	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.38, 0.24, 0.19);
	glVertex2f(619.0f, -728.0f);
	glVertex2f(633.0f, -725.0f);
	glVertex2f(638.0f, -732.0f);
	glVertex2f(636.0f, -737.0f);
	glVertex2f(632.0f, -739.0f);
	glVertex2f(622.0f, -737.0f);

	glEnd();

	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.38, 0.24, 0.19);
	glVertex2f(952.0f, -708.0f);
	glVertex2f(960.0f, -707.0f);
	glVertex2f(967.0f, -712.0f);
	glVertex2f(965.0f, -717.0f);
	glVertex2f(961.0f, -719.0f);
	glVertex2f(951.0f, -717.0f);

	glEnd();


	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.38, 0.24, 0.19);
	glVertex2f(1024.0f, -733.0f);
	glVertex2f(1032.0f, -732.0f);
	glVertex2f(1039.0f, -737.0f);
	glVertex2f(1037.0f, -742.0f);
	glVertex2f(1033.0f, -744.0f);
	glVertex2f(1023.0f, -742.0f);

	glEnd();
	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.38, 0.24, 0.19);
	glVertex2f(1151.0f, -733.0f);
	glVertex2f(1159.0f, -732.0f);
	glVertex2f(1166.0f, -737.0f);
	glVertex2f(1164.0f, -742.0f);
	glVertex2f(1160.0f, -744.0f);
	glVertex2f(1150.0f, -742.0f);

	glEnd();

	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.38, 0.24, 0.19);
	glVertex2f(1123.0f, -709.0f);
	glVertex2f(1131.0f, -708.0f);
	glVertex2f(1138.0f, -713.0f);
	glVertex2f(1136.0f, -718.0f);
	glVertex2f(1132.0f, -720.0f);
	glVertex2f(1122.0f, -718.0f);

	glEnd();

	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.38, 0.24, 0.19);
	glVertex2f(1076.0f, -708.0f);
	glVertex2f(1084.0f, -707.0f);
	glVertex2f(1091.0f, -712.0f);
	glVertex2f(1089.0f, -717.0f);
	glVertex2f(1085.0f, -719.0f);
	glVertex2f(1075.0f, -717.0f);

	glEnd();
	glBegin(GL_POLYGON); // Begin drawing a polygon
	glColor3f(0.38, 0.24, 0.19);
	glVertex2f(996.0f, -711.0f);
	glVertex2f(1004.0f, -710.0f);
	glVertex2f(1011.0f, -715.0f);
	glVertex2f(1008.0f, -720.0f);
	glVertex2f(1005.0f, -722.0f);
	glVertex2f(995.0f, -720.0f);

	glEnd();




	glPopMatrix();


}

void draw_back_ground_Menu()

{

	glPushMatrix();

	glTranslatef(-30, 80, 0);
	glScalef(0.17f, 0.27f, 0.0f);

	glBegin(GL_QUADS);
	glColor3f(0.54, 0.43, 0.09);
	glVertex2f(19.0f, -36.0f);   // Bottom-left corner
	glVertex2f(335.0f, -36.0f);  // Bottom-right corner
	glVertex2f(335.0f, -438.0f); // Top-right corner
	glVertex2f(19.0f, -438.0f);  // Top-left corner
	glEnd();

	glBegin(GL_QUADS); // Begin drawing a quadrilateral
	glColor3f(0.27, 0.16, 0.12); // Set the color to your desired color
	glVertex2f(32.0f, -50.0f);   // Bottom-left corner
	glVertex2f(315.0f, -50.0f);  // Bottom-right corner
	glVertex2f(315.0f, -422.0f); // Top-right corner
	glVertex2f(32.0f, -422.0f);  // Top-left corner

	glEnd();



	glPopMatrix();


}

void win() {

	currentState = WIN;
	glClearColor(0.53, 0.35, 0.25, 1.0f);
	//fill the whole color buffer with the clear color
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(0, 1, 0);
	glRasterPos2f(-15, 0);
	string message = "YOU WIN!!";
	for (char c : message) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
	}
	glColor3f(1.0f, 1.0f, 1.0f);

	glRasterPos2f(-20, -10);
	string text = "press on 'M' to play again";
	for (char c : text) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
	}
	if (yaay == 0)
	{
		bossTheme->drop();
		soundEffects->play2D("media/audio/Yaay.mp3", false);
		yaay = 1;
	}

}