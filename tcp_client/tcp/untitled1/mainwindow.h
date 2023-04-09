#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include<QHostAddress>
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

private slots:
    void receivemessage();
    void statechanged(QAbstractSocket::SocketState);
    void on_send_clicked();
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
};
#endif // MAINWINDOW_H
