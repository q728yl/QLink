#ifndef MUTIGMAMEFORM_H
#define MUTIGMAMEFORM_H

#include <QWidget>
#include <QTimer>
#include <QPoint>
#include <QTimerEvent>
#include "game.h"

namespace Ui {
class MutiGameForm;
}

class MutiGameForm : public QWidget
{
    Q_OBJECT

public:
    explicit MutiGameForm(QWidget *parent = nullptr);
    ~MutiGameForm();
    void findHintPath();
    void loadFile(QString fileName);
    void paintBackground(QWidget *widget);
signals:
    void closed();


    // QWidget interface
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
    Ui::MutiGameForm *ui;
    int mainTimerID, drawLineTimer1ID, drawLineTimer2ID;
    int cellWidth, cellHeight;
    QPixmap blocks[11];
    QPixmap pixmaps[13];

public:

    int score1, score2;  //分数
    int player1X, player1Y, player2X, player2Y;   //玩家位置
    int selX1, selY1, selX2, selY2;   //玩家选择的方块位置
    int matchedSelXY[2][2][2];   //连接后的两个方块位置
    int leftTime;   //游戏剩下的时间（单位为秒）
    int totalBlocks; //剩下的总方块数
    int posXY[2][2][2]; //连接线的转折点位置
    int hint1X, hint1Y, hint2X, hint2Y; //hint的两个方块位置
    int toolMaxCnt; //地图上出现的最大工具数
    int toolCnt; //地图上出现的工具数
    int hintTime; //剩下的hint时间
    int freezeTime1, freezeTime2;
    int dizzyTime1, dizzyTime2;
    Game gameModel; //model层，用来进行逻辑处理

};

#endif // MUTIGAMEFORM_H
