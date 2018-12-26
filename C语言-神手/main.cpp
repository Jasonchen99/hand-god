/*******
项目名称：神手
项目作者：陈鹏宇
创建时间：2018年6月20日
项目说明：完整代码
*******/
#include <graphics.h>
#include <cstdio>
#include <cstring>
#include <time.h>
#include <cstdlib>
#include <windows.h>
#include <iostream>
#include <conio.h>
#include <stdlib.h>

PIMAGE bk;
PIMAGE helpbk;
PIMAGE helping;
PIMAGE soundon;
PIMAGE soundoff;
PIMAGE map1;
PIMAGE map2;
PIMAGE mstone;
PIMAGE barrier1;
PIMAGE barrier2;
PIMAGE barrier3;
PIMAGE rlist;
PIMAGE fingerL;
PIMAGE fingerR;
PIMAGE scoreboard;
PIMAGE gameoverbk;
PIMAGE returning;
PIMAGE restart;

using namespace std;

typedef struct postion
{
    int x;
    int y;
} POSITION;
POSITION s[4];

int Switch=1;
int mapy1=0;
int mapy2=-844;
int mapspeed=5;
int speed=5;
float score=0;
int level=2;

int me1x=142;
int me2x=260;
int me1y=642;
int me2y=642;

void Interface();
void playGame();
void helpface();

int length=0;
float data[10];

int readGameFile()
{
    FILE *fp;
    char s[20];
    char t[50];
    setcolor(BLACK);
    setfont(40,0,"微软雅黑");
    fp=fopen("gamefile.txt","r+");
    if(fp==NULL)
    {
        ege::outtextxy(120,200,"Open file error !");
        exit(0);
    }
    int i=0;
    while(!feof(fp))
    {
        fread(&data[i],sizeof(data),1,fp);
        i++;
    }
    length=--i;

    for(int i=0; i<length; i++)
    {
        sprintf(s,"No.%d: %.1f m",i+1,data[i]);
        ege::outtextxy(145,i*60+113,s);
    }
    fclose(fp);
}

int saveGame(float Tscore)
{
    FILE *fp;
    length=0;
    fp=fopen("gamefile.txt","r");
    if(fp==NULL)
    {
        ege::outtextxy(120,200,"Open file error !");
        exit(0);
    }
    int i=0;
    while(!feof(fp) && i<10)
    {
        fread(&data[i],sizeof(data),1,fp);
        i++;
    }
    length=--i;
    fclose(fp);

    float ex;
    ex=Tscore;
    for(i=length; i>=0; i--)
    {
        if(ex>data[i])
        {
            data[i+1]=data[i];
        }
        else
            break;
    data[i]=ex;
    }

    fp=fopen("gamefile.txt","w");
    if(fp==NULL)
    {
        ege::outtextxy(120,200,"Open file error !");
        exit(0);
    }
    for(int i=0; i<length+1; i++)
        fwrite(&data[i],sizeof(data),1,fp);
    fclose(fp);
}

mouse_msg msg;
int Mouse_X, Mouse_Y;
bool Mouse_Up;
bool Mouse_Down;
bool Mouse_Left;
bool Mouse_Right;

int initmouse()
{
    Mouse_X = 0;
    Mouse_Y = 0;
    Mouse_Down = false;
    Mouse_Left = false;
    Mouse_Right = false;
    Mouse_Up = false;
    return 0;
}
int updatemouse()
{
    mouse_msg msg = { 0 };
    while (mousemsg())
    {
        msg = getmouse();
        Mouse_X = msg.x;
        Mouse_Y = msg.y;
        Mouse_Down = msg.is_down();
        Mouse_Left = msg.is_left();
        Mouse_Right = msg.is_right();
        Mouse_Up = msg.is_up();
    }
    return 0;
}

void getImg()
{
    map1=newimage();
    getimage(map1,"pic\\road1.png");
    map2=newimage();
    getimage(map2,"pic\\road2.png");
    helpbk=newimage();
    getimage(helpbk,"pic\\helpbk.png");
    helping=newimage();
    getimage(helping,"pic\\helping.png");
    bk=newimage();
    getimage(bk,"pic\\bk.png");
    soundon=newimage();
    getimage(soundon,"pic\\soundon.png");
    soundoff=newimage();
    getimage(soundoff,"pic\\soundoff.png");
    rlist=newimage();
    getimage(rlist,"pic\\rlist.png");
    mstone=newimage();
    getimage(mstone,"pic\\ms.png");
    barrier1=newimage();
    getimage(barrier1,"pic\\b1.png");
    barrier2=newimage();
    getimage(barrier2,"pic\\b2.png");
    fingerL=newimage();
    getimage(fingerL,"pic\\fl.png");
    fingerR=newimage();
    getimage(fingerR,"pic\\fr.png");
    scoreboard=newimage();
    getimage(scoreboard,"pic\\scoreboard.png");
    gameoverbk=newimage();
    getimage(gameoverbk,"pic\\gameoverbk.png");
    returning=newimage();
    getimage(returning,"pic\\return.png");
    restart=newimage();
    getimage(restart,"pic\\restart.png");
}
void delImg()
{
    delimage(bk);
    delimage(soundon);
    delimage(soundon);
    delimage(map1);
    delimage(map2);
    delimage(mstone);
    delimage(barrier1);
    delimage(barrier2);
    delimage(fingerL);
    delimage(fingerR);
    delimage(helpbk);
    delimage(helping);
    delimage(rlist);
}

void getSound()
{
    mciSendString("open sounds\\BGM_InGame.mp3",NULL,0,NULL);
    mciSendString("open sounds\\BGM_Title.mp3",NULL,0,NULL);
}
DWORD WINAPI Click(LPVOID lpParameter)
{
    mciSendString("open sounds\\SE_Enter.mp3",NULL,0,NULL);
    mciSendString("play sounds\\SE_Enter.mp3",NULL,0,NULL);
    return 0;
}
void PlayClick()
{
    HANDLE threadtoplay;
    threadtoplay=CreateThread(NULL,0,Click,NULL,0,NULL);
    CloseHandle(threadtoplay);
}

DWORD WINAPI Lose(LPVOID lpParameter)
{
    mciSendString("open sounds\\SE_GameOver.mp3",NULL,0,NULL);
    mciSendString("play sounds\\SE_GameOver.mp3",NULL,0,NULL);
    return 0;
}
void PlayLose()
{
    HANDLE threadtoplay;
    threadtoplay=CreateThread(NULL,0,Lose,NULL,0,NULL);
    CloseHandle(threadtoplay);
}

int maxxy(int y1,int y2)
{
    if(y1>=y2)
        return 1;
    else
        return 0;
}
void enemy_move( )
{
    int ram[4];
    int higher;
    if(abs(s[0].y-s[2].y)<=200)
    {
        higher=maxxy(s[0].y,s[2].y);
        if(higher==0)
            s[0].y=s[0].y-150;
        else
            s[2].y=s[2].y-150;
    }
    if(abs(s[1].y-s[3].y)<=200)
    {
        higher=maxxy(s[1].y,s[3].y);
        if(higher==0)
            s[1].y=s[1].y-150;
        else
            s[3].y=s[3].y-150;
    }
    for(int i=0; i<4; i++)
    {
        if(i==0)
        {
            s[i].y=s[i].y+speed;
            ege::putimage_withalpha(NULL,barrier1,s[i].x,s[i].y);
        }
        if(i==1)
        {
            s[i].y=s[i].y+speed;
            ege::putimage_withalpha(NULL,barrier1,s[i].x,s[i].y);
        }
        if(i==2)
        {
            s[i].y=s[i].y+speed;
            ege::putimage_withalpha(NULL,barrier2,s[i].x,s[i].y);
        }
        if(i==3)
        {
            s[i].y=s[i].y+speed;
            ege::putimage_withalpha(NULL,barrier2,s[i].x,s[i].y);
        }
    }
    srand(time(0));
    for(int j=0; j<4; ++j)
    {

        ram[j] = rand()%2;
        if(j==0&&s[j].y>=844)
        {
            s[j].y=-150;
            if(ram[j]==0)
                s[j].x=5;
            if(ram[j]==1)
                s[j].x=123;
        }
        if(j==1&&s[j].y>=844)
        {
            s[j].y=-150;
            if(ram[j]==0)
                s[j].x=243;
            if(ram[j]==1)
                s[j].x=360;
        }

        if(j==2&&s[j].y>=844)
        {
            s[j].y=-500;
            if(ram[j]==0)
                s[j].x=5;
            if(ram[j]==1)
                s[j].x=123;
        }
        if(j==3&&s[j].y>=844)
        {
            s[j].y=-500;
            if(ram[j]==0)
                s[j].x=243;
            if(ram[j]==1)
                s[j].x=360;
        }
    }
}
void disPlayMap()
{
    putimage(0,mapy1,map1);
    putimage(0,mapy2,map2);
    mapy1=mapy1+mapspeed;
    mapy2=mapy2+mapspeed;
    if(mapy1>844)
        mapy1=0;
    if(mapy2>0)
        mapy2=-844;
}
void disPlayer()
{
    putimage_transparent(NULL,fingerL,63,715,0);
    putimage_transparent(NULL,fingerR,312,715,0);
    putimage_transparent(NULL,mstone,me1x,me1y,0);
    putimage_transparent(NULL,mstone,me2x,me2y,0);
}
void controlm()
{
    char key;
    me1x=142;
    me2x=260;
    if(keystate('A'))
        me1x=30;
    if(keystate('D'))
        me2x=385;
}
void hit()
{
    void gameOver();
    for(int i=0; i<4; i++)
    {
        if((abs(s[i].x-me1x)<=50&&abs(s[i].y-me1y)<=50)||(abs(s[i].x-me2x)<=50&&abs(s[i].y-me2y)<=50))
            {
                saveGame(score);
                gameOver();
            }

    }
}

void gameOver()
{
    char txc2[10];
    mciSendString("stop sounds\\BGM_InGame.mp3",NULL,0,NULL);
    PlayLose();
    for(; is_run(); delay_fps(60))
    {
        cleardevice();
        initmouse();
        updatemouse();
        putimage(0,0,gameoverbk);
        setbkmode(TRANSPARENT);
        putimage_transparent(NULL,returning,210,635,0);
        putimage_transparent(NULL,restart,185,710,0);
        sprintf(txc2,"%.1f m",score);
        ege::outtextxy(165,520,txc2);
        if(Mouse_X>=210&&Mouse_Y>=635&&Mouse_X<=260&&Mouse_Y<=685&&Mouse_Left&&Mouse_Down)
            Interface();
        if(Mouse_X>=185&&Mouse_Y>=710&&Mouse_X<=305&&Mouse_Y<=770&&Mouse_Left&&Mouse_Down)
            playGame();
    }
}

void playGame()
{
    char txc[10];
    me1x=142;
    me2x=260;
    me1y=642;
    me2y=642;
    score=0;
    s[0].x=5;
    s[1].x=243;
    s[2].x=123;
    s[3].x=360;
    s[0].y=-400;
    s[1].y=-400;
    s[2].y=-844;
    s[3].y=-844;
    if(level==1)
    {
        mapspeed=5;
        speed=5;
    }
    if(level==2)
    {
        mapspeed=8;
        speed=8;
    }
    if(level==3)
    {
        mapspeed=10;
        speed=10;
    }
    int flag1=1;
    int flag2=1;
    int flag3=1;
    int flag4=1;
    mciSendString("stop sounds\\BGM_Title.mp3",NULL,0,NULL);
    if(Switch==1)
    {
        mciSendString("play sounds\\BGM_InGame.mp3 repeat",NULL,0,NULL);
    }
    for(; is_run(); delay_fps(60))
    {
        cleardevice();
        disPlayMap();
        setbkmode(TRANSPARENT);
        putimage_transparent(NULL,scoreboard,110,0,0);
        updatemouse();
        sprintf(txc,"%.1f m",score);
        ege::outtextxy(165,5,txc);
        score=score+0.03;
        if(score>=50&&score<100&&flag1==1)
        {
            mapspeed=mapspeed+2;
            speed=speed+2;
            flag1=0;
        }
        if(score>=100&&score<150&&flag2==1)
        {
            mapspeed=mapspeed+2;
            speed=speed+2;
            flag2=0;
        }
        if(score>=150&&score<200&&flag3==1)
        {
            mapspeed=mapspeed+2;
            speed=speed+2;
            flag3=0;
        }
        if(score>=200&&flag4==1)
        {
            mapspeed=mapspeed+3;
            speed=speed+3;
            flag4=0;
        }
        controlm();
        disPlayer();
        enemy_move( );
        hit();
    }
}
void setting()
{
    char title1[10]="难度选择";
    char llow[10]="菜鸟局",lcom[10]="普通局",lhigh[10]="高端局";
    initmouse();
    for(; is_run(); delay_fps(60))
    {
        cleardevice();
        updatemouse();
        putimage(0,0,map1);
        setbkmode(TRANSPARENT);
        setcolor(BLUE);
        setfont(80,0,"微软雅黑");
        outtextxy(115,0,title1);
        if(Mouse_X>=170&&Mouse_Y>=251&&Mouse_X<=371&&Mouse_Y<=309)
        {
            setcolor(RED);
            setfont(65,0,"微软雅黑");
            outtextxy(170,251,llow);
            setcolor(BLACK);
            outtextxy(170,429,lcom);
            outtextxy(170,590,lhigh);
            if(Mouse_Left&&Mouse_Down)
            {
                if(Switch==1)
                {
                    PlayClick();
                }
                level=1;
                playGame();
            }
        }
        else if(Mouse_X>=170&&Mouse_Y>=429&&Mouse_X<=371&&Mouse_Y<=485)
        {
            setcolor(RED);
            setfont(65,0,"微软雅黑");
            outtextxy(170,429,lcom);
            setcolor(BLACK);
            outtextxy(170,251,llow);
            outtextxy(170,590,lhigh);
            if(Mouse_Left&&Mouse_Down)
            {
                if(Switch==1)
                {
                    PlayClick();
                }
                level=2;
                playGame();
            }
        }
        else if(Mouse_X>=170&&Mouse_Y>=590&&Mouse_X<=371&&Mouse_Y<=644)
        {
            setcolor(RED);
            setfont(65,0,"微软雅黑");
            outtextxy(170,590,lhigh);
            setcolor(BLACK);
            outtextxy(170,251,llow);
            outtextxy(170,429,lcom);
            if(Mouse_Left&&Mouse_Down)
            {
                if(Switch==1)
                {
                    PlayClick();
                }
                level=3;
                playGame();
            }
        }
        else
        {
            setcolor(BLACK);
            setfont(65,0,"微软雅黑");
            outtextxy(170,251,llow);
            outtextxy(170,429,lcom);
            outtextxy(170,590,lhigh);
        }
        putimage_transparent(NULL,returning,210,710,0);
        if(Mouse_X>=210&&Mouse_Y>=710&&Mouse_X<=260&&Mouse_Y<=760&&Mouse_Left&&Mouse_Down)
            Interface();
    }
}
void ranklist()
{
    for(; is_run(); delay_fps(60))
    {
        cleardevice();
        initmouse();
        putimage(0,0,rlist);
        setbkmode(TRANSPARENT);
        readGameFile();
        updatemouse();
        putimage_transparent(NULL,returning,210,710,0);
        if(Mouse_X>=210&&Mouse_Y>=710&&Mouse_X<=260&&Mouse_Y<=760&&Mouse_Left&&Mouse_Down)
            Interface();
    }

}
void helpface()
{
    for(; is_run(); delay_fps(60))
    {
        cleardevice();
        initmouse();
        updatemouse();
        putimage(0,0,helpbk);
        putimage(29,150,helping);
        if(Mouse_X>=28&&Mouse_Y>=153&&Mouse_X<=443&&Mouse_Y<=677)
        {
            if(Mouse_Left&&Mouse_Down)
                playGame();
        }
        else
        {
            if(Mouse_Left&&Mouse_Down)
                Interface();
        }

    }
}

void Interface()
{
    initmouse();
    char start[10]="开始游戏",sit[10]="难度设置",rlist[10]="排行榜",help[10]="游戏指南",exit1[10]="退出游戏";
    mciSendString("stop sounds\\BGM_InGame.mp3",NULL,0,NULL);
    if(Switch==1)
    {
        mciSendString("play sounds\\BGM_Title.mp3 repeat",NULL,0,NULL);
    }
    else
    {
        mciSendString("stop sounds\\BGM_Title.mp3",NULL,0,NULL);
    }
    for(; is_run(); delay_fps(60))
    {
        cleardevice();

        putimage(0,0,bk);
        setbkmode(TRANSPARENT);
        updatemouse();
        if(Switch==1)
        {
            putimage_transparent(NULL,soundon,411,777,0);
        }
        else
        {
            putimage_transparent(NULL,soundoff,408,772,0);
        }
        if(Mouse_X>=100&&Mouse_Y>=245&&Mouse_X<=371&&Mouse_Y<=324)
        {
            setcolor(RED);
            setfont(65,0,"微软雅黑");
            outtextxy(137,250,start);
            setcolor(BLACK);
            outtextxy(137,360,sit);
            outtextxy(158,470,rlist);
            outtextxy(137,578,help);
            outtextxy(137,687,exit1);
            if(Mouse_Left&&Mouse_Down)
            {
                if(Switch==1)
                {
                    PlayClick();
                }
                playGame();
            }
        }

        else if(Mouse_X>=100&&Mouse_Y>=354&&Mouse_X<=371&&Mouse_Y<=432)
        {
            setcolor(RED);
            setfont(65,0,"微软雅黑");
            outtextxy(137,360,sit);
            setcolor(BLACK);
            outtextxy(137,250,start);
            outtextxy(158,470,rlist);
            outtextxy(137,578,help);
            outtextxy(137,687,exit1);
            if(Mouse_Left&&Mouse_Down)
            {
                if(Switch==1)
                {
                    PlayClick();
                }
                setting();
            }


        }
        else if(Mouse_X>=100&&Mouse_Y>=462&&Mouse_X<=371&&Mouse_Y<=544)
        {
            setcolor(RED);
            setfont(65,0,"微软雅黑");
            outtextxy(158,470,rlist);
            setcolor(BLACK);
            outtextxy(137,250,start);
            outtextxy(137,360,sit);
            outtextxy(137,578,help);
            outtextxy(137,687,exit1);
            if(Mouse_Left&&Mouse_Down)
            {
                if(Switch==1)
                {
                    PlayClick();
                }
                ranklist();
            }

        }
        else if(Mouse_X>=100&&Mouse_Y>=572&&Mouse_X<=371&&Mouse_Y<=652)
        {
            setcolor(RED);
            setfont(65,0,"微软雅黑");
            outtextxy(137,578,help);
            setcolor(BLACK);
            outtextxy(137,250,start);
            outtextxy(137,360,sit);
            outtextxy(158,470,rlist);
            outtextxy(137,687,exit1);
            if(Mouse_Left&&Mouse_Down)
            {
                if(Switch==1)
                {
                    PlayClick();
                }
                helpface();
            }
        }
        else if(Mouse_X>=100&&Mouse_Y>=681&&Mouse_X<=371&&Mouse_Y<=761)
        {
            setcolor(RED);
            setfont(65,0,"微软雅黑");
            outtextxy(137,687,exit1);
            setcolor(BLACK);
            outtextxy(137,250,start);
            outtextxy(137,360,sit);
            outtextxy(158,470,rlist);
            outtextxy(137,578,help);
            if(Mouse_Left&&Mouse_Down)
            {
                exit(0);
            }
        }
        else if(Mouse_X>=408&&Mouse_Y>=772&&Mouse_X<=473&&Mouse_Y<=844)
        {
            setcolor(BLACK);
            setfont(65,0,"微软雅黑");
            outtextxy(137,250,start);
            outtextxy(137,360,sit);
            outtextxy(158,470,rlist);
            outtextxy(137,578,help);
            outtextxy(137,687,exit1);
            if(Mouse_Left&&Mouse_Down)
            {
                if(Switch==1)
                {
                    Switch=0;
                }
                else
                {
                    Switch=1;
                }
                Interface();
            }
        }
        else
        {
            setcolor(BLACK);
            setfont(65,0,"微软雅黑");
            outtextxy(137,250,start);
            outtextxy(137,360,sit);
            outtextxy(158,470,rlist);
            outtextxy(137,578,help);
            outtextxy(137,687,exit1);
        }
    }
}

int main()
{
    setinitmode(0);
    initgraph(473,844);
    getImg();
    getSound();
    setcaption("指神v2.0终极版");
    Interface();
    delImg();
    getch();
    closegraph();
    return 0;
}
