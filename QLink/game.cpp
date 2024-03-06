#include "game.h"
#include <QTime>


using namespace std;

void Game::getPosition(int &x1, int &y1, int &x2, int & y2, int pic1, int pic2) {
    flagA = false;
    flagB = false;
    flagC = false;
    x1 = pic1 / 20;
    y1 = pic1 % 20;
    x2 = pic2 / 20;
    y2 = pic2 % 20;
}
//连水平竖直线
bool Game::link(int x1, int y1, int x2, int y2) {
    int k = 0;
    if (map[x1][y1] != map[x2][y2]) {
        return false;
    }
    if (x1 != x2 && y1 != y2) {
        return false;
    }
    if (map[x1][y1] < 1 || map[x2][y2] < 1) {
        return false;
    }
    if (x1 == x2) {
        for (k = y1 + 1; k < y2; k++)
            if (map[x1][k] > 0)
                return false;
        for (k = y2 + 1; k < y1; k++)
            if (map[x1][k] > 0)
                return false;
    }
    if (y1 == y2) {
        for (k = x1 + 1; k < x2; k++)
            if (map[k][y1] > 0)
                return false;
        for (k = x2 + 1; k < x1; k++)
            if (map[k][y1] > 0)
                return false;
    }
    return true;
}

bool Game::linkWithNoCorner(int pic1, int pic2) {
    int x1, x2, y1, y2;
    getPosition(x1, y1, x2, y2, pic1, pic2);

    bool result = link(x1, y1, x2, y2);
    if (result == true) {
        map[x1][y1] = 0;
        map[x2][y2] = 0;
    }
    flagA = true;
    return result;
}

bool Game::linkWithOneCorner(int pic1, int pic2, int& pos2) {
    int x1, x2, y1, y2;
    getPosition(x1, y1, x2, y2, pic1, pic2);
    if (map[x1][y1] != map[x2][y2]) {
        return false;
    }

    if (x1 == x2 || y1 == y2) {
        return false;
    }

    bool planA = false, planB = false;
    if (map[x1][y2] < 1) {
        map[x1][y2] = map[x1][y1];
        if (link(x1, y2, x1, y1) && link(x1, y2, x2, y2)) {
            planA = true;
            pos2 = x1*20 + y2;
            map[x1][y1] = 0;
            map[x2][y2] = 0;
        }
        map[x1][y2] = 0;
    }

    if (map[x2][y1] < 1) {
        map[x2][y1] = map[x1][y1];
        if (link(x2, y1, x1, y1) && link(x2, y1, x2, y2)) {
            planB = true;
            pos2 = x2*20 + y1;
            map[x1][y1] = 0;
            map[x2][y2] = 0;
        }
        map[x2][y1] = 0;
    }

    if (planA || planB) {
        flagB = true;
        return true;
    }
    else
        return false;
}

bool Game::linkWithTwoCorner(int pic1, int pic2, int& pos2, int& pos3) {
    int x1, x2, y1, y2;
    getPosition(x1, y1, x2, y2, pic1, pic2);


    if (map[x1][y1] != map[x2][y2]) {
        return false;
    }

    bool planA = false, planB = false, planC = false, planD = false;
    //向上
    for (int i = x1 - 1; i >= 0; i--) {
        if (map[i][y1] > 0)
            break;
        map[i][y1] = map[x1][y1];
        int p1 = i*20 + y1;
        int p2 = x2*20 + y2;
        if (linkWithOneCorner(p1, p2, pos2)) {
            flagB = false;
            planA = true;
            map[x1][y1] = 0;
            map[x2][y2] = 0;
            int tmp = pos2;
            pos2 = p1;
            pos3 = tmp;
            break;
        }
        map[i][y1] = 0;
    }

    //向左
    for (int i = y1 - 1; i >= 0; i--) {
        if (map[x1][i] > 0)
            break;
        map[x1][i] = map[x1][y1];
        int p1 = x1*20 + i;
        int p2 = x2*20 + y2;
        if (linkWithOneCorner(p1, p2, pos2)) {
            flagB = false;
            planA = true;
            map[x1][y1] = 0;
            map[x2][y2] = 0;

            int tmp = pos2;
            pos2 = p1;
            pos3 = tmp;
            break;
        }
        map[x1][i] = 0;
    }

    //向右
    for (int i = y1 + 1; i < 20; i++) {
        if (map[x1][i] > 0)
            break;
        map[x1][i] = map[x1][y1];
        int p1 = x1*20 + i;
        int p2 = x2*20 + y2;
        if (linkWithOneCorner(p1, p2, pos2)) {
            flagB = false;
            planA = true;
            map[x1][y1] = 0;
            map[x2][y2] = 0;

            int tmp = pos2;
            pos2 = p1;
            pos3 = tmp;
            break;
        }
        map[x1][i] = 0;
    }

    //向下
    for (int i = x1 + 1; i < 14; i++) {
        if (map[i][y1] > 0)
            break;
        map[i][y1] = map[x1][y1];
        int p1 = i*20 + y1;
        int p2 = x2*20 + y2;
        if (linkWithOneCorner(p1, p2, pos2)) {
            flagB = false;
            planA = true;
            map[x1][y1] = 0;
            map[x2][y2] = 0;
            int tmp = pos2;
            pos2 = p1;
            pos3 = tmp;
            break;
        }
        map[i][y1] = 0;
    }

    if (planA || planB || planC || planD) {
        flagC = true;
        return true;
    }
    else {
        return false;
    }

}

void Game::useTool(int pic1, int pic2) {
    int x1, x2, y1, y2;
    getPosition(x1, y1, x2, y2, pic1, pic2);
    map[x1][y1] = 0;
    map[x2][y2] = 0;
}

void Game::shuffle() {
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    for (int i = 0; i < 300; i ++)
    {
        //随机生成位置
        int randx1 = qrand() % 10;
        int randy1 = qrand() % 16;
        int randx2 = qrand() % 10;
        int randy2 = qrand() % 16;
        int tmp = map[randx1 + 2][randy1 + 2];
        map[randx1 + 2][randy1 + 2] = map[randx2 + 2][randy2 + 2];
        map[randx2 + 2][randy2 + 2] = tmp;
    }
}
//初始化
void Game::init() {
    for (int i = 0; i < 160; i ++)
        rawMap[i / 16][i % 16] = i % 10 + 1;
    for (int i = 0; i < 14; i++)
        for (int j = 0; j < 20 ; j++)
            map[i][j] = 0;
    for (int i = 0; i < 10; i++)
        for (int j = 0; j < 16 ; j++)
            map[i + 2][j + 2] = rawMap[i][j];
    shuffle();
}

bool Game::isWalkable(int x1, int y1, int x2, int y2, int &pos)
{
    //标记已经走过的位置，防止位置在尝试方向的时候，把走过的位置当成新的位置。
    int mask[14][20] = {0};
    //依次代表四个方向的坐标变化，move[0]是没用的。
    Move move[5] = {{0,0},{-1,0},{0,1},{1,0},{0,-1}};
    Position in, out, location;
    in.row = x1;
    in.col = y1;
    out.row = x2;
    out.col = y2;
    //先将目标位置信息赋给location
    location = out;
    location.direction = 1;
    mask[out.row][out.col] = 1;
    path.clear();
    path.push(location);
    pos = -1;
    while (!(location.row == in.row && location.col == in.col) && !path.isEmpty()) {
        int row = move[location.direction].row + location.row;
        int col = move[location.direction].col + location.col;

        if (row >= 0 && row <14 && col >=0 && col < 20 && map[row][col] < 1 && mask[row][col] == 0) {
            //该位置没有方块可以走， mask = 0 表示没有走过
            //更新location的信息, 并标记该位置已经走过
            location.row = row;
            location.col = col;
            location.direction = 1;
            path.push(location);
            mask[row][col] = 1;
        } else if (location.direction == 4) {
            //代表四个位置不可行
            path.pop();
            if (!path.isEmpty())
                location = path.top();
        } else {
            location.direction ++;
        }
    }
    if (path.isEmpty()) {
        return false;
    } else {
        if (map[x2][y2] > 0) {
            pos = path[1].row * 20 + path[1].col;
        }
        return true;
    }
}
