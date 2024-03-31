#define WINVER 0x0A00
#define _WIN32_WINNT 0x0A00

#include <graphics.h>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <conio.h>

#include <ShellScalingApi.h>		// 引用头文件
#pragma comment(lib, "Shcore.lib")	// 链接库文件

#define RATIO 80
#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define ROW_NUM 12
#define LINE_NUM 9 
#define isValid(next_pos) (next_pos##.x>0&&next_pos##.x<ROW_NUM&&next_pos##.x>0&&next_pos##.x<LINE_NUM)



#define KEY_UP      'w'
#define KEY_LEFT    'a'
#define KEY_RIGHT   'd'
#define KEY_DOWN    's'
#define KEY_QUIT    'q'

struct _POS {
	int x;//小人所在的行
	int y;//小人所在的列
};

enum _PROPS {
	WALL,//墙
	FLOOR,//地板
	BOX_DES,//箱子目的地
	MAN,//人
	BOX,//箱子
	HIT,//箱子命中目标
	OTHER//这个枚举变量表示这个游戏还可以添加新的元素
};

enum _DERECTION {
	UP,
	DOWN,
	LEFT,
	RIGHT
};

using namespace std;

int map[9][12] = {
	{0,0,0,0,0,0,0,0,0,0,0,0},
	{0,1,0,1,1,1,1,1,1,1,0,0},
	{0,1,4,1,0,2,1,0,2,1,0,0},
	{0,1,0,1,0,1,0,0,1,1,1,0},
	{0,1,0,2,0,1,1,4,1,1,1,0},
	{0,1,1,1,0,3,1,1,1,4,1,0},
	{0,1,2,1,1,4,1,1,1,1,1,0},
	{0,1,0,0,1,0,1,1,0,0,1,0},
	{0,0,0,0,0,0,0,0,0,0,0,0}
};

struct _POS man;

IMAGE bg_image, image[6];


/**********************************
*改变地图视图中的道具并且重新显示
* 输入:
*		line -道具在地图数组中的行下标
*		row  -道具在地图数组中的列下标
*		prop -道具的类型
***********************************/
void changeMap(struct _POS *pos, enum _PROPS prop) {
	map[pos->x][pos->y] = prop;
	putimage((SCREEN_WIDTH - ROW_NUM * RATIO) / 2 + pos->y * RATIO, (SCREEN_HEIGHT - LINE_NUM * RATIO) / 2 +  pos->x* RATIO, &image[prop]);
}

void gameControl(enum _DERECTION derect) {

	struct _POS next_pos = man;
	struct _POS next_next_pos = man;//人前进方向的前两个
	switch (derect) {
	case UP:
		next_pos.x--;
		next_next_pos.x -= 2;
		break;
	case DOWN:
		next_pos.x++;
		next_next_pos.x += 2;
		break;
	case LEFT:
		next_pos.y--;
		next_next_pos.y -= 2;
		break;
	case RIGHT:
		next_pos.y++;
		next_next_pos.y += 2;
		break;	
	}
	

	/*备注:
	在这里原作者提到,需要检查下一个位置的坐标(进行合法性检查),
	但是我认为没有必要,因为人只可以在允许行走的地方行走,但是
	这个地方一定位于地图内部,通过确保每一次移动的合法性,就可
	以确保全局的合法性,但是如果地板不一定在地图内部(哈?怎么可能?)
	*/
	if (isValid(next_pos)&&map[next_pos.x][next_pos.y] == FLOOR) {
		changeMap(&next_pos, MAN);//将人移动到下一个位置
		changeMap(&man, FLOOR);
		man = next_pos;
	}
	else if (isValid(next_pos) && map[next_pos.x][next_pos.y] == BOX) {//人的前方是地板
		//两种情况,箱子前面是地板或者箱子前面是目的地
		if (isValid(next_pos) && map[next_next_pos.x][next_next_pos.y] == FLOOR) {
			changeMap(&next_next_pos, BOX);
			changeMap(&next_pos, MAN);//将人移动到下一个位置
			changeMap(&man, FLOOR);
			man = next_pos;
		}else if (isValid(next_pos) && map[next_next_pos.x][next_next_pos.y] == BOX_DES) {
			changeMap(&next_next_pos, HIT);
			changeMap(&next_pos, MAN);//将人移动到下一个位置
			changeMap(&man, FLOOR);
			man = next_pos;
		}
	}
	
}

bool isGameOver() {
	for (int i = 0; i < LINE_NUM; i++) {
		for (int j = 0; j < ROW_NUM; j++) {
			if (map[i][j] == BOX_DES) return false;
		}
	}
	return true;
}


void gameOverScreen(IMAGE *bg) {
	putimage(0, 0, bg);
	settextcolor(WHITE);
	RECT rec = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT };
	settextstyle(20, 0, _T("宋体"));
	drawtext(_T("恭喜,你完成了游戏!"),&rec,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
}


int main(void) {
	SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);


	//载入背景
	loadimage(&bg_image, _T(R"(picture\blackground.bmp)"), SCREEN_WIDTH, SCREEN_HEIGHT, true);
	initgraph(SCREEN_WIDTH, SCREEN_HEIGHT);
	putimage(0, 0, &bg_image);

	//加载地图元素
	loadimage(&image[0], _T(R"(picture\wall_right.bmp)"), RATIO, RATIO, true);
	loadimage(&image[1], _T(R"(picture\floor.bmp)"), RATIO, RATIO, true);
	loadimage(&image[2], _T(R"(picture\des.bmp)"), RATIO, RATIO, true);
	loadimage(&image[3], _T(R"(picture\man.bmp)"), RATIO, RATIO, true);
	loadimage(&image[4], _T(R"(picture\box.bmp)"), RATIO, RATIO, true);
	loadimage(&image[5], _T(R"(picture\box.bmp)"), RATIO, RATIO, true);

	//绘制地图
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 12; j++) {
			
			if (map[i][j] == MAN) {//获取小人的位置.
				man.x = j;
				man.y = i;
			}
			putimage((SCREEN_WIDTH - ROW_NUM * RATIO) / 2 + j * RATIO, (SCREEN_HEIGHT - LINE_NUM * RATIO) / 2 + i * RATIO, &image[map[i][j]]);

		}
	}

	//获取热键是一个循环的过程,一旦退出,也是直接退出游戏
	bool quit = false;//
	do {
		if (_kbhit()) {//玩家按键
			char ch = _getch();
			if (ch >= 'A' && ch <= 'Z') {//
				ch += 32;
			}
			switch (ch) {
			case KEY_UP:
				gameControl(UP);
				break;
			case KEY_DOWN:
				gameControl(DOWN);
				break;
			case KEY_LEFT:
				gameControl(LEFT);
				break;
			case KEY_RIGHT:
				gameControl(RIGHT);
				break;
			case KEY_QUIT:
				quit = true;
				break;
			}

		}
		Sleep(30);

		if (isGameOver()) {
			gameOverScreen(&bg_image);
			system("pause");
			quit = true;
		}

	} while (!quit);

	//游戏结束
	closegraph();


	return 0;
}