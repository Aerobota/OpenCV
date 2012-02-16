#include "PhononPlayer.h"

PhononPlayer::PhononPlayer(QWidget *parent) :
        QWidget(parent)
{

    //audioOutput = new Phonon::AudioOutput(Phonon::VideoCategory, this);//MusicCategory, this);
    mediaObject = new Phonon::MediaObject(this);
    //metaInformationResolver = new Phonon::MediaObject(this);

    videoPlayer = new Phonon::VideoPlayer(Phonon::VideoCategory, this);
    videoPlayer->setFixedSize(QSize(400,300));
    //videoPlayer->setGeometry(0 , 35 , 360 , 420);
    videoPlayer->play(Phonon::MediaSource(QUrl::fromLocalFile("/Volumes/HDD_120/vuelos/09022012/20120209093344.mp4")));

    connect(videoPlayer, SIGNAL(finished()), videoPlayer, SLOT(deleteLater()));

    mediaObject->setTickInterval(1000);

    connect(mediaObject, SIGNAL(tick(qint64)), this, SLOT(tick(qint64)));
    connect(mediaObject, SIGNAL(stateChanged(Phonon::State,Phonon::State)), this, SLOT(stateChanged(Phonon::State,Phonon::State)));
//    connect(metaInformationResolver, SIGNAL(stateChanged(Phonon::State,Phonon::State)), this, SLOT(metaStateChanged(Phonon::State,Phonon::State)));
//    connect(mediaObject, SIGNAL(currentSourceChanged(Phonon::MediaSource)), this, SLOT(sourceChanged(Phonon::MediaSource)));
//    connect(mediaObject, SIGNAL(aboutToFinish()), this, SLOT(aboutToFinish()));

    //Phonon::createPath(mediaObject, audioOutput);

    createMenu();
}

void PhononPlayer::createMenu()
{
    playAction = new QPushButton(style()->standardIcon(QStyle::SP_MediaPlay), tr("Play"), this);
    //playAction->setShortcut(tr("Crl+P"));
    //playAction->setDisabled(true);
    pauseAction = new QPushButton(style()->standardIcon(QStyle::SP_MediaPause), tr("Pause"), this);
    stopAction = new QPushButton(style()->standardIcon(QStyle::SP_MediaStop), tr("Stop"), this);
    //pauseAction->setShortcut(tr("Ctrl+A"));
    //pauseAction->setDisabled(true);

    connect(playAction, SIGNAL(clicked()), mediaObject, SLOT(play()));
    connect(pauseAction, SIGNAL(clicked()), mediaObject, SLOT(pause()) );

    QHBoxLayout *buttons = new QHBoxLayout;
    buttons->addWidget(playAction);
    buttons->addWidget(pauseAction);
    buttons->addWidget(stopAction);

    QVBoxLayout *playbackLayout = new QVBoxLayout;
    playbackLayout->addWidget(videoPlayer);
    playbackLayout->addLayout(buttons);

    this->setLayout(playbackLayout);
}

void PhononPlayer::stateChanged(Phonon::State newState, Phonon::State /* oldState */)
{
    switch (newState) {
        case Phonon::ErrorState:
            if (mediaObject->errorType() == Phonon::FatalError) {
                QMessageBox::warning(this, tr("Fatal Error"),
                mediaObject->errorString());
            } else {
                QMessageBox::warning(this, tr("Error"),
                mediaObject->errorString());
            }
            break;
        case Phonon::PlayingState:
                playAction->setEnabled(false);
                pauseAction->setEnabled(true);
                stopAction->setEnabled(true);
                break;
        case Phonon::StoppedState:
                stopAction->setEnabled(false);
                playAction->setEnabled(true);
                pauseAction->setEnabled(false);
                //timeLcd->display("00:00");
                break;
        case Phonon::PausedState:
                pauseAction->setEnabled(false);
                stopAction->setEnabled(true);
                playAction->setEnabled(true);
                break;
        case Phonon::BufferingState:
                break;
        default:
            ;
    }
}
