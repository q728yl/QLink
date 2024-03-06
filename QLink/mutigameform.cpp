#include "mutigameform.h"
#include "ui_mutigameform.h"
#include <QPixmap>
#include <QPainter>
#include <QDebug>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QMessageBox>
#include <QTime>
#include <QFileDialog>
#include <QFile>


MutiGameForm::MutiGameForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MutiGameForm)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);
    leftTime = 200;
    totalBlocks = 160;
    toolMaxCnt = 2;
    toolCnt = 0;
    score1 = score2 = 0;
    hintTime = 0;
    freezeTime1 = freezeTime2 = dizzyTime1 = dizzyTime2 = 0;
    hint1X = hint1Y = hint2X = hint2Y = -2;
    ui->time_progressBar->setRange(0, 200);
    ui->time_progressBar->setValue(200);
    ui->time_label->setText(QString::number(leftTime) + " 秒");
    ui->canvas_widget->installEventFilter(this);
    mainTimerID = -1;
    drawLineTimer1ID = -1;
    drawLineTimer2ID = -1;
    player1X = 0;
    player1Y = 0;
    player2X = 19;
    player2Y = 13;
    selX1 = selY1 = selX2 = selY2 -2;
    cellWidth = ui->canvas_widget->width() / 20;
    cellHeight = ui->canvas_widget->height() / 14;

    ui->canvas_widget->setEnabled(false);
    ui->start_pushButton->setEnabled(true);
    ui->continue_pushButton->setEnabled(false);
    ui->save_pushButton->setEnabled(false);
    ui->load_pushButton->setEnabled(false);
    ui->stop_pushButton->setEnabled(false);
    ui->return_pushButton->setEnabled(true);

    for (int i = 1; i < 11 ; i ++)
    {
        blocks[i] = QPixmap(cellWidth, cellHeight);
        blocks[i].fill(QColor(0, 0, 0, 0));
        QPainter painter(&blocks[i]);
        painter.drawImage(QRect(0, 0, cellWidth, cellHeight), QImage(":/res/imgs/res/" + QString::number(i) + ".png").scaled(cellWidth, cellHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    }

    for (int i = 1; i < 7 ; i ++)
    {
        pixmaps[i] = QPixmap(cellWidth, cellHeight);
        pixmaps[i].fill(QColor(255, 255, 0, 255));
        QPainter painter(&pixmaps[i]);
        painter.drawImage(QRect(0, 0, cellWidth, cellHeight), QImage(":/res/imgs/res/-" + QString::number(i) + ".png").scaled(cellWidth, cellHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    }

    for (int i = 11; i < 13 ; i ++)
    {
        pixmaps[i] = QPixmap(cellWidth, cellHeight);
        pixmaps[i].fill(QColor(0, 0, 0, 0));
        QPainter painter(&pixmaps[i]);
        painter.drawImage(QRect(0, 0, cellWidth, cellHeight), QImage(":/res/imgs/res/-" + QString::number(i) + ".png").scaled(cellWidth, cellHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    }
    for(int i = 0; i < 2; i ++)
        for(int j = 0; j < 2; j ++)
            for(int k = 0; k < 2; k ++)
            {
                posXY[i][j][k] = -2;
                matchedSelXY[i][j][k] = -2;
            }

    gameModel.init();
    this->setFocus();
}

MutiGameForm::~MutiGameForm()
{
    delete ui;
}

void MutiGameForm::closeEvent(QCloseEvent *)
{
    if(mainTimerID > -1)
        killTimer(mainTimerID);
    if(drawLineTimer1ID > -1)
        killTimer(drawLineTimer1ID);
    if(drawLineTimer2ID > -1)
        killTimer(drawLineTimer2ID);
    emit closed();
}
//寻找可到达的提示对
void MutiGameForm::findHintPath()
{
    int pos2, pos3;
    for (int x1 = 2; x1 < 12; x1 ++)
        for(int y1 = 2; y1 < 18; y1 ++)
            for(int x2 = 2; x2 < 12; x2 ++)
                for(int y2 = 2; y2 < 18; y2 ++)
                {
                    if(x1 == x2 && y1 == y2)
                        continue;
                    int pic1 = x1 * 20 + y1, pic2 = x2 * 20 + y2, tmp1, tmp2;
                    tmp1 = gameModel.map[x1][y1];
                    tmp2 = gameModel.map[x2][y2];
                    if (gameModel.linkWithNoCorner(pic1, pic2) ||
                            gameModel.linkWithOneCorner(pic1, pic2, pos2) ||
                            gameModel.linkWithTwoCorner(pic1, pic2, pos2, pos3)) {
                        gameModel.map[x1][y1] = tmp1;
                        gameModel.map[x2][y2] = tmp2;
                        int pos;
                        //必须人物可到达，否则不会显示高亮
                        if (gameModel.isWalkable(player1Y, player1X, x1, y1, pos) && gameModel.isWalkable(player1Y, player1X, x2, y2, pos)){
                            hint1X = y1;
                            hint1Y = x1;
                            hint2X = y2;
                            hint2Y = x2;
                            break;
                        }
                    }
                }
    qDebug() << "Hint: " << hint1X << " " << hint1Y << " " << hint2X << " " << hint2Y << endl;
}

void MutiGameForm::timerEvent(QTimerEvent *event)
{
    int currentId = event->timerId();

    if (currentId == mainTimerID) {
        qDebug() << "MainTimerID: " << currentId << endl;
        leftTime -= 1;
        if(freezeTime1 > 0){
            freezeTime1 -= 1;
        }
        if(freezeTime2 > 0){
            freezeTime2 -= 1;
        }
        if(dizzyTime1 > 0){
            dizzyTime1 -= 1;
        }
        if(dizzyTime2 > 0){
            dizzyTime2 -= 1;
        }
        if (hintTime > 0)
            hintTime -= 1;
        else {
            hint1X = hint1Y = hint2X = hint2Y = -2;
        }
        //一些bar的数据改动，右边栏加入一些道具信息
        ui->time_progressBar->setValue(leftTime);
        ui->time_label->setText(QString::number(leftTime) + " 秒");
        ui->hint_label->setText("HintTime: " + QString::number(hintTime) + "s");
        ui->blueFreeze_label->setText("BlueFreezeTime: " + QString::number(freezeTime1) + "s");
        ui->blueDizzy_label->setText("BlueDizzyTime: " + QString::number(dizzyTime1) + "s");
        ui->greenFreeze_label->setText("GreenFreezeTime: " + QString::number(freezeTime2) + "s");
        ui->greenDizzy_label->setText("GreenDizzyTime: " + QString::number(dizzyTime2) + "s");
        if(totalBlocks == 0) {
            if(score1 > score2) {
                QMessageBox::information(this, "游戏结束", "蓝色玩家胜利!");
            } else if(score1 < score2) {
                QMessageBox::information(this, "游戏结束", "绿色玩家胜利!");
            } else {
                QMessageBox::information(this, "游戏结束", "平局!");
            }
            killTimer(mainTimerID);
            ui->canvas_widget->setEnabled(false);
            ui->start_pushButton->setEnabled(false);
            ui->continue_pushButton->setEnabled(false);
            ui->save_pushButton->setEnabled(false);
            ui->load_pushButton->setEnabled(false);
            ui->stop_pushButton->setEnabled(false);
            ui->return_pushButton->setEnabled(true);
            return;
        } else if (leftTime == 0) {
            killTimer(mainTimerID);
            if(score1 > score2) {
                QMessageBox::information(this, "游戏结束", "蓝色胜利!");
            } else if(score1 < score2) {
                QMessageBox::information(this, "游戏结束", "绿色胜利!");
            } else {
                QMessageBox::information(this, "游戏结束", "平局!");
            }
            ui->canvas_widget->setEnabled(false);
            ui->start_pushButton->setEnabled(false);
            ui->continue_pushButton->setEnabled(false);
            ui->save_pushButton->setEnabled(false);
            ui->load_pushButton->setEnabled(false);
            ui->stop_pushButton->setEnabled(false);
            ui->return_pushButton->setEnabled(true);
            return;
        }
        if (toolCnt < toolMaxCnt && qrand() % 5 == 0)
        {
            int tx, ty;
            tx = qrand() % 20;
            ty = qrand() % 16;
            if (gameModel.map[ty][tx] == 0 && !(tx == player1X && ty == player1Y) && !(tx == player2X && ty == player2Y))
            {
                gameModel.map[ty][tx] = - (qrand() % 6 + 1);
                if(gameModel.map[ty][tx] == -4)
                    gameModel.map[ty][tx] = 0;
                else
                    toolCnt ++;
            }
        }
    } else if (currentId == drawLineTimer1ID) {
        qDebug() << "DrawLineTimer1ID: " << currentId << endl;
        for(int j = 0; j < 2; j ++)
            for(int k = 0; k < 2; k ++)
            {
                posXY[0][j][k] = -2;
                matchedSelXY[0][j][k] = -2;
            }
        killTimer(drawLineTimer1ID);
        drawLineTimer1ID = -1;
    } else if (currentId == drawLineTimer2ID) {
        qDebug() << "DrawLineTimer2ID: " << currentId << endl;
        for(int j = 0; j < 2; j ++)
            for(int k = 0; k < 2; k ++)
            {
                posXY[1][j][k] = -2;
                matchedSelXY[1][j][k] = -2;
            }
        killTimer(drawLineTimer2ID);
        drawLineTimer2ID = -1;
    }
    QWidget::timerEvent(event);
}

void MutiGameForm::keyPressEvent(QKeyEvent *event)
{
    if (mainTimerID < 0)
        return;
    this->setFocus();
    int t1X, t1Y, t2X, t2Y, move1 = 1, move2 = 1;
    t1X = player1X;
    t1Y = player1Y;
    t2X = player2X;
    t2Y = player2Y;
    if(dizzyTime1 > 0)
        move1 = - move1;
    if(dizzyTime2 > 0)
        move2 = - move2;
    if(freezeTime1 > 0) {
        player1X = t1X;
        player1Y = t1Y;
    } else if (event->key() == Qt::Key_W) {
        player1Y -= move1;
    } else if (event->key() == Qt::Key_S) {
        player1Y += move1;
    } else if (event->key() == Qt::Key_A) {
        player1X -= move1;
    } else if (event->key() == Qt::Key_D) {
        player1X += move1;
    }

    if(freezeTime2 > 0) {
        player2X = t2X;
        player2Y = t2Y;
    } else if (event->key() == Qt::Key_Up) {
        player2Y -= move2;
    } else if (event->key() == Qt::Key_Down) {
        player2Y += move2;
    } else if (event->key() == Qt::Key_Left) {
        player2X -= move2;
    } else if (event->key() == Qt::Key_Right) {
        player2X += move2;
    }

    if (player1X < 0 || player1X > 19 || player1Y < 0 || player1Y > 13) {
        player1X = t1X;
        player1Y = t1Y;
    }

    if (player2X < 0 || player2X > 19 || player2Y < 0 || player2Y > 13) {
        player2X = t2X;
        player2Y = t2Y;
    }

    if (gameModel.map[player1Y][player1X] > 0) { // 判断蓝色玩家到达的位置是否方块
        int pos2 = -1, pos3 = -1;
        if (selX1 == player1X && selY1 == player1Y) { // 判断该方块是否已经被选择过， 如果是的话消除选择状态
            selX1 = -2;
            selY1 = -2;
        } else if(selX1 == -2 && selY1 == -2) { //判断上次有没有选择的方块
            selX1 = player1X;
            selY1 = player1Y;
        } else if (gameModel.linkWithNoCorner(selY1 * 20 + selX1, player1Y * 20 + player1X) ||
                  gameModel.linkWithOneCorner(selY1 * 20 + selX1, player1Y * 20 + player1X, pos2) ||
                  gameModel.linkWithTwoCorner(selY1 * 20 + selX1, player1Y * 20 + player1X, pos2, pos3)) {
            if (pos2 == -1) {
                pos2 = pos3 = selY1 * 20 + selX1;
            } else if (pos3 == -1) {
                pos3 = pos2;
            }
            gameModel.map[selY1][selX1] = 0;
            gameModel.map[player1Y][player1X] = 0;
            posXY[0][0][0] = pos2 % 20;
            posXY[0][0][1] = pos2 / 20;
            posXY[0][1][0] = pos3 % 20;
            posXY[0][1][1] = pos3 / 20;

            matchedSelXY[0][0][0] = selX1;
            matchedSelXY[0][0][1] = selY1;
            matchedSelXY[0][1][0] = player1X;
            matchedSelXY[0][1][1] = player1Y;
            if (hintTime > 0 && ((matchedSelXY[0][0][0] == hint1X && matchedSelXY[0][0][1] ==hint1Y && matchedSelXY[0][1][0] == hint2X && matchedSelXY[0][1][1] == hint2Y) ||
                                 (matchedSelXY[0][0][0] == hint2X && matchedSelXY[0][0][1] ==hint2Y && matchedSelXY[0][1][0] == hint1X && matchedSelXY[0][1][1] == hint1Y))) {
                findHintPath();//找hint
            }
            selX1 = selY1 = -2;
            drawLineTimer1ID = startTimer(200, Qt::PreciseTimer);
            totalBlocks -= 2;
            score1 += 2;
            ui->blueScore_label->setText("BlueScore: " + QString::number(score1));
        }
        else {
            selX1 = player1X;
            selY1 = player1Y;
        }

        player1X = t1X;
        player1Y = t1Y;
    } else if(gameModel.map[player1Y][player1X] == -1) { //判断玩家到达的位置是否"+1s"工具
        gameModel.map[player1Y][player1X] = 0;
        leftTime = leftTime + 30;
        if (leftTime > 200) {
            leftTime = 200;
        }
        toolCnt -= 1;
    } else if(gameModel.map[player1Y][player1X] == -2) { //判断玩家到达的位置是否"Shuffle"工具
        gameModel.map[player1Y][player1X] = 0;
        gameModel.shuffle();

        if(hintTime > 0) {
            findHintPath();
        }
        if (gameModel.map[player1Y][player1X] > 0) {
            player1X = 0;
            player1Y = 0;
        }
        if (gameModel.map[player2Y][player2X] > 0) {
            player2X = 19;
            player2Y = 13;
        }
        selX1 = selY1 = selX2 = selY2 = -2;
        toolCnt -= 1;
    } else if(gameModel.map[player1Y][player1X] == -3) { //判断玩家到达的位置是否"Hint"工具
        gameModel.map[player1Y][player1X] = 0;
        hintTime = 10;
        findHintPath();
        toolCnt -= 1;
    } else if(gameModel.map[player1Y][player1X] == -5) { //判断玩家到达的位置是否"Freeze"工具
        gameModel.map[player1Y][player1X] = 0;
        freezeTime2 = 3;
        toolCnt -= 1;
    } else if(gameModel.map[player1Y][player1X] == -6) { //判断玩家到达的位置是否"Dizzy"工具
        gameModel.map[player1Y][player1X] = 0;
        dizzyTime2 = 10;
        toolCnt -= 1;
    }


    if (gameModel.map[player2Y][player2X] > 0) { // 判断绿色玩家到达的位置是否方块
        int pos2 = -1, pos3 = -1;
        if (selX2 == player2X && selY2 == player2Y) { // 判断该方块是否已经被选择过， 如果是的话消除选择状态
            selX2 = -2;
            selY2 = -2;
        } else if(selX2 == -2 && selY2 == -2) { //判断上次有没有选择的方块
            selX2 = player2X;
            selY2 = player2Y;
        } else if (gameModel.linkWithNoCorner(selY2 * 20 + selX2, player2Y * 20 + player2X) ||
                  gameModel.linkWithOneCorner(selY2 * 20 + selX2, player2Y * 20 + player2X, pos2) ||
                  gameModel.linkWithTwoCorner(selY2 * 20 + selX2, player2Y * 20 + player2X, pos2, pos3)) {
            if (pos2 == -1) {
                pos2 = pos3 = selY2 * 20 + selX2;
            } else if (pos3 == -1) {
                pos3 = pos2;
            }
            gameModel.map[selY2][selX2] = 0;
            gameModel.map[player2Y][player2X] = 0;
            posXY[1][0][0] = pos2 % 20;
            posXY[1][0][1] = pos2 / 20;
            posXY[1][1][0] = pos3 % 20;
            posXY[1][1][1] = pos3 / 20;

            matchedSelXY[1][0][0] = selX2;
            matchedSelXY[1][0][1] = selY2;
            matchedSelXY[1][1][0] = player2X;
            matchedSelXY[1][1][1] = player2Y;
            if (hintTime > 0 && ((matchedSelXY[1][0][0] == hint1X && matchedSelXY[1][0][1] ==hint1Y && matchedSelXY[1][1][0] == hint2X && matchedSelXY[1][1][1] == hint2Y) ||
                                 (matchedSelXY[1][0][0] == hint2X && matchedSelXY[1][0][1] ==hint2Y && matchedSelXY[1][1][0] == hint1X && matchedSelXY[1][1][1] == hint1Y))) {
                findHintPath();
            }
            selX2 = selY2 = -2;
            drawLineTimer2ID = startTimer(200, Qt::PreciseTimer);
            totalBlocks -= 2;
            score2 += 2;
            ui->greenScore_label->setText("GreenScore: " + QString::number(score2));
        }
        else {
            selX2 = player2X;
            selY2 = player2Y;
        }

        player2X = t2X;
        player2Y = t2Y;
    } else if(gameModel.map[player2Y][player2X] == -1) { //判断玩家到达的位置是否"+1s"工具
        gameModel.map[player2Y][player2X] = 0;
        leftTime = leftTime + 30;
        if (leftTime > 200) {
            leftTime = 200;
        }
        toolCnt -= 1;
    } else if(gameModel.map[player2Y][player2X] == -2) { //判断玩家到达的位置是否"Shuffle"工具
        gameModel.map[player2Y][player2X] = 0;
        gameModel.shuffle();

        if(hintTime > 0) {
            findHintPath();
        }
        if (gameModel.map[player1Y][player1X] > 0) {
            player1X = 0;
            player1Y = 0;
        }
        if (gameModel.map[player2Y][player2X] > 0) {
            player2X = 19;
            player2Y = 13;
        }
        selX1 = selY1 = selX2 = selY2 = -2;
        toolCnt -= 1;
    } else if(gameModel.map[player2Y][player2X] == -3) { //判断玩家到达的位置是否"Hint"工具
        gameModel.map[player2Y][player2X] = 0;
        hintTime = 10;
        findHintPath();
        toolCnt -= 1;
    } else if(gameModel.map[player2Y][player2X] == -5) { //判断玩家到达的位置是否"Freeze"工具
        gameModel.map[player2Y][player2X] = 0;
        freezeTime1 = 3;
        toolCnt -= 1;
    } else if(gameModel.map[player2Y][player2X] == -6) { //判断玩家到达的位置是否"Dizzy"工具
        gameModel.map[player2Y][player2X] = 0;
        dizzyTime1 = 10;
        toolCnt -= 1;
    }
}

bool MutiGameForm::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->canvas_widget && event->type() == QEvent::Paint) {
        paintBackground(ui->canvas_widget);
        return true;
    }
    else if(event->type() == QEvent::KeyPress) {
        qDebug() << "KeyPressed" << endl;
        this->setFocus();
        return true;
    }
    return false;
}

void MutiGameForm::on_return_pushButton_clicked()
{
    this->close();
}

void MutiGameForm::on_stop_pushButton_clicked()
{
    killTimer(mainTimerID);
    mainTimerID = -1;
    this->setFocus();
    ui->canvas_widget->setEnabled(false);
    ui->start_pushButton->setEnabled(false);
    ui->continue_pushButton->setEnabled(true);
    ui->save_pushButton->setEnabled(true);
    ui->load_pushButton->setEnabled(true);
    ui->stop_pushButton->setEnabled(false);
    ui->return_pushButton->setEnabled(true);
}

void MutiGameForm::on_continue_pushButton_clicked()
{
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    mainTimerID = startTimer(1000, Qt::PreciseTimer);
    this->setFocus();
    ui->canvas_widget->setEnabled(true);
    ui->start_pushButton->setEnabled(false);
    ui->continue_pushButton->setEnabled(false);
    ui->save_pushButton->setEnabled(false);
    ui->load_pushButton->setEnabled(false);
    ui->stop_pushButton->setEnabled(true);
    ui->return_pushButton->setEnabled(true);
}

void MutiGameForm::on_save_pushButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "保存游戏", ".", "多人游戏文件(*.multi)");
    if (fileName.isEmpty())
        return;
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, "错误", QString("打开文件\"%1\"失败!").arg(fileName), QMessageBox::Ok);
        return;
    }

    QTextStream f(&file);
    f << "MultiGame" << endl;
    f << score1 << " " << player1X << " " << player1Y << " " << selX1 << " " << selY1 << " " << score2 << " " << player2X << " " << player2Y << " " << selX2 << " " << selY2 << " " << leftTime << " " << hintTime << " " << freezeTime1 << " " << freezeTime2 << " " << dizzyTime1 << " " << dizzyTime2 << " " << hint1X << " " << hint1Y << " " << hint2X << " " << hint2Y << " " << toolCnt << endl;
    for (int i = 0; i < 14; i ++) {
        for (int j = 0; j < 20; j ++) {
            f << gameModel.map[i][j] << " ";
        }
        f << endl;
    }
    f.flush();
    file.close();
    qDebug() << "Save File Name: " << fileName << endl;
}

void MutiGameForm::loadFile(QString fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "错误", QString("打开文件\"%1\"失败!").arg(fileName), QMessageBox::Ok);
        return;
    }
    QTextStream rd(&file);
     QString tstr;
     rd >> tstr;
     if (tstr != "MultiGame") {
         QMessageBox::critical(this, "错误", QString("文件\"%1\"不是单人游戏文件!").arg(fileName), QMessageBox::Ok);
         file.close();
         return;
     }
     rd >> score1 >> player1X >> player1Y >> selX1 >> selY1 >> score2 >> player2X >> player2Y >> selX2 >> selY2 >> leftTime >> hintTime >> freezeTime1 >> freezeTime2 >> dizzyTime1 >> dizzyTime2 >> hint1X >> hint1Y >> hint2X >> hint2Y >> toolCnt;
     qDebug() << score1 << " " << player1X << " " << player1Y << " " << selX1 << " " << selY1 << " " << score2 << " " << player2X << " " << player2Y << " " << selX2 << " " << selY2 << " " << leftTime << " " << hintTime << " " << hint1X << " " << hint1Y << " " << hint2X << " " << hint2Y << " " << toolCnt << endl;
     for (int i = 0; i < 14; i ++)
         for (int j = 0; j < 20; j ++) {
             rd >> gameModel.map[i][j];
         }
     ui->blueScore_label->setText("BlueScore: " + QString::number(score1));
     ui->greenScore_label->setText("GreenScore: " + QString::number(score2));
     ui->hint_label->setText("HintTime: " + QString::number(hintTime));
     ui->blueFreeze_label->setText("BlueFreezeTime: " + QString::number(freezeTime1) + "s");
     ui->blueDizzy_label->setText("BlueDizzyTime: " + QString::number(dizzyTime1) + "s");
     ui->greenFreeze_label->setText("GreenFreezeTime: " + QString::number(freezeTime2) + "s");
     ui->greenDizzy_label->setText("GreenDizzyTime: " + QString::number(dizzyTime2) + "s");
     ui->time_progressBar->setValue(leftTime);
     ui->time_label->setText(QString::number(leftTime) + " 秒");
     file.close();
     ui->canvas_widget->setEnabled(false);
     ui->start_pushButton->setEnabled(false);
     ui->continue_pushButton->setEnabled(true);
     ui->save_pushButton->setEnabled(true);
     ui->load_pushButton->setEnabled(true);
     ui->stop_pushButton->setEnabled(false);
     ui->return_pushButton->setEnabled(true);
}

void MutiGameForm::on_load_pushButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "加载游戏", ".", "多人游戏文件(*.multi)");
    if (fileName.isEmpty())
        return;
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "错误", QString("打开文件\"%1\"失败!").arg(fileName), QMessageBox::Ok);
        return;
    }

    QTextStream rd(&file);
    QString tstr;
    rd >> tstr;
    if (tstr != "MultiGame") {
        QMessageBox::critical(this, "错误", QString("文件\"%1\"不是多人游戏文件!").arg(fileName), QMessageBox::Ok);
        file.close();
        return;
    }
    file.close();
    loadFile(fileName);

    qDebug() << "Load File Name: " << fileName << endl;
}

void MutiGameForm::paintBackground(QWidget *widget)
{
    QPixmap pixmap(cellWidth * 20, cellHeight * 14);
    QPainter pixmapPainter(&pixmap);
    QBrush bgBrush(QColor(154, 217, 234));
    pixmapPainter.fillRect(0, 0, pixmap.width(), pixmap.height(), bgBrush);
    pixmapPainter.setPen(QColor(255, 255, 255));
    for (int i = 0; i < 14; i ++) {
        for (int j = 0; j < 20; j ++) {
            if (gameModel.map[i][j] == 0) {
                continue;
            }
            else if (gameModel.map[i][j] > 0) {
                if ((hint1X == j && hint1Y == i) || (hint2X == j && hint2Y == i))
                    pixmapPainter.fillRect(j * cellWidth, i * cellHeight, cellWidth, cellHeight, QColor(255, 0, 0));
                pixmapPainter.drawPixmap(j * cellWidth, i * cellHeight, blocks[gameModel.map[i][j]]);
            }
            else {
                pixmapPainter.drawPixmap(j * cellWidth, i * cellHeight, pixmaps[-gameModel.map[i][j]]);
            }
        }
    }
    pixmapPainter.drawImage(player1X * cellWidth, player1Y * cellHeight, QImage(":/res/imgs/res/-11.png").scaled(cellWidth, cellHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    pixmapPainter.drawImage(player2X * cellWidth, player2Y * cellHeight, QImage(":/res/imgs/res/-12.png").scaled(cellWidth, cellHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

    pixmapPainter.setPen(QPen(QBrush(QColor(0, 0, 255)), 5));
    pixmapPainter.drawRect(selX1 * cellWidth, selY1 * cellHeight, cellWidth, cellHeight);
    pixmapPainter.drawRect(matchedSelXY[0][0][0] * cellWidth, matchedSelXY[0][0][1] * cellHeight, cellWidth, cellHeight);
    pixmapPainter.drawRect(matchedSelXY[0][1][0] * cellWidth, matchedSelXY[0][1][1] * cellHeight, cellWidth, cellHeight);
    pixmapPainter.drawLine(matchedSelXY[0][0][0] * cellWidth + cellWidth / 2, matchedSelXY[0][0][1] * cellHeight  + cellHeight / 2, posXY[0][0][0] * cellWidth + cellWidth / 2, posXY[0][0][1] * cellHeight  + cellHeight / 2);
    pixmapPainter.drawLine(posXY[0][0][0] * cellWidth + cellWidth / 2, posXY[0][0][1] * cellHeight  + cellHeight / 2, posXY[0][1][0] * cellWidth + cellWidth / 2, posXY[0][1][1] * cellHeight + cellHeight / 2);
    pixmapPainter.drawLine(posXY[0][1][0] * cellWidth + cellWidth / 2, posXY[0][1][1] * cellHeight + cellHeight / 2, matchedSelXY[0][1][0] * cellWidth + cellWidth / 2, matchedSelXY[0][1][1] * cellHeight  + cellHeight / 2);

    pixmapPainter.setPen(QPen(QBrush(QColor(0, 255, 0)), 5));
    pixmapPainter.drawRect(selX2 * cellWidth, selY2 * cellHeight, cellWidth, cellHeight);
    pixmapPainter.drawRect(matchedSelXY[1][0][0] * cellWidth, matchedSelXY[1][0][1] * cellHeight, cellWidth, cellHeight);
    pixmapPainter.drawRect(matchedSelXY[1][1][0] * cellWidth, matchedSelXY[1][1][1] * cellHeight, cellWidth, cellHeight);
    pixmapPainter.drawLine(matchedSelXY[1][0][0] * cellWidth + cellWidth / 2, matchedSelXY[1][0][1] * cellHeight  + cellHeight / 2, posXY[1][0][0] * cellWidth + cellWidth / 2, posXY[1][0][1] * cellHeight  + cellHeight / 2);
    pixmapPainter.drawLine(posXY[1][0][0] * cellWidth + cellWidth / 2, posXY[1][0][1] * cellHeight  + cellHeight / 2, posXY[1][1][0] * cellWidth + cellWidth / 2, posXY[1][1][1] * cellHeight + cellHeight / 2);
    pixmapPainter.drawLine(posXY[1][1][0] * cellWidth + cellWidth / 2, posXY[1][1][1] * cellHeight + cellHeight / 2, matchedSelXY[1][1][0] * cellWidth + cellWidth / 2, matchedSelXY[1][1][1] * cellHeight  + cellHeight / 2);

    QPainter canvasPainter(widget);
    canvasPainter.drawPixmap(0, 0, pixmap.width(), pixmap.height(), pixmap);
    widget->update();
}

void MutiGameForm::on_start_pushButton_clicked()
{
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    mainTimerID = startTimer(1000, Qt::PreciseTimer);
    ui->canvas_widget->setEnabled(true);
    ui->start_pushButton->setEnabled(false);
    ui->continue_pushButton->setEnabled(false);
    ui->save_pushButton->setEnabled(false);
    ui->load_pushButton->setEnabled(false);
    ui->stop_pushButton->setEnabled(true);
    ui->return_pushButton->setEnabled(true);
    this->setFocus();
}

