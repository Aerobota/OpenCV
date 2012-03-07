#include "OpenCVWidget.h"
#include "ui_OpenCVWidget.h"

OpenCVWidget::OpenCVWidget(QWidget *parent) :
        QWidget(parent),        
        ui(new Ui::OpenCVWidget)
{
    ui->setupUi(this);

    OverlayData* overlayData = new OverlayData(300, 300, this);
    QHBoxLayout* hlButtons = new QHBoxLayout();
    btPlay = new QPushButton(QIcon(":/icons/Play.png"), "", this);
    btStop = new QPushButton(QIcon(":/icons/Stop.png"), "", this);
    btFile = new QPushButton(QIcon(":/icons/Open.png"), "", this);
    btRTSP = new QPushButton(QIcon(":/icons/Radio.png"), "", this);
    btRecord = new QPushButton(QIcon(":/icons/Record.png"), "", this);

    cxFilter = new QCheckBox("Filter", this);
    cxFilter->setCheckable(true);
    cxFilter->setChecked(false);
    cxStabilizer = new QCheckBox("Stabilize", this);
    cxStabilizer->setCheckable(true);
    cxStabilizer->setChecked(false);
    slSizeKernel = new QSlider(Qt::Horizontal, this);
    slSizeKernel->setMinimum(2);
    slSizeKernel->setMaximum(12);
    slSizeKernel->setValue(2);

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

    connect(cxFilter, SIGNAL(stateChanged(int)), overlayData, SLOT(setEnableFilter(int)));
    connect(cxStabilizer, SIGNAL(stateChanged(int)), overlayData, SLOT(setEnableStabilizer(int)));
    connect(slSizeKernel, SIGNAL(valueChanged(int)), overlayData, SLOT(setSizeKernel(int)));

    QVBoxLayout* vlControls = new QVBoxLayout();

    vlControls->addWidget(overlayData);
    vlControls->addLayout(hlButtons);
    vlControls->addWidget(cxFilter);
    vlControls->addWidget(cxStabilizer);
    vlControls->addWidget(slSizeKernel);
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
