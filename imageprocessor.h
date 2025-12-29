#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <QMainWindow>
#include <QAction>
#include <QMenu>
#include <QToolBar>
#include <QImage>
#include <QLabel>
#include <QStatusBar>

// Custom QLabel that tracks mouse movement
class ImageLabel : public QLabel
{
    Q_OBJECT

public:
    explicit ImageLabel(QWidget *parent = nullptr);
    void setImage(const QImage &image);

signals:
    void mousePositionChanged(int x, int y, int grayValue);

protected:
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    QImage currentImage;
};

class ImageProcessor : public QMainWindow
{
    Q_OBJECT

public:
    ImageProcessor(QWidget *parent = nullptr);
    ~ImageProcessor();
    void createActions();
    void createMenus();
    void createToolBars();
    void loadFile(QString filename);

private slots:
    void showOpenFile();
    void getZoomOut();
    void getZoomIn();
    void updateStatusBar(int x, int y, int grayValue);

private:
    QWidget     *central;
    QMenu       *fileMenu;
    QToolBar    *fileTool;
    QImage      img;
    QString     filename;
    ImageLabel  *imgWin;
    QAction     *openFileAction;
    QAction     *exitAction;
    QAction     *zoomOut;
    QAction     *zoomIn;

};
#endif // IMAGEPROCESSOR_H
