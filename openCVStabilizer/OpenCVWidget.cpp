#include "OpenCVWidget.h"
#include "ui_OpenCVWidget.h"

OpenCVWidget::OpenCVWidget(QWidget *parent) :
        QWidget(parent),        
        ui(new Ui::OpenCVWidget)
{
    ui->setupUi(this);

    OverlayData* overlayData = new OverlayData(300, 300, this);
    QHBoxLayout* hlButtons = new QHBoxLayout();
    btPlay = new QPushButton("Play");
    btStop = new QPushButton("Stop");
    btFile = new QPushButton("File");
    btRTSP = new QPushButton("RTSP");
    btRecord = new QPushButton("Record");

    lbTitle = new QLabel("---");
    lbTitle->setMaximumHeight(15);

    hlButtons->addWidget(btPlay);
    hlButtons->addWidget(btStop);
    hlButtons->addWidget(btFile);
    hlButtons->addWidget(btRTSP);
    hlButtons->addWidget(btRecord);

    connect(btPlay, SIGNAL(clicked()), overlayData, SLOT(playMovie()));
    connect(btStop, SIGNAL(clicked()), overlayData, SLOT(stopMovie()));
    connect(btFile, SIGNAL(clicked()), overlayData, SLOT(openFile()));
    connect(btRTSP, SIGNAL(clicked()), overlayData, SLOT(openRTSP()));
    connect(btRecord, SIGNAL(clicked()), overlayData, SLOT(record()));
    connect(overlayData, SIGNAL(emitCaptureImage(QImage)), this, SLOT(showCaptureImage(QImage)));
    connect(overlayData, SIGNAL(emitTitle(QString)), lbTitle, SLOT(setText(QString)));

    QVBoxLayout* vlControls = new QVBoxLayout();

    vlControls->addWidget(overlayData);
    vlControls->addLayout(hlButtons);
    vlControls->addWidget(lbTitle);

    setLayout(vlControls);

    setWindowTitle("VIDEO");
}

OpenCVWidget::~OpenCVWidget()
{
    delete ui;
}

void OpenCVWidget::showCaptureImage(QImage img)
{
    Q_UNUSED(img);
    //lbTitle->setPixmap(QPixmap::fromImage(img));
}
