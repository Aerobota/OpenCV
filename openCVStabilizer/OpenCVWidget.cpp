#include "OpenCVWidget.h"
#include "ui_OpenCVWidget.h"

OpenCVWidget::OpenCVWidget(QWidget *parent) :
        QWidget(parent),
        video(NULL),
        ui(new Ui::OpenCVWidget)
{
    ui->setupUi(this);

    connect(ui->btPlay, SIGNAL(clicked()), this, SLOT(playMovie()));
    connect(ui->btStop, SIGNAL(clicked()), this, SLOT(stopMovie()));
    connect(ui->btRTSP, SIGNAL(clicked()), this, SLOT(playRTSP()));
    connect(ui->btFile, SIGNAL(clicked()), this, SLOT(playFile()));

    mytimer = new QTimer(this);
    mytimer->setInterval(34);
    connect(mytimer, SIGNAL(timeout()), this, SLOT(timerRTSP()));

    setWindowTitle("Video");
}

OpenCVWidget::~OpenCVWidget()
{
    delete ui;
}

void OpenCVWidget::playFile()
{
    QString filename = QFileDialog::getOpenFileName(this, "Open Video", "Video Files (*.mp4, *.mpg)");

    if(!filename.isEmpty())
    {
        setURL(filename);
    }
}

void OpenCVWidget::playRTSP()
{
    QString videoStreamAddress = "rtsp:192.168.1.90:554/axis-media/media.amp";

    setURL(videoStreamAddress);
}

void OpenCVWidget::setURL(QString url)
{
    captureRTSP.release();

    if(!captureRTSP.open(url.toAscii().data()))
    {
        qDebug() << "Error opening video stream or file";
        return;
    }

    ui->lbTitle->setText(url);
}

void OpenCVWidget::playMovie()
{
    if(!mytimer->isActive())
        mytimer->start();
}

void OpenCVWidget::stopMovie()
{
    if(mytimer->isActive())
        mytimer->stop();
}

void OpenCVWidget::timerRTSP()
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

    cv::cvtColor(frame,frame, CV_BGR2GRAY);
    cv::Mat output = cv::Mat::zeros(frame.rows, frame.cols, CV_8UC1);
    video->stabilizeImage(frame,output);
    cv::cvtColor(output,output, CV_GRAY2RGB);

    //writerMovie << output;

    QImage img = QImage((const unsigned char*)(output.data),
                        output.cols,
                        output.rows,
                        QImage::Format_RGB888);

    ui->lbDisplay->setPixmap(QPixmap::fromImage(img).scaled(500, 500, Qt::KeepAspectRatio));
    //ui->lbDisplay->resize(ui->lbDisplay->pixmap()->size());
}
