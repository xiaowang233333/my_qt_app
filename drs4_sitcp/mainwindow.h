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

private slots:
    void on_tcp_connect_clicked();

    void on_open_file_clicked();

    void on_text_clear_clicked();

    void on_tcp_disconnect_clicked();

    void statechanged(QAbstractSocket::SocketState);

    void receivemessage();

    void on_send_data_clicked();

    void on_bind_udp_clicked();

    void on_abort_udp_clicked();

    void readPendingDatagrams();

    void on_create_file_clicked();

    void on_head_send_clicked();

    void on_ADC_CFG_clicked();

    void on_DAC_CFG_clicked();

    void on_trigger_cfg_clicked();

    void on_soft_reset_clicked();

    void on_trigger_soft_clicked();

    void on_drs_cfg_clicked();

private:
    Ui::MainWindow *ui;
    QTcpSocket *tcpsocket;
    QUdpSocket *udpsocket;

    QLabel* statuslabel;
    QFile file;



};
#endif // MAINWINDOW_H
