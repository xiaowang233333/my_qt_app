#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    tcpserver = new QTcpServer(this);
    connect(tcpserver,SIGNAL(newConnection()),this,SLOT(newconnect()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::newconnect()
{
    //connect to client
    QTcpSocket *tcpsocket= tcpserver->nextPendingConnection();
    QString ipaddr =tcpsocket->peerAddress().toString();
    quint16 port=tcpsocket->peerPort();
    ui->comboBox->addItem(ipaddr+":"+QString::number(port));
    ui->textBrowser->append("客户端ip地址："+ipaddr);
    ui->textBrowser->append("客户端端口："+QString::number(port));
    connect(tcpsocket,SIGNAL(readyRead()),this,SLOT(receivemessage()));
    connect(tcpsocket,SIGNAL(stateChanged(QAbstractSocket::SocketState)),this,SLOT(statechanged(QAbstractSocket::SocketState)));
}

void MainWindow::receivemessage()
{
    QTcpSocket *TMPTCP=(QTcpSocket *)sender();
    //接收数据
    ui->textBrowser->append(TMPTCP->readAll());

}

void MainWindow::statechanged(QAbstractSocket::SocketState state)
{
    QTcpSocket *TMPTCP=(QTcpSocket *)sender();
    switch(state){
        case QAbstractSocket::UnconnectedState:
        ui->textBrowser->append("client disconnect");
        TMPTCP->deleteLater();
        break;

        case QAbstractSocket::ConnectedState:
        ui->textBrowser->append("client connect");
        break;

        default:break;
    }
}


void MainWindow::on_SEND_clicked()
{
    QList< QTcpSocket *> socketlist =tcpserver->findChildren<QTcpSocket*>();
    foreach( QTcpSocket *TMPTCP,socketlist){
        TMPTCP->write(ui->lineEdit->text().toUtf8());
    }

}


void MainWindow::on_start_clicked()
{
    tcpserver->listen(QHostAddress("192.168.0.148"),8080);
    ui->textEdit->clear();
    ui->textEdit->setText("current listen ip/port  192.168.0.148:8080");

}


void MainWindow::on_stop_clicked()
{
    tcpserver->close();
}

