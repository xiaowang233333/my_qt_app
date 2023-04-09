#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qcustomplot.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void paintEvent(QPaintEvent*);

    QLabel* statuslabel;

    QLabel* statuslabel2;

private slots:
    void on_btn_datafile_clicked();

    void on_btn_pllfile_clicked();

    void on_ASICFile_clicked();

    void on_plot_clicked();

    void on_refresh_clicked();

    void on_waveFile_clicked();



    void on_btn_usb_clicked();

    void on_connect_usb_clicked();


    void on_in_list_currentTextChanged(const QString &arg1);

    void on_outlist_currentTextChanged(const QString &arg1);

    void on_btn_pll_clicked();

    unsigned char* qstrtounchar(QString x);

    void on_btn_dac_clicked();

    void on_btn_send_clicked();

    void on_btn_sendfile_clicked();

    void on_start_acq_clicked();

//    void on_UPLOAD_clicked();

    void on_reset_clicked();

    void on_btn_asic_clicked();

    void on_pll_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
