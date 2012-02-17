#ifndef FFMPEGPLAYER_H
#define FFMPEGPLAYER_H

#include <QWidget>
#include <QMessageBox>
#include <QPainter>
#include <QFileDialog>
#include <QTimer>

#include "QVideoEncoder.h"
#include "QVideoDecoder.h"
#include "videoStabilizer.h"

namespace Ui {
    class FfmpegPlayer;
}

class FfmpegPlayer : public QWidget
{
    Q_OBJECT

public:
    explicit FfmpegPlayer(QWidget *parent = 0);
    ~FfmpegPlayer();

protected:
    QVideoDecoder decoder;
    videoStabilizer* video;


    void changeEvent(QEvent *e);

    // Helper

    void image2Pixmap(QImage &img,QPixmap &pixmap);

    /**
      Decode and display a frame
    **/
    void displayFrame();
    /**
      Prompts the user for the video to load, and display the first frame
    **/
    void loadVideo(QString fileName);
    void errLoadVideo();
    bool checkVideoLoadOk();
    void nextFrame();

    // Encoder demo
    void GenerateSyntheticVideo(QString filename);

    void processImage(QImage image);
    QImage subtract(QImage firstImage, QImage secondImage);

private:
    Ui::FfmpegPlayer *ui;
    QImage tImageP;
    QImage tImageO;
    QTimer* reloj;
    bool activeCorrection;



private slots:
    /**
      Prompts the user for a file
      Create the file
      Pass the file to the video generation function (alternatively the file name could be passed)
    **/
    void on_actionSave_synthetic_video_triggered();
    /**
      Display next frame
    **/
    void on_pushButtonNextFrame_clicked();
    void on_pushButtonSeekMillisecond_clicked();
    void on_pushButtonSeekFrame_clicked();
    void on_actionLoad_video_triggered();
    void on_actionQuit_triggered();

    void changeStatusTimer();
    void enableCorrection(bool status);
};

#endif // FFMPEGPLAYER_H
