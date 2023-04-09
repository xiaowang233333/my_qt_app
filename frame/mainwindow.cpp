#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qcustomplot.h"
#include <QPainter>
#include <QPixmap>
#include <QString>
#include <QFileDialog>
#include <QVector>
#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <windows.h>
#include <CyAPI.h>
#include <QTime>
#include <QDateTime>
#include <QThread>


#define N 128
QVector<double> plotdata(N);
QVector<double> xnum(N);
CCyUSBDevice *myusb=NULL;
CCyBulkEndPoint *BulkInEpt;
CCyBulkEndPoint *BulkOutEpt;
QByteArray data;
int usb_count=0;
QByteArray rec_data;
QString file_temp;
int acqway=0;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //set background and transparency
    ui->waveform->axisRect()->setBackgroundScaled(true);//启用背景缩放
    ui->waveform->axisRect()->setBackgroundScaledMode(Qt::AspectRatioMode::IgnoreAspectRatio);//自由缩放
    QPixmap backg=QPixmap(":/1.jpg");
    QPixmap temp(backg.size());
    temp.fill(Qt::transparent);
    QPainter p1(&temp);
    p1.setCompositionMode(QPainter::CompositionMode_Source);
    p1.drawPixmap(0,0,backg);
    p1.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    p1.fillRect(temp.rect(),QColor(0,0,0,100));
    p1.end();
    ui->waveform->axisRect()->setBackground(temp);
    ui->waveform->xAxis2->setVisible(false);
    ui->waveform->yAxis2->setVisible(false);
    //set status
    statuslabel=new QLabel("idle");
    statuslabel->setStyleSheet("color:green");
    ui->statusbar->addWidget(statuslabel);
    statuslabel2=new QLabel("LAST OP:idle");
    statuslabel2->setStyleSheet("color:blue");
    ui->statusbar->addWidget(statuslabel2);
    //set xnum
    for(int i=0;i<N;i++){
        xnum[i]=i+1;
    }
}

void MainWindow::paintEvent(QPaintEvent*a){
    QPixmap logo=QPixmap(":/2.png");
    QPainter p2(this);
    p2.drawPixmap(0,-90,logo.scaled(800,400,Qt::KeepAspectRatio));

};

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_btn_datafile_clicked()
{
    QString datafile=QFileDialog::getOpenFileName(this,"select one data file",".");
    ui->dataPath->setText(datafile);
}



void MainWindow::on_btn_pllfile_clicked()
{
    QString pllfile=QFileDialog::getOpenFileName(this,"select one pll file",".");
    ui->pllPath->setText(pllfile);

}



void MainWindow::on_ASICFile_clicked()
{
    QString asicfile=QFileDialog::getOpenFileName(this,"select one asic file",".");
    ui->ASICPath->setText(asicfile);
}


void MainWindow::on_plot_clicked()
{
    statuslabel->setText("busy...");
    statuslabel->setStyleSheet("color:red");
    QFile file(file_temp);
     //QFile file("D:/Backup/Downloads/65dB.bin");
    if(!file.open(QIODevice::ReadOnly))
    {
        statuslabel->setText("idle");
        statuslabel->setStyleSheet("color:green");
        return;
    }
    rec_data=file.readAll();
    file.close();
    //acqway=0;
    int xx=rec_data.indexOf(0x018d);
   // qDebug()<<QString::number(xx,10);
    if(acqway==0){
    quint8 data_t[2*N];
    for(int j=0;j<N;j++){
        data_t[2*j]=rec_data[4*j+8+xx];
        data_t[2*j+1]=rec_data[4*j+10+xx];
    }
    quint16* data_f;
    data_f=(quint16*)data_t;
    for(int i=0;i<N;i++){
        plotdata[i]=*(data_f+i);
    }}
    else{
        quint8 data_t[4*N];
        for(int j=0;j<N;j++){
            data_t[4*j]=rec_data[8*j+8+xx];
            data_t[4*j+1]=rec_data[8*j+xx+10];
            data_t[4*j+2]=rec_data[8*j+12+xx];
            data_t[4*j+3]=rec_data[8*j+14+xx];
        }
        quint32* data_f;
        data_f=(quint32*)data_t;
        for(int i=0;i<N;i++){
            plotdata[i]=*(data_f+i);
        }
    }
    ui->waveform->clearGraphs();
    ui->waveform->addGraph();
    ui->waveform->graph(0)->setPen(QPen(Qt::red));
    ui->waveform->graph(0)->setBrush(QBrush(QColor(0,0,255,20)));
    ui->waveform->graph(0)->setData(xnum,plotdata);
    ui->waveform->graph(0)->rescaleAxes();
    ui->waveform->replot();
    ui->plot->setEnabled(false);
    statuslabel2->setText("LAST OP:plot data");
    statuslabel->setText("idle");
    statuslabel->setStyleSheet("color:green");

}


void MainWindow::on_refresh_clicked()
{
//    for(int i=0;i<N;i++){
//        plotdata[i]=plotdata[i]+2;
//    }
//    ui->waveform->replot();
}


void MainWindow::on_waveFile_clicked()
{
    QString wavefile=QFileDialog::getExistingDirectory(this,"choose the directory to save data",".");
    ui->wavepath->setText(wavefile);
}





void MainWindow::on_btn_usb_clicked()
{
    statuslabel->setText("busy...");
    statuslabel->setStyleSheet("color:red");
    myusb=new CCyUSBDevice(NULL);
    usb_count=myusb->DeviceCount();
    if (myusb == NULL||usb_count==0)    //判断是否有USB设备连接
        {
        QMessageBox::critical(this," ","No Device");

        }
    for(int j=0;j<usb_count;j++){
        QString vid = QString::number(myusb->VendorID,16);
        QString pid = QString::number(myusb->ProductID,16);
        QString devicename =myusb->DeviceName;
        QString usbitem =devicename+" vid:"+vid+" pid:"+pid;
        ui->usb_device->addItem(usbitem);
    }
    statuslabel2->setText("LAST OP:LIST USB ");
    statuslabel->setText("idle");
    statuslabel->setStyleSheet("color:green");
}




void MainWindow::on_connect_usb_clicked()
{
    statuslabel->setText("busy...");
    statuslabel->setStyleSheet("color:red");
    ui->in_list->clear();
    ui->outlist->clear();

    if (myusb == NULL||usb_count==0)    //判断是否有USB设备连接
    {
        QMessageBox::critical(this," ","No Device");
        statuslabel->setText("idle");
        statuslabel->setStyleSheet("color:green");
        return;
    }
    int index=ui->usb_device->currentIndex();
    myusb->Open(index);
    int ept_count=0;
    ept_count=myusb->EndPointCount();


    if (ept_count<=1){
        QMessageBox::critical(this," ","No avaliable endpoint");
    }
    else{
        CCyBulkEndPoint * tempout;
        CCyBulkEndPoint *tempin;
        int inx=0;
        int outx=0;
        for(int k=1;k<ept_count;k++){
            bool in=myusb->EndPoints[k]->bIn;
            if(!in){
                BulkOutEpt=(CCyBulkEndPoint *)myusb->EndPoints[k];
                QString outept = QString::number(myusb->EndPoints[k]->Address,16);
                ui->outlist->addItem(outept);
                if(!outx){tempout=BulkOutEpt;}
                outx++;
            }
            if(in){
                BulkInEpt=(CCyBulkEndPoint *)myusb->EndPoints[k];
                QString inept = QString::number(myusb->EndPoints[k]->Address,16);
                ui->in_list->addItem(inept);
                if(!inx){tempin=BulkInEpt;}
                inx++;
            }
        }
        BulkOutEpt=tempout;
        BulkInEpt=tempin;
    }
    statuslabel2->setText("LAST OP:connect USB ");
       statuslabel->setText("idle");
       statuslabel->setStyleSheet("color:green");
}




void MainWindow::on_in_list_currentTextChanged(const QString &arg1)
{
    if(myusb==NULL){}
    else{
        int ept_count;
        ept_count=myusb->EndPointCount();
        for(int k=1;k<ept_count;k++){
            bool p=QString::number(myusb->EndPoints[k]->Address,16)==arg1;
            if(p)
                BulkInEpt=(CCyBulkEndPoint *)myusb->EndPoints[k];
        }
    }
}


void MainWindow::on_outlist_currentTextChanged(const QString &arg1)
{
    if(myusb==NULL){}
    else{
        int ept_count;
        ept_count=myusb->EndPointCount();
        for(int k=1;k<ept_count;k++){
            bool p=QString::number(myusb->EndPoints[k]->Address,16)==arg1;
            if(p)
            BulkOutEpt=(CCyBulkEndPoint *)myusb->EndPoints[k];
        }
    }
}


void MainWindow::on_btn_pll_clicked()
{
    statuslabel->setText("busy...");
    statuslabel->setStyleSheet("color:red");
    QString path=ui->pllPath->text();
    QFile file(path);
    QString pll;
    QString header1="08";
    QString header2="48";
    QString header3="41";
    QByteArray buffero;
    bool flag;
    long len=20;
    if (myusb == NULL||usb_count==0)
    {
        QMessageBox::critical(this," ","No Device");
        statuslabel->setText("idle");
        statuslabel->setStyleSheet("color:green");
        return;
    }
    unsigned char* buffer=NULL;
    file.open(QIODevice::ReadOnly);
    int m;
    while(!file.atEnd()){
        QString temp=file.readLine();
        m=temp.indexOf("x");
        QString plldata=temp.mid(m+1,8);
        pll=header1+"8C"+header1+"9C"+header2+header2+header2+header3+header2+plldata.mid(0,2)+header2+plldata.mid(2,2)+header2+plldata.mid(4,2)+header2+plldata.mid(6,2)+header1+"AC"+header1+"BC";
        buffero=QByteArray::fromHex(pll.toUtf8());
        buffer=(unsigned char*)buffero.data();
        flag=BulkOutEpt->XferData(buffer,len);
        if (!flag)
        {
            QMessageBox::critical(this," ","send data wrong!");
            statuslabel->setText("idle");
            statuslabel->setStyleSheet("color:green");
            return;
        }
    }
    statuslabel2->setText("LAST OP:set PLL ");
    statuslabel->setText("idle");
    statuslabel->setStyleSheet("color:green");
}

unsigned char* MainWindow::qstrtounchar(QString x){
    QByteArray* array=new QByteArray;
    *array=QByteArray::fromHex(x.toUtf8());
    unsigned char* res=(unsigned char*)(*array).data();
    return res;
}


void MainWindow::on_btn_dac_clicked()
{
    statuslabel->setText("busy...");
    statuslabel->setStyleSheet("color:red");
//    QString value=ui->dacValue->text();
    bool* cc;
    float voltage=ui->dacValue->text().toFloat(cc);
    int value_d=voltage*1000;
    QString value= QString::number(value_d,16).rightJustified(3,'0');
//qDebug()<<ui->dacValue->text()<<value_d<<"  "<<voltage<<"  "<<value;
    int index;
    index = ui->dac_channel->currentIndex();
    QString dac;
    QString header1="10";
    QString header2="50";
    QString header3="41";
    unsigned char* buffer=NULL;
    bool flag;
    long len=20;
    if (myusb == NULL||usb_count==0)
    {
        QMessageBox::critical(this," ","No Device");
        statuslabel->setText("idle");
        statuslabel->setStyleSheet("color:green");
        return;
    }
    QString sel="89AB";
    QString channel=sel.at(index);
    dac=header1+"8C"+header1+"9C"+header2+header2+header2+header3+header2+"00"+header2+"00"+header2+channel+value.at(0)+header2+value.mid(1,2)+header1+"AC"+header1+"BC";
    buffer=qstrtounchar(dac);
//    qDebug()<<*buffer<<"    "<<dac;
    flag=BulkOutEpt->XferData(buffer,len);
    if (!flag)
    {
        QMessageBox::critical(this," ","send data wrong!");
        statuslabel->setText("idle");
        statuslabel->setStyleSheet("color:green");
        return;
    }
    statuslabel2->setText("LAST OP:set DAC ");
    statuslabel->setText("idle");
    statuslabel->setStyleSheet("color:green");
}




void MainWindow::on_btn_send_clicked()
{
    statuslabel->setText("busy...");
    statuslabel->setStyleSheet("color:red");
    if (myusb == NULL||usb_count==0)
    {
        QMessageBox::critical(this," ","No Device");
        statuslabel->setText("idle");
        statuslabel->setStyleSheet("color:green");
        return;
    }
    QString datasend=ui->dataEdit->text();
    long len=(long)(datasend.length())/2;
    //     qDebug()<<len<<datasend.length()/2<<datasend<<QString::number(BulkOutEpt->Address,16);;
    unsigned char *senddata=qstrtounchar(datasend);
    bool flag=BulkOutEpt->XferData(senddata,len);
    //    qDebug()<<len<<datasend.length()/2<<datasend<<flag;
    if (!flag)
    {
        QMessageBox::critical(this," ","send data wrong!");
        statuslabel->setText("idle");
        statuslabel->setStyleSheet("color:green");
        return;
    }
    statuslabel2->setText("LAST OP:send "+datasend);
    statuslabel->setText("idle");
    statuslabel->setStyleSheet("color:green");
}


void MainWindow::on_btn_sendfile_clicked()
{
    statuslabel->setText("busy...");
    statuslabel->setStyleSheet("color:red");
    if (myusb == NULL||usb_count==0)
    {
        QMessageBox::critical(this," ","No Device");
        statuslabel->setText("idle");
        statuslabel->setStyleSheet("color:green");
        return;
    }
    QString path=ui->dataPath->text();
    QFile file(path);
    file.open(QIODevice::ReadOnly);
    QString datasend=file.readAll();
    datasend=datasend.remove(QChar('\n')).remove(QChar('\r')).remove(QChar(' '));
    long len=datasend.length()/2;
    unsigned char *senddata=qstrtounchar(datasend);
    bool flag=BulkOutEpt->XferData(senddata,len);
    if (!flag)
    {
        QMessageBox::critical(this," ","send data wrong!");
        statuslabel->setText("idle");
        statuslabel->setStyleSheet("color:green");
        return;
    }
//    QFile file1("C:\\Users\\Administrator\\Desktop\\usb_controler\\11\\11\\o.txt");
//    file1.open(QIODevice::ReadOnly);
//    QString datasend=file1.readAll();
//    QString datasend1=datasend.remove(QChar('\n')).remove(QChar('\r')).remove(QChar(' '));
//    qDebug()<<datasend1<<"    "<<datasend;
//    file1.close();
    file.close();
    statuslabel2->setText("LAST OP:send file data");
    statuslabel->setText("idle");
    statuslabel->setStyleSheet("color:green");
}


void MainWindow::on_start_acq_clicked()
{
    statuslabel->setText("busy...");
    statuslabel->setStyleSheet("color:red");
    if (myusb == NULL||usb_count==0)
    {
        QMessageBox::critical(this," ","No Device");
        statuslabel->setText("idle");
        statuslabel->setStyleSheet("color:green");
        return;
    }

    QTime timeNow = QTime::currentTime();
    QDateTime dateNow = QDateTime::currentDateTime();
    QString dir = ui->wavepath->text();
    QString datafile = dir + "/" +dateNow.toString("MM-dd-")+timeNow.toString("hh-mm-ss")+".bin";
    QFile file(datafile);
    file_temp=datafile;
    if(!file.open(QIODevice::WriteOnly))
    {
        statuslabel->setText("idle");
        statuslabel->setStyleSheet("color:green");
        return;
    }
    int acq_N = ui->acq_N->text().toInt();
    int index = ui->acq_channel->currentIndex();
    int acq_way = ui->acq_way->currentIndex();
    acqway=acq_way;
    QString N_str= QString::number(acq_N,16).rightJustified(2,'0');

    QVector <QString> KKlist;
    KKlist << "10"<<"01";
    QString KK =KKlist[acq_way];

    QVector <QString> header1list;
    QVector <QString> header2list;
    header1list<<"17"<<"1F"<<"27"<<"2F";
    header2list<<"57"<<"5F"<<"67"<<"6F";
    QString header1 =header1list[index];
    QString header2 =header2list[index];
    QString header3 = "41";


//    QVector <QString> objaddrlist;
//    objaddrlist <<"010111"<<"011111"<<"100111"<<"101111";
//    QString first_cmd_temp=KK+objaddrlist[index];
    QString first_cmd_temp=KK+"000001";
    int temp =first_cmd_temp.toInt(nullptr,2);
    QString first_cmd= QString::number(temp,16);
    QString acq_cmd ;
    acq_cmd=header1+"8C"+header1+"9C"+header2+header2+header2+header3+header2+first_cmd+header2+"00"+header2+"00"+header2+N_str+header1+"AC"+header1+"BC";
    QString cmd_1;
    cmd_1="0F8C0F9C4F4F4F414F004F004F804F000FAC0FBC";
    long len_1=cmd_1.length()/2;
    long len=acq_cmd.length()/2;

    unsigned char *sendcmd=qstrtounchar(cmd_1);
    bool flag=BulkOutEpt->XferData(sendcmd,len_1);
    if (!flag)
    {
        QMessageBox::critical(this," ","send command1 wrong!");
        statuslabel->setText("idle");
        statuslabel->setStyleSheet("color:green");
        return;
    }
    sendcmd=qstrtounchar(acq_cmd);
    flag=BulkOutEpt->XferData(sendcmd,len);
    if (!flag)
    {
        QMessageBox::critical(this," ","send command2 wrong!");
        statuslabel->setText("idle");
        statuslabel->setStyleSheet("color:green");
        return;
    }


    long rec_len=512;
    //rec_len=14+2*N*(2+2*acq_way);
    unsigned char *recdata= new unsigned char[rec_len];
    memset(recdata,0xEF,rec_len);
    bool rec_flag;
    qint64 fileflag;
    BulkInEpt->TimeOut=10000;
    do{
//    Sleep(10);
//    qDebug()<<"a:  "<<rec_len<<"   ";
    rec_flag =BulkInEpt->XferData(recdata,rec_len);
//    qDebug()<<"b:  "<<rec_len<<"   ";
    if (!rec_flag)
    {
        QMessageBox::critical(this," ","receive data wrong!");
        statuslabel->setText("idle");
        statuslabel->setStyleSheet("color:green");
        return;
    }

    fileflag =file.write((char*)recdata,rec_len);

    if (fileflag==-1)
    {
        QMessageBox::critical(this," ","write data to file wrong!");
        statuslabel->setText("idle");
        statuslabel->setStyleSheet("color:green");
        return;
    }}while(rec_len==512);

    file.close();
    ui->plot->setEnabled(true);
//    qDebug()<<datafile<<"    "<<header1<<"   "<<acq_cmd<<"   "<<first_cmd;
    statuslabel2->setText("LAST OP:start acq");
    statuslabel->setText("idle");
    statuslabel->setStyleSheet("color:green");
}


void MainWindow::on_reset_clicked()
{
    statuslabel->setText("busy...");
    statuslabel->setStyleSheet("color:red");
    if (myusb == NULL||usb_count==0)
    {
        QMessageBox::critical(this," ","No Device");
        statuslabel->setText("idle");
        statuslabel->setStyleSheet("color:green");
        return;
    }
    unsigned char* buffer=NULL;
    bool flag;
    long len=20;
    QString reset_cmd;
    reset_cmd="0F8C0F9C4F4F4F414F004F004F804F000FAC0FBC";
    buffer=qstrtounchar(reset_cmd);
    flag=BulkOutEpt->XferData(buffer,len);
    if (!flag)
    {
        QMessageBox::critical(this," ","send data wrong!");
        statuslabel->setText("idle");
        statuslabel->setStyleSheet("color:green");
        return;
    }
    statuslabel2->setText("LAST OP:reset asic");
    statuslabel->setText("idle");
    statuslabel->setStyleSheet("color:green");
}


void MainWindow::on_btn_asic_clicked()
{
    statuslabel->setText("busy...");
    statuslabel->setStyleSheet("color:red");

    int acc_N=ui->accN->text().toInt();
    QString value= QString::number(acc_N,16).rightJustified(4,'0');
    int index;
    index = ui->asic_channel->currentIndex();
    int flc;
    flc=ui->asic_flc->isChecked();
    int asic_M=ui->asicM->text().toInt();
    QString valueM= QString::number(asic_M,16).rightJustified(1,'0');
    QString asic_n;
    QString header1="0F8C0F9C4F4F4F414F004F004F004F000FAC0FBC0F8C0F9C4F4F4F414F004F01";
    QString header2="0FAC0FBC0F8C0F9C4F4F4F414F004F01";
    QString header3="0FAC0FBC0F8C0F9C4F4F4F414F004F01";
    QString header4="0FAC0FBC0F8C0F9C4F4F4F414F004F004F804F000FAC0FBC";
    unsigned char* buffer=NULL;
    bool flag;
    long len=100;
    if (myusb == NULL||usb_count==0)
    {
        QMessageBox::critical(this," ","No Device");
        statuslabel->setText("idle");
        statuslabel->setStyleSheet("color:green");
        return;
    }
    QString sel1="CDEF";
    QString chans1=sel1.at(index);
    QString sel2="0123";
    QString chans2=sel2.at(index);
    QString sel3="89AB";
    QString chans3=sel3.at(index);
    QString flcs="23";
    QString flcss=flcs.at(flc);
    asic_n=header1+"4F1"+chans1+"4F"+value.mid(2,2)+header2+"4F2"+chans2+"4F"+value.mid(0,2)+header3+"4F1"+chans3+"4F"+flcss+valueM+header4;
    buffer=qstrtounchar(asic_n);
    flag=BulkOutEpt->XferData(buffer,len);
    if (!flag)
    {
        QMessageBox::critical(this," ","send data wrong!");
        statuslabel->setText("idle");
        statuslabel->setStyleSheet("color:green");
        return;
    }
    statuslabel2->setText("LAST OP:set asic N ");
    statuslabel->setText("idle");
    statuslabel->setStyleSheet("color:green");
}


void MainWindow::on_pll_clicked()
{
   statuslabel->setText("busy...");
    statuslabel->setStyleSheet("color:red");
    if (myusb == NULL||usb_count==0)
    {
        QMessageBox::critical(this," ","No Device");
        statuslabel->setText("idle");
        statuslabel->setStyleSheet("color:green");
        return;
    }


    float freq=ui->pllfreq->text().toFloat();
    float pll_num;
    int plln=0.75*freq;
    pll_num=(0.75*freq-plln)*4000000;
    int pllnum=pll_num;

    QString pllN= QString::number(plln,16).rightJustified(3,'0');
    QString pllNUM= QString::number(pllnum,2).rightJustified(22,'0');
    bool ok;
    QString data_1="1100010"+pllNUM.mid(0,10)+"010000001010001";
    QString data1=QString::number(data_1.mid(0,16).toInt(&ok,2),16).rightJustified(4,'0')+QString::number(data_1.mid(16,16).toInt(&ok,2),16).rightJustified(4,'0');
    QString data2="4"+pllN+QString::number(pllNUM.mid(10,12).toInt(&ok,2),16).rightJustified(3,'0')+"0";

    QString datapll="021FE80F4082C10D210050CA03C7C039207DDBF800082317000004C60030E805000000042008F3E30FD09002"+data1+data2+data2;
    QString header1="08";
    QString header2="48";
    QString header3="41";
    QString pll;
    QString plldata;

    //qDebug()<<datapll<<QString::number(data_1.toInt(&ok,2),16)<<pllN<<freq<<"  "<<plln;
    bool flag;
    unsigned char *buffer;
    long len=20;
    int ll=datapll.length()/8;
    for(int tem=0;tem<ll;tem++){
    plldata=datapll.mid(8*tem,8);
    pll=header1+"8C"+header1+"9C"+header2+header2+header2+header3+header2+plldata.mid(0,2)+header2+plldata.mid(2,2)+header2+plldata.mid(4,2)+header2+plldata.mid(6,2)+header1+"AC"+header1+"BC";
    buffer=qstrtounchar(pll);
    flag=BulkOutEpt->XferData(buffer,len);
    if (!flag)
    {
        QMessageBox::critical(this," ","send data wrong!");
        statuslabel->setText("idle");
        statuslabel->setStyleSheet("color:green");
        return;
    }
    }
    statuslabel2->setText("LAST OP:config pll freq ");
    statuslabel->setText("idle");
    statuslabel->setStyleSheet("color:green");
}

