#ifndef OPENCVWIDGET_H
#define OPENCVWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QDebug>
#include <QFileDialog>
#include <QMouseEvent>

#include "core/core.hpp"
#include "highgui/highgui.hpp"
#include "../videoStabilizer.h"
#include "imgproc/imgproc.hpp"

namespace Ui {
    class OpenCVWidget;
}

class OpenCVWidget : public QWidget
{
    Q_OBJECT

public:
    explicit OpenCVWidget(QWidget *parent = 0);
    ~OpenCVWidget();

    void mousePressEvent(QMouseEvent *event);

private:
    Ui::OpenCVWidget *ui;
    cv::VideoCapture captureRTSP;
    cv::VideoWriter writerMovie;
    QTimer* mytimer;
    /** This is the video stabilizer algorithm class*/
    videoStabilizer* video;
    int x, y;

public slots:
    void playRTSP();
    void playFile();
    void timerRTSP();
    void setURL(QString url);
    void playMovie();
    void stopMovie();
};

#endif // OPENCVWIDGET_H
