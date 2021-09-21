#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <QBoxLayout>
#include <QElapsedTimer>

#include "vehicledetector.h"
#include "charactersdetector.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "firstwindow.h"

using namespace cv;
using namespace std;

//Initialiser les variables utilisees
char* warp_rotate_window = "License Plate";
Mat frame;
QImage imgIn;
vector<Mat> RedressedImages;
vector<QString> ContextSTR;
QStringList strlDir;
QString absolute;

vector<Rect> box;
vector<string> classes;
Doub test;
plate car;
vector<float> conf;
vector<Solution> vect;
vector<Rect> chars_box;
vector<int> class_id;
vector<int> ids;
VehicleDetector detector;
charactersDetector chars;
Mat img;
float confidence;
vector<Image> vect_Images;
QString str;
Mat newImage;
vector<double> degree;
Mat rot_mat( 2, 3, CV_32FC1 );
Mat warp_mat( 2, 3, CV_32FC1 );
Mat src, warp_dst, warp_rotate_dst;
vector<LP> licensePlate;
std::ostringstream name;
int iterate;
VideoCapture cap;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Système ALPR de MAScIR");

    QPixmap pix("/home/hp/Pictures/Mascir.png");
    pix = pix.scaledToWidth(ui->Mascir_Image->width(),Qt::SmoothTransformation);
    ui->Mascir_Image->setPixmap(pix);

    //Creation des Layout et ajout des widgets.
    ui->horizontalLayout_2->addWidget(ui->Mascir_Image);
    ui->horizontalLayout_2->setAlignment(Qt::AlignHCenter);

    ui->verticalLayout->addWidget(ui->btn_detect);
    ui->verticalLayout->addWidget(ui->lbl_detect);
    ui->verticalLayout->addWidget(ui->btn_redresse);
    ui->verticalLayout->addWidget(ui->lbl_redresse);
    ui->verticalLayout->addWidget(ui->btn_reconnaitre);
    ui->verticalLayout->addWidget(ui->lbl_reconnaitre);
    ui->verticalLayout->setAlignment(Qt::AlignHCenter);

    ui->horizontalLayout->addWidget(ui->previous);
    ui->horizontalLayout->addWidget(ui->next);

    ui->verticalLayout_2->addWidget(ui->btn_image);
    ui->verticalLayout_2->addWidget(ui->lbl_image);

//    ui->verticalLayout->setSizeConstraint(120);
    ui->lbl_detect->hide();
    ui->btn_detect->hide();
    ui->lbl_reconnaitre->hide();
    ui->btn_reconnaitre->hide();
    ui->lbl_redresse->hide();
    ui->btn_redresse->hide();
    ui->next->hide();
    ui->previous->hide();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btn_image_clicked()
{
    ui->lbl_detect->setText("");
    detector.initNetwork();
    chars.initNetwork();

    //Ouverture de l'image
    QString filename = QFileDialog::getOpenFileName(this,
                                                    tr("Choose"),
                                                    QDir::homePath(),
                                                    tr("Images(*.png *.jpg *.jpeg *.bmp *.gif *.webm)"));
//    ui->lbl_detect->setText(filename);

    String path = filename.toStdString().c_str();
    cap.open(path);
    frame=imread(path);

    //Creation d'un Timer pour connaitre le temps d'execution de la detection.
    QElapsedTimer timer1;
    timer1.start();
    while (true)
    {
        waitKey(1);
        cap >> frame;
        if (frame.empty())
        {
            waitKey();
            break;
        }
//        imshow("frame",frame);
//        waitKey(1);
        //Detection de la PI
        car = detector.detect(frame);
        box = car.box;
        conf = car.conf;
        vector<int> indices = car.id;
    //    cout<<indices.size()<<endl;

        //Boucler sur les PI detecter.
        for (int j = 0; j < indices.size(); ++j) {
            int idx = indices[j];
            Rect boxPlate = box[idx];
            frame(boxPlate).copyTo(img);
            confidence = conf[idx];
            vect_Images.push_back({img,confidence});
        }
    //    cout<<vect_Images.size()<<endl;

        //for (vector<Image>::iterator iter = vect_Images.begin() ; iter != vect_Images.end(); ++iter){
//        if (QString::compare(filename,QString()) != 0) {
            QImage image;
            image = QImage((uchar*) frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
//            bool valid = image.load(filename);
//            if (valid) {

                //Afficher l'image du vehicule dans l'IG (UI)
                image = image.scaledToWidth(ui->lbl_image->width(),Qt::SmoothTransformation);
                ui->lbl_image->setPixmap(QPixmap::fromImage(image));

                //Recuperer le chemin absolut de cette image
                QDir fileDir = QFileInfo(filename).absoluteDir();
                absolute=fileDir.absolutePath();
                QStringList filters;
                filters << "*.png" << "*.xpm" << "*.jpg" << "*.jpeg" << "*.webm";
                fileDir.setNameFilters(filters);
                strlDir = fileDir.entryList();
                iterate = strlDir.indexOf(filename);
    //            ui->id->setText(absolute);
    //            ui->id->setText(QString::number(iterate));
                ui->lbl_detect->show();
                ui->btn_detect->show();
                ui->next->show();
                ui->previous->show();
    //            ui->lbl_reconnaitre->hide();
    //            ui->btn_reconnaitre->hide();
    //            ui->lbl_redresse->hide();
    //            ui->btn_redresse->hide();
//            }
//            else {
//                //Erreur
//            }
        //}
        //}
    }
    cout<< "The detection took : " << timer1.elapsed() << " milliseconds"<<endl;
}
void MainWindow::open(QString filename)
{
    vect_Images.clear();
    detector.initNetwork();
    chars.initNetwork();

    String path = filename.toStdString().c_str();
    frame=imread(path);
    //Detection de la PI
    car = detector.detect(frame);
    box = car.box;
    conf =car.conf;
    vector<int> indices = car.id;
//    cout<<indices.size()<<endl;

    //Boucler sur les PI detecter.
    for (int j = 0; j < indices.size(); ++j) {
        int idx = indices[j];
        Rect boxPlate = box[idx];
        frame(boxPlate).copyTo(img);
        confidence = conf[idx];
        vect_Images.push_back({img,confidence});
    }
//    cout<<vect_Images.size()<<endl;
    if (QString::compare(filename,QString()) != 0) {
        QImage image;
        bool valid = image.load(filename);
        if (valid) {

            //Afficher l'image du vehicule dans l'IG (UI)
            image = image.scaledToWidth(ui->lbl_image->width(),Qt::SmoothTransformation);
            ui->lbl_image->setPixmap(QPixmap::fromImage(image));
        }
        else {
            //Erreur
        }
    }
}

//On click du boutton "suivant"
void MainWindow::on_next_clicked()
{
    //Creation d'un Timer pour connaitre le temps pri pour afficher l'image suivante.
    QElapsedTimer timer2;
    timer2.start();

    if (iterate +1 > 100)
       return;

    iterate++;
    QString path = absolute+"/"+strlDir.at(iterate);
    //cout<<path.toStdString().c_str()<<endl;
    open(path);
    ui->lbl_detect->clear();
    ui->lbl_redresse->clear();
    ui->lbl_reconnaitre->setText("");
    ui->btn_redresse->hide();
    ui->btn_reconnaitre->hide();
//    ui->id->setText(QString::number(iterate));
    cout<< "The next took : " << timer2.elapsed() << " milliseconds"<<endl;

}

//On click du boutton "precedant"
void MainWindow::on_previous_clicked()
{
    //Creation d'un Timer pour connaitre le temps pri pour afficher l'image precedante.
    QElapsedTimer timer3;
    timer3.start();
    if (iterate -1 < 0)
       return;

    iterate--;
    QString path = absolute+"/"+strlDir.at(iterate);
    open(path);
    ui->lbl_detect->clear();
    ui->lbl_redresse->clear();
    ui->lbl_reconnaitre->setText("");
    ui->btn_redresse->hide();
    ui->btn_reconnaitre->hide();
//    ui->id->setText(QString::number(iterate));
    cout<< "The previous took : " << timer3.elapsed() << " milliseconds"<<endl;

}

//On click du boutton "Detecter la plaque"
void MainWindow::on_btn_detect_clicked()
{
//    Mat useImage;
    //Boucler sur les PI detecter
    for (vector<Image>::iterator iter = vect_Images.begin() ; iter != vect_Images.end(); ++iter){
        cv::resize(iter->image,iter->image,cv::Size(400,100));
        cvtColor(iter->image, iter->image, CV_BGR2RGB);

        //Afficher l'image de la PI dans l'IG (UI)
        imgIn= QImage((uchar*) iter->image.data, iter->image.cols, iter->image.rows, iter->image.step, QImage::Format_RGB888);
        //image = iter->image;
        imgIn = imgIn.scaledToWidth(ui->lbl_detect->width(),Qt::SmoothTransformation);
        ui->lbl_detect->setPixmap(QPixmap::fromImage(imgIn));
        ui->lbl_detect->setAlignment(Qt::AlignCenter);

        newImage = iter->image;
        cv::cvtColor(newImage,newImage, CV_BGR2GRAY);

        //Canny edge detector
        cv::Canny(newImage,newImage,100,50);
        vector<Vec4i> lines;

        //Houghlines pour detecter les lignes dans l'image de la PI
        cv::HoughLinesP(newImage, lines, 1, CV_PI/180, 100,100,100);

        src = iter->image; //newImage
        if (!lines.empty()) {
            for( size_t i = 0; i < lines.size(); i++ )
              {
                Vec4i l = lines[i];
                cv::line( newImage, Point(l[0], l[1]), Point(l[2], l[3]), cv::Scalar(94.0, 206.0, 165.0), 3, CV_AA);
                Point p1, p2;
                p1=Point(l[0], l[1]);
                p2=Point(l[2], l[3]);
                degree.push_back(fmodf((atan2(p1.y - p2.y, p1.x - p2.x) * (180.0 / 3.14159265)) + 360, 360));
              }

            double MaxDegree = degree[0];
            for (int var = 1; var < degree.size(); ++var) {
                if(MaxDegree < degree[var]){
                    MaxDegree = degree[var];
                }
            }
            //Définir l'image src du même type et de la même taille que src
            warp_dst = Mat::zeros( src.rows, src.cols, src.type() );

            //Calculer une matrice de rotation par rapport au centre de l'image
            Point center = Point( src.cols/2, src.rows/2 );

            //Un moyen facile de garder des angles entre -179 et 180 degrés
            int newAngle = MaxDegree;
            while (newAngle <= -180) newAngle += 360;
            while (newAngle > 180) newAngle -= 360;

            double angle2 = newAngle - 180;
            double scale = 1;
            cout<< "Angle of rotation = ";
            cout<<angle2<<endl;

            //Tester sur l'angle de rotation pour l'affichage des widgets du redressement
            if (angle2!=0) {
                ui->lbl_redresse->show();
                ui->btn_redresse->show();
                ui->btn_reconnaitre->hide();
                ui->lbl_reconnaitre->hide();
            }
            else{
                ui->lbl_redresse->hide();
                ui->btn_redresse->hide();
                ui->lbl_reconnaitre->show();
                ui->btn_reconnaitre->show();
            }

            //Obtenir la matrice de rotation avec les spécifications ci-dessus
            rot_mat = getRotationMatrix2D( center, angle2 , scale );

            //Faire pivoter l'image déformée
            cv::warpAffine( src, src, rot_mat, src.size() );
            RedressedImages.push_back(src);
        }
        //Reconnaissance
        //Detection des caracteres
        test = chars.detect(src);
        chars_box = test.box;
        class_id = test.classID;
        ids = test.id;

        string classesFile = "/home/hp/Yolo/data6/PL.names";
        ifstream ifs(classesFile.c_str());
        string line;
        while (getline(ifs, line)) classes.push_back(line);
        for (int j = 0; j < ids.size(); ++j) {
            int idx1 = ids[j];
            int idx2 = class_id[idx1];
            vect.push_back({classes[idx2],chars_box[idx1].x});
        }

        //Trier dans le bon ordre les caracteres detecter
        sort(vect.begin(), vect.end(), [](const Solution& a, const Solution& b) {
              return a.box < b.box;
        });

        //Reconnaitre les caracteres detecter
        string a= "أ";
        for(auto it = vect.begin(); it != vect.end(); it++) {
            if(it->classe == "ch_0") {
                //cout << "0";
                str+= "0";
            }
            if(it->classe == "ch_1") str+= "1";//cout << "1";
            if(it->classe == "ch_2") str+= "2";//cout << "2";
            if(it->classe == "ch_3") str+= "3";//cout << "3";
            if(it->classe == "ch_4") str+= "4";//cout << "4";
            if(it->classe == "ch_5") str+= "5";//cout << "5";
            if(it->classe == "ch_6") str+= "6";//cout << "6";
            if(it->classe == "ch_7") str+= "7";//cout << "7";
            if(it->classe == "ch_8") str+= "8";//cout << "8";
            if(it->classe == "ch_9") str+= "9";//cout << "9";

            //Pour respecter le tri meme apres ajout d'une lettre Arabe
            if(it->classe == "lA_a") str+= QString(QChar(0x202D)) + "أ" + QString(QChar(0x202C));
            //"أ";//cout << a.c_str();
            if(it->classe == "lA_b") str+= QString(QChar(0x202D)) + "ب" + QString(QChar(0x202C));
            //"ب";//cout << "ب";
            if(it->classe == "lA_w") str+= QString(QChar(0x202D)) + "و" + QString(QChar(0x202C));
            //"و";//cout << "و";
            if(it->classe == "lA_d") str+= QString(QChar(0x202D)) + "د" + QString(QChar(0x202C));
            //"د";//cout << "د";
            if(it->classe == "lA_h") str+= QString(QChar(0x202D)) + "ه" + QString(QChar(0x202C));
            //"ه";//cout << "ه";
            if(it->classe == "lA_maghrib") str+= QString(QChar(0x202D)) + "المغرب" + QString(QChar(0x202C));
            //"المغرب";//cout << "المغرب";
            if(it->classe == "lA_j") str+= QString(QChar(0x202D)) + "ج" + QString(QChar(0x202C));
            //"ج";//cout << "ج";
            if(it->classe == "lA_ch") str+= QString(QChar(0x202D)) + "ش" + QString(QChar(0x202C));
            //"ش";//cout << "ش";
            if(it->classe == "lA_h_d_maghrib"){
                //str+= QString(QChar(0x202D)) + "ه د المغرب" + QString(QChar(0x202C));
                str+= QString(QChar(0x202D)) + "ب" + QString(QChar(0x202C));
                str+= QString(QChar(0x202D)) + "ر" + QString(QChar(0x202C));
                str+= QString(QChar(0x202D)) + "غ" + QString(QChar(0x202C));
                str+= QString(QChar(0x202D)) + "م" + QString(QChar(0x202C));
                str+= QString(QChar(0x202D)) + "ل" + QString(QChar(0x202C));
                str+= QString(QChar(0x202D)) + "ا" + QString(QChar(0x202C));
                str+= QString(QChar(0x202D)) + "." + QString(QChar(0x202C));
                str+= QString(QChar(0x202D)) + "د" + QString(QChar(0x202C));
                str+= QString(QChar(0x202D)) + "." + QString(QChar(0x202C));
                str+= QString(QChar(0x202D)) + "ه" + QString(QChar(0x202C));
            }
            //"ه.د. المغرب";//cout << "ه.د. المغرب";
            if(it->classe == "lA_h_k_maghrib"){
                //str+= QString(QChar(0x202D)) + "ه ك المغرب" + QString(QChar(0x202C));
                str+= QString(QChar(0x202D)) + "ب" + QString(QChar(0x202C));
                str+= QString(QChar(0x202D)) + "ر" + QString(QChar(0x202C));
                str+= QString(QChar(0x202D)) + "غ" + QString(QChar(0x202C));
                str+= QString(QChar(0x202D)) + "م" + QString(QChar(0x202C));
                str+= QString(QChar(0x202D)) + "ل" + QString(QChar(0x202C));
                str+= QString(QChar(0x202D)) + "ا" + QString(QChar(0x202C));
                str+= QString(QChar(0x202D)) + "." + QString(QChar(0x202C));
                str+= QString(QChar(0x202D)) + "ك" + QString(QChar(0x202C));
                str+= QString(QChar(0x202D)) + "." + QString(QChar(0x202C));
                str+= QString(QChar(0x202D)) + "ه" + QString(QChar(0x202C));
            }
            //"ه.ك. المغرب";//cout << "ه.ك. المغرب";
            if(it->classe == "lA_mat_maghrib"){
                //str+= QString(QChar(0x202D)) + "م أ ت المغرب" + QString(QChar(0x202C));
                str+= QString(QChar(0x202D)) + "ب" + QString(QChar(0x202C));
                str+= QString(QChar(0x202D)) + "ر" + QString(QChar(0x202C));
                str+= QString(QChar(0x202D)) + "غ" + QString(QChar(0x202C));
                str+= QString(QChar(0x202D)) + "م" + QString(QChar(0x202C));
                str+= QString(QChar(0x202D)) + "ل" + QString(QChar(0x202C));
                str+= QString(QChar(0x202D)) + "ا" + QString(QChar(0x202C));
                str+= QString(QChar(0x202D)) + "." + QString(QChar(0x202C));
                str+= QString(QChar(0x202D)) + "ت" + QString(QChar(0x202C));
                str+= QString(QChar(0x202D)) + "." + QString(QChar(0x202C));
                str+= QString(QChar(0x202D)) + "أ" + QString(QChar(0x202C));
                str+= QString(QChar(0x202D)) + "." + QString(QChar(0x202C));
                str+= QString(QChar(0x202D)) + "م" + QString(QChar(0x202C));
            }
            //"م.أ.ت. المغرب";//cout << "م.أ.ت. المغرب";
            if(it->classe == "lA_td_maghrib"){
                //str+= QString(QChar(0x202D)) + "ت د المغرب" + QString(QChar(0x202C));
                str+= QString(QChar(0x202D)) + "ب" + QString(QChar(0x202C));
                str+= QString(QChar(0x202D)) + "ر" + QString(QChar(0x202C));
                str+= QString(QChar(0x202D)) + "غ" + QString(QChar(0x202C));
                str+= QString(QChar(0x202D)) + "م" + QString(QChar(0x202C));
                str+= QString(QChar(0x202D)) + "ل" + QString(QChar(0x202C));
                str+= QString(QChar(0x202D)) + "ا" + QString(QChar(0x202C));
                str+= QString(QChar(0x202D)) + "." + QString(QChar(0x202C));
                str+= QString(QChar(0x202D)) + "د" + QString(QChar(0x202C));
                str+= QString(QChar(0x202D)) + "." + QString(QChar(0x202C));
                str+= QString(QChar(0x202D)) + "ت" + QString(QChar(0x202C));
            }
            //"ت.د. المغرب";//cout << "ت.د. المغرب";
            if(it->classe == "lF_WW") str+= "WW";//cout << "WW";
            if(it->classe == "lF_W") str+= "W";//cout << "W";
            if(it->classe == "lF_MA") str+= "MA";//cout << "MA";
            if(it->classe == "lF_CC_MAROC") str+= "CC MAROC";//cout << "CC MAROC";
            if(it->classe == "lF_CD_MAROC") str+= "CD MAROC";//cout << "CD MAROC";
            if(it->classe == "lF_CI_MAROC") str+= "CI MAROC";//cout << "CI MAROC";
            if(it->classe == "lF_FAT_MAROC") str+= "FAT MAROC";//cout << "FAT MAROC";
            if(it->classe == "separateur") {
                //str+= " | ";//cout << " | ";
                str+= QString(QChar(0x202D)) + " | " + QString(QChar(0x202C));
            }
            }
        ContextSTR.push_back(str);
        vect.clear();
        str.clear();
        degree.clear();
        name.str("");
    }
    vect_Images.clear();
}

//On click du boutton "Redresser la plaque"
void MainWindow::on_btn_redresse_clicked()
{
    //Boucler sur les PI a redresser
    for (int var = 0; var < RedressedImages.size(); ++var) {
        cvtColor(RedressedImages[var], RedressedImages[var], CV_BGR2RGB);
        imgIn= QImage((uchar*) RedressedImages[var].data, RedressedImages[var].cols, RedressedImages[var].rows, RedressedImages[var].step, QImage::Format_RGB888);
        imgIn = imgIn.scaledToWidth(ui->lbl_redresse->width(),Qt::SmoothTransformation);
        ui->lbl_redresse->setPixmap(QPixmap::fromImage(imgIn));
        ui->lbl_redresse->setAlignment(Qt::AlignCenter);
    }

    //Afficher l'image de la PI redresser dans l'IG (UI)
    ui->lbl_reconnaitre->show();
    ui->btn_reconnaitre->show();
}

//On click du boutton "Reconnaitre la plaque"
void MainWindow::on_btn_reconnaitre_clicked()
{
    for (int var = 0; var < ContextSTR.size(); ++var) {
        //std::string txt =ContextSTR[var].c_str();
        QString qstr = ContextSTR[var];//QString::fromStdString()

        //Afficher le contexte de la PI dans l'IG (UI)
        ui->lbl_reconnaitre->setText(qstr);
        ui->lbl_reconnaitre->setAlignment(Qt::AlignCenter);
    }
}


void MainWindow::on_pushButton_clicked()
{
    FirstWindow fw;
    fw.setModal(true);
    fw.exec();
}
