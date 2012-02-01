/**
 * @file
 *   @brief Definition of class VLCVideoWidget
 *
 *   @author Alejandro Molina Pulido <am.alex09@gmail.com>
 *
 */

#include "VLCVideoWidget.h"
#include "ui_VLCVideoWidget.h"

#define qtu( i ) ((i).toUtf8().constData())



VLCVideoWidget::VLCVideoWidget(const QString path, QWidget *parent) :
        QWidget(parent),
        pathVideo(path),
        ui(new Ui::VLCVideoWidget)
{
    ui->setupUi(this);

    char const *argv[] =
    {
        //"-vvvvv",
        //"--no-video-title-show",
        "--no-skip-frames",
        //"--no-audio",
        //"--plugin-path", VLC_TREE "/modules",
        "--ignore-config", //Don't use VLC's config files
        "--rtsp-caching=400",
        "--http-caching=200"
        //"--{rtsp,http,sout-mux}-caching"
    };
    int argc = sizeof( argv ) / sizeof( *argv );

    instance = libvlc_new(argc, argv);//0, NULL);
    media = libvlc_media_new_path(instance, "");///Users/proyectovant01/temp/ToyStory3.avi");
    mediaPlayer = libvlc_media_player_new_from_media(media);

    vlcMacWidget = new VLCMacWidget(mediaPlayer, ui->groupBox);//this);

    vlDisplay = new QVBoxLayout(this);
    vlDisplay->addWidget(vlcMacWidget);

    hlButtonOptions = new QHBoxLayout();
    btPlay = new QPushButton(QIcon(":/images/actions/media-playback-start.svg"), "", this);
    btPlay->setToolTip("Reproducir");
    connect(btPlay, SIGNAL(clicked()), this, SLOT(play()));

    btStop = new QPushButton(QIcon(":/images/actions/media-playback-stop.svg"), "", this);
    btStop->setToolTip("Detener");
    connect(btStop, SIGNAL(clicked()), this, SLOT(stop()));

    btOpenRTSP = new QPushButton(QIcon(":/images/devices/network-wireless.svg"), "", this);
    btOpenRTSP->setToolTip("Abrir RTSP");
    connect(btOpenRTSP, SIGNAL(clicked()), this, SLOT(openRTSP()));

    btOpenFile = new QPushButton(QIcon(":/images/actions/folder-new.svg"), "", this);
    btOpenFile->setToolTip("Abrir Video");
    connect(btOpenFile, SIGNAL(clicked()), this, SLOT(openFile()));

    btPlay->setText("Play");
    btStop->setText("Stop");
    btOpenRTSP->setText("Open RTSP");
    btOpenFile->setText("Open File");

    hlButtonOptions->addWidget(btPlay);
    hlButtonOptions->addWidget(btStop);
    hlButtonOptions->addWidget(btOpenRTSP);
    hlButtonOptions->addWidget(btOpenFile);
    vlDisplay->addLayout(hlButtonOptions);

    QHBoxLayout *hlLabelMedia = new QHBoxLayout();
    lbTittle = new QLabel(this);
    lbTittle->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    lbTittle->setText("");
    hlLabelMedia->addWidget(lbTittle);
    lbFile = new QLabel(this);
    lbFile->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    lbFile->setText("");
    hlLabelMedia->addWidget(lbFile);
    vlDisplay->addLayout(hlLabelMedia);    

    styleButtonRed = QString("QAbstractButton { background-color: rgb(255, 5, 0); border-color: rgb(10, 10, 10)} QAbstractButton:checked { border: 2px solid #379AC3; }");
    styleButtonGreen = QString("QAbstractButton { background-color: rgb(11, 255, 0); border-color: rgb(10, 10, 10)} QAbstractButton:checked { border: 2px solid #379AC3; }");

    isPlaying = false;

    this->setLayout(vlDisplay);
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateInterface()));
    timer->start(50);

    ui->groupBox->setLayout(vlDisplay);

    acVolume = new QAction("Set visible slider volume", this);
    acMediaPosition = new QAction("Set visible media position", this);
    acVolume->setCheckable(true);
    acVolume->setChecked(false);
    acMediaPosition->setCheckable(true);
    acMediaPosition->setChecked(false);

    QDir* tempDirectory = new QDir("/");;
    pathVideo = "/";

    if(!tempDirectory->exists("videoEstable"))
    {
        if(tempDirectory->mkpath("videoEstable"))
        {
            pathVideo = "/videoEstable/";
        }
    }

    setWindowTitle("Estabilizacion de Video");
    qDebug() << "CHECK END VLCVIDEOWIDGET "<<QTime::currentTime().toString("HHmmss");
}

void VLCVideoWidget::play()
{
    if(!lbTittle->text().isEmpty())
    {
        if(libvlc_media_player_is_playing(mediaPlayer)==1)
        {
            libvlc_media_player_pause(mediaPlayer);            
            isPlaying = false;
            btPlay->setText("Play");
        }
        else
        {
            libvlc_media_player_play(mediaPlayer);            
            btPlay->setText("Pause");
            isPlaying = true;
        }
    }
}

void VLCVideoWidget::stop()
{
    if(libvlc_media_player_is_playing(mediaPlayer)==1)
    {
        libvlc_media_player_stop(mediaPlayer);        
        //btPlay->setIcon(QIcon(":/images/actions/media-playback-start.svg"));
        btPlay->setText("Play");
        //viewSnapShot();
    }
}

VLCVideoWidget::~VLCVideoWidget()
{
    if(instance)
    {
        libvlc_release(instance);
    }

    delete vlcMacWidget;

    delete ui;
}

void VLCVideoWidget::changePosition(int pos)
{
    if(mediaPlayer)
    {
        libvlc_media_player_set_position(mediaPlayer,(float)pos/(float)1000);
    }
}

void VLCVideoWidget::updateInterface()
{    
    libvlc_media_t *curMedia = libvlc_media_player_get_media(mediaPlayer);

    if (curMedia == NULL)
    {
        return;
    }

    if(mediaPlayer)
    {        
        libvlc_state_t state = libvlc_media_player_get_state(mediaPlayer);

        captureSnapshot();

        if(state < 7)
        {
            int aa;
            switch(state)
            {
            case 0:
                aa=0;
                break;
            case 1:
                aa=1;
                break;
            case 2:
                aa=2;
                break;
            case 3:
                aa=3;
                break;
            case 4:
                aa=4;
                break;
            case 5:
                aa=5;
                break;
            case 6:
                libvlc_media_player_stop(mediaPlayer);                
                btPlay->setText("Play");
                break;
            case 7:
                libvlc_media_player_release(mediaPlayer);                
                btPlay->setText("Play");
                break;
            default:
                break;
            }
        }
        else
        {
            libvlc_media_player_stop(mediaPlayer);
            btPlay->setText("Play");
        }

    }
    else
    {        
        libvlc_media_player_stop(mediaPlayer);        
        btPlay->setText("Play");
    }
}

void VLCVideoWidget::changeVolume(int vol)
{
    if(mediaPlayer)
    {
        libvlc_audio_set_volume(mediaPlayer,vol);
    }
}

void VLCVideoWidget::openRTSP()
{
    bool ok;

    QString text = QInputDialog::getText(this, tr("Please enter URL"), tr("High degrees:"), QLineEdit::Normal, tr("rtsp://192.168.1.90:554/axis-media/media.amp?videocodec=h264"), &ok);

    if (ok && !text.isEmpty())
    {
        addURL(text);
    }
}

void VLCVideoWidget::openFile()
{
    QString fileName(QFileDialog::getOpenFileName(this, tr("Open File Movie"), "/", tr("Movie Files (*.avi | *.mp4)")));

    if (fileName.isEmpty())
        return;

    addURL(fileName);
}

void VLCVideoWidget::addURL(const QString url)
{
    stop();

    if(instance)
    {
        libvlc_release(instance);
    }

    char const *argv[] =
    {
        //"-vvvvv",
        //"--no-video-title-show",
        "--no-skip-frames",
        //"--no-audio",
        //"--plugin-path", VLC_TREE "/modules",
        "--ignore-config", //Don't use VLC's config files
        "--rtsp-caching=400",
        "--http-caching=200"
        // "--{rtsp,http,sout-mux}-caching"
        //"--http-reconnect=0"
        //"--http-continuous=0"
        //"--http-forward-cookies=0"
    };

    int argc = sizeof( argv ) / sizeof( *argv );
    instance = libvlc_new(argc, argv);
    media = libvlc_media_new_path(instance, qtu(url));
    libvlc_media_player_set_media(mediaPlayer, media);

    play();
    lbTittle->setText(url);
}

void VLCVideoWidget::changePATHVideo(const QString &path)
{
    this->pathVideo = path;
}

void VLCVideoWidget::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    menu.addAction(acVolume);
    menu.addAction(acMediaPosition);
    menu.exec(event->globalPos());
}

void VLCVideoWidget::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);

    qDebug()<<"Cerrando VLC Video Widget";
}

void VLCVideoWidget::captureSnapshot()
{
    if(isPlaying)
    {
        qDebug()<<"snapshot: "<< libvlc_video_take_snapshot(mediaPlayer, 0, pathVideo.toAscii().data(), 640, 480);

        QDir directory = QDir(pathVideo);
        QString fileName = "*";
        files = directory.entryList(QStringList(fileName), QDir::Files | QDir::NoSymLinks);

        foreach (const QString &str, files)
        {
            QImage myImage;
            myImage.load(pathVideo+str);//load snapshot
            QFile::remove(pathVideo+str);//remove snapshot
            listImages.append(myImage);//saved QImage to QList images
            ui->lbImageSnapShot->setPixmap(QPixmap::fromImage(myImage));//assigned the image generated
            processImage(myImage);//process the image

            qDebug()<<str;
        }
    }
}

void VLCVideoWidget::processImage(QImage image)
{
    QRect sizeImage = image.rect();
    qDebug()<<"Size Image: "<<sizeImage.height()<<" x "<<sizeImage.width();

    QImage img(sizeImage.width(), sizeImage.height(), QImage::Format_RGB16);

    for(int ii =0; ii< sizeImage.width(); ii++)//704
    {
        for(int tt =0; tt< sizeImage.height(); tt++)//480
        {
            QRgb color = image.pixel(ii, tt);
            matriz[ii][tt].rColor = qRed(color);
            matriz[ii][tt].gColor = qGreen(color);
            matriz[ii][tt].bColor = qBlue(color);

            QColor colorRGB(matriz[ii][tt].rColor, matriz[ii][tt].gColor, matriz[ii][tt].bColor);
            img.setPixel(ii, tt, colorRGB.rgb());
        }
    }

    ui->lbImageMatrixRGB->setPixmap(QPixmap::fromImage(img));
}
