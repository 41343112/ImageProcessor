#include "imageprocessor.h"
#include <QHBoxLayout>
#include <QMenuBar>
#include <QFileDialog>
#include <QDebug>
#include <QMouseEvent>

// ImageLabel implementation
ImageLabel::ImageLabel(QWidget *parent)
    : QLabel(parent)
{
    setMouseTracking(true);
}

void ImageLabel::setImage(const QImage &image)
{
    currentImage = image;
}

void ImageLabel::mouseMoveEvent(QMouseEvent *event)
{
    if (!currentImage.isNull())
    {
        // Get the mouse position
        int x = event->pos().x();
        int y = event->pos().y();
        
        // Scale the coordinates to match the actual image size
        QSize labelSize = size();
        QSize imageSize = currentImage.size();
        
        // Calculate the actual image coordinates
        int actualX = (x * imageSize.width()) / labelSize.width();
        int actualY = (y * imageSize.height()) / labelSize.height();
        
        // Check if the coordinates are within the image bounds
        if (actualX >= 0 && actualX < currentImage.width() &&
            actualY >= 0 && actualY < currentImage.height())
        {
            // Get the pixel color and calculate grayscale value
            QRgb pixel = currentImage.pixel(actualX, actualY);
            int grayValue = qGray(pixel);
            
            emit mousePositionChanged(actualX, actualY, grayValue);
        }
    }
    
    QLabel::mouseMoveEvent(event);
}

ImageProcessor::ImageProcessor(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(QStringLiteral("影像處理"));
    central = new QWidget();
    QHBoxLayout *mainLayout = new QHBoxLayout(central);
    imgWin = new ImageLabel();
    QPixmap     *initPixmap = new QPixmap(300, 200);
    initPixmap->fill(QColor(255, 255, 255));
    imgWin->resize(300 ,200);
    imgWin->setScaledContents(true);
    imgWin->setPixmap(*initPixmap);
    mainLayout->addWidget(imgWin);
    setCentralWidget(central);
    
    // Create status bar
    statusBar = new QStatusBar();
    setStatusBar(statusBar);
    statusBar->showMessage(QStringLiteral("準備就緒"));
    
    // Connect mouse position signal to status bar update
    connect(imgWin, &ImageLabel::mousePositionChanged, this, &ImageProcessor::updateStatusBar);
    
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
    imgWin->setImage(img);
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

void ImageProcessor::updateStatusBar(int x, int y, int grayValue)
{
    QString message = QString("位置: (%1, %2) - 灰階值: %3").arg(x).arg(y).arg(grayValue);
    statusBar->showMessage(message);
}
