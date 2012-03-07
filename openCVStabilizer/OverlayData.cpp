/*===================================================================
======================================================================*/

/**
 * @file
 *   @brief Head Up Display (HUD)
 *
 *   @author Lorenz Meier <mavteam@student.ethz.ch>
 *
 */
#include "OverlayData.h"

template<typename T>
inline bool isnan(T value)
{
    return value != value;
}

template<typename T>
inline bool isinf(T value)
{
    return std::numeric_limits<T>::has_infinity && (value == std::numeric_limits<T>::infinity() || (-1*value) == std::numeric_limits<T>::infinity());
}

OverlayData::OverlayData(int width, int height, QWidget* parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent),    
    yawInt(0.0f),
    mode(tr("UNKNOWN MODE")),
    state(tr("UNKNOWN STATE")),
    //fuelStatus(tr("00.0V (00m:00s)")),
    xCenterOffset(0.0f),
    yCenterOffset(0.0f),
    vwidth(200.0f),
    vheight(150.0f),
    //vGaugeSpacing(50.0f),
    //vPitchPerDeg(6.0f), ///< 4 mm y translation per degree)
    defaultColor(QColor(255, 255, 255)),
    infoColor(QColor(255, 255, 255)),
    refreshTimer(new QTimer(this)),
    //noCamera(true),
    //hardwareAcceleration(true),
    //strongStrokeWidth(1.5f),
    //normalStrokeWidth(1.0f),
    //fineStrokeWidth(0.5f),
    roll(0.0f),
    pitch(0.0f),
    yaw(0.0f),
    rollLP(0.0f),
    pitchLP(0.0f),
    yawLP(0.0f),
    yawDiff(0.0f),
    //xPos(0.0),
    //yPos(0.0),
    //zPos(0.0),
    //xSpeed(0.0),
    //ySpeed(0.0),
    //zSpeed(0.0),
    //lastSpeedUpdate(0),
    //totalSpeed(0.0),
    //totalAcc(0.0),
    lat(0.0),
    lon(0.0),
    alt(0.0),    
    //offlineDirectory(""),
    //nextOfflineImage(""),
    hudInstrumentsEnabled(false),
    videoEnabled(true),
    video(NULL),
    isRecord(false),
    existFileMovie(false),
    enableFilter(0),
    enableStabilizer(0),
    sizeKernel(0)
{    
    setAutoFillBackground(false);
    setMinimumSize(80, 60);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QImage fill = QImage(width, height, QImage::Format_Indexed8);
    fill.setNumColors(3);
    fill.setColor(0, qRgb(0, 0, 0));
    fill.setColor(1, qRgb(0, 0, 0));
    fill.setColor(2, qRgb(0, 0, 0));
    fill.fill(0);

    refreshTimer->setInterval(updateInterval);

    connect(refreshTimer, SIGNAL(timeout()), this, SLOT(paintHUD()));

    createActions();
    setVisible(true);
}

OverlayData::~OverlayData()
{
    refreshTimer->stop();
}

QSize OverlayData::sizeHint() const
{
    return QSize(width(), (width()*3.0f)/4);
}

void OverlayData::showEvent(QShowEvent* event)
{
    // React only to internal (pre-display)
    // events
    Q_UNUSED(event)
    //refreshTimer->start(updateInterval);
    }

void OverlayData::hideEvent(QHideEvent* event)
{
    // React only to internal (pre-display)
    // events
    Q_UNUSED(event);
    refreshTimer->stop();
}

void OverlayData::contextMenuEvent (QContextMenuEvent* event)
{
    QMenu menu(this);    
    enableHUDAction->setChecked(hudInstrumentsEnabled);
    menu.addAction(enableHUDAction);
    menu.exec(event->globalPos());
}

void OverlayData::createActions()
{
    enableHUDAction = new QAction(tr("Habilitar vista de datos"), this);
    enableHUDAction->setCheckable(true);
    enableHUDAction->setChecked(hudInstrumentsEnabled);
    connect(enableHUDAction, SIGNAL(triggered(bool)), this, SLOT(enableHUDInstruments(bool)));
}

float OverlayData::refToScreenX(float x)
{
    //qDebug() << "sX: " << (scalingFactor * x);
    return (scalingFactor * x);
}

float OverlayData::refToScreenY(float y)
{
    //qDebug() << "sY: " << (scalingFactor * y);
    return (scalingFactor * y);
}

void OverlayData::paintCenterBackground(float roll, float pitch, float yaw)
{
    // Center indicator is 100 mm wide
    float referenceWidth = 70.0;
    float referenceHeight = 70.0;

    // HUD is assumed to be 200 x 150 mm
    // so that positions can be hardcoded
    // but can of course be scaled.

    double referencePositionX = vwidth / 2.0 - referenceWidth/2.0;
    double referencePositionY = vheight / 2.0 - referenceHeight/2.0;

    //this->width()/2.0+(xCenterOffset*scalingFactor), this->height()/2.0+(yCenterOffset*scalingFactor);

    setupGLView(referencePositionX, referencePositionY, referenceWidth, referenceHeight);

    // Store current position in the model view
    // the position will be restored after drawing
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    // Move to the center of the window
    glTranslatef(referenceWidth/2.0f,referenceHeight/2.0f,0);

    // Move based on the yaw difference
    glTranslatef(yaw, 0.0f, 0.0f);

    // Rotate based on the bank
    glRotatef((roll/M_PI)*180.0f, 0.0f, 0.0f, 1.0f);

    // Translate in the direction of the rotation based
    // on the pitch. On the 777, a pitch of 1 degree = 2 mm
    //glTranslatef(0, ((-pitch/M_PI)*180.0f * vPitchPerDeg), 0);
    glTranslatef(0.0f, 0.0f, 0.0f);

    // Ground
    glColor3ub(179,102,0);

    glBegin(GL_POLYGON);
    glVertex2f(-300,-300);
    glVertex2f(-300,0);
    glVertex2f(300,0);
    glVertex2f(300,-300);
    glVertex2f(-300,-300);
    glEnd();

    // Sky
    glColor3ub(0,153,204);

    glBegin(GL_POLYGON);
    glVertex2f(-300,0);
    glVertex2f(-300,300);
    glVertex2f(300,300);
    glVertex2f(300,0);
    glVertex2f(-300,0);

    glEnd();
}

void OverlayData::paintText(QString text, QColor color, float fontSize, float refX, float refY, QPainter* painter)
{
    QPen prevPen = painter->pen();
    float pPositionX = refToScreenX(refX) - (fontSize*scalingFactor*0.072f);
    float pPositionY = refToScreenY(refY) - (fontSize*scalingFactor*0.212f);

    QFont font("Bitstream Vera Sans");
    // Enforce minimum font size of 5 pixels
    int fSize = qMax(5, (int)(fontSize*scalingFactor*1.26f));
    font.setPixelSize(fSize);

    QFontMetrics metrics = QFontMetrics(font);
    int border = qMax(4, metrics.leading());
    QRect rect = metrics.boundingRect(0, 0, width() - 2*border, int(height()*0.125),
                                      Qt::AlignLeft | Qt::TextWordWrap, text);
    painter->setPen(color);
    painter->setFont(font);
    painter->setRenderHint(QPainter::TextAntialiasing);
    painter->drawText(pPositionX, pPositionY,
                      rect.width(), rect.height(),
                      Qt::AlignCenter | Qt::TextWordWrap, text);
    painter->setPen(prevPen);
}

void OverlayData::initializeGL()
{
    bool antialiasing = true;

    // Antialiasing setup
    if(antialiasing)
    {
        glEnable(GL_MULTISAMPLE);
        glEnable(GL_BLEND);

        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_POINT_SMOOTH);
        glEnable(GL_LINE_SMOOTH);

        glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    }
    else
    {
        glDisable(GL_BLEND);
        glDisable(GL_POINT_SMOOTH);
        glDisable(GL_LINE_SMOOTH);
    }
}

void OverlayData::setupGLView(float referencePositionX, float referencePositionY, float referenceWidth, float referenceHeight)
{
    int pixelWidth  = (int)(referenceWidth * scalingFactor);
    int pixelHeight = (int)(referenceHeight * scalingFactor);
    // Translate and scale the GL view in the virtual reference coordinate units on the screen
    int pixelPositionX = (int)((referencePositionX * scalingFactor) + xCenterOffset);
    int pixelPositionY = this->height() - (referencePositionY * scalingFactor) + yCenterOffset - pixelHeight;

    //qDebug() << "Pixel x" << pixelPositionX << "pixelY" << pixelPositionY;
    //qDebug() << "xCenterOffset:" << xCenterOffset << "yCenterOffest" << yCenterOffset


    //The viewport is established at the correct pixel position and clips everything
    // out of the desired instrument location
    glViewport(pixelPositionX, pixelPositionY, pixelWidth, pixelHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // The ortho projection is setup in a way that so that the drawing is done in the
    // reference coordinate space
    glOrtho(0, referenceWidth, 0, referenceHeight, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //glScalef(scaleX, scaleY, 1.0f);
}

//void OverlayData::paintRollPitchStrips()
//{
//}


void OverlayData::paintEvent(QPaintEvent *event)
{
    // Event is not needed
    // the event is ignored as this widget
    // is refreshed automatically
    Q_UNUSED(event);
}

void OverlayData::paintHUD()
{
    if (isVisible())
    {
        makeCurrent();

        if (videoEnabled)
        {
            cv::Mat frame;

            if(!captureRTSP.read(frame))
            {
                qDebug()  << "No frame" ;
                cv::waitKey();
            }

            if (video == NULL )
            {
                QRect imageSize;
                imageSize.setWidth(captureRTSP.get(CV_CAP_PROP_FRAME_WIDTH));
                imageSize.setHeight(captureRTSP.get(CV_CAP_PROP_FRAME_HEIGHT));

                qDebug()<<"width: "<< captureRTSP.get(CV_CAP_PROP_FRAME_WIDTH);
                qDebug()<<"height: "<< captureRTSP.get(CV_CAP_PROP_FRAME_HEIGHT);

                video = new videoStabilizer(imageSize);
                connect(video,SIGNAL(gotDuration(double&)), this, SLOT(updateTimeLabel(double&)));
            }

            if(captureRTSP.isOpened())
            {
                cv::cvtColor(frame,frame, CV_BGR2GRAY);
                cv::Mat output = cv::Mat::zeros(frame.rows, frame.cols, CV_8UC1);

                if(enableStabilizer == Qt::Checked)
                {
                    video->stabilizeImage(frame,output);
                }
                else
                {
                    output = frame;
                }
                cv::cvtColor(output,output, CV_GRAY2RGB);

                if(enableFilter == Qt::Checked)
                {
                }

                if(isRecord)
                {
                    if(!existFileMovie)
                    {
                        QString path = "/"+QTime::currentTime().toString("hhmmss")+".avi";
                        writerMovie.open(path.toAscii().data(), CV_FOURCC('D','I','V','X'), 30, frame.size(), true);

                        existFileMovie = true;
                    }

                    writerMovie << output;
                }


                glImage = QImage((const unsigned char*)(output.data), output.cols, output.rows, QImage::Format_RGB888);
            }            
        }
        else
        {            
            paintCenterBackground(0, 0, 0);
        }

        // END OF OPENGL PAINTING

        if (hudInstrumentsEnabled)
        {
            // Update scaling factor
            // adjust scaling to fit both horizontally and vertically
            scalingFactor = this->width()/vwidth;
            double scalingFactorH = this->height()/vheight;
            if (scalingFactorH < scalingFactor)
                scalingFactor = scalingFactorH;

            QPainter painter;
            painter.begin(this);
            painter.setRenderHint(QPainter::Antialiasing, true);
            painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
            painter.translate((this->vwidth/2.0+xCenterOffset)*scalingFactor, (this->vheight/2.0+yCenterOffset)*scalingFactor);

            painter.drawImage(refToScreenX((-vwidth/2.0)), refToScreenY(-vheight/2.0), glImage.scaled(this->width(), this->height(), Qt::KeepAspectRatioByExpanding));

            // COORDINATE FRAME IS NOW (0,0) at CENTER OF WIDGET            
            QString latitude("Distancia: %1 km");
            paintText(latitude.arg(viewTime(), 4, 'f', 1, '0'), infoColor, 3.0f, (-vwidth/2.0) + 10, -vheight/2.0 + 5, &painter);

            QString speed("Bateria: %1 v");
            paintText(speed.arg(viewTime(), 4, 'f', 1, '0'), infoColor, 3.0f, (-vwidth/2.0) + 50, -vheight/2.0 + 5, &painter);

            QString speed2("T. Vuelo: %1");
            paintText(speed2.arg(QTime::currentTime().toString("HH:mm:ss")), infoColor, 3.0f, (-vwidth/2.0) + 90, -vheight/2.0 + 5, &painter);


            QString speed4("Latitud: %1 N");
            paintText(speed4.arg(viewTime(), 4, 'f', 1, '0'), infoColor, 3.0f, (-vwidth/2.0) + 10, vheight/2 - 5, &painter);

            QString speed5("Longitud: %1 O");
            paintText(speed5.arg(viewTime(), 4, 'f', 1, '0'), infoColor, 3.0f, (-vwidth/2.0) + 50, vheight/2 - 5, &painter);

            QString altitude("Altura: %1 m");
            paintText(altitude.arg(viewTime(), 4, 'f', 1, '0'), infoColor, 3.0f, (-vwidth/2.0) + 90, vheight/2 - 5, &painter);

            const float centerWidth = 4.0f;
            const float centerCrossWidth = 10.0f;

            painter.setPen(defaultColor);
            painter.drawLine(QPointF(refToScreenX(-centerWidth / 1.0f), refToScreenY(0.0f)), QPointF(refToScreenX(-centerCrossWidth / 1.0f), refToScreenY(0.0f)));
            // right
            painter.drawLine(QPointF(refToScreenX(centerWidth / 1.0f), refToScreenY(0.0f)), QPointF(refToScreenX(centerCrossWidth / 1.0f), refToScreenY(0.0f)));

            painter.drawLine(QPointF(refToScreenX(0.0f), refToScreenY(-centerWidth / 1.0f)), QPointF(refToScreenX(0.0f), refToScreenY(-centerCrossWidth / 1.0f)));
            painter.drawLine(QPointF(refToScreenX(0.0f), refToScreenY(+centerWidth / 1.0f)), QPointF(refToScreenX(0.0f), refToScreenY(+centerCrossWidth / 1.0f)));

            drawVerticalIndicator(-90.0f, -60.0f, 120.0f, -90.0f, 90.0f, viewTime(), &painter);
            drawHorizontalIndicator(-50.0f, vheight/2 - 15, 120.0f, -180.0f, 180.0f, viewTime(), &painter);

            painter.end();
        }
        else
        {
            QPainter painter;
            painter.begin(this);
            painter.drawImage(0,0, glImage.scaled(this->width(), this->height(), Qt::KeepAspectRatioByExpanding));
            painter.end();
        }
    }
}

float OverlayData::refLineWidthToPen(float line)
{
    return line * 2.50f;
}

void OverlayData::drawPolygon(QPolygonF refPolygon, QPainter* painter)
{
    // Scale coordinates
    QPolygonF draw(refPolygon.size());
    for (int i = 0; i < refPolygon.size(); i++)
    {
        QPointF curr;
        curr.setX(refToScreenX(refPolygon.at(i).x()));
        curr.setY(refToScreenY(refPolygon.at(i).y()));
        draw.replace(i, curr);
    }
    painter->drawPolygon(draw);
}

void OverlayData::drawVerticalIndicator(float xRef, float yRef, float height, float minRate, float maxRate, float value, QPainter* painter)
{
    QBrush brush(defaultColor, Qt::NoBrush);
    painter->setBrush(brush);
    QPen rectPen(Qt::SolidLine);
    rectPen.setWidth(0);
    rectPen.setColor(defaultColor);
    painter->setPen(rectPen);

    float scaledValue = value;

    // Saturate value
    if (value > maxRate) scaledValue = maxRate;
    if (value < minRate) scaledValue = minRate;

    //           x (Origin: xRef, yRef)
    //           -
    //           |
    //           |
    //           |
    //           =
    //           |
    //   -0.005 >|
    //           |
    //           -

    const float width = height / 8.0f;
    const float lineWidth = 0.5f;

    // Indicator lines
    // Top horizontal line
    drawLine(xRef-width/4.0f, yRef, xRef+width/4.0f, yRef, lineWidth, defaultColor, painter);
    // Vertical main line
    //drawLine(xRef+width/2.0f, yRef, xRef+width/2.0f, yRef+height, lineWidth, defaultColor, painter);
    drawLine(xRef, yRef, xRef, yRef+height, lineWidth, defaultColor, painter);
    // Zero mark
    drawLine(xRef-width/4.0f, yRef+height/2.0f, xRef+width/4.0f, yRef+height/2.0f, lineWidth, defaultColor, painter);
    // Horizontal bottom line
    drawLine(xRef-width/4.0f, yRef+height, xRef+width/4.0f, yRef+height, lineWidth, defaultColor, painter);

    // Text
    QString label;
    label.sprintf("< %+06.2f", value);
    paintText(label, defaultColor, 3.0f, xRef+1.0f, yRef+height-((scaledValue - minRate)/(maxRate-minRate))*height - 1.6f, painter);
}

void OverlayData::drawHorizontalIndicator(float xRef, float yRef, float height, float minRate, float maxRate, float value, QPainter* painter)
{
    QBrush brush(defaultColor, Qt::NoBrush);
    painter->setBrush(brush);
    QPen rectPen(Qt::SolidLine);
    rectPen.setWidth(0);
    rectPen.setColor(defaultColor);
    painter->setPen(rectPen);

    float scaledValue = -value;

    // Saturate value
    if (value > maxRate) scaledValue = maxRate;
    if (value < minRate) scaledValue = minRate;

    //                                      v
    //           x (Origin: xRef, yRef) |-------|-------|

    const float width = height / 8.0f;
    const float lineWidth = 0.5f;

    //line main horizontal
    drawLine(xRef, yRef, xRef+height, yRef, lineWidth, defaultColor, painter);
    drawLine(xRef, yRef+width/4.0f, xRef, yRef-width/4.0f, lineWidth, defaultColor, painter);
    drawLine(xRef+height/2.0f, yRef+width/4.0f, xRef+height/2.0f, yRef-width/4.0f, lineWidth, defaultColor, painter);
    drawLine(xRef+height, yRef+width/4.0f, xRef+height, yRef-width/4.0f, lineWidth, defaultColor, painter);

    // Text
    QString label;
    label.sprintf("%+06.2f", value);
    paintText(label, defaultColor, 3.0f, xRef+height-((scaledValue - minRate)/(maxRate-minRate))*height - 4.8f, yRef-width/2.0f - 2.0f, painter);

    QString label2;
    label2.sprintf("v", value);
    paintText(label2, defaultColor, 3.0f, xRef+height-((scaledValue - minRate)/(maxRate-minRate))*height - 1.6, yRef-4.0f, painter);
}

void OverlayData::drawLine(float refX1, float refY1, float refX2, float refY2, float width, const QColor& color, QPainter* painter)
{
    QPen pen(Qt::SolidLine);
    pen.setWidth(refLineWidthToPen(width));
    pen.setColor(color);
    painter->setPen(pen);
    painter->drawLine(QPoint(refToScreenX(refX1), refToScreenY(refY1)), QPoint(refToScreenX(refX2), refToScreenY(refY2)));
}

void OverlayData::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //    //glOrtho(0, w, 0, h, -1, 1);
    //    glMatrixMode(GL_PROJECTION);
    //    glPolygonMode(GL_NONE, GL_FILL);
    //FIXME
    //paintHUD();
}

void OverlayData::enableHUDInstruments(bool enabled)
{
    hudInstrumentsEnabled = enabled;
}

void OverlayData::openFile()
{
    QString filename = QFileDialog::getOpenFileName(this, "Open Video", "Video Files (*.mp4, *.mpg)");

    if(!filename.isEmpty())
    {
        setURL(filename);
    }
}

void OverlayData::openRTSP()
{
    bool ok;
    QString filename = QInputDialog::getText(this, tr("Ingrese la URL"), tr("URL RTSP Axis:"), QLineEdit::Normal, tr("rtsp://192.168.1.90:554/axis-media/media.amp"), &ok);

    if (ok && !filename.isEmpty())
    {
        setURL(filename);
    }
}

void OverlayData::record()
{
    isRecord = !isRecord;

    if(!isRecord)
    {
        existFileMovie = false;
    }
}

void OverlayData::setURL(QString url)
{
    captureRTSP.release();

    if(!captureRTSP.open(url.toAscii().data()))
    {
        qDebug() << "Error opening video stream or file";
        return;
    }

    emit emitTitle(url);
}

void OverlayData::playMovie()
{
    refreshTimer->start(updateInterval);
}

void OverlayData::stopMovie()
{
    refreshTimer->stop();
}

double OverlayData::viewTime()
{
    //hh:mm:ss.zzz 14:13:09.042
    QString timeFormat = "ss.zzz";
    return QTime::currentTime().toString(timeFormat).toDouble();//QString::number(time);
}

void OverlayData::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        xMouse = event->x();
        yMouse = event->y();

        qDebug()<<"X: "<<xMouse;
        qDebug()<<"Y: "<<xMouse;

        if(!glImage.isNull())
        {
            emit emitCaptureImage(glImage.copy(xMouse, yMouse, 40, 40));
        }
    }

    QWidget::mousePressEvent(event);
}

void OverlayData::setEnableFilter(int enable)
{
    this->enableFilter = enable;
}

void OverlayData::setEnableStabilizer(int enable)
{
    this->enableStabilizer = enable;
}

void OverlayData::setSizeKernel(int value)
{
    int kernel = 1;
    switch(value)
    {
    case 1:
        kernel = 1;
        break;
    case 2:
        kernel = 3;
        break;
    case 3:
        kernel = 5;
        break;
    case 4:
        kernel = 7;
        break;
    case 5:
        kernel = 9;
        break;
    case 6:
        kernel = 11;
        break;
    case 7:
        kernel = 13;
        break;
    case 8:
        kernel = 15;
        break;
    case 9:
        kernel = 17;
        break;
    case 10:
        kernel = 19;
        break;
    case 11:
        kernel = 21;
        break;
    case 12:
        kernel = 23;
        break;
    }
    this->sizeKernel = kernel;
    qDebug()<<sizeKernel;
}
