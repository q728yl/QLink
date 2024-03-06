#ifndef SINGLEGAMEFORM_H
#define SINGLEGAMEFORM_H

#include <QWidget>
#include <QTimer>
#include <QPoint>
#include <QTimerEvent>
#include "game.h"

namespace Ui {
class SingleGameForm;
}

class SingleGameForm : public QWidget
{
    Q_OBJECT

public:
    explicit SingleGameForm(QWidget *parent = nullptr);
    ~SingleGameForm();
    void drawCanvas(QWidget *widget);
    void canvasMousePressEvent(QMouseEvent *event);
    void findHint();
    void loadFile(QString fileName);
signals:
    void closed();

protected:
    void closeEvent(QCloseEvent *event);
private slots:
    void on_return_pushButton_clicked();
    void on_stop_pushButton_clicked();
    void on_continue_pushButton_clicked();
    void on_save_pushButton_clicked();
    void on_load_pushButton_clicked();
    void on_start_pushButton_clicked();

public:
    bool eventFilter(QObject *watched, QEvent *event);

protected:
    void keyPressEvent(QKeyEvent *event);

protected:
    void timerEvent(QTimerEvent *event);
private:
    Ui::SingleGameForm *ui;
    int mainTimerID, drawLineTimerID;
    int cellWidth, cellHeight;
    QPixmap blocks[11];
    QPixmap pixmaps[13];

public:

    int score;  //玩家分数
    int playerX, playerY;   //玩家位置
    int selX1, selY1;   //玩家选择的方块位置
    int matchedSelX1, matchedSelY1, matchedSelX2, matchedSelY2;   //连接后的两个方块位置
    int leftTime;   //游戏剩下的时间（单位为秒）
    int totalBlocks; //剩下的总方块数
    int pos2X, pos2Y, pos3X, pos3Y; //连接线的转折点位置
    int hint1X, hint1Y, hint2X, hint2Y; //hint的两个方块位置
    int toolMaxCnt; //地图上出现的最大工具数
    int toolCnt; //地图上出现的工具数
    int hintTime; //剩下的hint时间
    int flashTime; //剩下的flash时间
    Game gameModel; //model层，用来进行逻辑处理

};

#endif // SINGLEGAMEFORM_H
