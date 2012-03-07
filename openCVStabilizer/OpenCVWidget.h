#ifndef OPENCVWIDGET_H
#define OPENCVWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QDebug>
#include <QFileDialog>
#include <QMouseEvent>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QCheckBox>
#include <QSlider>

#include "core/core.hpp"
#include "highgui/highgui.hpp"
#include "../videoStabilizer.h"
#include "imgproc/imgproc.hpp"
#include "OverlayData.h"

namespace Ui {
    class OpenCVWidget;
}

class OpenCVWidget : public QWidget
{
    Q_OBJECT

public:
    explicit OpenCVWidget(QWidget *parent = 0);
    ~OpenCVWidget();

private:
    Ui::OpenCVWidget *ui;

    QPushButton* btPlay;
    QPushButton* btStop;
    QPushButton* btFile;
    QPushButton* btRTSP;
    QPushButton* btRecord;
    QLabel* lbTitle;

    QCheckBox* cxStabilizer;
    QCheckBox* cxFilter;
    QSlider* slSizeKernel;

public slots:
    void showCaptureImage(QImage img);
};

#endif // OPENCVWIDGET_H
