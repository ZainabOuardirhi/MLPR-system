#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <QBoxLayout>
#include <QElapsedTimer>

#include "vehicledetector.h"
#include "charactersdetector.h"
#include "firstwindow.h"
#include "ui_firstwindow.h"
#include "videowindow.h"
#include "imagewindow.h"


FirstWindow::FirstWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FirstWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Système ALPR de MAScIR");

    QPixmap pix("/home/hp/Pictures/Mascir.png");
    pix = pix.scaledToWidth(ui->lbl_mascir_video->width(),Qt::SmoothTransformation);
    ui->lbl_mascir_video->setPixmap(pix);
    ui->buttons_horizontalLayout->addWidget(ui->btn_image);
    ui->buttons_horizontalLayout->addWidget(ui->btn_video);
    ui->buttons_horizontalLayout->setAlignment(Qt::AlignHCenter);
    ui->buttons_horizontalLayout_2->addWidget(ui->lbl_mascir_video);
    ui->buttons_horizontalLayout_2->setAlignment(Qt::AlignHCenter);
}

FirstWindow::~FirstWindow()
{
    delete ui;
}

void FirstWindow::on_btn_image_clicked()
{
    this->close();
    ImageWindow iw;
    iw.setModal(true);
    iw.exec();
}

void FirstWindow::on_btn_video_clicked()
{
    this->close();
    VideoWindow vw;
    vw.setModal(true);
    vw.exec();
}

