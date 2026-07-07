/*
 * 项目名称：s101cmake-sfml-tetris
 * 开发者：李仕
 * 所属组织：xLab-HDU
 * 代码仓库：https://github.com/xLab-HDU
 * 
 * 本代码为开源项目，欢迎提交 Issue 和 Pull Request。
 */

#include "Game.h"

int main()
{
	Game tetrisGame;
	while (tetrisGame.window.isOpen())
	{
		tetrisGame.gameRun();
	}
	return 0;
}