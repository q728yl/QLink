#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);
    this->setWindowTitle("连连看");
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_singleGame_pushButton_clicked()
{
    SingleGameForm *sgf = new SingleGameForm();
    sgf->setWindowTitle("连连看－单人模式");
    connect(sgf, SIGNAL(closed()), this, SLOT(show()));
    this->hide();
    sgf->show();
}

void MainWindow::on_multiGame_pushButton_clicked()
{
    MutiGameForm *mgf = new MutiGameForm();
    mgf->setWindowTitle("连连看－多人模式");
    connect(mgf, SIGNAL(closed()), this, SLOT(show()));
    this->hide();
    mgf->show();
}

void MainWindow::on_loadGame_pushButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "加载游戏", ".", "QLink游戏文件(*.sing *.multi)");
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
    file.close();
    if (tstr == "SingleGame") {
        SingleGameForm *sgf = new SingleGameForm();
        sgf->setWindowTitle("连连看－单人模式");

        sgf->loadFile(fileName);

        connect(sgf, SIGNAL(closed()), this, SLOT(show()));
        this->hide();
        sgf->show();
    } else if(tstr == "MultiGame") {
        MutiGameForm *mgf = new MutiGameForm();
        mgf->setWindowTitle("连连看－多人模式");

        mgf->loadFile(fileName);

        connect(mgf, SIGNAL(closed()), this, SLOT(show()));
        this->hide();
        mgf->show();
    } else {
        QMessageBox::critical(this, "错误", QString("文件\"%1\"不是QLink游戏文件!").arg(fileName), QMessageBox::Ok);
        file.close();
    }

    qDebug() << "Load File Name: " << fileName << endl;
}

void MainWindow::on_exitGame_pushButton_clicked()
{
    this->close();
}

