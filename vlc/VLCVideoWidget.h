/**
 * @file
 *   @brief Definition of class VLCVideoWidget
 *
 *   @author Alejandro Molina Pulido <am.alex09@gmail.com>
 *
 */

#ifndef VLCVIDEOWIDGET_H
#define VLCVIDEOWIDGET_H

#include <QWidget>
#include "VLCMacWidget.h"
#include "vlc/vlc.h"
#include <QLayout>
#include <QSlider>
#include <QSpacerItem>
#include <VLCInstance.h>
#include "VLCInstance.h"
#include <unistd.h>
#include <signal.h>
#include <QTcpSocket>
#include <QHostAddress>
#include "DataMatrix.h"
#include <QImage>
#include <QColor>

namespace Ui {
    class VLCVideoWidget;
}

class VLCVideoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VLCVideoWidget(const QString path, QWidget *parent = 0);
    ~VLCVideoWidget();

private:
    Ui::VLCVideoWidget *ui;
    VLCMacWidget* vlcMacWidget;
    libvlc_instance_t * instance;    
    libvlc_media_player_t * mediaPlayer;
    libvlc_media_t * media;

    QList<QImage> listImages;
    QStringList files;
    DataMatrix::MColor matriz[704][480];
    QPushButton *btPlay;
    QPushButton *btStop;    
    QPushButton *btOpenRTSP;
    QPushButton *btOpenFile;
    QLabel* lbFile;
    QFileInfo*fileInfo;
    QString nameFile;    
    void addURL(const QString url);
    QVBoxLayout *vlDisplay;    
    QHBoxLayout *hlButtonOptions;
    QLabel *lbTittle;    
    bool isPlaying;
    QString styleButtonRed;
    QString styleButtonGreen;
    QString pathVideo;
    void contextMenuEvent(QContextMenuEvent *);
    QAction* acVolume, *acMediaPosition;    

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void changePosition(int pos);
    void changeVolume(int vol);
    void play();
    void stop();
    void openRTSP();
    void openFile();
    void updateInterface();
    void captureSnapshot();
    void processImage(QImage image);

public slots:
    void changePATHVideo(const QString& path);    
};

#endif // VLCVIDEOWIDGET_H
