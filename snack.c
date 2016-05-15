#pragma warning (disable:4996)
#include<stdio.h>
#include<time.h>
#include<Windows.h>

/*��Ϸ����Ҫ���巽����˶������������ĸ�����*/

#define DIR_UP		(int) 0 //���������ƶ�
#define DIR_DOWN    (int) 1 //��
#define DIR_LEFT	(int) 2 //��
#define DIR_RIGHT	(int) 3 //��


/*������Ϸ��20*40�Ķ�ά�����¼��Ҳ�����˿հ��ַ���ǽ���ַ������ַ�����Ϸ����Ĵ�С���Լ������ɴ�д�������*/

#define blank		(char) ' '		//�հ��ַ�
#define fence		(char) 37		//ǽ���ַ�
#define empty		(char) '\0'		//���ַ�
#define LMARGIN		(int)  5		//��߿հ�
#define TMARGIN		(int)  2		//�Ϸ��հ�

char CHART[20][41];					//��¼������Ϸ�ַ��ľ���
char LETTER[] = 
{
	'A','B','C','D','E','F','G','H','I','J','K','L','M',
	'N','O','P','Q','R','S','T','U','V','W','X','Y','Z'
};									//��ʾ26����д��ĸ����

/*��Ϸ��ɫ����ĸ�ߡ��ɽṹ�嶨�壬������pos����*/

typedef struct POS
{
	char X;							//�к�
	char Y;							//�к�
}POS;

//��ʾ�߽ڵ�ṹ��

typedef struct SNACK
{
	struct POS		pos;			//��ǰ�к�
	char			letter;			//��ĸ
	struct SNACK	*NEXT;			//��һ���ڵ��ַ
}SNACK;

/*SNACK�ṹ��ָ��snack��snackͬʱҲ��ʾ����ı�ͷ��*/

char foodLetter;					//��ʾʳ���ַ�
struct SNACK *snack = NULL;			//��ʾ��ͷ�Ľڵ�
char GameRuning;					//��Ϸ״̬��0��ʾδ���У�1����ʾ�����У�

/*������������*/

char PressKey(int key);						//��Ϸ��������
void GetKeyCode(char *key);					//��ȡ��ǰ�������ַ�
void PrintGameChar();						//����Ļ�����������Ϸ�ַ��ľ���
void CreateChain(char NowLetter, struct POS pos);							//�����µ��߽ڵ�
void CreateFood();							//����ʳ����ĸ
int EatSelf(struct POS pos);				//�ж��Ƿ�Ե��Լ�
void MoveSnack(struct POS pos,int dir);		//�ƶ���ĸ��
void InitiateGame();						//��Ϸ��ʼ������
void GameRun();								//��Ϸ���д���

//��Ϸ��������
char PressKey(int key)
{
	key = toupper((char)key);

	if (key == 27)							//����ESC���˳�
		return 0;
	
	switch (key)							//�ı���ͷ���򲢰���ͷ������ʱ���긳ֵ
	{
	case 75:								//�����
	case 65:								//"A"
		MoveSnack(snack->pos, DIR_LEFT);	//��
		break;
	case 80:								//�·����
	case 83:								//"��"
		MoveSnack(snack->pos, DIR_DOWN);	//��
		break;
	case 77:								//�ҷ����
	case 68:								//"D"
		MoveSnack(snack->pos, DIR_RIGHT);	//��
		break;
	case 72:								//�Ϸ����
	case 87:								//"W"
		MoveSnack(snack->pos, DIR_UP);		//��
		break;
	default:
		break;
	}

	return 1;
}

//�ƶ���ͷ
void MoveSnack(struct POS pos, int dir)
{
	struct POS newPos;
	struct POS nowPos;
	struct POS nextPos;
	struct SNACK *p;
	int n = 0;

	HANDLE handle;
	COORD coord = { LMARGIN + 10,TMARGIN + 9 };	//����̨��Ļ��������ṹ�����
	handle = GetStdHandle(STD_OUTPUT_HANDLE);	//��ȡ��׼������
	SetConsoleCursorPosition(handle, coord);	//���õ�ǰ����ΪĬ�����Ͻ�����

	//���ݷ������һ����λ�ü����ƶ����������
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

	if (CHART[newPos.Y][newPos.X] == fence)		//�����ǰ����Ϊǽ����ĸ����������ײ��ǽ����ȥ��gameover
	{
		printf("��ײǽ��----GAME OVER\n");
		GameRuning = 0;
		coord.Y = coord.Y + 1;
		coord.X = coord.X - 2;
		SetConsoleCursorPosition(handle, coord);//���õ�ǰ����Ĭ��Ϊ���Ͻ�����
		return;
	}

	if (EatSelf(newPos) == 1)
	{
		printf("���Լ���----GAME OVER\n");
		GameRuning = 0;
		coord.Y = coord.Y + 1;
		coord.X = coord.X - 2;
		SetConsoleCursorPosition(handle, coord);//���õ�ǰ����Ĭ��Ϊ���Ͻ�����
		return;
	}

	if (GameRuning == 1)
	{
		if (CHART[newPos.Y][newPos.X] == foodLetter)//�Ե�ʳ��
		{
			CreateChain(foodLetter - 32, newPos);	//�����µĽڵ�
			CreateFood();						//�������ʳ��
		}
		else
		{
			p = snack;							//�����µ�λ���ƶ�
			while (p != NULL)					//������������ͷ�Ƶ����µ�λ�ã�����ڵ��λ��Ϊ���ϸ��ڵ��λ��
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

			if (p == NULL)						//���һ���ڵ��ɿհ��ַ�
			{
				CHART[nextPos.Y][nextPos.X] = blank;
			}
		}
	}
}

//�����µ��߽ڵ�
void CreateChain(char NowLetter, struct POS pos)
{
	struct SNACK *p_new = NULL;
	if (snack == NULL)							//û���κνڵ��ʱ�򣬴�����һ���ڵ�
	{
		snack = malloc(sizeof(struct SNACK));
		snack->letter = NowLetter;
		snack->pos = pos;
		snack->NEXT = NULL;
		return;
	}
	if (snack != NULL)							//�����µ������ڵ�
	{
		p_new = malloc(sizeof(struct SNACK));
		p_new->letter = NowLetter;
		p_new->pos = pos;
		p_new->NEXT = snack;
		snack = p_new;
	}
}

//����ʳ����ĸ
void CreateFood()
{
	int exists = 1;
	int x, y;
	struct SNACK *snk;
	
	while (exists == 1)
	{
		srand((unsigned int)time(NULL));		//������һ��ʳ������λ��
		x = rand() % 39;
		y = rand() % 19;
		if (y <= 0)y = 1;
		if (x <= 0)x = 1;
		snk = snack;
		while (snk != NULL)
		{
			if (snk->pos.X == x&&snk->pos.Y == y)//����µ�λ���Ƿ�Ϊ�����λ��
			{
				exists = 1;
				break;
			}
			snk = snk->NEXT;
		}
		exists = 0;
	}
	foodLetter = LETTER[rand() % 26] + 32;
	CHART[y][x] = foodLetter;					//�޸�CHART�����Ӧ��λ��
}

//�ж��Ƿ�Ե��Լ�
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

//����Ļ�����������Ϸ�ַ��ľ���
void PrintGameChar()
{
	char i;
	HANDLE handle;
	COORD coord = { LMARGIN,TMARGIN };				//����̨��Ļ��������ṹ�����
	handle = GetStdHandle(STD_OUTPUT_HANDLE);		//��ȡ��׼������
	for (i = 0; i < 20; i++)
	{
		SetConsoleCursorPosition(handle, coord);	//���õ�ǰ����Ĭ�����Ͻ�����
		printf("%s\n", CHART[i]);
		coord.Y = coord.Y + 1;
	}
}

//��Ϸ��ʼ������
void InitiateGame()
{
	char i, j;
	struct POS movePos;
	char headLetter;
	
	for (i = 0; i < 20; i++)						//��ʼ��CHART���������е��ַ�
	{
		for (j = 0; j < 41; j++)
		{
			if (j != 40 && (i == 0 || i == 19 || j == 0 || j == 39))
			{
				CHART[i][j] = fence;				//ǽ��
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
		movePos.Y = 5;								//̫������ǽ��
	if (movePos.X <= 5)
		movePos.X = 5;								//̫������ǽ��
	if (movePos.Y >= 18)
		movePos.Y = 13;								//̫������ǽ��
	if (movePos.X >= 39)
		movePos.X = 34;								//̫������ǽ��

	headLetter = LETTER[rand() % 26];				//�������һ����ͷ��ĸ��ʳ����ĸ���

	CHART[movePos.Y][movePos.X] = headLetter;		//�޸�CHART�����Ӧ��ֵ

	CreateChain(headLetter, movePos);				
}

//��Ϸ���д���
void GameRun()
{
	char key;
	unsigned long * p = NULL;
	HANDLE hThread;

	system("title ̰����");
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

//��ȡ��ǰ�����ַ�
void GetKeyCode(char * keycode)
{
	while (GameRuning)
	{
		*keycode = getch();
	}
}

//������
int main()
{
	GameRun();
	return 0;
}