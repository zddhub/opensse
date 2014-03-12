#ifndef RESULTPHOTOWIDGET_H
#define RESULTPHOTOWIDGET_H

#include <QTableWidget>

#include "searchengine.h"

class ResultPhotoWidget : public QTableWidget
{
    Q_OBJECT
public:
    explicit ResultPhotoWidget(QWidget *parent = 0);
signals:

public slots:
    void updateResults(QueryResults &res);
private:
    QueryResults results;
};

#endif // RESULTPHOTOWIDGET_H
