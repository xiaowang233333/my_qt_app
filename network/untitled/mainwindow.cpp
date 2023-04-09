#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    statuslabel=new QLabel("network test tool  ---designed by wangxuan");
    statuslabel->setStyleSheet("color:gray");
    ui->statusbar->addWidget(statuslabel);

    tcpsocket=new  QTcpSocket(this);
     connect(tcpsocket,SIGNAL(readyRead()),this,SLOT(receivemessage()));
     connect(tcpsocket,SIGNAL(stateChanged(QAbstractSocket::SocketState)),this,SLOT(statechanged(QAbstractSocket::SocketState)));


     tcpserver = new QTcpServer(this);
     connect(tcpserver,SIGNAL(newConnection()),this,SLOT(newconnect()));


     udpsocket = new QUdpSocket(this);
     connect(udpsocket, &QUdpSocket::readyRead,this, &MainWindow::readPendingDatagrams);


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::receivemessage()
{
    ui->textBrowser->setTextColor(Qt::red);
    if(ui->text_hex->isChecked()){
    ui->textBrowser->append(tcpsocket->readAll().toHex());
    }
    else
    {ui->textBrowser->append(tcpsocket->readAll());}
}


void MainWindow::statechanged(QAbstractSocket::SocketState state)
{
    switch (state) {
        case QAbstractSocket::UnconnectedState:
        ui->client_status->setStyleSheet("font-size:20px;color:yellow");
        ui->client_status->setText("IDLE");
        ui->tcp_cli_dis->setEnabled(false);
        ui->tcp_cli_con->setEnabled(true);
        ui->tcp_cli_send->setEnabled(false);
               break;
        case QAbstractSocket::ConnectedState:
        ui->client_status->setStyleSheet("font-size:20px;color:green");
        ui->client_status->setText("CONNECTED");
        ui->tcp_cli_dis->setEnabled(true);
        ui->tcp_cli_con->setEnabled(false);
        ui->tcp_cli_send->setEnabled(true);
               break;

    default:break;
    }
}
void MainWindow::on_tcp_cli_con_clicked()
{
    tcpsocket->connectToHost(QHostAddress(ui->serip->text()),ui->serport->text().toUInt());
}


void MainWindow::on_tcp_cli_dis_clicked()
{
    tcpsocket->disconnectFromHost();
    ui->client_status->setText("");
}


void MainWindow::on_tcp_cli_send_clicked()
{
    if(ui->client_sendhex->isChecked()){
        tcpsocket->write(QByteArray::fromHex(ui->tcp_clien_text->toPlainText().toUtf8()));
    }
    else {
        tcpsocket->write(ui->tcp_clien_text->toPlainText().toUtf8());
    }

  }

void MainWindow::on_text_clear_clicked()
{
    ui->textBrowser->clear();
}

/*
unsigned char* MainWindow::qstrtounchar(QString x){
    QByteArray* array=new QByteArray;
    *array=QByteArray::fromHex(x.toUtf8());
    unsigned char* res=(unsigned char*)(*array).data();
    return res;
} */

void MainWindow::on_openhost_clicked()
{
   bool flag= tcpserver->listen(QHostAddress(ui->hostip->text()),ui->hostport->text().toUInt());
   if(flag){
       ui->server_status->setStyleSheet("font-size:20px;color:yellow");
       ui->server_status->setText("IDLE");
       ui->closehost->setEnabled(true);
       ui->openhost->setEnabled(false);
   }
}


void MainWindow::statechanged_1(QAbstractSocket::SocketState state)
{
    QTcpSocket *TMPTCP=(QTcpSocket *)sender();
    QString current_socket;
    switch(state){
        case QAbstractSocket::UnconnectedState:
        ser_con_num=ser_con_num-1;

        if(ser_con_num==0){
            ui->server_status->setStyleSheet("font-size:20px;color:yellow");
            ui->server_status->setText("IDLE");
            ui->tcp_ser_send->setEnabled(false);
        }
        else {
        ui->server_status->setStyleSheet("font-size:20px;color:green");
        ui->server_status->setText(QString::number(ser_con_num)+" devices connected");
    }
        TMPTCP->deleteLater();
        current_socket= TMPTCP->peerAddress().toString()+":"+QString::number(TMPTCP->peerPort());
        ui->comboBox->removeItem(ui->comboBox->findText(current_socket));
        break;

        case QAbstractSocket::ConnectedState:
        break;

        default:break;
    }
}


void MainWindow::receivemessage_1()
{
    QTcpSocket *TMPTCP=(QTcpSocket *)sender();

    ui->textBrowser->setTextColor(Qt::blue);
    if(ui->text_hex->isChecked()){
    ui->textBrowser->append(TMPTCP->readAll().toHex());
    }
    else
    {ui->textBrowser->append(TMPTCP->readAll());}

}

void MainWindow::newconnect()
{
    //connect to client
    QTcpSocket *tcpsocket1= tcpserver->nextPendingConnection();
    QString ipaddr =tcpsocket1->peerAddress().toString();
    quint16 port=tcpsocket1->peerPort();
    ui->comboBox->addItem(ipaddr+":"+QString::number(port));
    ser_con_num=ser_con_num+1;
    ui->server_status->setStyleSheet("font-size:20px;color:green");
    ui->server_status->setText(QString::number(ser_con_num)+" devices connected");
    ui->tcp_ser_send->setEnabled(true);
    connect(tcpsocket1,SIGNAL(readyRead()),this,SLOT(receivemessage_1()));
    connect(tcpsocket1,SIGNAL(stateChanged(QAbstractSocket::SocketState)),this,SLOT(statechanged_1(QAbstractSocket::SocketState)));
}


void MainWindow::on_closehost_clicked()
{
    QList< QTcpSocket *> socketlist =tcpserver->findChildren<QTcpSocket*>();
    foreach( QTcpSocket *TMPTCP,socketlist){
       TMPTCP->close() ;
    }

    tcpserver->close();
    ui->server_status->setText("");
    ui->tcp_ser_send->setEnabled(false);
    ui->closehost->setEnabled(false);
    ui->openhost->setEnabled(true);
}


void MainWindow::on_tcp_ser_send_clicked()
{
    QList< QTcpSocket *> socketlist =tcpserver->findChildren<QTcpSocket*>();
    QString current_socket;
    foreach( QTcpSocket *TMPTCP,socketlist){
        current_socket= TMPTCP->peerAddress().toString()+":"+QString::number(TMPTCP->peerPort());
        if (ui->comboBox->itemText(ui->comboBox->currentIndex())==current_socket){
            if(ui->ser_hex->isChecked())
            {TMPTCP->write(QByteArray::fromHex(ui->tcp_ser_text->toPlainText().toUtf8()));}
            else {TMPTCP->write(ui->tcp_ser_text->toPlainText().toUtf8());}
    }}
}


void MainWindow::on_path_clicked()
{
    QString textfile=QFileDialog::getExistingDirectory(this,"choose the directory to save data",".");
    ui->path_line->setText(textfile);
}



void MainWindow::on_save_clicked()
{
    QTime timeNow = QTime::currentTime();
    QDateTime dateNow = QDateTime::currentDateTime();
    QString dir = ui->path_line->text();
    QString datafile = dir + "/" +dateNow.toString("MM-dd-")+timeNow.toString("hh-mm-ss")+".txt";
    QFile file(datafile);

    if(!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::critical(this," ","OPEN FILE WRONG");
        return;
    }
    file.write(ui->textBrowser->toPlainText().toUtf8());

    file.close();

}


void MainWindow::on_udpbind_clicked()
{
    bool flag=udpsocket->bind(ui->bindport->text().toUInt());
    if(flag) {
    ui->udp_status->setStyleSheet("font-size:20px;color:green");
    ui->udp_status->setText("BINDED");
    ui->udp_send->setEnabled(true);
    ui->udpbind->setEnabled(false);
    ui->udpabort->setEnabled(true);

    }

}

void MainWindow::readPendingDatagrams(){
    QByteArray tmp_array;
    QHostAddress ipaddr;
    quint16 port;
    ui->textBrowser->setTextColor(Qt::green);

    while (udpsocket->hasPendingDatagrams()) {
        tmp_array.resize(udpsocket->pendingDatagramSize());
        udpsocket->readDatagram(tmp_array.data(),tmp_array.size(),&ipaddr,&port);
        ui->textBrowser->append("from udp  "+ipaddr.toString()+":"+QString::number(port));
        if(ui->text_hex->isChecked()){
            ui->textBrowser->append(tmp_array.toHex());
        }
        else
        {ui->textBrowser->append(tmp_array);}
    }

};


void MainWindow::on_udp_send_clicked()
{
    if(ui->udphex->isChecked()){
    udpsocket->writeDatagram(QByteArray::fromHex(ui->udp_text->toPlainText().toUtf8()),QHostAddress(ui->udp_sendip->text()),ui->udp_sendport->text().toUInt());
    }
    else {
    udpsocket->writeDatagram(ui->udp_text->toPlainText().toUtf8(),QHostAddress(ui->udp_sendip->text()),ui->udp_sendport->text().toUInt());
    }
}


void MainWindow::on_udpabort_clicked()
{
    udpsocket->abort();
    ui->udp_status->setText("");
    ui->udp_send->setEnabled(false);
    ui->udpbind->setEnabled(true);
    ui->udpabort->setEnabled(false);

}

