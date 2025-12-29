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
    if (!currentImage.isNull() && pixmap() && !pixmap()->isNull())
    {
        // Get the mouse position relative to the label
        QPoint mousePos = event->pos();
        
        // Get label and pixmap dimensions
        QSize labelSize = size();
        QSize pixmapSize = pixmap()->size();
        
        // Calculate the actual display area of the pixmap (accounting for aspect ratio)
        QRect pixmapRect;
        float labelAspect = (float)labelSize.width() / labelSize.height();
        float pixmapAspect = (float)pixmapSize.width() / pixmapSize.height();
        
        if (labelAspect > pixmapAspect) {
            // Label is wider - pixmap is constrained by height
            int displayWidth = (int)(labelSize.height() * pixmapAspect);
            int xOffset = (labelSize.width() - displayWidth) / 2;
            pixmapRect = QRect(xOffset, 0, displayWidth, labelSize.height());
        } else {
            // Label is taller - pixmap is constrained by width
            int displayHeight = (int)(labelSize.width() / pixmapAspect);
            int yOffset = (labelSize.height() - displayHeight) / 2;
            pixmapRect = QRect(0, yOffset, labelSize.width(), displayHeight);
        }
        
        // Check if mouse is within the pixmap display area
        if (pixmapRect.contains(mousePos))
        {
            // Calculate actual image coordinates with proper rounding
            float relativeX = (float)(mousePos.x() - pixmapRect.x()) / pixmapRect.width();
            float relativeY = (float)(mousePos.y() - pixmapRect.y()) / pixmapRect.height();
            
            int actualX = (int)(relativeX * currentImage.width());
            int actualY = (int)(relativeY * currentImage.height());
            
            // Clamp to image bounds
            actualX = qBound(0, actualX, currentImage.width() - 1);
            actualY = qBound(0, actualY, currentImage.height() - 1);
            
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
    
    // Set up status bar
    statusBar()->showMessage(QStringLiteral("準備就緒"));
    
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
    statusBar()->showMessage(message);
}
