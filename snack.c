#pragma warning (disable:4996)
#include<stdio.h>
#include<time.h>
#include<Windows.h>

/*游戏中需要定义方向，因此定义上下左右四个常量*/

#define DIR_UP		(int) 0 //定义向上移动
#define DIR_DOWN    (int) 1 //下
#define DIR_LEFT	(int) 2 //左
#define DIR_RIGHT	(int) 3 //右


/*整个游戏由20*40的二维数组记录，也定义了空白字符，墙壁字符，空字符，游戏矩阵的大小，以及蛇身由大写数字组成*/

#define blank		(char) ' '		//空白字符
#define fence		(char) 37		//墙壁字符
#define empty		(char) '\0'		//空字符
#define LMARGIN		(int)  5		//左边空白
#define TMARGIN		(int)  2		//上方空白

char CHART[20][41];					//记录整个游戏字符的矩阵
char LETTER[] = 
{
	'A','B','C','D','E','F','G','H','I','J','K','L','M',
	'N','O','P','Q','R','S','T','U','V','W','X','Y','Z'
};									//表示26个大写字母数组

/*游戏角色‘字母蛇’由结构体定义，坐标由pos定义*/

typedef struct POS
{
	char X;							//行号
	char Y;							//列号
}POS;

//表示蛇节点结构体

typedef struct SNACK
{
	struct POS		pos;			//当前行号
	char			letter;			//字母
	struct SNACK	*NEXT;			//下一个节点地址
}SNACK;

/*SNACK结构体指针snack，snack同时也表示链表的表头。*/

char foodLetter;					//表示食物字符
struct SNACK *snack = NULL;			//表示蛇头的节点
char GameRuning;					//游戏状态（0表示未运行，1个表示运行中）

/*函数声明代码*/

char PressKey(int key);						//游戏按键处理
void GetKeyCode(char *key);					//获取当前按键在字符
void PrintGameChar();						//在屏幕上输出整个游戏字符的矩阵
void CreateChain(char NowLetter, struct POS pos);							//创建新的蛇节点
void CreateFood();							//创建食物字母
int EatSelf(struct POS pos);				//判断是否吃到自己
void MoveSnack(struct POS pos,int dir);		//移动字母蛇
void InitiateGame();						//游戏初始化处理
void GameRun();								//游戏运行处理

//游戏按键处理
char PressKey(int key)
{
	key = toupper((char)key);

	if (key == 27)							//按下ESC键退出
		return 0;
	
	switch (key)							//改变蛇头方向并把蛇头对着临时坐标赋值
	{
	case 75:								//左方向键
	case 65:								//"A"
		MoveSnack(snack->pos, DIR_LEFT);	//左
		break;
	case 80:								//下方向键
	case 83:								//"下"
		MoveSnack(snack->pos, DIR_DOWN);	//下
		break;
	case 77:								//右方向键
	case 68:								//"D"
		MoveSnack(snack->pos, DIR_RIGHT);	//右
		break;
	case 72:								//上方向键
	case 87:								//"W"
		MoveSnack(snack->pos, DIR_UP);		//上
		break;
	default:
		break;
	}

	return 1;
}

//移动蛇头
void MoveSnack(struct POS pos, int dir)
{
	struct POS newPos;
	struct POS nowPos;
	struct POS nextPos;
	struct SNACK *p;
	int n = 0;

	HANDLE handle;
	COORD coord = { LMARGIN + 10,TMARGIN + 9 };	//控制台屏幕缓冲坐标结构体对象
	handle = GetStdHandle(STD_OUTPUT_HANDLE);	//获取标准输出句柄
	SetConsoleCursorPosition(handle, coord);	//设置当前坐标为默认右上角坐标

	//根据方向和上一坐标位置计算移动后的新坐标
	switch (dir)
	{
	case DIR_UP:
		newPos.Y = pos.Y - 1;
		newPos.X = pos.X;
		break;
	case DIR_DOWN:
		newPos.Y = pos.Y + 1;
		newPos.X = pos.X;
		break;
	case DIR_LEFT:
		newPos.Y = pos.Y;
		newPos.X = pos.X - 1;
		break;
	case DIR_RIGHT:
		newPos.Y = pos.Y;
		newPos.X = pos.X + 1;
		break;
	default:
		break;
	}

	if (CHART[newPos.Y][newPos.X] == fence)		//如果当前坐标为墙壁字母，则蛇由于撞到墙壁死去，gameover
	{
		printf("你撞墙了----GAME OVER\n");
		GameRuning = 0;
		coord.Y = coord.Y + 1;
		coord.X = coord.X - 2;
		SetConsoleCursorPosition(handle, coord);//设置当前坐标默认为左上角坐标
		return;
	}

	if (EatSelf(newPos) == 1)
	{
		printf("吃自己了----GAME OVER\n");
		GameRuning = 0;
		coord.Y = coord.Y + 1;
		coord.X = coord.X - 2;
		SetConsoleCursorPosition(handle, coord);//设置当前坐标默认为左上角坐标
		return;
	}

	if (GameRuning == 1)
	{
		if (CHART[newPos.Y][newPos.X] == foodLetter)//吃到食物
		{
			CreateChain(foodLetter - 32, newPos);	//产生新的节点
			CreateFood();						//随机产生食物
		}
		else
		{
			p = snack;							//蛇向新的位置移动
			while (p != NULL)					//遍历链表，将蛇头移到最新的位置，其余节点的位置为其上个节点的位置
			{
				if (n == 0)
				{
					nowPos = newPos;
					nextPos = p->pos;
				}
				else
				{
					nowPos = nextPos;
					nextPos = p->pos;
				}
				p->pos = nowPos;
				CHART[nowPos.Y][nowPos.X] = p->letter;
				p = p->NEXT;
				n = n + 1;
			}

			if (p == NULL)						//最后一个节点变成空白字符
			{
				CHART[nextPos.Y][nextPos.X] = blank;
			}
		}
	}
}

//创建新的蛇节点
void CreateChain(char NowLetter, struct POS pos)
{
	struct SNACK *p_new = NULL;
	if (snack == NULL)							//没有任何节点的时候，创建第一个节点
	{
		snack = malloc(sizeof(struct SNACK));
		snack->letter = NowLetter;
		snack->pos = pos;
		snack->NEXT = NULL;
		return;
	}
	if (snack != NULL)							//生成新的其他节点
	{
		p_new = malloc(sizeof(struct SNACK));
		p_new->letter = NowLetter;
		p_new->pos = pos;
		p_new->NEXT = snack;
		snack = p_new;
	}
}

//创建食物字母
void CreateFood()
{
	int exists = 1;
	int x, y;
	struct SNACK *snk;
	
	while (exists == 1)
	{
		srand((unsigned int)time(NULL));		//产生下一个食物的随机位置
		x = rand() % 39;
		y = rand() % 19;
		if (y <= 0)y = 1;
		if (x <= 0)x = 1;
		snk = snack;
		while (snk != NULL)
		{
			if (snk->pos.X == x&&snk->pos.Y == y)//检查新的位置是否为蛇身的位置
			{
				exists = 1;
				break;
			}
			snk = snk->NEXT;
		}
		exists = 0;
	}
	foodLetter = LETTER[rand() % 26] + 32;
	CHART[y][x] = foodLetter;					//修改CHART数组对应的位置
}

//判断是否吃到自己
int EatSelf(struct POS pos)
{
	struct SNACK * p_ls;
	p_ls = snack;

	while (p_ls != NULL)
	{
		if (p_ls->pos.X == pos.X && p_ls->pos.Y == pos.Y)
		{
			return 1;
		}
		p_ls = p_ls->NEXT;
	}
	return 0;
}

//在屏幕上输出整个游戏字符的矩阵
void PrintGameChar()
{
	char i;
	HANDLE handle;
	COORD coord = { LMARGIN,TMARGIN };				//控制台屏幕缓冲坐标结构体对象
	handle = GetStdHandle(STD_OUTPUT_HANDLE);		//获取标准输出句柄
	for (i = 0; i < 20; i++)
	{
		SetConsoleCursorPosition(handle, coord);	//设置当前坐标默认左上角坐标
		printf("%s\n", CHART[i]);
		coord.Y = coord.Y + 1;
	}
}

//游戏初始化处理
void InitiateGame()
{
	char i, j;
	struct POS movePos;
	char headLetter;
	
	for (i = 0; i < 20; i++)						//初始化CHART数组中所有的字符
	{
		for (j = 0; j < 41; j++)
		{
			if (j != 40 && (i == 0 || i == 19 || j == 0 || j == 39))
			{
				CHART[i][j] = fence;				//墙壁
			}
			else if (j == 40)
			{
				CHART[i][j] = empty;
			}
			else
			{
				CHART[i][j] = blank;
			}
		}
	}

	CreateFood();

	movePos.X = rand() % 40;
	movePos.Y = rand() % 19;
	if (movePos.Y <= 5)
		movePos.Y = 5;								//太靠近上墙壁
	if (movePos.X <= 5)
		movePos.X = 5;								//太靠近左墙壁
	if (movePos.Y >= 18)
		movePos.Y = 13;								//太靠近下墙壁
	if (movePos.X >= 39)
		movePos.X = 34;								//太靠近右墙壁

	headLetter = LETTER[rand() % 26];				//随机产生一个蛇头字母由食物字母变成

	CHART[movePos.Y][movePos.X] = headLetter;		//修改CHART数组对应的值

	CreateChain(headLetter, movePos);				
}

//游戏运行处理
void GameRun()
{
	char key;
	unsigned long * p = NULL;
	HANDLE hThread;

	system("title 贪吃蛇");
	InitiateGame();

	key = 75;
	hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)GetKeyCode, &key, 0, p);
	GameRuning = 1;
	while (GameRuning)
	{
		PrintGameChar();
		WaitForSingleObject(hThread, 200);
		PressKey(key);
	}
}

//获取当前按键字符
void GetKeyCode(char * keycode)
{
	while (GameRuning)
	{
		*keycode = getch();
	}
}

//主函数
int main()
{
	GameRun();
	return 0;
}