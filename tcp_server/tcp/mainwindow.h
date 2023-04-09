#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>

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
    QTcpServer *tcpserver;



private slots:
    void newconnect();
    void receivemessage();
    void statechanged(QAbstractSocket::SocketState);

    void on_SEND_clicked();
    void on_start_clicked();
    void on_stop_clicked();
};
#endif // MAINWINDOW_H
