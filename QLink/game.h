#pragma once

#include <QString>
#include <QStack>

//处理游戏逻辑

//Location表示了位置的信息，Direction表示该位置尝试前进的方向
//Direction（1，2，3，4）分别代表（上，右，下，左）
struct Position{
    int row;
    int col;
    int direction;
};
//移动的坐标
struct Move{
    int row;
    int col;
};

class Game {
public:
    int map[14][20];    //地图数组
    QString selectedPic;
    bool flagA = false, flagB = false, flagC = false; //转折点数量
    int rawMap[10][16];//用于初始化的
    QStack<Position> path;  //存放Position类位置的堆栈，用于判断两个位置之间有没有路径可走。

    bool isWalkable(int x1, int y1, int x2, int y2, int &pos);

    bool linkWithNoCorner(int pic1, int pic2);
    bool linkWithOneCorner(int pic1, int pic2, int& pos2);
    bool linkWithTwoCorner(int pic1, int pic2, int& pos2, int& pos3);
    void useTool(int pic1, int pic2);
    void shuffle();
    void init();


protected:
//    bool linkWithNoCorner(int &x1, int &y1, int &x2, int & y2);

private:
    void getPosition(int &x1, int &y1, int &x2, int & y2, int pic1, int pic2);
    bool link(int x1, int y1, int x2, int y2);

};
