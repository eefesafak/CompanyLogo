// headers
#include "mainwindow.h"
#include "ui_mainwindow.h"

// c++
#include <iostream>
#include <stdlib.h>
#include <stdio.h>

//opencv
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

//namespace
using namespace cv;
using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QImage imagee("/home/efe/Desktop/resim/imagess.png");

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::process()
{
    foreach(const QString& string, m_fileList)
    {
        QImage   image(string);
        QString  logoPath = "/home/efe/Downloads/logo.png";
        QImage   logo(logoPath);
        QImage   logo2 = logo.scaled(50, 50, Qt::KeepAspectRatio);
        QPainter painter(&image); painter.setOpacity(0.5);


        // setting and drawing logo to bottom right corner of QImage
        if(contourDetection(string)==0){
            painter.drawImage(logoToTopLeft(), logo2);
            painter.end();
        }else if(contourDetection(string)==1){
            painter.drawImage(logoToTopRight(image, logo2), logo2);
            painter.end();
        }else if(contourDetection(string)==2){
            painter.drawImage(logoToBottomLeft(image, logo2), logo2);
            painter.end();
        }else{
            painter.drawImage(logoToBottomRight(image, logo2), logo2);
            painter.end();
        }

        ui->imageLabel->setPixmap(QPixmap::fromImage(image));

        QByteArray ba;
        QBuffer    buffer(&ba);
        buffer.open(QIODevice::WriteOnly);
        image.save(&buffer, "PNG");
        QThread::msleep(3000);
    }
}

quint8 MainWindow::contourDetection(const QString path)
{
    Mat image = imread(path.toStdString()); // (317, 159)
    Mat image_gray;
    cvtColor(image, image_gray, COLOR_BGR2GRAY);
    Mat thresh;
    Canny(image_gray, thresh, 50, 150);

    Mat cropped_image_top_left     = thresh(Range(0, 50), Range(0, 50));
    Mat cropped_image_top_right    = thresh(Range(0, 50), Range(image.size().width - 50, image.size().width));
    Mat cropped_image_bottom_left  = thresh(Range(image.size().height - 50, image.size().height), Range(0, 50));
    Mat cropped_image_bottom_right = thresh(Range(image.size().height - 50, image.size().height), Range(image.size().width - 50, image.size().width));

    int min = 10000;
    QList<cv::Mat> matList = {cropped_image_top_left, cropped_image_top_right, cropped_image_bottom_left, cropped_image_bottom_right};
    foreach(const Mat& mat, matList)
    {
        if(contoursArea(mat) < min)
        {
            min = contoursArea(mat);
        }
    }
    quint8 result;
    for(int i=0; i<4; i++)
    {
        if(min == contoursArea(matList.at(i)))
        {
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

    findContours(corner, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_NONE);
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
//    contourDetection();
}

void MainWindow::on_browse_clicked()
{
    QStringList files = QFileDialog::getOpenFileNames(
                                                      this,
                                                      "Select one or more files to open",
                                                      "/home/efe/Desktop/resim",
                                                      "Images (*.png *.xpm *.jpg)");
    if(files.isEmpty())
        return;

    m_fileList = files;
    ui->label->setText("");
    ui->label->setText(ui->imageLabel->text() + "Added successfully");
}


