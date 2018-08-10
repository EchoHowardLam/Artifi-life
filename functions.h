#ifndef AL_FUNCTIONS_H
#define AL_FUNCTIONS_H

#include <windows.h>
#include <chrono>
#include <random>
#include "classes.h"

static unsigned short font_color=7;
static unsigned short back_color=0;

static COORD buffer_size;
static COORD point;

std::default_random_engine raneng;

void static gotoxy(int x,int y)
{
    if ((x>0)|(x<81)|(y>0)|(y<26))
    {
        point.X=x-1,point.Y=y-1;
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),point);
    }else{exit(3);}
}

void static cls()
{
    system("cls");
}

void static textcolor(short color)
{
    if (color>-1 && color<16){SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),(font_color=color) | back_color);}else{exit(2);}
}

void static textbackground(short color)
{
    if (color>-1 && color<16){SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),(back_color=color*16) | font_color);}else{exit(2);}
}

void hidecursor()
{
   HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
   CONSOLE_CURSOR_INFO info;
   info.dwSize = 25;
   info.bVisible = FALSE;
   SetConsoleCursorInfo(consoleHandle, &info);
}

void static randomize()
{
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    raneng.seed(seed);
}
int static rrand(int lower,int upper)
{
    double tmp;
    tmp=raneng()%(upper-lower+1)+lower;
    return ((int)tmp);
}

bool static is_number(char input)
{
    return ((input>='0')&&(input<='9'));
}
bool static is_dir(char input)
{
    return ((input>='0')&&(input<='7'));
}

#endif
