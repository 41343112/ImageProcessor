#include "imageprocessor.h"
#include <QHBoxLayout>
#include <QMenuBar>
#include <QFileDialog>
#include <QDebug>

ImageProcessor::ImageProcessor(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(QStringLiteral("影像處理"));
    central = new QWidget();
    QHBoxLayout *mainLayout = new QHBoxLayout(central);
    imgWin = new QLabel();
    QPixmap     *initPixmap = new QPixmap(300, 200);
    initPixmap->fill(QColor(255, 255, 255));
    imgWin->resize(300 ,200);
    imgWin->setScaledContents(true);
    imgWin->setPixmap(*initPixmap);
    mainLayout->addWidget(imgWin);
    setCentralWidget(central);
    createActions();
    createMenus();
    createToolBars();

}

ImageProcessor::~ImageProcessor() {}

void ImageProcessor::createActions()
{
    openFileAction = new QAction(QStringLiteral("開啟檔案&O"),this);
    openFileAction->setShortcut(tr("Ctrl+O"));
    openFileAction->setStatusTip(QStringLiteral("開啟影像檔案"));

    connect(openFileAction,SIGNAL(triggered()),this,SLOT(showOpenFile()));

    zoomOut = new QAction(QStringLiteral("縮小"),this);
    zoomOut->setStatusTip(QStringLiteral("縮小檔案"));

    connect(zoomOut,SIGNAL(triggered()),this,SLOT(getZoomOut()));


    zoomIn = new QAction(QStringLiteral("放大"),this);

    zoomIn->setStatusTip(QStringLiteral("放大檔案"));

    connect(zoomIn,SIGNAL(triggered()),this,SLOT(getZoomIn()));



    exitAction = new QAction(QStringLiteral("結束&Q"),this);
    exitAction->setShortcut(tr("Ctrl+Q"));
    exitAction->setStatusTip(QStringLiteral("退出程式"));
    connect(exitAction,SIGNAL(triggered()),this,SLOT(close()));

}

void ImageProcessor::createMenus()
{
    fileMenu = menuBar()->addMenu(QStringLiteral("檔案&F"));
    fileMenu->addAction(openFileAction);
    fileMenu->addAction(exitAction);

    fileMenu = menuBar()->addMenu(QStringLiteral("工具&T"));
    fileMenu->addAction(zoomOut);
    fileMenu->addAction(zoomIn);
}

void ImageProcessor::createToolBars()
{
    fileTool = addToolBar("file");
    fileTool->addAction(openFileAction);
    fileTool = addToolBar("file");
    fileTool->addAction(zoomOut);
    fileTool->addAction(zoomIn);
}
void ImageProcessor::loadFile(QString filename)
{
    qDebug()<<QString("file name:%1").arg(filename);
    QByteArray ba = filename.toLatin1();
    printf("FN:%S\n",(char *) ba.data());
    img.load(filename);
    imgWin->setPixmap(QPixmap::fromImage(img));
}
void ImageProcessor::showOpenFile()
{
    filename = QFileDialog::getOpenFileName(this,QStringLiteral("開啟影像"),tr("."),
                                            "bmp(*.bmp);;png(*.png)"
                                            ";;Jpeg(*.jpg)");
    if(!filename.isEmpty())
    {
        if(img.isNull())
        {
            loadFile(filename);
        }
        else
        {
            ImageProcessor *newIPWin = new ImageProcessor();
            newIPWin->show();
            newIPWin->loadFile(filename);
        }
    }
}
void ImageProcessor::getZoomOut()
{
    QImage zoomOuted;
    zoomOuted = img.scaled(img.width()/2,img.height()/2);
    QLabel *ret = new QLabel();
    ret->setPixmap(QPixmap::fromImage(zoomOuted));
    ret->setWindowTitle(tr("縮小結果"));
    ret->show();

}
void ImageProcessor::getZoomIn()
{
    QImage zoomIned;
    zoomIned = img.scaled(img.width()*2,img.height()*2);
    QLabel *ret = new QLabel();
    ret->setPixmap(QPixmap::fromImage(zoomIned));
    ret->setWindowTitle(tr("放大結果"));
    ret->show();

}
