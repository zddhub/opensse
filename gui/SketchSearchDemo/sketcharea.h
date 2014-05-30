/*************************************************************************
 * Copyright (c) 2014 Zhang Dongdong
 * All rights reserved.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
**************************************************************************/
#ifndef SKETCHAREA_H
#define SKETCHAREA_H

#include <QWidget>

#include <QColor>
#include <QImage>
#include <QPoint>
#include <QWidget>
#include <QMouseEvent>
#include <QKeyEvent>

/**
 * @brief The SketchArea class
 * draw sketch panel
 * when mouse release, emit newSketchDone signal
 * using setAutoSaveSketch save sketch image.
 */
class SketchArea : public QWidget
{
    Q_OBJECT
public:
    explicit SketchArea(QWidget *parent = 0);
    bool openImage(const QString &fileName);
    bool saveImage(const QString &fileName, const char *fileFormat);
    void setPenColor(const QColor &newColor);
    void setPenWidth(int newWidth);
    void setEraserColor(const QColor &newColor);
    void setEraserWidth(int newWidth);

    /**
     * @brief setAutoSaveSketch
     * @param save is true, when you want to save sketch,else false;
     * @param fileName
     */
    void setAutoSaveSketch(bool save, const QString &fileName = "temp.jpg");

    QString sketchPath() const { return saveSketchFileName; }
    bool isModified() const { return modified; }
    QColor penColor() const { return myPenColor; }
    int penWidth() const { return myPenWidth; }
    QColor eraserColor() const { return myEraserColor; }
    int eraserWidth() const { return myEraserWidth; }
signals:
    void newSketchDone(const QString &fileName);
    void clearSketchDone();
public slots:
    void clearImage();
    void print();

protected:
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    void drawLineTo(const QPoint &endPoint);
    void resizeImage(QImage *image, const QSize &newSize);

    bool autoSaveSketch;
    QString saveSketchFileName;

    bool modified;
    bool scribbling;
    int myPenWidth;
    QColor myPenColor;

    bool erasing;
    int myEraserWidth;
    QColor myEraserColor;

    QImage image;
    QPoint lastPoint;
};

#endif // SKETCHAREA_H
