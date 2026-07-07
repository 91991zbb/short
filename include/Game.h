#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <sstream>
#include "Tetris.h"

#define  INFO_CORNER_X			560
#define  INFO_CORNER_Y			290
#define  B_START_CORNER_X			621
#define  B_START_CORNER_Y			763
#define  B_HOLD_CORNER_X			621
#define  B_HOLD_CORNER_Y			822
#define  B_LEFT_CORNER_X			70
#define  B_LEFT_CORNER_Y			460
#define  B_RIGHT_CORNER_X			1295
#define  B_RIGHT_CORNER_Y			460
using namespace sf;			//SFML中的每个类都位于该命名空间之下，不设定sf命名空间的话，相应的函数前需要用作用域解析符，例如 sf::VideoMode(width* GRIDSIZE, height* GRIDSIZE)

enum ButtonState {
	Continue_Dark,	// 继续暗
	Continue_Light, // 继续亮
	Hold_Dark,		// 暂停暗
	Hold_Light,		// 暂停亮
	Close_Dark, // 结束暗
	Close_Light, // 结束亮
	Start_Dark, // 开始暗
	Start_Light, // 开始亮
};
class Game
{
public:
	Game();
	~Game();
	sf::RenderWindow window;
	//类的封装是一个很有意思的话题，首先要想想，类的对象是什么？对对象的定位不同，内容也会有很大的差异
	//工程的Tetris类，差不多是一个俄罗斯方块的小游戏。因为是双打的俄罗斯方块嘛^_^
	//Game类被作为游戏运行的平台，负责衔接Tetris类和操作系统
	Tetris player1, player2;
	bool isGameOver, isGameQuit;
	Clock clock;
	unsigned int Window_width, Window_height;
	int stageWidth, stageHeight;
	bool isGameBegin, isGameHold;//------->游戏是否开始
	int isGameOverState;//------->游戏结束的状态
	Vector2i mCornPoint;//游戏区域位置
	int gridSize;//块大小（15）
	int imgSetNo;
	Texture tBackground, tTiles, tButtons, tSwitcher, tFrame, tCover, tScore, tGameOver;		//创建纹理对象
	Sprite	sBackground, sTiles, sButtons, sSwitcher, sFrame, sCover, sScore, sGameOver;		//创建精灵对象
	sf::IntRect ButtonRectStart, ButtonRectHold, ButtonRectLeft, ButtonRectRight;
	int ButtonState_Start, ButtonState_Hold;
	Font font;
	Text text;
	SoundBuffer sbWin, sbBoom;
	Sound soundWin, soundBoom;
	Music bkMusic;
	// A Clock starts counting as soon as it's created
	sf::Clock gameClock, mouseClickTimer;

	void gameInitial();
	void LoadMediaData();
	void TextOut();

	void gameInput();
	void gameOverInput();
	void gameLogic();
	void gameDraw();
	void gameRun();

	void DrawButton();
	void DrawResults();
};

