#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QFileDialog>
#include <QtConcurrentRun>
#include <QPainter>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
             MainWindow(QWidget *parent = nullptr);
             ~MainWindow();
    void     process();
    double   contoursArea(cv::Mat corner);
    quint8   contourDetection(const QString path);
    QPoint   logoToTopLeft();
    QPoint   logoToTopRight(QImage image, QImage logo2);
    QPoint   logoToBottomLeft(QImage image, QImage logo2);
    QPoint   logoToBottomRight(QImage image, QImage logo2);

private slots:
    void on_browse_clicked();
    void on_process_clicked();

private:
    Ui::MainWindow *ui;
    QStringList     m_fileList;
};
#endif // MAINWINDOW_H
