#ifndef PHONONPLAYER_H
#define PHONONPLAYER_H

#include <QWidget>
#include <phonon/audiooutput.h>
#include <phonon/videowidget.h>
#include <phonon/videoplayer.h>
#include <phonon/seekslider.h>
#include <phonon/mediaobject.h>
#include <phonon/MediaSource>
#include <phonon/volumeslider.h>
#include <phonon/backendcapabilities.h>
#include <QList>
#include <QLabel>
#include <QUrl>
#include <QHBoxLayout>
#include <QAction>
#include <QStyle>
#include <QPushButton>
#include <QMessageBox>

class PhononPlayer : public QWidget
{
    Q_OBJECT
public:
    explicit PhononPlayer(QWidget *parent = 0);


    Phonon::SeekSlider *seekSlider;
    Phonon::MediaObject *mediaObject;
    //Phonon::MediaObject *metaInformationResolver;
    //Phonon::AudioOutput *audioOutput;
    Phonon::VideoWidget *videoWidget;
    Phonon::VideoPlayer *videoPlayer;

    Phonon::VolumeSlider *volumeSlider;

    QList<Phonon::MediaSource> sources;

    void createMenu();

private:
    QPushButton *playAction;
    QPushButton *pauseAction;
    QPushButton *stopAction;

signals:

public slots:
    void stateChanged(Phonon::State newState, Phonon::State /* oldState */);

};

#endif // PHONONPLAYER_H
