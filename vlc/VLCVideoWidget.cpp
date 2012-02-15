/**
 * @file
 *   @brief This class implements a video display widget
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
        ui(new Ui::VLCVideoWidget),
        video(NULL),
        myTimer(NULL)
{
    ui->setupUi(this);

    createInstanceVLC("");
    mediaPlayer = libvlc_media_player_new_from_media(media);
    createDisplayVLC();
    createControlsVLC();

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateInterface()));
    timer->start(50);

    QDir* tempDirectory = new QDir(QCoreApplication::applicationDirPath());;
    //pathVideo = QCoreApplication::applicationDirPath()+"/";

    if(!tempDirectory->exists("pathEstable"))
    {
        tempDirectory->mkpath("pathEstable");
    }

    pathVideo = QCoreApplication::applicationDirPath()+"/pathEstable/";

    setWindowTitle("Estabilizacion de Video");
    qDebug() << "CHECK END VLCVIDEOWIDGET "<<QTime::currentTime().toString("HHmmss");
}

void VLCVideoWidget::createInstanceVLC(const QString url)
{
    char const *argv[] =
    {
        "--no-video-title-show",        /* nor the filename displayed */
        "--no-sub-autodetect-file",     /* we don't want subtitles */
        "--no-disable-screensaver",     /* we don't want interfaces */
        "--no-snapshot-preview",        /* no blending in dummy vout */
        "--no-skip-frames",
        "--ignore-config",              /*Don't use VLC's config files */
        "--rtsp-caching=100"
    };
    int argc = sizeof( argv ) / sizeof( *argv );

    instance = libvlc_new(argc, argv);
    media = libvlc_media_new_path(instance, qtu(url));
}

void VLCVideoWidget::createDisplayVLC()
{
    vlcMacWidget = new VLCMacWidget(mediaPlayer, ui->groupBox);//this);
    vlcMacWidget->setMinimumHeight(400);
    vlcMacWidget->setMinimumWidth(400);
}
void VLCVideoWidget::createControlsVLC()
{
    slVolume = new QSlider(Qt::Horizontal);
    QObject::connect(slVolume, SIGNAL(sliderMoved(int)), this, SLOT(changeVolume(int)));
    slVolume->setValue(80);

    slMediaPosition = new QSlider(Qt::Horizontal);
    slMediaPosition->setMaximum(1000);
    QObject::connect(slMediaPosition, SIGNAL(sliderMoved(int)), this, SLOT(changePosition(int)));

    vlDisplay = new QVBoxLayout(this);
    vlDisplay->setContentsMargins(2,2,2,2);
    vlDisplay->addWidget(vlcMacWidget);
    vlDisplay->addWidget(slVolume);
    vlDisplay->addWidget(slMediaPosition);
    slVolume->setVisible(false);
    slMediaPosition->setVisible(false);

    hlButtonOptions = new QHBoxLayout();
    btPlay = new QPushButton(QIcon(":/images/icons_ET/Play.png"), "", this);
    btPlay->setText("Reproducir");
    connect(btPlay, SIGNAL(clicked()), this, SLOT(play()));

    btStop = new QPushButton(QIcon(":/images/icons_ET/Stop.png"), "", this);
    btStop->setText("Detener");
    connect(btStop, SIGNAL(clicked()), this, SLOT(stop()));

    btOpenRTSP = new QPushButton(QIcon(":/images/icons_ET/Radio.png"), "", this);
    btOpenRTSP->setText("Abrir RTSP");
    connect(btOpenRTSP, SIGNAL(clicked()), this, SLOT(openRTSP()));

    btOpenFile = new QPushButton(QIcon(":/images/icons_ET/Open.png"), "", this);
    btOpenFile->setText("Abrir Video");
    connect(btOpenFile, SIGNAL(clicked()), this, SLOT(openFile()));

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
    vlDisplay->addLayout(hlLabelMedia);    

    ui->groupBox->setLayout(vlDisplay);

    acVolume = new QAction("Ver barra de volumen", this);
    acMediaPosition = new QAction("Ver barra de posicion", this);
    acVolume->setCheckable(true);
    acVolume->setChecked(false);
    acMediaPosition->setCheckable(true);
    acMediaPosition->setChecked(false);

    connect(acVolume, SIGNAL(triggered(bool)), slVolume, SLOT(setVisible(bool)));
    connect(acMediaPosition, SIGNAL(triggered(bool)), slMediaPosition, SLOT(setVisible(bool)));

    connect(ui->btDirectory, SIGNAL(clicked()), this, SLOT(openDirectory()));
    connect(ui->btStop, SIGNAL(clicked()), this, SLOT(stopImageDirectory()));
    connect(ui->btReset, SIGNAL(clicked()), this, SLOT(resetImageDirectory()));
}

void VLCVideoWidget::play()
{
    if(!lbTittle->text().isEmpty())
    {
        if(libvlc_media_player_is_playing(mediaPlayer)==1)
        {
            libvlc_media_player_pause(mediaPlayer);            
            isPlaying = false;
            btPlay->setText("Reproducir");
        }
        else
        {
            libvlc_media_player_play(mediaPlayer);            
            btPlay->setText("Pausar");
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
        btPlay->setText("Reproducir");
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
                btPlay->setText("Reproducir");
                break;
            case 7:
                libvlc_media_player_release(mediaPlayer);                
                btPlay->setText("Reproducir");
                break;
            default:
                break;
            }
        }
        else
        {
            libvlc_media_player_stop(mediaPlayer);
            btPlay->setText("Reproducir");
        }

    }
    else
    {        
        libvlc_media_player_stop(mediaPlayer);        
        btPlay->setText("Reproducir");
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

    QString text = QInputDialog::getText(this, tr("Ingresar URL"), tr("High degrees:"), QLineEdit::Normal, tr("rtsp://192.168.1.90:554/axis-media/media.amp?videocodec=h264"), &ok);

    if (ok && !text.isEmpty())
    {
        addURL(text);
    }
}

void VLCVideoWidget::openFile()
{
    QString fileName(QFileDialog::getOpenFileName(this, tr("Abrir Video"), "/Users/malifeMb/Documents/Mariano/SEMAR/Inidetam/ImagenGrafica", tr("Movie Files (*.avi | *.mp4)")));

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

    createInstanceVLC(qtu(url));
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
        //        qDebug()<<"snapshot: "<< libvlc_video_take_snapshot(mediaPlayer, 0, pathVideo.toAscii().data(), 640, 480);
        libvlc_video_take_snapshot(mediaPlayer, 0, pathVideo.toAscii().data(), 640, 480);

        QDir directory = QDir(pathVideo);
        QString fileName = "*";
        QStringList files = directory.entryList(QStringList(fileName), QDir::Files | QDir::NoSymLinks);

        foreach (const QString &str, files)
        {
            QImage myImage;
            myImage.load(pathVideo+str);//load snapshot
            myImage = myImage.convertToFormat(QImage::Format_Indexed8);
            QFile::remove(pathVideo+str);//remove snapshot

            processImage(myImage);//process the image
        }
    }
}

void VLCVideoWidget::processImage(QImage image)
{
    QRect sizeImage = image.rect();
    static QImage img(sizeImage.width(), sizeImage.height(), QImage::Format_Indexed8);

    if (video == NULL )
    {
        video = new videoStabilizer(image.rect());
        img.setColorTable(image.colorTable());
    }

    //    qDebug()<<"Size Image: "<<sizeImage.height()<<" x "<<sizeImage.width();
    //    qDebug()<< "Depth: " << img.depth();
    //    qDebug()<< "Image CT: " << image.colorCount();
    //    qDebug()<< "Img CT: " << img.colorCount();

    video->stabilizeImage(&image,&img);    
    ui->lbImageMatrixRGB->setPixmap(QPixmap::fromImage(img.scaled(500, 500, Qt::KeepAspectRatio)));
    //ui->lbImageMatrixRGBSub->setPixmap(QPixmap::fromImage(img.scaled(500, 500, Qt::KeepAspectRatio)));
}

void VLCVideoWidget::openDirectory()
{
    QString directoryName(QFileDialog::getExistingDirectory(this, "Directory", "/"));

    if(directoryName.isEmpty())
        return;

    QDir directory = QDir(directoryName);
    pathDirectory = directory.absolutePath();
    QString fileName = "*";
    filesDirectory = new QStringList(directory.entryList(QStringList(fileName), QDir::Files | QDir::NoSymLinks));


    if(filesDirectory->count()>0)
    {
        qDebug()<<filesDirectory->count();
        myTimer = new QTimer(this);
        myTimer->setInterval(34);
        connect(myTimer, SIGNAL(timeout()), this, SLOT(readImageDirectory()));
        myTimer->start();
        countImage =0;
    }
}

void VLCVideoWidget::readImageDirectory()
{
    if(countImage < filesDirectory->count())
    {        
        QImage myImage;
        myImage.load(pathDirectory+"/"+filesDirectory->at(countImage));//load snapshot
        myImage = myImage.convertToFormat(QImage::Format_Indexed8);

        ui->lbImageNormal->setPixmap(QPixmap::fromImage(myImage.scaled(500, 500, Qt::KeepAspectRatio)));

        if(countImage>0)
        {
//            QImage tempImage;

//            tempImage.load(pathDirectory+"/"+filesDirectory->at(countImage-1));
//            tempImage = tempImage.convertToFormat(QImage::Format_Indexed8);

//            //static QImage imgtempImage.data_ptr(), tempImage.width(), tempImage.height(), QImage::Format_Indexed8);
//            //img.setColorTable(tempImage.colorTable());

//            ui->lbImageNormalSub->setPixmap(QPixmap::fromImage(tempImage.scaled(500, 500, Qt::KeepAspectRatio)));
        }

        processImage(myImage);
        countImage++;
    }
}

void VLCVideoWidget::stopImageDirectory()
{
    if (myTimer != NULL )
    {
        if(myTimer->isActive())
        {
            myTimer->stop();

            ui->lbImageNormal->setText("-");
            ui->lbImageNormal->resize(20,20);
            ui->lbImageMatrixRGB->setText("-");
            ui->lbImageMatrixRGB->resize(20,20);
        }
    }
}

void VLCVideoWidget::resetImageDirectory()
{
    if (myTimer != NULL )
    {
        if(myTimer->isActive())
        {
            if(filesDirectory->count()>0)
            {
                countImage =0;
            }
        }
    }
}
