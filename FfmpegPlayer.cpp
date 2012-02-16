#include "FfmpegPlayer.h"
#include "ui_FfmpegPlayer.h"

FfmpegPlayer::FfmpegPlayer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FfmpegPlayer),
    video(NULL)
{
    ui->setupUi(this);

    connect(ui->btOpenVideo, SIGNAL(clicked()), this, SLOT(on_actionLoad_video_triggered()));

    connect(ui->pushButtonNextFrame_2, SIGNAL(clicked()), this, SLOT(on_pushButtonNextFrame_clicked()));
    connect(ui->pushButtonSeekFrame_2, SIGNAL(clicked()), this, SLOT(on_pushButtonSeekFrame_clicked()));
    connect(ui->pushButtonSeekMillisecond_2, SIGNAL(clicked()), this, SLOT(on_pushButtonSeekMillisecond_clicked()));
}

FfmpegPlayer::~FfmpegPlayer()
{
    delete ui;
}

void FfmpegPlayer::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void FfmpegPlayer::on_actionQuit_triggered()
{
    close();
}

void FfmpegPlayer::image2Pixmap(QImage &img,QPixmap &pixmap)
{
   // Convert the QImage to a QPixmap for display
   pixmap = QPixmap(img.size());
   QPainter painter;
   painter.begin(&pixmap);
   painter.drawImage(0,0,img);
   painter.end();
}





/******************************************************************************
*******************************************************************************
* Decoder demo   Decoder demo   Decoder demo   Decoder demo   Decoder demo
*******************************************************************************
******************************************************************************/

void FfmpegPlayer::on_actionLoad_video_triggered()
{
    // Prompt a video to load
   QString fileName = QFileDialog::getOpenFileName(this, "Load Video",QString(),"Video (*.avi *.asf *.mpg *.mp4)");
   if(!fileName.isNull())
   {
      loadVideo(fileName);
   }
}

/**
  Prompts the user for the video to load, and display the first frame
**/
void FfmpegPlayer::loadVideo(QString fileName)
{
   decoder.openFile(fileName);
   if(decoder.isOk()==false)
   {
      QMessageBox::critical(this,"Error","Error loading the video");
      return;
   }

   // Get a new frame
   nextFrame();
   // Display a frame
   displayFrame();

}

void FfmpegPlayer::errLoadVideo()
{
   QMessageBox::critical(this,"Error","Load a video first");
}
bool FfmpegPlayer::checkVideoLoadOk()
{
   if(decoder.isOk()==false)
   {
      errLoadVideo();
      return false;
   }
   return true;
}

/**
  Decode and display a frame
**/
void FfmpegPlayer::displayFrame()
{
   // Check we've loaded a video successfully
   if(!checkVideoLoadOk())
      return;

   QImage img;

   // Decode a frame
   int et,en;
   if(!decoder.getFrame(img,&en,&et))
   {
      QMessageBox::critical(this,"Error","Error decoding the frame");
      return;
   }
   // Convert the QImage to a QPixmap for display

   QPixmap p;
   image2Pixmap(img,p);

   img = img.convertToFormat(QImage::Format_Indexed8);

   if(!tImageO.isNull())
   {
       //tImageO = tImageO.convertToFormat(QImage::Format_Indexed8);
       ui->lbImageOriginT->setPixmap(QPixmap::fromImage(subtract(tImageO, img).scaled(500, 500, Qt::KeepAspectRatio)));
   }

    tImageO = img;

   // Display the QPixmap
   ui->lbImageOrigin->setPixmap(p.scaled(500, 500, Qt::KeepAspectRatio));

   //img = img.convertToFormat(QImage::Format_Indexed8);
   processImage(img);

   // Display the video size
   ui->labelVideoInfo->setText(QString("Size %2 ms. Display: #%3 @ %4 ms.").arg(decoder.getVideoLengthMs()).arg(en).arg(et));

}

void FfmpegPlayer::nextFrame()
{
   if(!decoder.seekNextFrame())
   {
      QMessageBox::critical(this,"Error","seekNextFrame failed");
   }
}

/**
  Display next frame
**/
void FfmpegPlayer::on_pushButtonNextFrame_clicked()
{
   nextFrame();
   displayFrame();
}




void FfmpegPlayer::on_pushButtonSeekFrame_clicked()
{
   // Check we've loaded a video successfully
   if(!checkVideoLoadOk())
      return;

   bool ok;

   int frame = ui->lineEditFrame_2->text().toInt(&ok);
   if(!ok || frame < 0)
   {
      QMessageBox::critical(this,"Error","Invalid frame number");
      return;
   }

   // Seek to the desired frame
   if(!decoder.seekFrame(frame))
   {
      QMessageBox::critical(this,"Error","Seek failed");
      return;
   }
   // Display the frame
   displayFrame();

}


void FfmpegPlayer::on_pushButtonSeekMillisecond_clicked()
{
   // Check we've loaded a video successfully
   if(!checkVideoLoadOk())
      return;

   bool ok;

   int ms = ui->lineEditMillisecond_2->text().toInt(&ok);
   if(!ok || ms < 0)
   {
      QMessageBox::critical(this,"Error","Invalid time");
      return;
   }

   // Seek to the desired frame
   if(!decoder.seekMs(ms))
   {
      QMessageBox::critical(this,"Error","Seek failed");
      return;
   }
   // Display the frame
   displayFrame();


}





/******************************************************************************
*******************************************************************************
* Encoder demo   Encoder demo   Encoder demo   Encoder demo   Encoder demo
*******************************************************************************
******************************************************************************/

/**
  Prompts the user for a file
  Create the file
  Pass the file to the video generation function (alternatively the file name could be passed)
**/
void FfmpegPlayer::on_actionSave_synthetic_video_triggered()
{
   QString title("Save a synthetic video");
   QString fileName = QFileDialog::getSaveFileName(this, title,QString(),"Video (*.avi *.asf *.mpg)");
   if(!fileName.isNull())
   {
      GenerateSyntheticVideo(fileName);
   }
}

void FfmpegPlayer::GenerateSyntheticVideo(QString filename)
{
   int width=640;
   int height=480;
   int bitrate=1000000;
   int gop = 20;

   // The image on which we draw the frames
   QImage frame(width,height,QImage::Format_RGB32);     // Only RGB32 is supported

   // A painter to help us draw
   QPainter painter(&frame);
   painter.setBrush(Qt::red);
   painter.setPen(Qt::white);

   // Create the encoder
   QVideoEncoder encoder;
   encoder.createFile(filename,width,height,bitrate,gop);

   QEventLoop evt;      // we use an event loop to allow for paint events to show on-screen the generated video

   // Generate a few hundred frames
   int size=0;
   for(unsigned i=0;i<500;i++)
   {
      // Clear the frame
      painter.fillRect(frame.rect(),Qt::red);

      // Frame number
      painter.drawText(frame.rect(),Qt::AlignCenter,QString("Frame %1\nLast frame was %2 bytes").arg(i).arg(size));

      // Display the frame, and processes events to allow for screen redraw
      QPixmap p;
      image2Pixmap(frame,p);
      ui->lbImageOrigin->setPixmap(p);
      evt.processEvents();

      size=encoder.encodeImage(frame);
      printf("Encoded: %d\n",size);
   }

   encoder.close();

}


void FfmpegPlayer::processImage(QImage image)
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

    if(!tImageP.isNull())
        ui->lbImageProcessT->setPixmap(QPixmap::fromImage(subtract(tImageP, img).scaled(500, 500, Qt::KeepAspectRatio)));

    tImageP =  img;

    ui->lbImageProcess->setPixmap(QPixmap::fromImage(img.scaled(500, 500, Qt::KeepAspectRatio)));
}

QImage FfmpegPlayer::subtract(QImage firstImage, QImage secondImage)
{
    int width = firstImage.width() < secondImage.width() ? firstImage.width() : secondImage.width();
    int height = firstImage.height() < secondImage.height() ? firstImage.height() : secondImage.height();
    int x, y;

    for(y=0;y<height;y++)
        for(x=0;x<width;x++)
        {
        int r = firstImage.pixelIndex(x, y) - secondImage.pixelIndex(x, y);

        r = r < 60 ? 0 : r;
        firstImage.setPixel(x, y, r);
    }

    return firstImage;
}
