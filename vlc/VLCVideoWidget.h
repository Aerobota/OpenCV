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

#include"videoStabilizer.h"

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

    QSlider* slVolume;
    QSlider* slMediaPosition;
    //DataMatrix::MColor matriz[704][480];
    QPushButton *btPlay;
    QPushButton *btStop;    
    QPushButton *btOpenRTSP;
    QPushButton *btOpenFile;
    QLabel* lbFile;    
    void addURL(const QString url);
    QVBoxLayout *vlDisplay;    
    QHBoxLayout *hlButtonOptions;
    QLabel *lbTittle;    
    bool isPlaying;    
    QString pathVideo;
    void contextMenuEvent(QContextMenuEvent *);
    QAction* acVolume;
    QAction* acMediaPosition;

    videoStabilizer* video;

    QStringList* filesDirectory;
    QTimer* myTimer;
    int countImage;
    QString pathDirectory;

protected:
    void closeEvent(QCloseEvent *event);
    /**
     * @brief This method create the main instantiates VLC.
     *
     * @param path The path for instance to play
     **/
    void createInstanceVLC(const QString url);
    /**
     * @brief This method create the widget for visualize video player.
     **/
    void createDisplayVLC();
    /**
     * @brief This method initializa the controls for VLC video player.
     **/
    void createControlsVLC();

public slots:
    /**
     * @brief This method allows change the position of the video feed.
     *
     * @param pos Advanced the current position
     **/
    void changePosition(int pos);
    /**
     * @brief This method allows change the volume level.
     *
     * @param vol New volume level
     **/
    void changeVolume(int vol);
    /**
     * @brief This method playback starts.
     */
    void play();
    /**
     * @brief This method playback stops.
     */
    void stop();
    /**
     * @brief This method open URL RTSP.
     */
    void openRTSP();
    /**
     * @brief This method open file in a directory.
     */
    void openFile();
    /**
     * @brief This method updates the status of the video interface.
     */
    void updateInterface();
    /**
     * @brief This method captures an image of the player.
     */
    void captureSnapshot();
    /**
     * @brief This method allows process the captured image.
     *
     * @param image The captured image
     **/
    void processImage(QImage image);
    /**
     * @brief This method allows assign new path to video open/store.
     *
     * @param path The path where the video open/store
     **/
    void changePATHVideo(const QString& path);
    /**
     * @brief This method open directory of images.
     */
    void openDirectory();
    /**
     * @brief This method read the directory of images to stabilizer.
     */
    void readImageDirectory();
    /**
     * @brief This method stop the read of images to stabilizer.
     */
    void stopImageDirectory();
    /**
     * @brief This method reset the counter images.
     */
    void resetImageDirectory();
};

#endif // VLCVIDEOWIDGET_H
