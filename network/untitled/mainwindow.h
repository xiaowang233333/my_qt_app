#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTcpServer>
#include <QHostAddress>
#include <QString>
#include<QByteArray>
#include<QFileDialog>
#include<QFile>
#include <QTime>
#include <QDateTime>
#include<QMessageBox>
#include<QUdpSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QTcpSocket *tcpsocket;
    QTcpServer *tcpserver;
    QUdpSocket *udpsocket;
    quint16 ser_con_num=0;
    QLabel* statuslabel;

private slots:
    void receivemessage();
    void statechanged(QAbstractSocket::SocketState);

    void on_tcp_cli_con_clicked();
    void on_tcp_cli_dis_clicked();
    void on_tcp_cli_send_clicked();
    void on_text_clear_clicked();


   // unsigned char* qstrtounchar(QString x);
    void on_openhost_clicked();

    void newconnect();
    void receivemessage_1();
    void statechanged_1(QAbstractSocket::SocketState);
    void on_closehost_clicked();
    void on_tcp_ser_send_clicked();
    void on_path_clicked();
    void on_save_clicked();
    void on_udpbind_clicked();


    void readPendingDatagrams();
    void on_udp_send_clicked();
    void on_udpabort_clicked();
};
#endif // MAINWINDOW_H
