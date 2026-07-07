//感谢FamTrinl大佬提供的Tetris游戏基础代码
//感谢助教邓璇小姐姐提出的Tetris双打的建议
//感谢助教丁筱琳小姐姐给做的游戏UI设计
//感谢金邵涵小哥哥教会我hard drop、底部缓冲、bag7、hold、wallkick、nextBlock等高端玩法
//感谢李文馨小姐姐让追加shadow效果以及消除动画效果的建议
//你们的建议和帮助，让这个版本的Tetris更加圆满！
//爱你们！！
//李仕 写于20191114
#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <array>
#include <iostream>
#include <sstream>
#include "AITetris.h"

#define  GRIDSIZE 35
#define  STAGE_WIDTH	10
#define  STAGE_HEIGHT	20
#define  P1_STAGE_CORNER_X		156
#define  P1_STAGE_CORNER_Y		174
#define  P2_STAGE_CORNER_X		844
#define  P2_STAGE_CORNER_Y		174
#define  FIELD_WIDTH	10
#define  FIELD_HEIGHT	24
#define  P1_FIELD_CORNER_X		156
#define  P1_FIELD_CORNER_Y		34
#define  P2_FIELD_CORNER_X		844
#define  P2_FIELD_CORNER_Y		34
#define  P1_NXET_CORNER_X		587
#define  P1_NXET_CORNER_Y		125
#define  P2_NXET_CORNER_X		702
#define  P2_NXET_CORNER_Y		125
#define  P1_SCORE_RIGHT_X		506//
#define  P1_SCORE_RIGHT_Y		174//
#define  P2_SCORE_RIGHT_X		1194//
#define  P2_SCORE_RIGHT_Y		174//
#define  HOLD_CORNER_X			660
#define  HOLD_CORNER_Y			595	//#define  HOLD_CORNER_Y			275

#define  P1_SCORE_CORNER_X		366
#define  P1_SCORE_CORNER_Y		110
#define  P2_SCORE_CORNER_X		1195
#define  P2_SCORE_CORNER_Y		110
enum PLAYROLE {
	roleNONE,				//空
	rolePLAYER1,		    //玩家1
	rolePLAYER2,			//玩家2
};
enum gridShape {
	shapeI, // I
	shapeS, // S
	shapeZ, // Z
	shapeT, // T
	shapeL, // L
	shapeJ, // J
	shapeO, // O
};

using namespace sf;			//SFML中的每个类都位于该命名空间之下，不设定sf命名空间的话，相应的函数前需要用作用域解析符，例如 sf::VideoMode(width* GRIDSIZE, height* GRIDSIZE)

static int holdcolorNum, holdShapeNum;
static Vector2i holdSquare[4];

class Tetris
{
public:
	Tetris();
	~Tetris();
	Vector2i mCornPoint, nextSquareCornPoint, holdSquareCornPoint;//游戏区域位置
	int role;
	int gridSize;//块大小（18）
	int imgBGno, imgSkinNo;
	Texture tTiles;
	Texture tBackground, tButtons, tNum, tTimer, tCounter, tGameOver;		//创建纹理对象
	Sprite	sBackground, sTiles, sButtons, sNum, sTimer, sCounter, sGameOver;		//创建精灵对象

	int Field[FIELD_HEIGHT][FIELD_WIDTH] = { 0 };

	Vector2i currentSquare[4], nextSquare[4], tempSquare[4], shadowSquare[4];

	int Figures[7][4] =
	{
		5,3,1,7, // I
		4,5,2,7, // S
		4,5,3,6, // Z
		5,3,4,7, // T
		5,3,2,7, // L
		5,7,3,6, // J
		2,3,4,5, // O
	};
	int dx, score;
	bool isRotate, isHold, isHardDrop, newShapeFlag, animationFlag;
	bool isGameOver;
	bool aiEnabled;
	int aiTargetRotation;
	int aiTargetX;
	int aiTargetY;
	int aiCurrentRotation;
	bool aiTargetDirty;
	int aiStallFrames;
	int aiLastMinY;
	int animationRow[4];
	float animationCtrlValue;
	int currentColorNum, nextcolorNum, tempcolorNum;// holdcolorNum
	int currentShapeNum, nextShapeNum, tempShapeNum;//holdShapeNum
	RenderWindow* window;
	float timer, delay;

	void Initial(Texture* tex, RenderWindow* window);
	void Input(const std::optional<sf::Event>& event);
	void aiControl();
	void Logic();
	void Draw();
	void traditonLogic();
	bool hitTest();

	int b7array[7] = { 0 }, b7Int;
	int Bag7();
	void xMove();
	void yMove();
	void slowLoading();
	void shadow();
	void hardDropFunc();
	void rotateFunc();
	void holdFunc();
	void checkLine();
	void clearLine();
	void newShapeFunc();
	void animationFunc(int i);
	void isWin();
};



