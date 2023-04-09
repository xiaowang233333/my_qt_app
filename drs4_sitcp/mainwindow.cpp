#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    statuslabel=new QLabel("drs_test_upper  ---designed by wangxuan");
    statuslabel->setStyleSheet("color:gray");
    ui->statusbar->addWidget(statuslabel);

     tcpsocket=new  QTcpSocket(this);
     connect(tcpsocket,SIGNAL(readyRead()),this,SLOT(receivemessage()));
     connect(tcpsocket,SIGNAL(stateChanged(QAbstractSocket::SocketState)),this,SLOT(statechanged(QAbstractSocket::SocketState)));

     udpsocket = new QUdpSocket(this);
     connect(udpsocket, &QUdpSocket::readyRead,this, &MainWindow::readPendingDatagrams);

     QPixmap logo1=QPixmap(":/pic/logo.png");
     ui->logo->setPixmap(logo1);
     ui->logo->setScaledContents(true);

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_tcp_connect_clicked()
{
     tcpsocket->connectToHost(QHostAddress(ui->server_ip->text()),ui->server_port->text().toUInt());
}


void MainWindow::on_open_file_clicked()
{
    QString textfile=QFileDialog::getExistingDirectory(this,"choose the directory to save data",".");
    ui->file_path->setText(textfile);

}


void MainWindow::on_text_clear_clicked()
{
    ui->textBrowser->clear();
}


void MainWindow::on_tcp_disconnect_clicked()
{
    tcpsocket->disconnectFromHost();
}

void MainWindow::statechanged(QAbstractSocket::SocketState state)
{
    switch (state) {
        case QAbstractSocket::UnconnectedState:
        ui->client_status->setStyleSheet("font-size:15px;color:blue");
        ui->client_status->setText("TCP_NO_CONNECTED");
        ui->tcp_disconnect->setEnabled(false);
        ui->tcp_connect->setEnabled(true);
               break;
        case QAbstractSocket::ConnectedState:
        ui->client_status->setStyleSheet("font-size:15px;color:green");
        ui->client_status->setText("TCP_CONNECTED");
        ui->tcp_disconnect->setEnabled(true);
        ui->tcp_connect->setEnabled(false);
               break;

    default:break;
    }
}



void MainWindow::receivemessage()
{

    ui->textBrowser->setTextColor(Qt::green);
    QByteArray received_bytes;
    received_bytes = tcpsocket->readAll();
    ui->textBrowser->append(received_bytes.toHex());

    if(file.exists()){
            if(!file.open(QIODevice::Append))
            {    QMessageBox::critical(this," ","OPEN FILE WRONG");return;  }
            else {
                file.write(received_bytes);
                file.close();   }
    }
}


void MainWindow::on_send_data_clicked()
{
    QString udp_data;
    udp_data ="FF801201000001"+ui->rbcp_addr->text()+ui->cmd_tobesent->text();
    udpsocket->writeDatagram(QByteArray::fromHex(udp_data.toUtf8()),QHostAddress(ui->server_ip->text()),ui->udp_port->text().toUInt());

}


void MainWindow::on_bind_udp_clicked()
{
    bool flag=udpsocket->bind(1000);
    if(flag) {
    ui->udp_status->setStyleSheet("font-size:15px;color:green");
    ui->udp_status->setText("UDP_BINDED");
    ui->abort_udp->setEnabled(true);
    ui->bind_udp->setEnabled(false);
    ui->send_data->setEnabled(true);
    }
}


void MainWindow::on_abort_udp_clicked()
{
    udpsocket->abort();
    ui->udp_status->setStyleSheet("font-size:15px;color:blue");
    ui->udp_status->setText("UDP_NO_BINDED");
    ui->abort_udp->setEnabled(false);
    ui->bind_udp->setEnabled(true);
    ui->send_data->setEnabled(false);
}

void MainWindow::readPendingDatagrams(){
    QByteArray tmp_array;
    QHostAddress ipaddr;
    quint16 port;
    tmp_array.resize(udpsocket->pendingDatagramSize());
    udpsocket->readDatagram(tmp_array.data(),tmp_array.size(),&ipaddr,&port);
  //  QString udp_temp=tmp_array.toHex();

//    if(udp_temp.mid(3,1)=="8")
//    ui->textBrowser->setTextColor(Qt::blue);
//    ui->textBrowser->append("cmd--"+ui->cmd_tobesent->text()+"--send successfully");
};


void MainWindow::on_create_file_clicked()
{
    QTime timeNow = QTime::currentTime();
    QDateTime dateNow = QDateTime::currentDateTime();
    QString dir = ui->file_path->text();
    QString datafile = dir + "/" +dateNow.toString("MM-dd-")+timeNow.toString("hh-mm-ss")+".bin";
    file.setFileName(datafile);
    if(!file.open(QIODevice::WriteOnly))
    { QMessageBox::critical(this," ","CREATE FILE WRONG");  return;  }
    else {     file.close();   }
}


void MainWindow::on_head_send_clicked()
{
    QString head_data;
    head_data ="FF80120400000103"+ui->head_info->text();
    udpsocket->writeDatagram(QByteArray::fromHex(head_data.toUtf8()),QHostAddress(ui->server_ip->text()),ui->udp_port->text().toUInt());
}




void MainWindow::on_ADC_CFG_clicked()
{
    QString adc_data;
    adc_data ="FF8012030000010d"+ui->adc_cfg_data->text();
    udpsocket->writeDatagram(QByteArray::fromHex(adc_data.toUtf8()),QHostAddress(ui->server_ip->text()),ui->udp_port->text().toUInt());
}


void MainWindow::on_DAC_CFG_clicked()
{
    QString dac_data;
    dac_data ="FF80120300000110"+ui->dac_cfg_data->text();
    udpsocket->writeDatagram(QByteArray::fromHex(dac_data.toUtf8()),QHostAddress(ui->server_ip->text()),ui->udp_port->text().toUInt());
}




void MainWindow::on_trigger_cfg_clicked()
{
    QString trig_enable;
    bool ok;
    int index = ui->TRIGGER_SEL->currentIndex();
    QString trig_data= QString::number( ui->trigger_data->text().toInt(&ok,2) ,16);
     QVector <QString> list1;
     list1 <<"0"<<"1"<<"2"<<"3";
    trig_enable = "FF8012010000010c0"+list1[index];
    QString sel_num;
    sel_num ="FF80120400000108"+ui->self_trig_num->text();
    QString trigger_en;
    trigger_en ="FF801201000001130"+trig_data;
    udpsocket->writeDatagram(QByteArray::fromHex(sel_num.toUtf8()),QHostAddress(ui->server_ip->text()),ui->udp_port->text().toUInt());
    udpsocket->writeDatagram(QByteArray::fromHex(trig_enable.toUtf8()),QHostAddress(ui->server_ip->text()),ui->udp_port->text().toUInt());
    udpsocket->writeDatagram(QByteArray::fromHex(trigger_en.toUtf8()),QHostAddress(ui->server_ip->text()),ui->udp_port->text().toUInt());
}


void MainWindow::on_soft_reset_clicked()
{
    QString softrst;
    softrst ="FF8012010000010100";
    udpsocket->writeDatagram(QByteArray::fromHex(softrst.toUtf8()),QHostAddress(ui->server_ip->text()),ui->udp_port->text().toUInt());
}


void MainWindow::on_trigger_soft_clicked()
{
    QString softtrig;
    softtrig ="FF8012010000010700";
    udpsocket->writeDatagram(QByteArray::fromHex(softtrig.toUtf8()),QHostAddress(ui->server_ip->text()),ui->udp_port->text().toUInt());
}




void MainWindow::on_drs_cfg_clicked()
{
    int index = ui->drs_sel->currentIndex();
    QVector <QString> list1;
    list1 <<"14"<<"16"<<"18"<<"FF";
    QString drs_data;
    drs_data ="FF801202000001"+list1[index]+ui->drs_cfg_data->text()+"0";
    udpsocket->writeDatagram(QByteArray::fromHex(drs_data.toUtf8()),QHostAddress(ui->server_ip->text()),ui->udp_port->text().toUInt());
}

