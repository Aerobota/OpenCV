/*=====================================================================

QGroundControl Open Source Ground Control Station

(c) 2009, 2010 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>

This file is part of the QGROUNDCONTROL project

    QGROUNDCONTROL is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QGROUNDCONTROL is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QGROUNDCONTROL. If not, see <http://www.gnu.org/licenses/>.

======================================================================*/

/**
 * @file
 *   @brief Definition of Head up display
 *
 *   @author Lorenz Meier <mavteam@student.ethz.ch>
 *
 */

#ifndef HUD_H
#define HUD_H

#include <QImage>
#include <QGLWidget>
#include <QPainter>
#include <QFontDatabase>
#include <QTimer>
#include <QTime>
#include <QInputDialog>
#include <QShowEvent>
#include <QContextMenuEvent>
#include <QMenu>
#include <QDesktopServices>
#include <QFileDialog>

#include <QDebug>
#include <cmath>
#include <qmath.h>
#include <limits>

#include "core/core.hpp"
#include "highgui/highgui.hpp"
#include "../videoStabilizer.h"
#include "imgproc/imgproc.hpp"

/**
 * @brief Displays a Head Up Display (HUD)
 *
 * This class represents a head up display (HUD) and draws this HUD in an OpenGL widget (QGLWidget).
 * It can superimpose the HUD over the current live image stream (any arriving image stream will be auto-
 * matically used as background), or it draws the classic blue-brown background known from instruments.
 */
class OverlayData : public QGLWidget
{
    Q_OBJECT
public:
    /**
     * @warning The HUD widget will not start painting its content automatically
     *          to update the view, start the auto-update by calling HUD::start().
     *
     * @param width
     * @param height
     * @param parent
     */
    OverlayData(int width = 640, int height = 480, QWidget* parent = NULL);
    ~OverlayData();

    void resizeGL(int w, int h);

    int xMouse, yMouse;
    cv::VideoCapture captureRTSP;
    cv::VideoWriter writerMovie;
    /** This is the video stabilizer algorithm class*/
    videoStabilizer* video;
    bool isRecord;
    bool existFileMovie;

    void mousePressEvent(QMouseEvent *);

public slots:
    void initializeGL();    
    /** @brief Enable the HUD instruments */
    void enableHUDInstruments(bool enabled);

protected slots:
    /**
     * This functions works in the OpenGL view, which is already translated by
     * the x and y center offsets.
     *
     */
    void paintCenterBackground(float roll, float pitch, float yaw);
    /**
     * Paint text on top of the image and OpenGL drawings
     *
     * @param text chars to write
     * @param color text color
     * @param fontSize text size in mm
     * @param refX position in reference units (mm of the real instrument). This is relative to the measurement unit position, NOT in pixels.
     * @param refY position in reference units (mm of the real instrument). This is relative to the measurement unit position, NOT in pixels.
     */
    void paintText(QString text, QColor color, float fontSize, float refX, float refY, QPainter* painter);
    /** @brief Setup the OpenGL view for drawing a sub-component of the HUD
     * @param referencePositionX horizontal position in the reference mm-unit space
     * @param referencePositionY horizontal position in the reference mm-unit space
     * @param referenceWidth width in the reference mm-unit space
     * @param referenceHeight width in the reference mm-unit space
     */
    void setupGLView(float referencePositionX, float referencePositionY, float referenceWidth, float referenceHeight);
    void paintHUD();        
    void drawLine(float refX1, float refY1, float refX2, float refY2, float width, const QColor& color, QPainter* painter);    
    void drawVerticalIndicator(float xRef, float yRef, float height, float minRate, float maxRate, float value, QPainter* painter);
    void drawHorizontalIndicator(float xRef, float yRef, float height, float minRate, float maxRate, float value, QPainter* painter);
    void drawPolygon(QPolygonF refPolygon, QPainter* painter);

    void playMovie();
    void stopMovie();
    void openRTSP();
    void openFile();
    void record();
    void setURL(QString url);
    double viewTime();

protected:    
    /**
     * @param y coordinate in pixels to be converted to reference mm units
     * @return the screen coordinate relative to the QGLWindow origin
     */
    float refToScreenX(float x);
    /**
     * @param x coordinate in pixels to be converted to reference mm units
     * @return the screen coordinate relative to the QGLWindow origin
     */
    float refToScreenY(float y);
    /** @brief Convert mm line widths to QPen line widths */
    float refLineWidthToPen(float line);
    /** @brief Preferred Size */
    QSize sizeHint() const;
    /** @brief Start updating widget */
    void showEvent(QShowEvent* event);
    /** @brief Stop updating widget */
    void hideEvent(QHideEvent* event);
    void contextMenuEvent (QContextMenuEvent* event);
    void createActions();

    static const int updateInterval = 40;

    QImage glImage; ///< The background / camera image    
    float yawInt; ///< The yaw integral. Used to damp the yaw indication.
    QString mode; ///< The current vehicle mode
    QString state; ///< The current vehicle state
    double scalingFactor; ///< Factor used to scale all absolute values to screen coordinates
    float xCenterOffset, yCenterOffset; ///< Offset from center of window in mm coordinates
    float vwidth; ///< Virtual width of this window, 200 mm per default. This allows to hardcode positions and aspect ratios. This virtual image plane is then scaled to the window size.
    float vheight; ///< Virtual height of this window, 150 mm per default
    int xCenter; ///< Center of the HUD instrument in pixel coordinates. Allows to off-center the whole instrument in its OpenGL window, e.g. to fit another instrument
    int yCenter; ///< Center of the HUD instrument in pixel coordinates. Allows to off-center the whole instrument in its OpenGL window, e.g. to fit another instrument
    QColor defaultColor;       ///< Color for most HUD elements, e.g. pitch lines, center cross, change rate gauges
    QColor infoColor;          ///< Color for normal/default messages
    QTimer* refreshTimer;      ///< The main timer, controls the update rate
    QFont font;                ///< The HUD font, per default the free Bitstream Vera SANS, which is very close to actual HUD fonts
    float roll;
    float pitch;
    float yaw;
    float rollLP;
    float pitchLP;
    float yawLP;
    double yawDiff;
    double lat;
    double lon;
    double alt;    
    bool hudInstrumentsEnabled;
    bool videoEnabled;

    QAction* enableHUDAction;
    QAction* selectVideoChannelAction;
    void paintEvent(QPaintEvent *event);    

signals:
    void emitCaptureImage(QImage img);
    void emitTitle(QString title);
};

#endif // HUD_H
