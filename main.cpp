#include <iostream>
#include <fstream>
#include <stdlib.h>
//#include <ios>
#include <limits>
#include <windows.h>
#include <string>
//#include <sys/stat.h>
#include <iomanip>
#include <time.h>

#include "classes.h"
#include "functions.h"

#define DEBUG 0

using namespace std;
int simworld(int num,string seed_name[],string ai_seed[],char ai_type[]);

int main()
{
    system("title Artificial Life");
    buffer_size.X=80;buffer_size.Y=25;
    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE),buffer_size);
    randomize();

    int ai_num;int i;
    bool error;
    char buffer[MAX_PATH];
    GetModuleFileName( NULL, buffer, MAX_PATH );
    cout<<"Current directory:\n"<<buffer<<endl;
    do{
        cout<<"Please enter the number of AI(1-8):\n";
        cin>>ai_num;
        if(!cin.eof())
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(),'\n');
        }
        error=(!cin.good() || ai_num<1 || ai_num>8);
        if(error){cout<<"Invalid input!\n";};
    }while(error);

    char drive[]="G:\\";
    char importdir[50];char input[42];
    fstream aifile;int aifsize;
    char *u_ai_seed[ai_num];
    string seed_name[ai_num];
    string ai_seed[ai_num];
    char ai_type[ai_num];
    for (i=0;i<ai_num;i++)
    {
        do{
            error=false;
            cout<<"Directory of AI("<<i+1<<"/"<<ai_num<<"): ";
            cin.getline(input,41,'\n');
            strcpy(importdir,drive);
            strcat(importdir,input);
            strcat(importdir,".txt");
            aifile.open(importdir,ios::in | ios::ate);
            if(!aifile)
            {
                cout<<"Fail to open file "<<"\""<<importdir<<"\"\n";error=true;
            }else{
                aifsize=aifile.tellg();
                aifile.seekg(0,ios::beg);
            }
        }while(error);
        u_ai_seed[i]=new char[aifsize];
        aifile.getline(u_ai_seed[i],aifsize+1,'\0');
        aifile.close();

        ai_seed[i]=code_purification(u_ai_seed[i],seed_name[i],ai_type[i]);
        cout<<ai_seed[i]<<" "<<seed_name[i];
        if (ai_type[i]=='0'){cout<<" virus";}
        else if (ai_type[i]=='1'){cout<<" creature";}
        else{
            cls();cout<<"AI "<<i+1<<" has error in its type: not creature or virus"<<endl;
            cout<<" ("<<ai_type[i]<<") is not a proper type id.";getch();exit(0);
        }
        cout<<endl;
    }
    getch();
    simworld(ai_num,seed_name,ai_seed,ai_type);
    getch();
    exit(0);
}

int simworld(int num,string seed_name[],string ai_seed[],char ai_type[])
{
    cls();
    static SMALL_RECT window_size;
    OSVERSIONINFO version;
    version.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
    if (!GetVersionEx(&version)){exit(1);};
    if (version.dwMajorVersion>5.1)
    {
        window_size.Top=0;
        window_size.Left=0;
        window_size.Bottom=24+(DEBUG*37);
        window_size.Right=79+(DEBUG*100);
        buffer_size.X=80+(DEBUG*100);buffer_size.Y=25+(DEBUG*37);
        SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE),buffer_size);
        SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE),1,&window_size);
    }
    if (DEBUG)
    {
        gotoxy(85,5);
        cout<<"7 0 4";
        gotoxy(85,6);
        cout<<"3   1";
        gotoxy(85,7);
        cout<<"6 2 5";
        gotoxy(1,1);
    }
    hidecursor();

    int ai_amount[num]; //amount of that kind
    int tmp_ai_amount[num];
    string ai_type_name[num]; //virus/creature
    fill_n(ai_amount,num,1);
    fill_n(tmp_ai_amount,num,0);
    region::startup=50;
    region *world=new region();
    life::location=world;
    life::id_count=tmp_ai_amount;
    cout<<"                                                       ¢x Artificial-Life      \n";
    cout<<"                                                       ¢x                      \n";
    cout<<"                                                       ¢x Name Type      Amount\n";
    cout<<"                                                       ¢x                      \n";
    cout<<"                                                       ¢x                      \n";
    cout<<"                                                       ¢x                      \n";
    cout<<"                                                       ¢x                      \n";
    cout<<"                                                       ¢x                      \n";
    cout<<"                                                       ¢x                      \n";
    cout<<"                                                       ¢x                      \n";
    cout<<"                                                       ¢x                      \n";
    cout<<"                                                       ¢x                      \n";
    cout<<"                                                       ¢x Instructions         \n";
    cout<<"                                                       ¢x [Space] Run a frame  \n"; //32
    cout<<"                                                       ¢x [,] Run a creature   \n"; //44
    cout<<"                                                       ¢x [.] Debug mode: off  \n"; //46
    cout<<"                                                       ¢x [Enter] Run a command\n"; // enter 13
    cout<<"                                                       ¢x                      \n";
    cout<<"                                                       ¢x Logs                 \n";
    cout<<"                                                       ¢x                      \n";
    cout<<"                                                       ¢x                      \n";
    cout<<"                                                       ¢x                      \n";
    cout<<"                                                       ¢x                      \n";
    cout<<"                                                       ¢x                      \n";
    cout<<"                                                       ¢x                     ";
    //Spawning
    if (ai_type[0]=='0'){ai_type_name[0]="virus";life::beg=new virus(rrand(0,REGION_WIDTH-1),rrand(0,REGION_HEIGHT-1),1,ai_seed[0],'o');}
    //else if (ai_type[0]=='1'){ai_type_name[0]="creature";}
    else{cls();getch();exit(0);}
    life::last=life::beg;tmp_ai_amount[0]++;
    {
        short tmp_x,tmp_y;
        life *life_init_buf;
        for (int i=1;i<num;i++)
        {
            do{
                tmp_x=rrand(0,REGION_WIDTH-1);
                tmp_y=rrand(0,REGION_HEIGHT-1);
            }while(!world->check_empty(tmp_x,tmp_y));
            if (ai_type[i]=='0'){ai_type_name[i]="virus";life_init_buf=new virus(tmp_x,tmp_y,i+1,ai_seed[i],'o');}
            //else if (ai_type[i]=='1'){ai_type_name[i]="creature";}
            else{cls();getch();exit(0);}
            life::add_life(life_init_buf);
        }
    }
    for (int i=0;i<num;i++)
    {
        textcolor(14-i);
        gotoxy(59,4+i);
        cout<<setiosflags(ios::left)<<setfill(' ')<<setw(4)<<seed_name[i]<<" ";
        cout<<setfill(' ')<<setw(10)<<ai_type_name[i];
        cout<<setfill(' ')<<setw(6)<<ai_amount[i];
    }
    //Giant loop
    int tmp_key=0;life::log=false;
    life *cur=life::beg;life *tmp_trash;
    while (1)
    {
        tmp_key=getch();
        switch (tmp_key)
        {
        case 32:
            for (;cur!=NULL;)
            {
                if (cur->action(0)==-1)
                {
                    tmp_trash=cur;
                    cur=cur->get_next();
                    delete tmp_trash;
                }else{
                    cur=cur->get_next();
                }
            }
            cur=life::beg;
            break;
        case 44:
            if (life::beg==NULL){break;}
            if (cur->action(0)==-1)
            {
                tmp_trash=cur;
                cur=cur->get_next();
                delete tmp_trash;
            }else{
                cur=cur->get_next();
            }
            if (cur==NULL){cur=life::beg;}
            break;
        case 13:
            if (DEBUG){world->print();}
            cout<<'\a';tmp_key=getch();
            if (tmp_key==99)
            {
                if (DEBUG){
                    gotoxy(1,53);
                    int tmp_x,tmp_y;
                    cout<<"X: ";cin>>tmp_x;
                    cout<<"Y: ";cin>>tmp_y;
                    cout<<world->g_code(tmp_x-1,tmp_y-1);
                    /*life *cure=life::beg;
                    for (;cure!=NULL;cure=cure->get_next())
                    {
                        cout<<cure->g_code().substr(0,10)<<" ";
                    }
                    if (cure==NULL){cure=life::beg;}*/
                }
            }else if(tmp_key==8){
                life::log=false;int a=1;
                for (;cur!=NULL;)
                {
                    tmp_trash=cur->get_next();
                    cur->damaged(a);
                    cur=tmp_trash;
                }
                if (cur==NULL){cur=life::beg;}
            }
            break;
        case 46:
            life::log=(!life::log);
            textcolor(7);
            gotoxy(76,16);
            if (life::log){cout<<"n ";}
            else{cout<<"ff";}
        }
        for (int i=0;i<num;i++)
        {
            if (ai_amount[i]!=tmp_ai_amount[i])
            {
                ai_amount[i]=tmp_ai_amount[i];
                textcolor(14-i);
                gotoxy(74,4+i);
                cout<<setfill(' ')<<setw(6)<<ai_amount[i];
            }
        }
    }
    return 1;
}
