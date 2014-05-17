#include "sketcharea.h"


#include <QPainter>
#include <QPrinter>
#include <QPrintDialog>

#include <QDebug>

SketchArea::SketchArea(QWidget *parent) :
    QWidget(parent)
{
    setAttribute(Qt::WA_StaticContents);
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setSizePolicy(sizePolicy);

    this->setFocusPolicy(Qt::StrongFocus);

    modified = false;

    scribbling = false;
    myPenWidth = 3;
    myPenColor = Qt::black;

    erasing = false;
    myEraserWidth = 25;
    myEraserColor = Qt::white;

    autoSaveSketch = false;
    saveSketchFileName = "temp.jpg";
}

bool SketchArea::openImage(const QString &fileName)
{
    QImage loadedImage;
    if (!loadedImage.load(fileName))
        return false;

    clearImage();

    loadedImage.save(saveSketchFileName, "JPG");

    QSize newSize;
    if(loadedImage.size().width() > size().width() || loadedImage.size().height() > size().height())
    {
        newSize = size();
        loadedImage = loadedImage.scaled(size().width(), size().height());
    }
    else
        newSize = loadedImage.size().expandedTo(size());
    resizeImage(&loadedImage, newSize);

    //Form Indexed8 to Format_RGB32
    image = loadedImage.convertToFormat(QImage::Format_ARGB32);

    modified = false;

    emit newSketchDone(fileName);
    update();
    return true;
}

bool SketchArea::saveImage(const QString &fileName, const char *fileFormat)
{
    QImage visibleImage = image;
    resizeImage(&visibleImage, size());

    if (visibleImage.save(fileName, fileFormat)) {
        modified = false;
        return true;
    } else {
        return false;
    }
}

void SketchArea::setPenColor(const QColor &newColor)
{
    myPenColor = newColor;
}

void SketchArea::setPenWidth(int newWidth)
{
    myPenWidth = newWidth;
}

void SketchArea::setEraserColor(const QColor &newColor)
{
    myEraserColor = newColor;
}

void SketchArea::setEraserWidth(int newWidth)
{
    myEraserWidth = newWidth;
}

void SketchArea::setAutoSaveSketch(bool save, const QString &fileName)
{
    autoSaveSketch = save;
    saveSketchFileName = fileName;
}

void SketchArea::clearImage()
{
    image.fill(qRgb(255, 255, 255));
    modified = true;
    update();
    emit clearSketchDone();
}

void SketchArea::keyPressEvent(QKeyEvent *event)
{
    switch(event->key()) {
    case Qt::Key_C:
        clearImage();
        break;
    case Qt::Key_E:
        erasing = true;
        break;
    default:
        QWidget::keyPressEvent(event);
    }
}

void SketchArea::keyReleaseEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_E:
        erasing = false;
        break;
    default:
        QWidget::keyPressEvent(event);
        break;
    }
}

void SketchArea::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        lastPoint = event->pos();
        scribbling = true;
    }
    else if(event->button() == Qt::RightButton) {
        lastPoint = event->pos();
        erasing = true;
    }
}

void SketchArea::mouseMoveEvent(QMouseEvent *event)
{
     if ((event->buttons() & Qt::LeftButton) && scribbling)
         drawLineTo(event->pos());
     if ((event->buttons() & Qt::RightButton) && erasing)
         drawLineTo(event->pos());
}

void SketchArea::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && scribbling) {
        drawLineTo(event->pos());
        scribbling = false;
    }

    if (event->button() == Qt::RightButton && erasing) {
        drawLineTo(event->pos());
        erasing = false;
    }

    if(event->button() == Qt::MidButton)
    {
        clearImage();
    }

    if(autoSaveSketch)
        saveImage(saveSketchFileName, "JPG");

    if(event->button() == Qt::LeftButton || event->button() == Qt::RightButton)
        emit newSketchDone(saveSketchFileName);
}

void SketchArea::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QRect dirtyRect = event->rect();

    painter.drawImage(dirtyRect, image, dirtyRect);
}

void SketchArea::resizeEvent(QResizeEvent *event)
{
//    if (width() > image.width() || height() > image.height()) {
//        int newWidth = qMax(width() + 128, image.width());
//        int newHeight = qMax(height() + 128, image.height());
//        resizeImage(&image, QSize(newWidth, newHeight));
//        update();
//    }

    QSize newSize(width(), height());
    QImage newImage(newSize, QImage::Format_RGB32);
    newImage.fill(qRgb(255, 255, 255));
    QPainter painter(&newImage);
    QRect rect;
    rect.setWidth(qMin(width(), image.width()));
    rect.setHeight(qMin(height(), image.height()));
    painter.drawImage(rect, image.copy(rect));
    image = newImage;
    update();

    QWidget::resizeEvent(event);
}

void SketchArea::drawLineTo(const QPoint &endPoint)
{
    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);//ƽ��ֱ��

    if(erasing)
        painter.setPen(QPen(myEraserColor, myEraserWidth, Qt::SolidLine, Qt::RoundCap,
                         Qt::RoundJoin));
    else
        painter.setPen(QPen(myPenColor, myPenWidth, Qt::SolidLine, Qt::RoundCap,
                         Qt::RoundJoin));

    if(endPoint.x() < rect().width() && endPoint.y() < rect().height())
        painter.drawLine(lastPoint, endPoint);

    modified = true;

    int rad = ((erasing == true ? myEraserWidth : myPenWidth) / 2) + 2;
    update(QRect(lastPoint, endPoint).normalized()
                                  .adjusted(-rad, -rad, +rad, +rad));
    lastPoint = endPoint;
}

void SketchArea::resizeImage(QImage *image, const QSize &newSize)
{
    if (image->size() == newSize)
     return;

    QImage newImage(newSize, QImage::Format_RGB32);
    newImage.fill(qRgb(255, 255, 255));
    QPainter painter(&newImage);
    painter.drawImage(QPoint(0, 0), *image);
    *image = newImage;
    painter.end();
}

void SketchArea::print()
{
#ifndef QT_NO_PRINTER
    QPrinter printer(QPrinter::HighResolution);

    QPrintDialog *printDialog = new QPrintDialog(&printer, this);
    if (printDialog->exec() == QDialog::Accepted) {
        QPainter painter(&printer);
        QRect rect = painter.viewport();
        QSize size = image.size();
        size.scale(rect.size(), Qt::KeepAspectRatio);
        painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
        painter.setWindow(image.rect());
        painter.drawImage(0, 0, image);
    }
#endif // QT_NO_PRINTER
}
