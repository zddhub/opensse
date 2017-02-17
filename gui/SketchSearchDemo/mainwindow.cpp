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
#include "mainwindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QList>
#include <QMenu>
#include <QMenuBar>
#include <QFileDialog>

#include "sketchsearcher.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    sse::PropertyTree_t config;

    boost::property_tree::read_json("/tmp/SketchSearchDemo/config.json", config); //
    searchEngine = new SketchSearcher(config);

    setupMenuBar();

    sketchArea = new SketchArea;
    sketchArea->setAutoSaveSketch(true);
    triMeshView = new TriMeshView;
    //triMeshView->readMesh("./data/1_01.obj");
    QSplitter *leftSplitter = new QSplitter(Qt::Vertical);
    leftSplitter->addWidget(sketchArea);
    leftSplitter->addWidget(triMeshView);
    //leftSplitter->setStretchFactor(1, 1);

    resultPhotoWidget = new ResultPhotoWidget;
    QSplitter *mainSplitter = new QSplitter(Qt::Horizontal);
    mainSplitter->addWidget(leftSplitter);
    mainSplitter->addWidget(resultPhotoWidget);
    //mainSplitter->setStretchFactor(1, 2);

    QSize size = this->size();
    QList<int> leftList;
    leftList.push_back(size.height()/2);
    leftList.push_back(size.height()/2);
    leftSplitter->setSizes(leftList);

    QList<int> mainList;
    mainList.push_back(size.width()/7);
    mainList.push_back(size.width()/3);
    mainSplitter->setSizes(mainList);

    this->setCentralWidget(mainSplitter);

    this->setMinimumSize(800, 600);

    //auto search
    connect(sketchArea, SIGNAL(newSketchDone(QString)), this, SLOT(search(QString)));

    connect(sketchArea, SIGNAL(clearSketchDone()), this, SLOT(clearResults()));
    connect(resultPhotoWidget, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(showLineDrawing(QTableWidgetItem*)));

}

void MainWindow::setupMenuBar()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

    QAction *openAction = fileMenu->addAction(tr("&Open"));
    connect(openAction, SIGNAL(triggered()), this, SLOT(openFile()));

    QAction *queryAction = menuBar()->addAction(tr("&Query"));
    connect(queryAction, SIGNAL(triggered()), this, SLOT(query()));
}

void MainWindow::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::currentPath(),
                                                    tr("Images (*.png *.bmp *.jpg)"));

    if(!fileName.isEmpty())
    {
        sketchArea->openImage(fileName);
    }
}

void MainWindow::query()
{
    QueryResults results;
    searchEngine->query(sketchArea->sketchPath().toStdString(), results);
    resultPhotoWidget->updateResults(results);
}

void MainWindow::search(const QString &fileName)
{
    QueryResults results;
    searchEngine->query(fileName.toStdString(), results);
    resultPhotoWidget->updateResults(results);
}

void MainWindow::clearResults()
{
    QueryResults res;
    resultPhotoWidget->updateResults(res);
    triMeshView->clearMesh();
}

void MainWindow::showLineDrawing(QTableWidgetItem *item)
{
    QString fileName = item->whatsThis();
    QString modelFileName = fileName.left(fileName.lastIndexOf("view")) + ".off";

//    std::cout << "fileName:" << fileName.toStdString()<<endl;

//    qDebug(fileName.toStdString().c_str());

    QString xfFileName = fileName.right(fileName.length() - fileName.lastIndexOf("view"));

    //std::cout << "xffilename:" << xfFileName.replace(".jpg", ".xf").toStdString()<<endl;

    triMeshView->readMesh(modelFileName.toStdString().c_str(),
                                xfFileName.replace(".jpg", ".xf").toStdString().c_str());

 //   qDebug(fileName.toStdString().c_str());

}

MainWindow::~MainWindow()
{

}
