#ifndef AL_CLASSES_H
#define AL_CLASSES_H

#include <conio.h>
#include <iostream>
#include <string>
#include <iomanip>
#include "functions.h"

#define REGION_WIDTH 55
#define REGION_HEIGHT 25
#define LOGS_X 59
#define LOGS_Y 20

using namespace std;

class life;
class virus;
class creature;
class region;

static unsigned int fin_pos_mask=0;

void draw_life(short x,short y,int id,char outlook)
{
    textcolor(15-id);
    gotoxy(x+1,y+1);
    cout<<outlook;
}
void clean_life(short x,short y)
{
    gotoxy(x+1,y+1);
    cout<<" ";
}
void dir_to_coor(short direction,short &x,short &y)
{
    if ((direction<0)||(direction>7)){return;}
    if ((direction!=1)&&(direction!=3)) //here y
    {
        switch (direction)
        {
        case 0:case 4:case 7:
            y-=1;break;
        default:
            y+=1;break;
        }
    }
    if ((direction!=0)&&(direction!=2)) //here x
    {
        switch (direction)
        {
        case 1:case 4:case 5:
            x+=1;break;
        default:
            x-=1;break;
        }
    }
}

class region
{
public:
    static short startup;
    region(){for (int i=0;i<REGION_WIDTH;i++){for (int j=0;j<REGION_HEIGHT;j++){
                food[i][j]=startup;
                organism[i][j]=NULL;}}}
    bool add_food(short x,short y,int input){if (food[x][y]+input<=100){food[x][y]+=input;return 1;}return 0;}
    bool set_food(short x,short y,unsigned short input){if ((input>=0)&&(input<=100)){food[x][y]=input;return 1;}return 0;}
    unsigned int code_length(short x,short y);
    bool bite(short x,short y);
    bool edit(short x,short y,string &n_code);
    bool edit(short x,short y,string &n_code,unsigned int &ins_pos);
    void void_life(short x,short y){organism[x][y]=NULL;clean_life(x,y);}
    bool set_life(short x,short y,life *input);
    void redraw(short x,short y);
    bool check_empty(short x,short y){if (organism[x][y]==NULL){return 1;}return 0;}
    short get_life_id(short x,short y);
    short get_life_type(short x,short y);
    unsigned short get_food_amount(short x,short y){return food[x][y];}
    short static genx(short &x){if (x<0){x+=REGION_WIDTH;}else{x=x%REGION_WIDTH;}return x;}
    short static geny(short &y){if (y<0){y+=REGION_HEIGHT;}else{y=y%REGION_HEIGHT;}return y;}
    short static _genx(short &x){if (x<0){x+=(1-((x+1)/REGION_WIDTH))*REGION_WIDTH;}else{x=x%REGION_WIDTH;}return x;}
    short static _geny(short &y){if (y<0){y+=(1-((y+1)/REGION_HEIGHT))*REGION_HEIGHT;}else{y=y%REGION_HEIGHT;}return y;}
    void print();
    //bool compare(short x,short y,life *input){if (organism[x][y]==input){return true;}gotoxy(x+1,y+1);cout<<organism[x][y];return false;}
    string g_code(short x,short y);
private:
    unsigned short food[REGION_WIDTH][REGION_HEIGHT];
    life *organism[REGION_WIDTH][REGION_HEIGHT];
};
short region::startup=10;

class life
{
public:
    static life *beg;
    static life *last;
    static region *location;
    static int *id_count;
    static bool log;
    char outlook;
    virtual ~life(){if (prev!=NULL){if (next!=NULL){prev->next_life(next);next->prev_life(prev);}else{life::last=prev;prev->next_life(NULL);}}
        else{life::beg=next;if (next!=NULL){next->prev_life(NULL);}else{life::last=NULL;}}id_count[_id-1]--;}
    virtual unsigned int code_length()=0;
    virtual void damaged(int &dmg)=0;
    virtual void biten()=0;
    virtual bool edited(string &n_code)=0; //overwrite
    virtual void edited(string &n_code,unsigned int &ins_pos)=0; //append inject
    virtual int action(unsigned int ini_step,unsigned int &fin_step=fin_pos_mask,int layer=0)=0;
    //virtual int step(int index)=0; //later development, cancelled?
    void static add_life(life *new_life){life::last->next_life(new_life);new_life->prev_life(life::last);life::last=new_life;id_count[new_life->id()-1]++;}
    void next_life(life *new_life){next=new_life;}
    void prev_life(life *new_life){prev=new_life;}
    life *get_next(){return next;}
    life *get_prev(){return prev;}
    short id(){return _id;}
    short type(){return _type;}
    void change_id(short input){_id=input;}

    string g_code(){return code;}
protected:
    life *prev=NULL;
    life *next=NULL;
    short _id;
    short _type;
    string code="";
};
string code_purification(string raw_code,string &name,char &type)
{
    unsigned int b=0;string code="";name="";type='.';
    for (;b<4;b++){if ((raw_code[b]>=32)&&(raw_code[b]<=126)&&(raw_code[b]!='.')&&(b<raw_code.length())){name+=raw_code[b];}else{break;}}
    if (raw_code[b]=='.'){b++;}
    for (;b<raw_code.length();b++)
    {
        if ((raw_code[b]>='(')&&(raw_code[b]<='?'))
        {
                code+=raw_code[b];
        }
    }
    if (code.length()>0)
    {
        type=code[0];
        return code.substr(1);
    }
    return "";
}
life *life::beg=NULL;
life *life::last=NULL;
region *life::location=NULL;
int *life::id_count=NULL;
bool life::log=false;

class virus:public life
{
public:
    virus(int inx,int iny,int inid,string incode,char n_look):x(inx),y(iny)
        {outlook=n_look;_id=inid;_type=0;code=incode;life::location->set_life(inx,iny,this);}
    virtual ~virus(){life::location->void_life(x,y);}
    virtual unsigned int code_length(){return code.length();}
    virtual void damaged(int &dmg){delete this;}
    virtual void biten()
    {
        if (code.length()>0)
        {
            if ((outlook!=',')&&(outlook!='.'))
            {
                if (outlook=='O'){outlook=',';}
                else{outlook='.';}
                life::location->redraw(x,y);
            }
            code.erase(rrand(0,code.length()-1),1);
            if (life::log)
            {
                textcolor(7);
                gotoxy(LOGS_X,LOGS_Y);
                cout<<" bite ";
                textcolor(15-_id);
                cout<<"AI "<<_id<<" ";
                gotoxy(LOGS_X,LOGS_Y+1);
                cout<<"Code: "<<code.substr(0,15);
            }
        }else{
            if (life::log)
            {
                textcolor(7);
                gotoxy(LOGS_X,LOGS_Y);
                cout<<" swallowed ";
                textcolor(15-_id);
                cout<<"AI "<<_id;
                gotoxy(LOGS_X,LOGS_Y+1);
                cout<<string(' ',22);
            }
            delete this;
        }
    }
    virtual bool edited(string &n_code){  //overwrite
        if (n_code!=code)
        {
            if (outlook!='O'){outlook='O';life::location->redraw(x,y);}
            code=n_code;if (life::log){gotoxy(LOGS_X,LOGS_Y+2);cout<<"Code: "<<code;}return 1;
        }else{return 0;}}
    virtual void edited(string &n_code,unsigned int &ins_pos){ //append inject
        if (outlook!='O'){outlook='O';life::location->redraw(x,y);}
        if (ins_pos>=0){if (ins_pos>code.length()){ins_pos=code.length();}code.insert(ins_pos,n_code);}
        else{code.append(n_code);}
        if (life::log){gotoxy(LOGS_X,LOGS_Y+2);cout<<"Code: "<<code;}}
    virtual int action(unsigned int ini_step,unsigned int &fin_step=fin_pos_mask,int layer=0)
    {
        int return_val=0,calc_buf=0,operation=0;bool ret_slot=false;
        for (unsigned int step=ini_step;;)
        {
            if (step>=code.length())
            {
                if (!layer){act=false;return 0;} //good
                else{self_destructing=step+1;return -1;} //bad in brackets
            }
            if (code[step]==')')
            {
                if ((!layer)||(!ret_slot)){code_error(self_destructing);return -1;} //bad at outermost
                fin_step=step;break; //good
            }
            switch (code[step])
            {
            case '1':
                if (ret_slot){self_destructing=step+1;break;}
                if (layer){ret_slot=true;}
                step++;
                if (step>=code.length()){self_destructing=step+1;break;}
                if (is_dir(code[step])){return_val=_move(code[step]-48);}
                else if(code[step]=='('){return_val=_move(action(step+1,step,layer+1)%8);if (self_destructing){break;}}
                else{self_destructing=step+1;break;}
                break;
            case '2':
                if (ret_slot){self_destructing=step+1;break;}
                if (layer){ret_slot=true;}
                step++;
                if (step>=code.length()){self_destructing=step+1;break;}
                if (is_dir(code[step])){return_val=_bite(code[step]-48);}
                else if(code[step]=='('){return_val=_bite(action(step+1,step,layer+1)%8);if (self_destructing){break;}}
                else{self_destructing=step+1;break;}
                break;
            case '3':
                {
                    if (ret_slot){self_destructing=step+1;break;}
                    if (layer){ret_slot=true;}
                    step++;int tmp_ver=0;
                    if (step>=code.length()){self_destructing=step+1;break;}
                    if (code[step]==','){tmp_ver=1;step++;if (step>=code.length()){self_destructing=step+1;break;}}
                    else if(code[step]=='.'){tmp_ver=2;step++;if (step>=code.length()){self_destructing=step+1;break;}}
                    if (is_dir(code[step])){return_val=_detect(tmp_ver,code[step]-48);}
                    else if(code[step]=='('){return_val=_detect(tmp_ver,action(step+1,step,layer+1)%8);if (self_destructing){break;}}
                    else{self_destructing=step+1;break;}
                }
                break;
            case '4':
                if (ret_slot){self_destructing=step+1;break;}
                if (layer){ret_slot=true;}
                return_val=_yield();
                break;
            case '5':
                {
                    if (ret_slot){self_destructing=step+1;break;}
                    if (layer){ret_slot=true;}
                    step++;int tmp_direction;
                    if (step>=code.length()){self_destructing=step+1;break;}
                    if (is_dir(code[step])){tmp_direction=code[step]-48;}
                    else if(code[step]=='('){tmp_direction=action(step+1,step,layer+1)%8;if (self_destructing){break;}}
                    else{self_destructing=step+1;break;}
                    step++;
                    if (step>=code.length()){self_destructing=step+1;break;}
                    if (is_number(code[step]))
                    {
                        int tmp_amount=(code[step]-48);
                        unsigned int i;
                        for (i=step+1;i<step+3;i++)
                        {
                            if (i>=code.length()){self_destructing=i+1;break;}
                            if (is_number(code[i])){tmp_amount*=10;tmp_amount+=(code[i]-48);}
                            else if(code[i]=='.'){break;}
                            else{self_destructing=i+1;break;}
                        }
                        if (self_destructing){break;}
                        step=i;
                        return_val=_transfer(tmp_direction,tmp_amount);
                    }else if(code[step]=='('){return_val=_transfer(tmp_direction,action(step+1,step,layer+1));if (self_destructing){break;}}
                    else{self_destructing=step+1;break;}
                }
                break;
            case '6':
                {
                    if (ret_slot){self_destructing=step+1;break;}
                    if (layer){ret_slot=true;}
                    step++;int tmp_ver=0;int tmp_direction;
                    if (step>=code.length()){self_destructing=step+1;break;}
                    if (code[step]==','){tmp_ver=1;step++;if (step>=code.length()){self_destructing=step+1;break;}}
                    else if(code[step]=='.'){tmp_ver=2;step++;if (step>=code.length()){self_destructing=step+1;break;}}
                    if (is_dir(code[step])){tmp_direction=code[step]-48;}
                    else if(code[step]=='('){tmp_direction=action(step+1,step,layer+1)%8;if (self_destructing){break;}}
                    else{self_destructing=step+1;break;}
                    if (tmp_ver==0){return_val=_create(tmp_direction);break;}
                    if (tmp_ver==1)
                    {
                        step++;
                        if (step>=code.length()){self_destructing=step+1;break;}
                        if (!is_number(code[step])){self_destructing=step+1;break;}
                        unsigned int tmp_ini_pos=(code[step]-48);
                        unsigned int i;
                        for (i=step+1;i<step+3;i++)
                        {
                            if (i>=code.length()){self_destructing=i+1;break;}
                            if (is_number(code[i])){tmp_ini_pos*=10;tmp_ini_pos+=(code[i]-48);}
                            else if(code[i]==','){break;}
                            else{self_destructing=i+1;break;}
                        }
                        if (self_destructing){break;}
                        i++;
                        if (i>=code.length()){self_destructing=i+1;break;}
                        if (!is_number(code[i])){self_destructing=i+1;break;}
                        step=i;unsigned int tmp_fin_pos=(code[i]-48);
                        for (i=step+1;i<step+3;i++)
                        {
                            if (i>=code.length()){self_destructing=i+1;break;}
                            if (is_number(code[i])){tmp_fin_pos*=10;tmp_fin_pos+=(code[i]-48);}
                            else if(code[i]=='.'){break;}
                            else{self_destructing=i+1;break;}
                        }
                        if (self_destructing){break;}
                        step=i;
                        return_val=_create(tmp_direction,tmp_ini_pos,tmp_fin_pos);
                    }else{
                        step++;
                        if (step>=code.length()){self_destructing=step+1;break;}
                        if (code[step]!='('){self_destructing=step+1;break;}
                        unsigned int i,tmp_layer=0,tmp_beg=step+1;
                        for (i=step+1;;i++)
                        {
                            if (i>=code.length()){self_destructing=i+1;break;}
                            if (code[i]=='('){tmp_layer++;}
                            else if(code[i]==')'){
                                if (tmp_layer==0){break;}
                                tmp_layer--;
                            }
                        }
                        if (self_destructing){break;}
                        step=i;
                        string tmp_code=code.substr(tmp_beg,step-tmp_beg);
                        return_val=_create(tmp_direction,tmp_code);
                    }
                }
                break;
            case '7':
                {
                    if (ret_slot){self_destructing=step+1;break;}
                    if (layer){ret_slot=true;}
                    step++;int tmp_ver=0;short tmp_direction=0;unsigned int tmp_ins_pos=-1;string tmp_n_code="";
                    if (step>=code.length()){self_destructing=step+1;break;}
                    if (code[step]==','){tmp_ver=1;step++;if (step>=code.length()){self_destructing=step+1;break;}}
                    else if(code[step]=='.'){tmp_ver=2;step++;if (step>=code.length()){self_destructing=step+1;break;}}
                    if (is_dir(code[step])){tmp_direction=code[step]-48;}
                    else if(code[step]=='('){tmp_direction=action(step+1,step,layer+1)%8;if (self_destructing){break;}}
                    else{self_destructing=step+1;break;}
                    step++;
                    if (step>=code.length()){self_destructing=step+1;break;}
                    if (tmp_ver==2)
                    {
                        if (!is_number(code[step])){self_destructing=step+1;break;}
                        tmp_ins_pos=(code[step]-48);
                        unsigned int i;
                        for (i=step+1;i<step+3;i++)
                        {
                            if (i>=code.length()){self_destructing=i+1;break;}
                            if (is_number(code[i])){tmp_ins_pos*=10;tmp_ins_pos+=(code[i]-48);}
                            else if(code[i]=='('){break;}
                            else{self_destructing=i+1;break;}
                        }
                        if (self_destructing){break;}
                        step=i;
                    }
                    if (code[step]!='('){self_destructing=step+1;break;}
                    unsigned int i,tmp_layer=0,tmp_beg=step+1;
                    for (i=step+1;;i++)
                    {
                        if (i>=code.length()){self_destructing=i+1;break;}
                        if (code[i]=='('){tmp_layer++;}
                        else if(code[i]==')'){
                            if (tmp_layer==0){break;}
                            tmp_layer--;
                        }
                    }
                    if (self_destructing){break;}
                    step=i;
                    tmp_n_code=code.substr(tmp_beg,step-tmp_beg);
                    if (tmp_n_code=="*"){tmp_n_code=code;}
                    if (tmp_ver==0){return_val=_edit(tmp_direction,tmp_n_code);}
                    else{return_val=_edit(tmp_direction,tmp_n_code,tmp_ins_pos);} //append insert
                }
                break;
            case '9':
                {
                    if (layer){self_destructing=step+1;break;}
                    step++;bool jump=true;
                    if (step>=code.length()){self_destructing=step+1;break;}
                    if(code[step]=='(')
                    {
                        jump=action(step+1,step,layer+1);if (self_destructing){break;}
                        step++;
                    }
                    if (is_number(code[step]))
                    {
                        unsigned int tmp_dest=(code[step]-48);
                        unsigned int i;
                        for (i=step+1;i<step+3;i++)
                        {
                            if (i>=code.length()){self_destructing=i+1;break;}
                            if (is_number(code[i])){tmp_dest*=10;tmp_dest+=(code[i]-48);}
                            else if(code[i]=='.'){break;}
                            else{self_destructing=i+1;break;}
                        }
                        if (self_destructing){break;}
                        if (jump){step=tmp_dest-2;if (food){food--;}else{self_destructing=-1;break;}}
                        else{step=i;}
                    }else{self_destructing=step+1;break;}
                }
                break;
            case '.':
                break;
            case '+':
                if (!ret_slot){self_destructing=step+1;break;}
                calc_buf=return_val;operation=1;ret_slot=false;break;
            case '-':
                if (!ret_slot){self_destructing=step+1;break;}
                calc_buf=return_val;operation=2;ret_slot=false;break;
            case '*':
                if (!ret_slot){self_destructing=step+1;break;}
                calc_buf=return_val;operation=3;ret_slot=false;break;
            case '/':
                if (!ret_slot){self_destructing=step+1;break;}
                calc_buf=return_val;operation=4;ret_slot=false;break;
            case '(':
                if ((!layer)||(ret_slot)){self_destructing=step+1;break;}
                ret_slot=true;
                return_val=action(step+1,step,layer+1);
                break;
            case ':':
                {
                    if ((!layer)||(ret_slot)){self_destructing=step+1;break;}
                    ret_slot=true;step++;
                    if (step>=code.length()){self_destructing=step+1;break;}
                    if (!is_number(code[step])){self_destructing=step+1;break;}
                    return_val=(code[step]-48);
                    unsigned int i;
                    for (i=step+1;i<step+3;i++)
                    {
                        if (i>=code.length()){break;}
                        if (is_number(code[i])){return_val*=10;return_val+=(code[i]-48);}else{break;}
                    }
                    step=i-1;
                    break;
                }
            case ';':
                if (layer)
                {
                    if (ret_slot){self_destructing=step+1;break;}
                    ret_slot=true;step++;int var_num;
                    if (step>=code.length()){self_destructing=step+1;break;}
                    if (is_number(code[step])){var_num=code[step]-48;}
                    else if(code[step]=='('){var_num=action(step+1,step,layer+1);if (self_destructing){break;}}
                    else{self_destructing=step+1;break;}
                    switch (var_num)
                    {
                    case 0:
                        return_val=x;break;
                    case 1:
                        return_val=y;break;
                    case 2:
                        return_val=_id;break;
                    case 3:
                        return_val=food;break;
                    case 4:
                        return_val=0;break;
                    case 5:
                        return_val=act;break;
                    case 6:
                        return_val=rrand(0,7);break;
                    case 7:
                        return_val=tmp;break;
                    case 8:
                        return_val=tmp2;break;
                    case 9:
                        return_val=tmp3;break;
                    default:
                        self_destructing=step+1;break;
                    }
                }else{
                    step++;int var_num;int tmp_num;
                    if (step>=code.length()){self_destructing=step+1;break;}
                    if ((code[step]>='7')&&(code[step]<='9')){var_num=code[step]-48;}
                    else if(code[step]=='('){var_num=action(step+1,step,layer+1);if (self_destructing){break;}}
                    else{self_destructing=step+1;break;}
                    step+=2;
                    if (step>=code.length()){self_destructing=code.length()+1;break;}
                    if (code.substr(step-1,2)!="=("){self_destructing=step;break;}
                    tmp_num=action(step+1,step,layer+1);if (self_destructing){break;}
                    switch (var_num)
                    {
                    case 7:
                        tmp=tmp_num;break;
                    case 8:
                        tmp2=tmp_num;break;
                    case 9:
                        tmp3=tmp_num;break;
                    default:
                        self_destructing=step+1;break;
                    }
                }
                break;
            case '<':
                {
                    if (!ret_slot){self_destructing=step+1;break;}
                    calc_buf=return_val;
                    step++;int tmp_type=1;
                    if (step>=code.length()){self_destructing=step+1;break;}
                    if (code[step]=='='){step++;tmp_type=2;}
                    int tmp_ret=action(step,step,layer+1);
                    if (self_destructing){break;}
                    step--;
                    if (tmp_type==1){return_val=(calc_buf<tmp_ret);}
                    else{return_val=(calc_buf<=tmp_ret);}
                }
                break;
            case '>':
                {
                    if (!ret_slot){self_destructing=step+1;break;}
                    calc_buf=return_val;
                    step++;int tmp_type=1;
                    if (step>=code.length()){self_destructing=step+1;break;}
                    if (code[step]=='='){step++;tmp_type=2;}
                    int tmp_ret=action(step,step,layer+1);
                    if (self_destructing){break;}
                    step--;
                    if (tmp_type==1){return_val=(calc_buf>tmp_ret);}
                    else{return_val=(calc_buf>=tmp_ret);}
                }
                break;
            case '=':
                {
                    if (!ret_slot){self_destructing=step+1;break;}
                    calc_buf=return_val;
                    step++;
                    int tmp_ret=action(step,step,layer+1);
                    if (self_destructing){break;}
                    step--;
                    return_val=(calc_buf==tmp_ret);
                }
                break;
            case '?':
                if (!layer){self_destructing=step+1;break;}
                if (ret_slot)
                {
                    step++;
                    if (step>=code.length()){self_destructing=step+1;break;}
                    if (code[step]!='='){self_destructing=step+1;break;}
                    calc_buf=return_val;step++;
                    int tmp_ret=action(step,step,layer+1);
                    if (self_destructing){break;}
                    step--;
                    return_val=(calc_buf!=tmp_ret);
                }else{
                    if (operation){operation*=-1;}
                    else{operation=-6;}
                }
                break;
            default:
                self_destructing=step+1;break;
            }
            if (self_destructing)
            {
                if (layer!=0){return -1;}
                else{code_error(self_destructing);return -1;}
            }
            if ((operation)&&(ret_slot))
            {
                if (operation<0){return_val=!(return_val);operation*=-1;}
                switch (operation)
                {
                case 1:
                    return_val+=calc_buf;break;
                case 2:
                    return_val=calc_buf-return_val;break;
                case 3:
                    return_val*=calc_buf;break;
                case 4:
                    return_val=calc_buf/return_val;break;
                //case 6: reserve for not
                }
                operation=0;
            }
            step++;
        }
        return return_val;
    }
    //virtual int step(int index){}
private:
    short x;
    short y;
    unsigned short food=0;
    bool act=false;
    int tmp=0;
    int tmp2=0;
    int tmp3=0;
    int self_destructing=0;
    void code_error(long step);
    bool _move(short direction);
    bool _bite(short direction);
    int _detect(short ver,short direction);
    int _yield();
    int _transfer(short direction,short amount);
    bool _create(short direction); //clone
    bool _create(short direction,int ini_pos,int end_pos); //inherit
    bool _create(short direction,string &out_code); //produce
    bool _edit(short direction,string &n_code); //overwrite
    bool _edit(short direction,string &n_code,unsigned int &ins_pos); //append inject
};

class creature:public life
{
};

unsigned int region::code_length(short x,short y){if (organism[x][y]!=NULL){return organism[x][y]->code_length();}return 0;}
bool region::bite(short x,short y){if (organism[x][y]!=NULL){organism[x][y]->biten();return 1;}return 0;}
bool region::edit(short x,short y,string &n_code){
    if (organism[x][y]!=NULL)
    {
        return organism[x][y]->edited(n_code);
    }return 0;
}
bool region::edit(short x,short y,string &n_code,unsigned int &ins_pos){
    if (organism[x][y]!=NULL)
    {
        organism[x][y]->edited(n_code,ins_pos);return 1;
    }return 0;
}
bool region::set_life(short x,short y,life *input){if (organism[x][y]==NULL){organism[x][y]=input;draw_life(x,y,input->id(),input->outlook);return 1;}return 0;}
void region::redraw(short x,short y){if (organism[x][y]!=NULL){draw_life(x,y,organism[x][y]->id(),organism[x][y]->outlook);}}
short region::get_life_id(short x,short y){if (organism[x][y]!=NULL){return organism[x][y]->id();}else{return 0;}}
short region::get_life_type(short x,short y){if (organism[x][y]!=NULL){return organism[x][y]->type()+1;}else{return 0;}}
void region::print(){
    textcolor(7);
    gotoxy(1,27);
    for (int i=0;i<REGION_HEIGHT;i++)
    {
        for (int j=0;j<REGION_WIDTH;j++)
        {
            if (food[j][i]!=100){cout<<food[j][i]/10;}else{cout<<"+";}//(organism[j][i]!=NULL);
        }
        cout<<" ";
        for (int j=0;j<REGION_WIDTH;j++)
        {
            cout<<(organism[j][i]!=NULL);
        }
        cout<<endl;
    }
}
string region::g_code(short x,short y){
    if (organism[x][y]!=NULL){return organism[x][y]->g_code();}
    else{return "";}
}
void virus::code_error(long step){
    if (life::log)
    {
            textcolor(15-_id);gotoxy(LOGS_X,LOGS_Y);
            if (step>=0)
            {
                cout<<"AI "<<_id<<" error, pos: "<<step<<"                     ";
                gotoxy(LOGS_X,LOGS_Y+1);cout<<code.substr(0,step-1)<<(char)26<<code.substr(step-1)<<"                    ";
            }else{cout<<"AI "<<_id<<" starved.                          ";}
            while (getch()==32);
    }
}
bool virus::_move(short direction){
    if (!act)
    {
        short buf_x=x,buf_y=y;
        dir_to_coor(direction,buf_x,buf_y);
        region::genx(buf_x);region::geny(buf_y);
        if (life::location->check_empty(buf_x,buf_y))
        {
            life::location->void_life(x,y);
            x=buf_x;y=buf_y;
            life::location->set_life(x,y,this);
            act=true;
            return true;
        }else{return false;}
    }else{return false;}
}
bool virus::_bite(short direction){
    if (!act)
    {
        short buf_x=x,buf_y=y;
        dir_to_coor(direction,buf_x,buf_y);
        region::genx(buf_x);region::geny(buf_y);
        if (life::location->bite(buf_x,buf_y)){if (food<100){food++;}act=true;return true;}return false;
    }else{return false;}
}
int virus::_detect(short ver,short direction){
    short buf_x=x,buf_y=y;
    dir_to_coor(direction,buf_x,buf_y);
    region::genx(buf_x);region::geny(buf_y);
    if (ver==0){return life::location->get_life_id(buf_x,buf_y);}
    if (ver==1){return life::location->get_life_type(buf_x,buf_y);}
    return life::location->get_food_amount(buf_x,buf_y);
}
int virus::_yield(){
    if (!act)
    {
        short max_food=life::location->get_food_amount(x,y);bool from_land=true;short gain;
        if (max_food<=0){max_food=5;from_land=false;}
        if (food+max_food<=100){gain=max_food;}else{gain=100-food;}
        if (from_land){life::location->add_food(x,y,-1*gain);}
        if (gain){act=true;food+=gain;}
        return gain;
    }else{return 0;}
}
int virus::_transfer(short direction,short amount){
    if (food<amount){amount=food;}
    short buf_x=x,buf_y=y;
    dir_to_coor(direction,buf_x,buf_y);
    region::genx(buf_x);region::geny(buf_y);
    short target=life::location->get_food_amount(buf_x,buf_y);short out;
    if (amount+target<=100){out=amount;}
    else{out=100-target;}
    if (out!=0)
    {
        food-=out;
        life::location->add_food(buf_x,buf_y,out);
    }
    return out;
}
bool virus::_create(short direction){ //clone
    if (act){return 0;}
    if (food<code.length()/5+1){return 0;}
    short buf_x=x,buf_y=y;
    dir_to_coor(direction,buf_x,buf_y);
    region::genx(buf_x);region::geny(buf_y);
    if (life::location->check_empty(buf_x,buf_y))
    {
        life *life_init_buf;
        life_init_buf=new virus(buf_x,buf_y,_id,code,outlook);
        life::add_life(life_init_buf);
        food-=(code.length()/5+1);
        act=true;
        return 1;
    }else{return 0;}
}
bool virus::_create(short direction,int ini_pos,int end_pos){ //inherit
    if (act){return 0;}
    if (food<code.substr(ini_pos,end_pos-ini_pos+1).length()/5+1){return 0;}
    short buf_x=x,buf_y=y;
    dir_to_coor(direction,buf_x,buf_y);
    region::genx(buf_x);region::geny(buf_y);
    if (life::location->check_empty(buf_x,buf_y))
    {
        life *life_init_buf;
        life_init_buf=new virus(buf_x,buf_y,_id,code.substr(ini_pos-1,end_pos-ini_pos+1),outlook);
        life::add_life(life_init_buf);
        food-=(code.substr(ini_pos,end_pos-ini_pos+1).length()/5+1);
        act=true;
        return 1;
    }else{return 0;}
}
bool virus::_create(short direction,string &out_code){ //produce
    if (act){return 0;}
    if (food<out_code.length()/5+1){return 0;}
    short buf_x=x,buf_y=y;
    dir_to_coor(direction,buf_x,buf_y);
    region::genx(buf_x);region::geny(buf_y);
    if (life::location->check_empty(buf_x,buf_y))
    {
        life *life_init_buf;
        life_init_buf=new virus(buf_x,buf_y,_id,out_code,outlook);
        life::add_life(life_init_buf);
        food-=(out_code.length()/5+1);
        act=true;
        return 1;
    }else{return 0;}
}
bool virus::_edit(short direction,string &n_code){ //overwrite
    short buf_x=x,buf_y=y;
    dir_to_coor(direction,buf_x,buf_y);
    region::genx(buf_x);region::geny(buf_y);
    int cost=(n_code.length()+life::location->code_length(buf_x,buf_y));
    if (food>=cost)
    {
        if (life::location->edit(buf_x,buf_y,n_code)){food-=cost;return 1;}return 0;
    }else{return 0;}
}
bool virus::_edit(short direction,string &n_code,unsigned int &ins_pos){ //append inject
    int cost=n_code.length();
    if (food>=cost)
    {
        short buf_x=x,buf_y=y;
        dir_to_coor(direction,buf_x,buf_y);
        region::genx(buf_x);region::geny(buf_y);
        if (life::location->edit(buf_x,buf_y,n_code,ins_pos)){food-=cost;return 1;}return 0;
    }else{return 0;}
}

#endif
