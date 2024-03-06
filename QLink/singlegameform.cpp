#include "singlegameform.h"
#include "ui_singlegameform.h"
#include <QPixmap>
#include <QPainter>
#include <QDebug>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QMessageBox>
#include <QTime>
#include <QFileDialog>
#include <QFile>


SingleGameForm::SingleGameForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SingleGameForm)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);
    leftTime = 200;
    totalBlocks = 160;
    toolMaxCnt = 1;
    toolCnt = 0;
    score = 0;
    hintTime = flashTime = 0;
    hint1X = hint1Y = hint2X = hint2Y = -2;
    ui->time_progressBar->setRange(0, 200);
    ui->time_progressBar->setValue(200);
    ui->time_label->setText(QString::number(leftTime) + " 秒");
    ui->canvas_widget->installEventFilter(this);
    mainTimerID = -1;
    drawLineTimerID = -1;
    playerX = 0;
    playerY = 0;
    selX1 = -2;
    selY1 = -2;
    matchedSelX1 = matchedSelX2 = matchedSelY1 = matchedSelY2 = -2;
    pos2X = pos2Y = pos3X = pos3Y = -2;
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

    gameModel.init();
    this->setFocus();
}

SingleGameForm::~SingleGameForm()
{
    delete ui;
}

void SingleGameForm::closeEvent(QCloseEvent *)
{
    if(mainTimerID > -1)
        killTimer(mainTimerID);
    if(drawLineTimerID > -1)
        killTimer(drawLineTimerID);
    emit closed();
}

void SingleGameForm::findHint()
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
                        if (gameModel.isWalkable(playerY, playerX, x1, y1, pos) && gameModel.isWalkable(playerY, playerX, x2, y2, pos)){
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

void SingleGameForm::timerEvent(QTimerEvent *event)
{
    int currentId = event->timerId();

    if (currentId == mainTimerID) {
        qDebug() << "MainTimerID: " << currentId << endl;
        leftTime -= 1;
        if (hintTime > 0)
            hintTime -= 1;
        else {
            hint1X = hint1Y = hint2X = hint2Y = -2;
        }
        if (flashTime > 0)
            flashTime -= 1;
        ui->time_progressBar->setValue(leftTime);
        ui->time_label->setText(QString::number(leftTime) + " 秒");
        ui->hint_label->setText("HintTime: " + QString::number(hintTime) + "s");
        ui->flash_label->setText("FlashTime: " + QString::number(flashTime) + "s");
        if(totalBlocks == 0) {
            killTimer(mainTimerID);
            QMessageBox::information(this, "游戏结束", "胜利!");
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
            QMessageBox::information(this, "游戏结束", "失败!");
            ui->canvas_widget->setEnabled(false);
            ui->start_pushButton->setEnabled(false);
            ui->continue_pushButton->setEnabled(false);
            ui->save_pushButton->setEnabled(false);
            ui->load_pushButton->setEnabled(false);
            ui->stop_pushButton->setEnabled(false);
            ui->return_pushButton->setEnabled(true);
            return;
        }
        if (toolCnt < toolMaxCnt && qrand() % 10 == 0)
        {
            int tx, ty;
            tx = qrand() % 20;
            ty = qrand() % 16;
            if (gameModel.map[ty][tx] == 0 && !(tx == playerX && ty == playerY))
            {
                gameModel.map[ty][tx] = - (qrand() % 4 + 1);
                toolCnt ++;
            }
        }
    } else if (currentId == drawLineTimerID) {
        qDebug() << "DrawLineTimerID: " << currentId << endl;
        matchedSelX1 = matchedSelX2 = matchedSelY1 = matchedSelY2 = -2;
        pos2X = pos2Y = pos3X = pos3Y = -2;
        killTimer(drawLineTimerID);
        drawLineTimerID = -1;
    }
    QWidget::timerEvent(event);
}

void SingleGameForm::canvasMousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && flashTime > 0) {
        int tx, ty, pos;
        tx = event->x() / cellWidth;
        ty = event->y() / cellHeight;
        qDebug() << "MousePos:" << tx << " " << ty << endl;
        if (gameModel.map[ty][tx] > 0)
            return;
        if (gameModel.isWalkable(playerY, playerX, ty, tx, pos)) {
            playerX = tx;
            playerY = ty;
        }

        qDebug() << "MousePos:" << event->x() << " " << event->y() << endl;
        ui->canvas_widget->update();
    }
}

void SingleGameForm::keyPressEvent(QKeyEvent *event)
{
    if (mainTimerID < 0)
        return;
    this->setFocus();
    int tX, tY;
    tX = playerX;
    tY = playerY;
    if (event->key() == Qt::Key_Up) {
        playerY -= 1;
    } else if (event->key() == Qt::Key_Down) {
        playerY += 1;
    } else if (event->key() == Qt::Key_Left) {
        playerX -= 1;
    } else if (event->key() == Qt::Key_Right) {
        playerX += 1;
    }
    if (playerX < 0 || playerX > 19 || playerY < 0 || playerY > 13) {
        playerX = tX;
        playerY = tY;
    }
    if (gameModel.map[playerY][playerX] > 0) { // 判断玩家到达的位置是否方块
        int pos2 = -1, pos3 = -1;
        if (selX1 == playerX && selY1 == playerY) { // 判断该方块是否已经被选择过， 如果是的话消除选择状态
            selX1 = -2;
            selY1 = -2;
        } else if(selX1 == -2 && selY1 == -2) { //判断上次有没有选择的方块
            selX1 = playerX;
            selY1 = playerY;
        } else if (gameModel.linkWithNoCorner(selY1 * 20 + selX1, playerY * 20 + playerX) ||
                  gameModel.linkWithOneCorner(selY1 * 20 + selX1, playerY * 20 + playerX, pos2) ||
                  gameModel.linkWithTwoCorner(selY1 * 20 + selX1, playerY * 20 + playerX, pos2, pos3)) {
            if (pos2 == -1) {
                pos2 = pos3 = selY1 * 20 + selX1;
            } else if (pos3 == -1) {
                pos3 = pos2;
            }
            gameModel.map[selY1][selX1] = 0;
            gameModel.map[playerY][playerX] = 0;
            pos2X = pos2 % 20;
            pos2Y = pos2 / 20;
            pos3X = pos3 % 20;
            pos3Y = pos3 / 20;
            matchedSelX1 = selX1;
            matchedSelY1 = selY1;
            matchedSelX2 = playerX;
            matchedSelY2 = playerY;
            if (hintTime > 0 && ((matchedSelX1 == hint1X && matchedSelY1 ==hint1Y && matchedSelX2 == hint2X && matchedSelY2 == hint2Y) ||
                                 (matchedSelX1 == hint2X && matchedSelY1 ==hint2Y && matchedSelX2 == hint1X && matchedSelY2 == hint1Y))) {
                findHint();
            }
            selX1 = selY1 = -2;
            drawLineTimerID = startTimer(200, Qt::PreciseTimer);
            totalBlocks -= 2;
            score += 2;
            ui->score_label->setText("Score: " + QString::number(score));
        }
        else {
            selX1 = playerX;
            selY1 = playerY;
        }

        playerX = tX;
        playerY = tY;
    } else if(gameModel.map[playerY][playerX] == -1) { //判断玩家到达的位置是否"+1s"工具
        gameModel.map[playerY][playerX] = 0;
        leftTime = leftTime + 30;
        if (leftTime > 200) {
            leftTime = 200;
        }
        toolCnt -= 1;
    } else if(gameModel.map[playerY][playerX] == -2) { //判断玩家到达的位置是否"Shuffle"工具
        gameModel.map[playerY][playerX] = 0;
        gameModel.shuffle();
        if(hintTime > 0) {
            findHint();
        }
        if (gameModel.map[playerY][playerX] > 0) {
            playerX = 0;
            playerY = 0;
        }
        selX1 = selY1 = -2;
        toolCnt -= 1;
    } else if(gameModel.map[playerY][playerX] == -3) { //判断玩家到达的位置是否"Hint"工具
        gameModel.map[playerY][playerX] = 0;
        hintTime = 10;
        findHint();
        toolCnt -= 1;
    } else if(gameModel.map[playerY][playerX] == -4) { //判断玩家到达的位置是否"Flash"工具
        gameModel.map[playerY][playerX] = 0;
//        gameModel.shuffle();
        flashTime = 5;
        toolCnt -= 1;
    }
}

bool SingleGameForm::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->canvas_widget && event->type() == QEvent::Paint) {
        drawCanvas(ui->canvas_widget);
        return true;
    }
    else if(event->type() == QEvent::KeyPress) {
        qDebug() << "KeyPressed" << endl;
        this->setFocus();
        return true;
    }
    else if(watched == ui->canvas_widget && event->type() == QEvent::MouseButtonPress) {
        qDebug() << "MousePressed" << endl;
        canvasMousePressEvent((QMouseEvent*)event);
        return true;
    }
    return false;
}

void SingleGameForm::on_return_pushButton_clicked()
{
    this->close();
}

void SingleGameForm::on_stop_pushButton_clicked()
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

void SingleGameForm::on_continue_pushButton_clicked()
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

void SingleGameForm::on_save_pushButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "保存游戏", ".", "单人游戏文件(*.sing)");
    if (fileName.isEmpty())
        return;
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, "错误", QString("打开文件\"%1\"失败!").arg(fileName), QMessageBox::Ok);
        return;
    }

    QTextStream wr(&file);
    wr << "SingleGame" << endl;
    wr << score << " " << playerX << " " << playerY << " " << selX1 << " " << selY1 << " " << leftTime << " " << hintTime << " " << flashTime << " " << hint1X << " " << hint1Y << " " << hint2X << " " << hint2Y << " " << toolCnt << endl;
    for (int i = 0; i < 14; i ++) {
        for (int j = 0; j < 20; j ++) {
            wr << gameModel.map[i][j] << " ";
        }
        wr << endl;
    }
    wr.flush();
    file.close();
    qDebug() << "Save File Name: " << fileName << endl;
}

void SingleGameForm::loadFile(QString fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "错误", QString("打开文件\"%1\"失败!").arg(fileName), QMessageBox::Ok);
        return;
    }
    QTextStream rd(&file);
     QString tstr;
     rd >> tstr;
     if (tstr != "SingleGame") {
         QMessageBox::critical(this, "错误", QString("文件\"%1\"不是单人游戏文件!").arg(fileName), QMessageBox::Ok);
         file.close();
         return;
     }
     rd >> score >> playerX >> playerY >> selX1 >> selY1 >> leftTime >> hintTime >> flashTime >> hint1X >> hint1Y >> hint2X >> hint2Y >> toolCnt;
     qDebug() << score << " " << playerX << " " << playerY << " " << selX1 << " " << selY1 << " " << leftTime << " " << hintTime << " " << flashTime << " " << hint1X << " " << hint1Y << " " << hint2X << " " << hint2Y << " " << toolCnt << endl;
     for (int i = 0; i < 14; i ++)
         for (int j = 0; j < 20; j ++) {
             rd >> gameModel.map[i][j];
         }
     ui->score_label->setText("Score: " + QString::number(score));
     ui->hint_label->setText("HintTime: " + QString::number(hintTime));
     ui->flash_label->setText("FlashTime: " + QString::number(flashTime));
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

void SingleGameForm::on_load_pushButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "加载游戏", ".", "单人游戏文件(*.sing)");
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
    if (tstr != "SingleGame") {
        QMessageBox::critical(this, "错误", QString("文件\"%1\"不是单人游戏文件!").arg(fileName), QMessageBox::Ok);
        file.close();
        return;
    }
    file.close();
    loadFile(fileName);

    qDebug() << "Load File Name: " << fileName << endl;
}

void SingleGameForm::drawCanvas(QWidget *widget)
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
    pixmapPainter.drawImage(playerX * cellWidth, playerY * cellHeight, QImage(":/res/imgs/res/-11.png").scaled(cellWidth, cellHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));


    pixmapPainter.setPen(QPen(QBrush(QColor(0, 0, 255)), 5));
    pixmapPainter.drawRect(selX1 * cellWidth, selY1 * cellHeight, cellWidth, cellHeight);
    pixmapPainter.drawRect(matchedSelX1 * cellWidth, matchedSelY1 * cellHeight, cellWidth, cellHeight);
    pixmapPainter.drawRect(matchedSelX2 * cellWidth, matchedSelY2 * cellHeight, cellWidth, cellHeight);
    pixmapPainter.drawLine(matchedSelX1 * cellWidth + cellWidth / 2, matchedSelY1 * cellHeight  + cellHeight / 2, pos2X * cellWidth + cellWidth / 2, pos2Y * cellHeight  + cellHeight / 2);
    pixmapPainter.drawLine(pos2X * cellWidth + cellWidth / 2, pos2Y * cellHeight  + cellHeight / 2, pos3X * cellWidth + cellWidth / 2, pos3Y * cellHeight + cellHeight / 2);
    pixmapPainter.drawLine(pos3X * cellWidth + cellWidth / 2, pos3Y * cellHeight + cellHeight / 2, matchedSelX2 * cellWidth + cellWidth / 2, matchedSelY2 * cellHeight  + cellHeight / 2);
    QPainter canvasPainter(widget);
    canvasPainter.drawPixmap(0, 0, pixmap.width(), pixmap.height(), pixmap);
    widget->update();
}

void SingleGameForm::on_start_pushButton_clicked()
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
    this->setFocus();//使之成为主窗口，操作它
}
