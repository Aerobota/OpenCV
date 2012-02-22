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

    connect(ui->btRTSP, SIGNAL(clicked()), this, SLOT(playRTSP()));
    connect(ui->btRecordRTSP, SIGNAL(clicked()), this, SLOT(recordRTSP()));
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

    qDebug()<<"Width: "<< imageSize.width();
    qDebug()<<"Height: "<< imageSize.height();

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

void MainWindow::playRTSP()
{
    //double frameRate = capture.get(CV_CAP_PROP_FPS);
    const std::string videoStreamAddress = "rtsp:192.168.1.90:554/axis-media/media.amp";
    /* it may be an address of an mjpeg stream,
            e.g. "http://user:pass@cam_address:8081/cgi/mjpg/mjpg.cgi?.mjpg" */

    //open the video stream and make sure it's opened



    if(!captureRTSP.open(videoStreamAddress))
    {
        qDebug() << "Error opening video stream or file";
        return;
    }






    mytimer = new QTimer(this);
    mytimer->setInterval(34);
    connect(mytimer, SIGNAL(timeout()), this, SLOT(timerRTSP()));
    mytimer->start();
}

void MainWindow::timerRTSP()
{
    cv::Mat frame;

    if(!captureRTSP.read(frame))
    {
        qDebug()  << "No frame" ;
        cv::waitKey();
    }

    //cv::imshow("Output Window", frame);//show image
    if (video == NULL )
    {
        QRect imageSize;
        imageSize.setWidth(captureRTSP.get(CV_CAP_PROP_FRAME_WIDTH));
        imageSize.setHeight(captureRTSP.get(CV_CAP_PROP_FRAME_HEIGHT));

        qDebug()<<"width: "<< captureRTSP.get(CV_CAP_PROP_FRAME_WIDTH);
        qDebug()<<"height: "<< captureRTSP.get(CV_CAP_PROP_FRAME_HEIGHT);

        video = new videoStabilizer(imageSize);
        connect(video,SIGNAL(gotDuration(double&)), this, SLOT(updateTimeLabel(double&)));

        //writerMovie.open("/movie.avi", CV_FOURCC('D','I','V','X'), 30, frame.size(), true);
    }

    QImage imgo = QImage((const unsigned char*)(frame.data),
                        frame.cols,
                        frame.rows,
                        QImage::Format_RGB888);

    ui->lbImageOrigin->setPixmap(QPixmap::fromImage(imgo));
    ui->lbImageOrigin->resize(ui->lbImageOrigin->pixmap()->size());

    cv::cvtColor(frame,frame, CV_BGR2GRAY);
    cv::Mat output = cv::Mat::zeros(frame.rows, frame.cols, CV_8UC1);
    video->stabilizeImage(frame,output);
    cv::cvtColor(output,output, CV_GRAY2RGB);

    //writerMovie << output;

    QImage img = QImage((const unsigned char*)(output.data),
                        output.cols,
                        output.rows,
                        QImage::Format_RGB888);

    ui->lbImage->setPixmap(QPixmap::fromImage(img));
    ui->lbImage->resize(ui->lbImage->pixmap()->size());
}
