#include "resultphotowidget.h"


#include <QHeaderView>
#include <QTableWidgetItem>

ResultPhotoWidget::ResultPhotoWidget(QWidget *parent) :
    QTableWidget(parent)
{
    resize(300, 800);
    //setMaximumWidth(300);
    setIconSize(QSize(200,200));


    //setColumnWidth(0, 108);

    //    setRowCount(1);
    //    setRowHeight(0,200);

    //
    this->horizontalHeader()->hide();
    this->verticalHeader()->hide();

    this->setStyleSheet("QTableView{selection-background-color: transparent;selection-color: transparent}");
    //
    this->setShowGrid(false);

    //
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setSizePolicy(sizePolicy);

    //
    this->setSelectionMode(QAbstractItemView::SingleSelection);

    //
    this->setFocusPolicy(Qt::NoFocus);
}


void ResultPhotoWidget::updateResults(QueryResults &res)
{
    results.clear();
    results = res;

    this->clear();
//    this->setColumnCount(results.size());
    this->setRowCount(results.size()/5 +1);
    this->setColumnCount(5);

    for(int i = 0; i < results.size();i++)
    {
        if(results[i].ratio <=0.0)
            break;

        QTableWidgetItem *item = new QTableWidgetItem();
        item->setWhatsThis(QString(results[i].imageName.c_str()));
        item->setIcon(QIcon(QString(results[i].imageName.c_str())));
        QString rank = tr("%1").arg(results[i].ratio);
        item->setToolTip(rank);
        item->setText(rank);
        item->setTextAlignment(Qt::AlignBottom);
        item->setTextColor(Qt::black);

        setItem(i/5, i%5, item);
//        setColumnWidth(i, 200);
       // setItem(i, 0, item);

        item->setTextAlignment(Qt::AlignCenter);

        setRowHeight(i/5, 200);
        setColumnWidth(i%5, 207);
    }
}
