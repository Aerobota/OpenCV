#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <core/core.hpp>
#include <highgui/highgui.hpp>
#include "../videoStabilizer.h"
#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void on_btSelect_clicked();

    void on_btPlay_clicked();

    void on_frameTimer_timeout();


    void on_btPause_clicked();

    void updateTimeLabel(double &timeInMs);

private:
    Ui::MainWindow *ui;

    /** This data member holds the OpenCV Video capture*/
    cv::VideoCapture capture;

    /** This is the video stabilizer algorithm class*/
    videoStabilizer* video;

    /** This timer gets called based on the video framerate and sets up the
        the frame processing*/
    QTimer* frameTimer;


};

#endif // MAINWINDOW_H
