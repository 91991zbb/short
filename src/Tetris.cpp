#include "Tetris.h"
#include <algorithm>
#define DELAYVALUE 0.5

Tetris::Tetris()
    :tTiles(), tBackground(), tButtons(), tNum(), tTimer(), tCounter(), tGameOver(),
    sBackground(tBackground), sTiles(tTiles), sButtons(tButtons), sNum(tNum), sTimer(tTimer), sCounter(tCounter), sGameOver(tGameOver)
{

}

Tetris::~Tetris()
{
}

void Tetris::Initial(Texture* tex, RenderWindow* w)
{
    window = w;
    // tTiles = tex;
    tTiles = *tex;	//将方块的纹理传给Tetris对象
    dx = 0; //X方向偏移量
    score = 0;
    isGameOver = false;
    isRotate = false; //是否旋转
    isHold = false;	//是否有hold块图形
    isHardDrop = false; //是否硬降
    newShapeFlag = false; //初始化时候做第一次生成，后面有条件控制生成
    animationFlag = true; //动画开启,由游戏是否开始决定
    aiEnabled = false;
    aiTargetRotation = 0;
    aiTargetX = FIELD_WIDTH / 2;
    aiTargetY = 0;
    aiCurrentRotation = 0;
    aiTargetDirty = true;
    aiStallFrames = 0;
    aiLastMinY = 0;
    animationCtrlValue = 1.0;
    currentColorNum = 1;	//色块的颜色
    timer = 0;
    delay = DELAYVALUE;	//下落的速度，
    b7Int = 0;

    if (role == rolePLAYER1)
    {
        mCornPoint = { P1_FIELD_CORNER_X,	P1_FIELD_CORNER_Y };
        nextSquareCornPoint = { P1_NXET_CORNER_X, P1_NXET_CORNER_Y };
    }
    if (role == rolePLAYER2)
    {
        mCornPoint = { P2_FIELD_CORNER_X,	P2_FIELD_CORNER_Y };
        nextSquareCornPoint = { P2_NXET_CORNER_X, P2_NXET_CORNER_Y };
    }
    holdSquareCornPoint = { HOLD_CORNER_X, HOLD_CORNER_Y };
    // sTiles.setTexture(*tTiles);
    sTiles.setTexture(tTiles, true);
    //sTiles.scale(1.75,1.75);

    //初始化方块图形
    currentColorNum = 1 + rand() % 7;
    currentShapeNum = rand() % 7;

    //更新下个方块图形
    nextcolorNum = 1 + rand() % 7;
    nextShapeNum = rand() % 7;

    holdShapeNum = 10;//游戏初始时候，hold区的图形设置一个异常值

    for (int i = 0; i < 4; i++)
    {
        currentSquare[i].x = Figures[currentShapeNum][i] % 2 + FIELD_WIDTH / 2;
        currentSquare[i].y = Figures[currentShapeNum][i] / 2;
        nextSquare[i].x = Figures[nextShapeNum][i] % 2;
        nextSquare[i].y = Figures[nextShapeNum][i] / 2;

        animationRow[i] = 99;//要播放动画的行数, 99为异常值，表待定的意思
    }

    for (int i = 0; i < FIELD_HEIGHT; i++)
    {
        for (int j = 0; j < FIELD_WIDTH; j++)
        {
            Field[i][j] = 0;
        }
    }
}
void Tetris::Input(const std::optional<sf::Event>& event)
{
    if (!event)
        return;

    if (aiEnabled)
        return;

    if (role == rolePLAYER1)//玩家1的按键响应
    {
        if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
        {
            if (keyPressed->scancode == sf::Keyboard::Scancode::W)
                if (currentShapeNum != shapeO)
                    isRotate = true;
            if (keyPressed->scancode == sf::Keyboard::Scancode::A)
                dx = -1;
            else if (keyPressed->scancode == sf::Keyboard::Scancode::D)
                dx = 1;
            if (keyPressed->scancode == sf::Keyboard::Scancode::S)
                delay = DELAYVALUE / 10;
        }
        if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>())
        {
            if (keyReleased->scancode == sf::Keyboard::Scancode::Space)
                isHardDrop = true;
            if (keyReleased->scancode == sf::Keyboard::Scancode::LControl)
                isHold = true;
            if (keyReleased->scancode == sf::Keyboard::Scancode::A || keyReleased->scancode == sf::Keyboard::Scancode::D)
                dx = 0; //水平移动
            if (keyReleased->scancode == sf::Keyboard::Scancode::S)
                delay = DELAYVALUE;	//时间响应间隔变大，方块下降速度变慢        
        }
        // if (Keyboard::isKeyPressed(Keyboard::Scancode::S)) 	delay = 0.05;
    }
    if (role == rolePLAYER2)//玩家2的按键响应
    {
        if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
        {
            if (keyPressed->scancode == sf::Keyboard::Scancode::Up)
                if (currentShapeNum != shapeO)
                    isRotate = true;
            if (keyPressed->scancode == sf::Keyboard::Scancode::Left)
                dx = -1;
            else if (keyPressed->scancode == sf::Keyboard::Scancode::Right)
                dx = 1;
            if (keyPressed->scancode == sf::Keyboard::Scancode::Down)
                delay = DELAYVALUE / 10;
        }
        if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>())
        {
            if (keyReleased->scancode == sf::Keyboard::Scancode::Enter)
                isHardDrop = true;
            if (keyReleased->scancode == sf::Keyboard::Scancode::RControl)
                isHold = true;
            if (keyReleased->scancode == sf::Keyboard::Scancode::Left || keyReleased->scancode == sf::Keyboard::Scancode::Right)
                dx = 0; //水平移动
            if (keyReleased->scancode == sf::Keyboard::Scancode::Down)
                delay = DELAYVALUE;	//时间响应间隔变大，方块下降速度变慢
        }
        //if (Keyboard::isKeyPressed(Keyboard::Scancode::Down)) delay = 0.05;
    }
}

void Tetris::aiControl()
{
    if (!aiEnabled || isGameOver || newShapeFlag || animationFlag)
        return;

    delay = 1.0f;

    if (aiTargetDirty)
    {
        AITetrisGame::Board board{};
        for (int row = 0; row < FIELD_HEIGHT; ++row)
        {
            for (int col = 0; col < FIELD_WIDTH; ++col)
            {
                board[row][col] = Field[row][col] != 0 ? 1 : 0;
            }
        }

        AITetrisGame::AIConfig config;
        const AITetrisGame::Move move = AITetrisGame::chooseBestMoveForBoard(board, currentShapeNum, 0, 0, 0, config);
        aiTargetRotation = move.rotation;
        aiTargetX = move.x;
        aiTargetY = move.landingY;
        aiTargetDirty = false;
        aiCurrentRotation = 0;
        aiStallFrames = 0;
        aiLastMinY = FIELD_HEIGHT;
    }

    if (currentShapeNum != shapeO && aiCurrentRotation < aiTargetRotation)
    {
        isRotate = true;
        ++aiCurrentRotation;
        aiStallFrames = 0;
        return;
    }

    int minX = FIELD_WIDTH;
    int maxX = 0;
    for (int i = 0; i < 4; ++i)
    {
        minX = std::min(minX, currentSquare[i].x);
        maxX = std::max(maxX, currentSquare[i].x);
    }

    const int centerX = (minX + maxX) / 2;
    if (centerX < aiTargetX)
    {
        dx = 1;
        aiStallFrames = 0;
        return;
    }
    if (centerX > aiTargetX)
    {
        dx = -1;
        aiStallFrames = 0;
        return;
    }

    dx = 0;

    int minY = FIELD_HEIGHT;
    for (int i = 0; i < 4; ++i)
    {
        minY = std::min(minY, currentSquare[i].y);
    }

    if (minY == aiLastMinY)
    {
        ++aiStallFrames;
    }
    else
    {
        aiStallFrames = 0;
    }
    aiLastMinY = minY;

    if (minY >= aiTargetY || aiStallFrames > 2)
    {
        isHardDrop = true;
        aiTargetDirty = true;
        aiStallFrames = 0;
        return;
    }
}

void Tetris::traditonLogic()
{
    if (aiEnabled)
    {
        aiControl();
    }

    //hold方块图形
    if (isHold)
    {
        holdFunc();
        isHold = false;
    }
    //// <- 水平Move -> ///
    xMove();

    //////Rotate//////
    if (isRotate)
    {
        rotateFunc();
        isRotate = false;
    }

    slowLoading();
    ///////Tick下落//////
    if (timer > delay)
    {
        yMove();
        timer = 0;
    }

    shadow();

    if (isHardDrop)
    {
        hardDropFunc();
        isHardDrop = false;
    }
}
void Tetris::Logic()
{
    if (!animationFlag)
        traditonLogic();
    //如果有消行，则等动画结束再new新的方块
    if (newShapeFlag)
    {
        if (animationFlag == false)
        {
            ///////check lines//////////
            checkLine();
            if (isGameOver == false && animationFlag == false)
                newShapeFunc();//落地应该就要生成新的方块;  图形生成函数里面再更新Flag的状态
            isWin();
        }
        else
        {
            animationCtrlValue -= 0.1;
            if (animationCtrlValue < 0)
            {
                animationFlag = false;
                animationCtrlValue = 1.0;

                for (int i = 0; i < 4; i++)
                {
                    animationRow[i] = -1;
                }
                clearLine();
                newShapeFunc();
            }
        }
    }
}
void Tetris::isWin()
{
    //原本应该用碰撞检测来做结束判断，但此处，两方块位置有一不为零，游戏即结束
    //if(!hitTest())
    for (int i = 0; i < FIELD_WIDTH; i++)
    {
        if (Field[4][i])
            isGameOver = true;
    }
}
void Tetris::Draw()
{
    if (animationFlag == false)//动画管控
    {
        //绘制Shadow的方块
        for (int i = 0; i < 4; i++)
        {
            if (shadowSquare[i].y > 3)	// 进入第五行后才显示，避免舞台外部显示shadow的问题
            {
                sTiles.setTextureRect(IntRect({ currentColorNum * GRIDSIZE, 0 }, { GRIDSIZE, GRIDSIZE }));
                sTiles.setPosition({ static_cast<float>(shadowSquare[i].x * GRIDSIZE), static_cast<float>(shadowSquare[i].y * GRIDSIZE) });
                sTiles.setColor(Color(50, 50, 50, 255));	//设置阴影的颜色
                sTiles.move({ static_cast<float>(mCornPoint.x), static_cast<float>(mCornPoint.y) }); //offset
                window->draw(sTiles);
                sTiles.setColor(Color(255, 255, 255, 255));//颜色改回原色
            }
        }
        //绘制活动的方块；；绘制的顺序调整到前面，想想为什么呢？

        for (int i = 0; i < 4; i++)
        {
            if (currentSquare[i].y > 3)	// 进入第五行后才显示，即进入舞台界面才显示
            {
                sTiles.setTextureRect(IntRect({ currentColorNum * GRIDSIZE, 0 }, { GRIDSIZE, GRIDSIZE }));
                sTiles.setPosition({ static_cast<float>(currentSquare[i].x * GRIDSIZE), static_cast<float>(currentSquare[i].y * GRIDSIZE) });
                sTiles.move({ static_cast<float>(mCornPoint.x), static_cast<float>(mCornPoint.y) }); //offset
                window->draw(sTiles);
            }
        }
    }

    //绘制固定的方块
    for (int i = 4; i < FIELD_HEIGHT; i++)
        if (i == animationRow[0] || i == animationRow[1] || i == animationRow[2] || i == animationRow[3])
            animationFunc(i);//遇到动画行的时候，执行特效动画
        else
            for (int j = 0; j < FIELD_WIDTH; j++)
            {
                if (Field[i][j] == 0)
                    continue;
                sTiles.setTextureRect(IntRect({ Field[i][j] * GRIDSIZE, 0 }, { GRIDSIZE, GRIDSIZE }));
                sTiles.setPosition({ static_cast<float>(j * GRIDSIZE),static_cast<float>(i * GRIDSIZE) });
                sTiles.move({ static_cast<float>(mCornPoint.x),static_cast<float>(mCornPoint.y) }); //offset
                //animationFlag = false;
                window->draw(sTiles);
            }
    //绘制Next区的方块
    for (int i = 0; i < 4; i++)
    {
        sTiles.setTextureRect(IntRect({ nextcolorNum * GRIDSIZE, 0 }, { GRIDSIZE, GRIDSIZE }));
        sTiles.setPosition({ static_cast<float>(nextSquare[i].x * GRIDSIZE), static_cast<float>(nextSquare[i].y * GRIDSIZE) });
        sTiles.move({ static_cast<float>(nextSquareCornPoint.x), static_cast<float>(nextSquareCornPoint.y) }); //offset
        window->draw(sTiles);
    }
    //绘制Hold区的方块
    if (holdShapeNum < 8)//hold区图形正常时候进行绘制
        for (int i = 0; i < 4; i++)
        {
            sTiles.setTextureRect(IntRect({ holdcolorNum * GRIDSIZE, 0 }, { GRIDSIZE, GRIDSIZE }));
            sTiles.setPosition({ static_cast<float>(holdSquare[i].x * GRIDSIZE), static_cast<float>(holdSquare[i].y * GRIDSIZE) });
            sTiles.move({ static_cast<float>(holdSquareCornPoint.x),static_cast<float>(holdSquareCornPoint.y) }); //offset
            window->draw(sTiles);
        }
}


bool Tetris::hitTest()
{
    for (int i = 0; i < 4; i++)
        if (currentSquare[i].x < 0 || currentSquare[i].x >= FIELD_WIDTH || currentSquare[i].y >= FIELD_HEIGHT)
            return false;
        else if (Field[currentSquare[i].y][currentSquare[i].x])
            return false;

    return true;
}
int Tetris::Bag7()
{
    int num;
    srand(time(NULL));
    num = rand() % 7;
    b7array[b7Int] = num;


    for (int i = 0; i < b7Int; i++)
    {
        if (b7array[i] == num)
        {
            i = -1;//i++后归零，数组重新遍历
            num = rand() % 7;
        }
    }
    b7array[b7Int] = num;

    b7Int++;
    if (b7Int == 7)
    {
        b7Int = 0;
        for (int i = 0; i < 7; i++)
            b7array[i] = 0;
    }
    return num;
}
void Tetris::xMove()
{
    for (int i = 0; i < 4; i++)
    {
        tempSquare[i] = currentSquare[i];
        currentSquare[i].x += dx;
    }
    dx = 0;

    if (!hitTest()) //如果撞上了
        for (int i = 0; i < 4; i++)
            currentSquare[i] = tempSquare[i];//到左右的边界，不能移出边界
}
void Tetris::slowLoading()//底部缓冲
{
    if (aiEnabled)
        return;

    for (int i = 0; i < 4; i++)
    {
        tempSquare[i] = currentSquare[i];
        currentSquare[i].y += 1;
    }

    if (!hitTest())//如果撞上了
    {
        delay = DELAYVALUE * 2;
    }
    for (int i = 0; i < 4; i++)
        currentSquare[i] = tempSquare[i];
}
void Tetris::shadow()
{
    for (int i = 0; i < 4; i++)
    {
        shadowSquare[i] = currentSquare[i];
    }
    for (int j = 0; j < FIELD_HEIGHT; j++)    //一直下落
    {
        for (int i = 0; i < 4; i++)
        {
            currentSquare[i].y += 1;
        }
        if (!hitTest())	//如果撞上了
        {
            for (int i = 0; i < 4; i++)      //到底了，返回上一次位置
            {
                currentSquare[i].y -= 1;
            }
            break;
        }
    }
    for (int i = 0; i < 4; i++)
    {
        tempSquare[i] = currentSquare[i];
        currentSquare[i] = shadowSquare[i];
        shadowSquare[i] = tempSquare[i];
    }
}
void Tetris::hardDropFunc()
{
    for (int i = 0; i < 4; i++)
    {
        currentSquare[i] = shadowSquare[i];
    }
}

void Tetris::yMove()
{
    for (int i = 0; i < 4; i++)
    {
        tempSquare[i] = currentSquare[i];
        currentSquare[i].y += 1;
    }

    if (!hitTest())//如果撞上了
    {
        for (int i = 0; i < 4; i++)
            Field[tempSquare[i].y][tempSquare[i].x] = currentColorNum;
        //newShape();//由于动画管理的需要，此处进行了代码封装，并移到外部，等动画结束后再new新的图形。
        newShapeFlag = true; //撞上了就要有新的图形生成
    }
}
void Tetris::rotateFunc()
{
    int originalHeight = currentSquare[0].y;
    for (int j = 0; j < 4; j++)//wall kick 中心偏移
    {
        Vector2i p = currentSquare[j]; //设置旋转中心点
        for (int i = 0; i < 4; i++)
        {//顺时针旋转90度
            int x = currentSquare[i].y - p.y;//原Y方向距离中心点的差值，作为新的差值，传递给X方向
            int y = currentSquare[i].x - p.x;//原X方向距离中心点的差值，作为新的差值，传递给Y方向
            currentSquare[i].x = p.x - x;//新坐标X=中心点坐标-新的X方向差值
            currentSquare[i].y = p.y + y;//新坐标Y=中心点坐标+新的Y方向差值
        }
        if (!hitTest()) //如果撞上了
            for (int i = 0; i < 4; i++)
                currentSquare[i] = tempSquare[i];
        else
        {
            int detaY = 0;
            detaY = currentSquare[0].y - originalHeight;//新重心与原重心的高度差
            if (detaY != 0)
                for (int i = 0; i < 4; i++)
                {//高度差修正
                    currentSquare[i].y -= detaY;
                }
            /*if(currentShapeNum != shapeT)
                if (j != 0)	//如果中心发生偏移，则切换中心。头文件的预设值按中心的潜在可能做了排序
                {
                    p = currentSquare[0];
                    currentSquare[0] = currentSquare[1];
                    currentSquare[1] = p;
                }*/
            break;	//	结束循环
        }
    }
}
void Tetris::holdFunc()
{
    Vector2i backUpSquare[4];
    tempcolorNum = holdcolorNum;
    tempShapeNum = holdShapeNum;

    holdcolorNum = currentColorNum;
    holdShapeNum = currentShapeNum;

    for (int i = 0; i < 4; i++)
    {
        holdSquare[i].x = Figures[holdShapeNum][i] % 2;
        holdSquare[i].y = Figures[holdShapeNum][i] / 2;
        tempSquare[i].x = Figures[tempShapeNum][i] % 2;
        tempSquare[i].y = Figures[tempShapeNum][i] / 2;
        backUpSquare[i] = currentSquare[i];
    }

    if (tempShapeNum > 8)//hold区图形的异常值表示hold区为空的状态，所以要从Next区取值
    {//如果原hold区为空，则当前图形从Next取
        newShapeFunc();//此处代码进行了封装20191122李仕
    }
    else//当前图形取原来hold图形的值，既发生交换
    {
        currentColorNum = tempcolorNum;
        currentShapeNum = tempShapeNum;
        //从hold区置换过来的方块图形，在舞台上坐标的计算。
        int minCurrentX = currentSquare[0].x,
            minCurrentY = currentSquare[0].y,
            minTempX = tempSquare[0].x,
            minTempY = tempSquare[0].y;
        int dx, dy;
        for (int i = 1; i < 4; i++)
        {
            if (currentSquare[i].x < minCurrentX)
                minCurrentX = currentSquare[i].x;
            if (currentSquare[i].y < minCurrentY)
                minCurrentY = currentSquare[i].y;
            if (tempSquare[i].x < minTempX)
                minTempX = tempSquare[i].x;
            if (tempSquare[i].y < minTempY)
                minTempY = tempSquare[i].y;
        }
        dx = minCurrentX - minTempX;
        dy = minCurrentY - minTempY;
        for (int i = 0; i < 4; i++)
        {
            currentSquare[i].x = tempSquare[i].x + dx;
            currentSquare[i].y = tempSquare[i].y + dy;
            holdSquare[i].x = Figures[holdShapeNum][i] % 2;
            holdSquare[i].y = Figures[holdShapeNum][i] / 2;
        }
    }

    if (!hitTest()) //如果撞上了
    {
        holdcolorNum = tempcolorNum;
        holdShapeNum = tempShapeNum;

        for (int i = 0; i < 4; i++)
        {
            currentSquare[i] = backUpSquare[i];//
        }
    }
}
void Tetris::clearLine()
{
    int k = FIELD_HEIGHT - 1;
    for (int i = FIELD_HEIGHT - 1; i > 0; i--)
    {
        int xCount = 0;
        for (int j = 0; j < FIELD_WIDTH; j++)
        {
            if (Field[i][j])
                xCount++;
            Field[k][j] = Field[i][j];//下降
        }
        if (xCount < FIELD_WIDTH)
            k--;
    }
}
void Tetris::checkLine()
{
    int k = FIELD_HEIGHT - 1;
    int yCount = 0;
    for (int i = FIELD_HEIGHT - 1; i > 0; i--)
    {
        int xCount = 0;
        for (int j = 0; j < FIELD_WIDTH; j++)
        {
            if (Field[i][j])
                xCount++;
            //Field[k][j] = Field[i][j];//下降
        }
        if (xCount < FIELD_WIDTH)
            k--;
        else
        {
            animationRow[yCount] = i;//要播放动画的行数
            yCount++;
            animationFlag = true;
        }
    }
    switch (yCount)
    {
    case 1:
        score += 10;
        break;
    case 2:
        score += 30;
        break;
    case 3:
        score += 60;
        break;
    case 4:
        score += 100;
        break;
    }
    //得分后的动画策略
    //①尽量不要去
}
void Tetris::newShapeFunc()
{
    //取下个方块图形
    currentColorNum = nextcolorNum;
    currentShapeNum = nextShapeNum;

    //更新下个方块图形
    nextcolorNum = 1 + rand() % 7;
    nextShapeNum = Bag7();

    for (int i = 0; i < 4; i++)
    {
        currentSquare[i] = nextSquare[i];//当前块更新
        currentSquare[i].x = currentSquare[i].x + FIELD_WIDTH / 2;
        nextSquare[i].x = Figures[nextShapeNum][i] % 2;
        nextSquare[i].y = Figures[nextShapeNum][i] / 2;
    }

    shadow();

    newShapeFlag = false;//这样下次才能再进来
    aiTargetDirty = true;
    aiCurrentRotation = 0;
    aiStallFrames = 0;
    aiLastMinY = 0;

    for (int i = 0; i < 4; i++)
        animationRow[i] = 99;//本来应该动画播放完之后就清零，但容易造成框架结构混乱；这里99为异常值，表待定的意思
}
void Tetris::animationFunc(int i)
{
    Vector2f p;
    sTiles.scale({ animationCtrlValue, animationCtrlValue });
    p = sTiles.getOrigin();
    sTiles.setOrigin({ GRIDSIZE / 2, GRIDSIZE / 2 });
    sTiles.rotate(sf::degrees(360 * animationCtrlValue));
    for (int j = 0; j < FIELD_WIDTH; j++)
    {
        if (Field[i][j] == 0)
            continue;
        sTiles.setTextureRect(IntRect({ Field[i][j] * GRIDSIZE, 0 }, { GRIDSIZE, GRIDSIZE }));
        sTiles.setPosition({ static_cast<float>(j * GRIDSIZE), static_cast<float>(i * GRIDSIZE) });
        sTiles.move({ static_cast<float>(mCornPoint.x + GRIDSIZE / 2), static_cast<float>(mCornPoint.y + GRIDSIZE / 2) }); //offset
        //让方块旋转并缩小
        window->draw(sTiles);
    }
    sTiles.scale({ static_cast<float>(1.0 / animationCtrlValue), static_cast<float>(1.0 / animationCtrlValue) });
    sTiles.rotate(sf::degrees(-360 * animationCtrlValue));
    sTiles.setOrigin(p);
}