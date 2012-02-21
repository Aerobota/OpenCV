#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <core/core.hpp>
#include <imgproc/imgproc.hpp>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    video(NULL),
    frameTimer(new QTimer(this)),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
    capture.release();
}

void MainWindow::on_btSelect_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,
                                            "Open Video",
                                            "Video Files (*.mp4, *.mpg)");

    frameTimer->stop();
    capture.release();

    capture.open(filename.toAscii().data());


    connect(frameTimer, SIGNAL(timeout()), this, SLOT(on_frameTimer_timeout()));
}

void MainWindow::on_btPlay_clicked()
{
    QRect imageSize;
    imageSize.setWidth(capture.get(CV_CAP_PROP_FRAME_WIDTH));
    imageSize.setHeight(capture.get(CV_CAP_PROP_FRAME_HEIGHT));

    if (video == NULL )
    {
        video = new videoStabilizer(imageSize);
        connect(video,SIGNAL(gotDuration(double&)), this, SLOT(updateTimeLabel(double&)));
    }

    double frameRate = capture.get(CV_CAP_PROP_FPS);

    frameTimer->setInterval(33);
    frameTimer->start();
}

void MainWindow::on_frameTimer_timeout(){
    cv::Mat frame;

    // read next frame if any
    if (!capture.read(frame)){
        frameTimer->stop();
        return;
    }

    cv::cvtColor(frame,frame, CV_BGR2GRAY);


    cv::Mat output = cv::Mat::zeros(frame.rows, frame.cols, CV_8UC1);

    video->stabilizeImage(frame,output);

    cv::cvtColor(output,output, CV_GRAY2RGB);

    QImage img = QImage((const unsigned char*)(output.data),
                        output.cols,
                        output.rows,
                        QImage::Format_RGB888);

    ui->lbImage->setPixmap(QPixmap::fromImage(img));
    ui->lbImage->resize(ui->lbImage->pixmap()->size());

}

void MainWindow::on_btPause_clicked()
{
    frameTimer->stop();
}

void MainWindow::updateTimeLabel(double &timeInMs){
    ui->lbTime->setText(QString::number(timeInMs));
}
