#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    tcpsocket=new  QTcpSocket(this);
     connect(tcpsocket,SIGNAL(readyRead()),this,SLOT(receivemessage()));
     connect(tcpsocket,SIGNAL(stateChanged(QAbstractSocket::SocketState)),this,SLOT(statechanged(QAbstractSocket::SocketState)));

}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::receivemessage()
{
    ui->textBrowser->append("服务器："+tcpsocket->readAll());
}

void MainWindow::statechanged(QAbstractSocket::SocketState state)
{
    switch (state) {
        case QAbstractSocket::UnconnectedState:
        ui->textBrowser->append("连接已断开");
               break;
        case QAbstractSocket::ConnectedState:
        ui->textBrowser->append("已连接");
               break;

    default:break;
    }
}


void MainWindow::on_send_clicked()
{
    if(tcpsocket->state()==QAbstractSocket::ConnectedState)
        tcpsocket->write(ui->content->text().toUtf8());
    else
        ui->textBrowser->append("请先连接");
}


void MainWindow::on_pushButton_clicked()
{
    tcpsocket->connectToHost(QHostAddress(ui->serip->text()),ui->serport->text().toUInt());
}


void MainWindow::on_pushButton_2_clicked()
{
    tcpsocket->disconnectFromHost();
}

