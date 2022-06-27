// headers
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>

//namespace
using namespace cv;
using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::process()
{
    foreach(const QString& path, m_fileList)
    {
        QImage   image(path);
        QString  logoPath = "/home/efe/Downloads/logo.png";
        QImage   logo(logoPath);
        QImage   logo2 = logo.scaled(50, 50, Qt::KeepAspectRatio);
        QPainter painter(&image); painter.setOpacity(1);

        // setting and drawing logo
        if(contourDetection(path)==0){
            painter.drawImage(logoToTopLeft(), logo2);
            painter.end();
        }else if(contourDetection(path)==1){
            painter.drawImage(logoToTopRight(image, logo2), logo2);
            painter.end();
        }else if(contourDetection(path)==2){
            painter.drawImage(logoToBottomLeft(image, logo2), logo2);
            painter.end();
        }else{
            painter.drawImage(logoToBottomRight(image, logo2), logo2);
            painter.end();
        }

        ui->imageLabel->setPixmap(QPixmap::fromImage(image));
        QThread::msleep(6000);
//        image.save(string, "PNG");
    }
}

quint8 MainWindow::contourDetection(const QString path)
{
    Mat image = imread(path.toStdString());

    Mat image_gray;
    cvtColor(image, image_gray, COLOR_BGR2GRAY);

    Mat thresh;
    Canny(image_gray, thresh, 50, 150);

    Mat cropped_image_top_left     = thresh(Range(0, 50), Range(0, 50));
    Mat cropped_image_top_right    = thresh(Range(0, 50), Range(image.size().width - 50, image.size().width));
    Mat cropped_image_bottom_left  = thresh(Range(image.size().height - 50, image.size().height), Range(0, 50));
    Mat cropped_image_bottom_right = thresh(Range(image.size().height - 50, image.size().height), Range(image.size().width - 50, image.size().width));

    imshow("0", cropped_image_top_left);
    imshow("1", cropped_image_top_right);
    imshow("2", cropped_image_bottom_left);
    imshow("3", cropped_image_bottom_right);

    qDebug() << "alan0" << contoursArea(cropped_image_top_left);
    qDebug() << "alan0" << contoursArea(cropped_image_top_right);
    qDebug() << "alan0" << contoursArea(cropped_image_bottom_left);
    qDebug() << "alan0" << contoursArea(cropped_image_bottom_right);

    int min = 10000;
    int result = 0;
    QList<Mat> matList = {cropped_image_top_left, cropped_image_top_right, cropped_image_bottom_left, cropped_image_bottom_right};
    for(int i=0; i<matList.size(); i++)
    {
        if(contoursArea(matList.at(i)) < min)
        {
            min = contoursArea(matList.at(i));
            result = i;
        }
    }
    return result;
}

double MainWindow::contoursArea(cv::Mat corner)
{
    Mat cornerCopy;
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;

    findContours(corner, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
    cornerCopy = corner.clone();
    drawContours(cornerCopy, contours, -1, Scalar(255, 0, 0), 1);

    double area = 0;
    for (unsigned int i = 0;  i < contours.size();  i++)
    {
         area += contourArea(contours[i]);
    }
    return area;
}

QPoint MainWindow::logoToTopLeft()
{
    QPoint point(0, 0);
    return point;
}

QPoint MainWindow::logoToTopRight(QImage image, QImage logo2)
{
    QPoint point(image.width()-logo2.width(), 0);
    return point;
}

QPoint MainWindow::logoToBottomLeft(QImage image, QImage logo2)
{
    QPoint point(0, image.height()-logo2.height());
    return point;
}

QPoint MainWindow::logoToBottomRight(QImage image, QImage logo2)
{
    QPoint point(image.width()-logo2.width(), image.height()-logo2.height());
    return point;
}

void MainWindow::on_process_clicked()
{
    QtConcurrent::run(this, &MainWindow::process);
}

void MainWindow::on_browse_clicked()
{
    QStringList files = QFileDialog::getOpenFileNames(
                                                      this,
                                                      "Select one or more files to open",
                                                      "/home/efe/Desktop/resim",
                                                      "Images (*.png *.xpm *.jpg *.jpeg)");
    if(files.isEmpty())
        return;

    ui->label->setText("");
    ui->label->setText(ui->imageLabel->text() + "Added successfully");
    m_fileList = files;
}
