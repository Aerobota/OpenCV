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
    fuelStatus(tr("00.0V (00m:00s)")),
    xCenterOffset(0.0f),
    yCenterOffset(0.0f),
    vwidth(200.0f),
    vheight(150.0f),
    vGaugeSpacing(50.0f),
    vPitchPerDeg(6.0f), ///< 4 mm y translation per degree)
    rawBuffer1(NULL),
    rawBuffer2(NULL),
    rawImage(NULL),
    rawLastIndex(0),
    rawExpectedBytes(0),
    bytesPerLine(1),
    imageStarted(false),
    receivedDepth(8),
    receivedChannels(1),
    receivedWidth(640),
    receivedHeight(480),
    defaultColor(QColor(255, 255, 255)),
    setPointColor(QColor(255, 255, 255)),
    warningColor(Qt::yellow),
    criticalColor(Qt::red),
    infoColor(QColor(255, 255, 255)),
    fuelColor(criticalColor),
    warningBlinkRate(5),
    refreshTimer(new QTimer(this)),
    noCamera(true),
    hardwareAcceleration(true),
    strongStrokeWidth(1.5f),
    normalStrokeWidth(1.0f),
    fineStrokeWidth(0.5f),
    waypointName(""),
    roll(0.0f),
    pitch(0.0f),
    yaw(0.0f),
    rollLP(0.0f),
    pitchLP(0.0f),
    yawLP(0.0f),
    yawDiff(0.0f),
    xPos(0.0),
    yPos(0.0),
    zPos(0.0),
    xSpeed(0.0),
    ySpeed(0.0),
    zSpeed(0.0),
    lastSpeedUpdate(0),
    totalSpeed(0.0),
    totalAcc(0.0),
    lat(0.0),
    lon(0.0),
    alt(0.0),
    load(0.0f),
    offlineDirectory(""),
    nextOfflineImage(""),
    hudInstrumentsEnabled(false),
    videoEnabled(true),
    xImageFactor(1.0),
    yImageFactor(1.0),
    video(NULL),
    isRecord(false),
    existFileMovie(false)
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

    //captureRTSP.open("/Volumes/HDD_120/vuelos/09022012/20120209093344.mp4");

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
    glTranslatef(0.0f, (-pitch * vPitchPerDeg * 16.5f), 0.0f);

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

void OverlayData::paintRollPitchStrips()
{
}


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
        // Read out most important values to limit hash table lookups
        // Low-pass roll, pitch and yaw
        rollLP = rollLP * 0.2f + 0.8f * roll;
        pitchLP = pitchLP * 0.2f + 0.8f * pitch;
        yawLP = yawLP * 0.2f + 0.8f * yaw;

        // Translate for yaw
        const float maxYawTrans = 60.0f;

        float newYawDiff = yawDiff;
        if (isinf(newYawDiff)) newYawDiff = yawDiff;
        if (newYawDiff > M_PI) newYawDiff = newYawDiff - M_PI;

        if (newYawDiff < -M_PI) newYawDiff = newYawDiff + M_PI;

        newYawDiff = yawDiff * 0.8 + newYawDiff * 0.2;

        yawDiff = newYawDiff;

        yawInt += newYawDiff;

        if (yawInt > M_PI) yawInt = (float)M_PI;
        if (yawInt < -M_PI) yawInt = (float)-M_PI;

        float yawTrans = yawInt * (float)maxYawTrans;
        yawInt *= 0.6f;

        if ((yawTrans < 5.0) && (yawTrans > -5.0)) yawTrans = 0;

        // Negate to correct direction
        yawTrans = -yawTrans;

        //qDebug() << "yaw translation" << yawTrans << "integral" << yawInt << "difference" << yawDiff << "yaw" << yaw;

        // Update scaling factor
        // adjust scaling to fit both horizontally and vertically
        scalingFactor = this->width()/vwidth;
        double scalingFactorH = this->height()/vheight;
        if (scalingFactorH < scalingFactor) scalingFactor = scalingFactorH;



        // OPEN GL PAINTING
        // Store model view matrix to be able to reset it to the previous state
        makeCurrent();
        //        glMatrixMode(GL_MODELVIEW);
        //        glPushMatrix();
        //        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Fill with black background
        if (videoEnabled)
        {
            cv::Mat frame;

            if(!captureRTSP.read(frame))
            {
                qDebug()  << "No frame" ;
                //refreshTimer->stop();
                //captureRTSP.release();
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

                //glPixelZoom(1.0f, 1.0f);
            }

            if(captureRTSP.isOpened())
            {
                cv::cvtColor(frame,frame, CV_BGR2GRAY);
                cv::Mat output = cv::Mat::zeros(frame.rows, frame.cols, CV_8UC1);
                video->stabilizeImage(frame,output);
                cv::cvtColor(output,output, CV_GRAY2RGB);

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
            //glDrawPixels(glImage.width(), glImage.height(), GL_RGB, GL_UNSIGNED_BYTE, glImage.bits());
        }
        else
        {
            // Blue / brown background
            paintCenterBackground(roll, pitch, yawTrans);
        }

        //        glMatrixMode(GL_MODELVIEW);
        //        glPopMatrix();

        // END OF OPENGL PAINTING

        if (hudInstrumentsEnabled)
        {

            //glEnable(GL_MULTISAMPLE);

            // QT PAINTING
            //makeCurrent();
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

void OverlayData::paintPitchLines(float pitch, QPainter* painter)
{
    QString label;

    const float yDeg = vPitchPerDeg;
    const float lineDistance = 5.0f; ///< One pitch line every 10 degrees
    const float posIncrement = yDeg * lineDistance;
    float posY = posIncrement;
    const float posLimit = sqrt(pow(vwidth, 2.0f) + pow(vheight, 2.0f));

    const float offsetAbs = pitch * yDeg;

    float offset = pitch;
    if (offset < 0) offset = -offset;
    int offsetCount = 0;
    while (offset > lineDistance)
    {
        offset -= lineDistance;
        offsetCount++;
    }

    int iPos = (int)(0.5f + lineDistance); ///< The first line
    int iNeg = (int)(-0.5f - lineDistance); ///< The first line

    offset *= yDeg;


    painter->setPen(defaultColor);

    posY = -offsetAbs + posIncrement; //+ 100;// + lineDistance;

    while (posY < posLimit)
    {
        paintPitchLinePos(label.sprintf("%3d", iPos), 0.0f, -posY, painter);
        posY += posIncrement;
        iPos += (int)lineDistance;
    }



    // HORIZON
    //
    //    ------------    ------------
    //
    const float pitchWidth = 30.0f;
    const float pitchGap = pitchWidth / 2.5f;
    const QColor horizonColor = defaultColor;
    const float diagonal = sqrt(pow(vwidth, 2.0f) + pow(vheight, 2.0f));
    const float lineWidth = refLineWidthToPen(0.5f);

    // Left horizon
    //drawLine(0.0f-diagonal, offsetAbs, 0.0f-pitchGap/2.0f, offsetAbs, lineWidth, horizonColor, painter);
    // Right horizon
    //drawLine(0.0f+pitchGap/2.0f, offsetAbs, 0.0f+diagonal, offsetAbs, lineWidth, horizonColor, painter);

    drawLine(0.0f-diagonal, offsetAbs, 0.0f-pitchGap/2.0f, offsetAbs, lineWidth, horizonColor, painter);
    drawLine(0.0f+pitchGap/2.0f, offsetAbs, 0.0f+diagonal, offsetAbs, lineWidth, horizonColor, painter);

    label.clear();

    posY = offsetAbs  + posIncrement;


    while (posY < posLimit)
    {
        paintPitchLineNeg(label.sprintf("%3d", iNeg), 0.0f, posY, painter);
        posY += posIncrement;
        iNeg -= (int)lineDistance;
    }
}

void OverlayData::paintPitchLinePos(QString text, float refPosX, float refPosY, QPainter* painter)
{
    //painter->setPen(QPen(QBrush, normalStrokeWidth));

    const float pitchWidth = 30.0f;
    const float pitchGap = pitchWidth / 2.5f;
    const float pitchHeight = pitchWidth / 12.0f;
    const float textSize = pitchHeight * 1.1f;
    const float lineWidth = 0.5f;

    // Positive pitch indicator:
    //
    //      _______      _______
    //     |10                  |
    //

    // Left vertical line
    drawLine(refPosX-pitchWidth/2.0f, refPosY, refPosX-pitchWidth/2.0f, refPosY+pitchHeight, lineWidth, defaultColor, painter);
    // Left horizontal line
    drawLine(refPosX-pitchWidth/2.0f, refPosY, refPosX-pitchGap/2.0f, refPosY, lineWidth, defaultColor, painter);
    // Text left
    paintText(text, defaultColor, textSize, refPosX-pitchWidth/2.0 + 0.75f, refPosY + pitchHeight - 1.75f, painter);

    // Right vertical line
    drawLine(refPosX+pitchWidth/2.0f, refPosY, refPosX+pitchWidth/2.0f, refPosY+pitchHeight, lineWidth, defaultColor, painter);
    // Right horizontal line
    drawLine(refPosX+pitchWidth/2.0f, refPosY, refPosX+pitchGap/2.0f, refPosY, lineWidth, defaultColor, painter);
}

void OverlayData::paintPitchLineNeg(QString text, float refPosX, float refPosY, QPainter* painter)
{
    const float pitchWidth = 30.0f;
    const float pitchGap = pitchWidth / 2.5f;
    const float pitchHeight = pitchWidth / 12.0f;
    const float textSize = pitchHeight * 1.1f;
    const float segmentWidth = ((pitchWidth - pitchGap)/2.0f) / 7.0f; ///< Four lines and three gaps -> 7 segments

    const float lineWidth = 0.1f;

    // Negative pitch indicator:
    //
    //      -10
    //     _ _ _ _|     |_ _ _ _
    //
    //

    // Left vertical line
    drawLine(refPosX-pitchGap/2.0, refPosY, refPosX-pitchGap/2.0, refPosY-pitchHeight, lineWidth, defaultColor, painter);
    // Left horizontal line with four segments
    for (int i = 0; i < 7; i+=2)
    {
        drawLine(refPosX-pitchWidth/2.0+(i*segmentWidth), refPosY, refPosX-pitchWidth/2.0+(i*segmentWidth)+segmentWidth, refPosY, lineWidth, defaultColor, painter);
    }
    // Text left
    paintText(text, defaultColor, textSize, refPosX-pitchWidth/2.0f + 0.75f, refPosY + pitchHeight - 1.75f, painter);

    // Right vertical line
    drawLine(refPosX+pitchGap/2.0, refPosY, refPosX+pitchGap/2.0, refPosY-pitchHeight, lineWidth, defaultColor, painter);
    // Right horizontal line with four segments
    for (int i = 0; i < 7; i+=2)
    {
        drawLine(refPosX+pitchWidth/2.0f-(i*segmentWidth), refPosY, refPosX+pitchWidth/2.0f-(i*segmentWidth)-segmentWidth, refPosY, lineWidth, defaultColor, painter);
    }
}

void rotatePointClockWise(QPointF& p, float angle)
{
    // Standard 2x2 rotation matrix, counter-clockwise
    //
    //   |  cos(phi)   sin(phi) |
    //   | -sin(phi)   cos(phi) |
    //

    //p.setX(cos(angle) * p.x() + sin(angle) * p.y());
    //p.setY(-sin(angle) * p.x() + cos(angle) * p.y());


    p.setX(cos(angle) * p.x() + sin(angle)* p.y());
    p.setY((-1.0f * sin(angle) * p.x()) + cos(angle) * p.y());
}

float OverlayData::refLineWidthToPen(float line)
{
    return line * 2.50f;
}

void OverlayData::rotatePolygonClockWiseRad(QPolygonF& p, float angle, QPointF origin)
{
    // Standard 2x2 rotation matrix, counter-clockwise
    //
    //   |  cos(phi)   sin(phi) |
    //   | -sin(phi)   cos(phi) |
    //
    for (int i = 0; i < p.size(); i++)
    {
        QPointF curr = p.at(i);

        const float x = curr.x();
        const float y = curr.y();

        curr.setX(((cos(angle) * (x-origin.x())) + (-sin(angle) * (y-origin.y()))) + origin.x());
        curr.setY(((sin(angle) * (x-origin.x())) + (cos(angle) * (y-origin.y()))) + origin.y());
        p.replace(i, curr);
    }
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

void OverlayData::drawChangeIndicatorGauge(float xRef, float yRef, float radius, float expectedMaxChange, float value, const QColor& color, QPainter* painter, bool solid)
{
    // Draw the circle
    QPen circlePen(Qt::SolidLine);
    if (!solid) circlePen.setStyle(Qt::DotLine);
    circlePen.setWidth(refLineWidthToPen(0.5f));
    circlePen.setColor(defaultColor);
    painter->setBrush(Qt::NoBrush);
    painter->setPen(circlePen);
    drawCircle(xRef, yRef, radius, 200.0f, 170.0f, 1.0f, color, painter);

    QString label;
    label.sprintf("%05.1f", value);

    // Draw the value
    paintText(label, color, 4.5f, xRef-7.5f, yRef-2.0f, painter);

    // Draw the needle
    // Scale the rotation so that the gauge does one revolution
    // per max. change
    const float rangeScale = (2.0f * M_PI) / expectedMaxChange;
    const float maxWidth = radius / 10.0f;
    const float minWidth = maxWidth * 0.3f;

    QPolygonF p(6);

    p.replace(0, QPointF(xRef-maxWidth/2.0f, yRef-radius * 0.5f));
    p.replace(1, QPointF(xRef-minWidth/2.0f, yRef-radius * 0.9f));
    p.replace(2, QPointF(xRef+minWidth/2.0f, yRef-radius * 0.9f));
    p.replace(3, QPointF(xRef+maxWidth/2.0f, yRef-radius * 0.5f));
    p.replace(4, QPointF(xRef,               yRef-radius * 0.46f));
    p.replace(5, QPointF(xRef-maxWidth/2.0f, yRef-radius * 0.5f));

    rotatePolygonClockWiseRad(p, value*rangeScale, QPointF(xRef, yRef));

    QBrush indexBrush;
    indexBrush.setColor(defaultColor);
    indexBrush.setStyle(Qt::SolidPattern);
    painter->setPen(Qt::SolidLine);
    painter->setPen(defaultColor);
    painter->setBrush(indexBrush);
    drawPolygon(p, painter);
}

void OverlayData::drawLine(float refX1, float refY1, float refX2, float refY2, float width, const QColor& color, QPainter* painter)
{
    QPen pen(Qt::SolidLine);
    pen.setWidth(refLineWidthToPen(width));
    pen.setColor(color);
    painter->setPen(pen);
    painter->drawLine(QPoint(refToScreenX(refX1), refToScreenY(refY1)), QPoint(refToScreenX(refX2), refToScreenY(refY2)));
}

void OverlayData::drawEllipse(float refX, float refY, float radiusX, float radiusY, float startDeg, float endDeg, float lineWidth, const QColor& color, QPainter* painter)
{
    Q_UNUSED(startDeg);
    Q_UNUSED(endDeg);
    QPen pen(painter->pen().style());
    pen.setWidth(refLineWidthToPen(lineWidth));
    pen.setColor(color);
    painter->setPen(pen);
    painter->drawEllipse(QPointF(refToScreenX(refX), refToScreenY(refY)), refToScreenX(radiusX), refToScreenY(radiusY));
}

void OverlayData::drawCircle(float refX, float refY, float radius, float startDeg, float endDeg, float lineWidth, const QColor& color, QPainter* painter)
{
    drawEllipse(refX, refY, radius, radius, startDeg, endDeg, lineWidth, color, painter);
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

void OverlayData::setImageSize(int width, int height, int depth, int channels)
{
    // Allocate raw image in correct size
    if (width != receivedWidth || height != receivedHeight || depth != receivedDepth || channels != receivedChannels || image == NULL)
    {
        // Set new size
        if (width > 0) receivedWidth  = width;
        if (height > 0) receivedHeight = height;
        if (depth > 1) receivedDepth = depth;
        if (channels > 1) receivedChannels = channels;

        rawExpectedBytes = (receivedWidth * receivedHeight * receivedDepth * receivedChannels) / 8;
        bytesPerLine = rawExpectedBytes / receivedHeight;
        // Delete old buffers if necessary
        rawImage = NULL;
        if (rawBuffer1 != NULL) delete rawBuffer1;
        if (rawBuffer2 != NULL) delete rawBuffer2;

        rawBuffer1 = (unsigned char*)malloc(rawExpectedBytes);
        rawBuffer2 = (unsigned char*)malloc(rawExpectedBytes);
        rawImage = rawBuffer1;
        // TODO check if old image should be deleted

        // Set image format
        // 8 BIT GREYSCALE IMAGE
        if (depth <= 8 && channels == 1)
        {
            image = new QImage(receivedWidth, receivedHeight, QImage::Format_Indexed8);
            // Create matching color table
            image->setNumColors(256);
            for (int i = 0; i < 256; i++)
            {
                image->setColor(i, qRgb(i, i, i));
                //qDebug() << __FILE__ << __LINE__ << std::hex << i;
            }

        }
        // 32 BIT COLOR IMAGE WITH ALPHA VALUES (#ARGB)
        else
        {
            image = new QImage(receivedWidth, receivedHeight, QImage::Format_ARGB32);
        }

        // Fill first channel of image with black pixels
        image->fill(0);
        glImage = QGLWidget::convertToGLFormat(*image);

        qDebug() << __FILE__ << __LINE__ << "Setting up image";

        // Set size once
        setFixedSize(receivedWidth, receivedHeight);
        setMinimumSize(receivedWidth, receivedHeight);
        setMaximumSize(receivedWidth, receivedHeight);
        // Lock down the size
        //setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
        //resize(receivedWidth, receivedHeight);
    }

}

void OverlayData::startImage(int imgid, int width, int height, int depth, int channels)
{
    Q_UNUSED(imgid);
    //qDebug() << "HUD: starting image (" << width << "x" << height << ", " << depth << "bits) with " << channels << "channels";

    // Copy previous image to screen if it hasn't been finished properly
    finishImage();

    // Reset image size if necessary
    setImageSize(width, height, depth, channels);
    imageStarted = true;
}

void OverlayData::finishImage()
{
    if (imageStarted)
    {
        commitRawDataToGL();
        imageStarted = false;
    }
}

void OverlayData::commitRawDataToGL()
{
    qDebug() << __FILE__ << __LINE__ << "Copying raw data to GL buffer:" << rawImage << receivedWidth << receivedHeight << image->format();
    if (image != NULL)
    {
        QImage::Format format = image->format();
        QImage* newImage = new QImage(rawImage, receivedWidth, receivedHeight, format);
        if (format == QImage::Format_Indexed8)
        {
            // Create matching color table
            newImage->setNumColors(256);
            for (int i = 0; i < 256; i++)
            {
                newImage->setColor(i, qRgb(i, i, i));
                //qDebug() << __FILE__ << __LINE__ << std::hex << i;
            }
        }

        glImage = QGLWidget::convertToGLFormat(*newImage);
        delete image;
        image = newImage;
        // Switch buffers
        if (rawImage == rawBuffer1)
        {
            rawImage = rawBuffer2;
            //qDebug() << "Now buffer 2";
        }
        else
        {
            rawImage = rawBuffer1;
            //qDebug() << "Now buffer 1";
        }
    }
    update();
}

void OverlayData::saveImage(QString fileName)
{
    image->save(fileName);
}

void OverlayData::saveImage()
{
    //Bring up popup
    QString fileName = "output.png";
    saveImage(fileName);
}

void OverlayData::startImage(quint64 timestamp)
{
    if (videoEnabled && offlineDirectory != "")
    {
        // Load and diplay image file
        nextOfflineImage = QString(offlineDirectory + "/%1.bmp").arg(timestamp);
    }
}

void OverlayData::enableHUDInstruments(bool enabled)
{
    hudInstrumentsEnabled = enabled;
}

void OverlayData::setPixels(int imgid, const unsigned char* imageData, int length, int startIndex)
{
    Q_UNUSED(imgid);
    //    qDebug() << "at" << __FILE__ << __LINE__ << ": Received startindex" << startIndex << "and length" << length << "(" << startIndex+length << "of" << rawExpectedBytes << "bytes)";

    if (imageStarted)
    {
        //if (rawLastIndex != startIndex) qDebug() << "PACKET LOSS!";

        if (startIndex+length > rawExpectedBytes)
        {
            qDebug() << "HUD: OVERFLOW! startIndex:" << startIndex << "length:" << length << "image raw size" << ((receivedWidth * receivedHeight * receivedChannels * receivedDepth) / 8) - 1;
        }
        else
        {
            memcpy(rawImage+startIndex, imageData, length);

            rawLastIndex = startIndex+length;

            // Check if we just reached the end of the image
            if (startIndex+length == rawExpectedBytes)
            {
                //qDebug() << "HUD: END OF IMAGE REACHED!";
                finishImage();
                rawLastIndex = 0;
            }
        }
    }
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
