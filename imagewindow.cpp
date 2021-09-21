#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <QBoxLayout>
#include <QElapsedTimer>
#include <QFileDialog>
#include "vehicledetector.h"
#include "charactersdetector.h"

#include "imagewindow.h"
#include "ui_imagewindow.h"

using namespace cv;
using namespace std;

//Initialiser les variables utilisees
char* warp_rotate_window1 = "License Plate";
Mat frame1;
QImage imgIn1;
vector<Mat> RedressedImages1;
vector<QString> ContextSTR1;
QStringList strlDir1;
QString absolute1;

vector<Rect> box1;
vector<string> classes1;
Doub test1;
plate car1;
vector<float> conf1;
vector<Solution> vect1;
vector<Rect> chars_box1;
vector<int> class_id1;
vector<int> ids1;
VehicleDetector detector1;
charactersDetector chars1;
Mat img1;
float confidence1;
vector<Image> vect_Images1;
QString strs;
Mat newImage1;
vector<double> degree1;
Mat rot_mat1( 2, 3, CV_32FC1 );
Mat warp_mat1( 2, 3, CV_32FC1 );
Mat src1, warp_dst1, warp_rotate_dst1;
vector<LP> licensePlate1;
std::ostringstream name1;
int iterate1;
VideoCapture cap1;

ImageWindow::ImageWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImageWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Système ALPR de MAScIR");
    QPixmap pix("/home/hp/Pictures/Mascir.png");
    pix = pix.scaledToWidth(ui->lbl_mascir_image->width(),Qt::SmoothTransformation);
    ui->lbl_mascir_image->setPixmap(pix);

    //Creation des Layout et ajout des widgets.
    ui->contenu_horizontalLayout->addWidget(ui->lbl_contenu_2);
    ui->contenu_horizontalLayout->addWidget(ui->lbl_contenu_image);
    ui->contenu_horizontalLayout->setAlignment(Qt::AlignHCenter);
    ui->lbl_contenu_2->hide();
}

ImageWindow::~ImageWindow()
{
    delete ui;
}

void ImageWindow::on_btn_parcourir_image_clicked()
{
    detector1.initNetwork();

    //Ouverture de l'image
    QString filename = QFileDialog::getOpenFileName(this,
                                                    tr("Choose"),
                                                    QDir::homePath(),
                                                    tr("Images(*.png *.jpg *.jpeg *.bmp *.gif)"));
//    ui->lbl_detect->setText(filename);

    String path = filename.toStdString().c_str();
    frame1=imread(path);

    //Creation d'un Timer pour connaitre le temps d'execution de la detection.
    QElapsedTimer timer1;
    timer1.start();

    //Detection de la PI
    car1 = detector1.detect(frame1);
    box1 = car1.box;
    conf1 =car1.conf;
    vector<int> indices = car1.id;
//    cout<<indices.size()<<endl;

    //Boucler sur les PI detecter.
    for (int j = 0; j < indices.size(); ++j) {
        int idx = indices[j];
        Rect boxPlate = box1[idx];
        frame1(boxPlate).copyTo(img1);
        confidence1 = conf1[idx];
        vect_Images1.push_back({img1,confidence1});
    }
//    cout<<vect_Images.size()<<endl;

    if (QString::compare(filename,QString()) != 0) {
        QImage image;
        bool valid = image.load(filename);
        if (valid) {

            //Afficher l'image du vehicule dans l'IG (UI)
            image = image.scaledToWidth(ui->lbl_image_frame->width(),Qt::SmoothTransformation);
            ui->lbl_image_frame->setPixmap(QPixmap::fromImage(image));

            //Recuperer le chemin absolut de cette image
            QDir fileDir = QFileInfo(filename).absoluteDir();
            absolute1=fileDir.absolutePath();
            QStringList filters;
            filters << "*.png" << "*.xpm" << "*.jpg" << "*.jpeg";
            fileDir.setNameFilters(filters);
            strlDir1 = fileDir.entryList();
            iterate1 = strlDir1.indexOf(filename);
        }
        else {
            //Erreur
        }
    }
        //}
    for (vector<Image>::iterator iter = vect_Images1.begin() ; iter != vect_Images1.end(); ++iter){
        cv::resize(iter->image,iter->image,cv::Size(400,100));
        newImage1 = iter->image;
        cv::cvtColor(newImage1,newImage1, CV_BGR2GRAY);
        //Canny edge detector
        cv::Canny(newImage1,newImage1,100,50);
        vector<Vec4i> lines;

        //Houghlines pour detecter les lignes dans l'image de la PI
        cv::HoughLinesP(newImage1, lines, 1, CV_PI/180, 100,100,100);
        cout<< "Number of lines = ";
        cout << lines.size() << endl;

        src1 = iter->image; //newImage
        if (!lines.empty()) {
            for( size_t i = 0; i < lines.size(); i++ )
              {
                Vec4i l = lines[i];
                cv::line( newImage1, Point(l[0], l[1]), Point(l[2], l[3]), cv::Scalar(94.0, 206.0, 165.0), 3, CV_AA);
                Point p1, p2;
                p1=Point(l[0], l[1]);
                p2=Point(l[2], l[3]);
                degree1.push_back(fmodf((atan2(p1.y - p2.y, p1.x - p2.x) * (180.0 / 3.14159265)) + 360, 360));
              }

            double MaxDegree = degree1[0];
            for (int var = 1; var < degree1.size(); ++var) {
                if(MaxDegree < degree1[var]){
                    MaxDegree = degree1[var];
                }
            }
            cout<< "Degree of the angle = ";
            cout<<MaxDegree<<endl;

            //Définir l'image src du même type et de la même taille que src
            warp_dst1 = Mat::zeros( src1.rows, src1.cols, src1.type() );

            //Calculer une matrice de rotation par rapport au centre de l'image
            Point center = Point( src1.cols/2, src1.rows/2 );

            //Un moyen facile de garder des angles entre -179 et 180 degrés
            int newAngle = MaxDegree;
            while (newAngle <= -180) newAngle += 360;
            while (newAngle > 180) newAngle -= 360;

            double angle2 = newAngle - 180;
            double scale = 1;
            cout<< "Angle of rotation = ";
            cout<<angle2<<endl;

            //Obtenir la matrice de rotation avec les spécifications ci-dessus
            rot_mat1 = getRotationMatrix2D( center, angle2 , scale );

            //Faire pivoter l'image déformée
            cv::warpAffine( src1, src1, rot_mat1, src1.size() );
            RedressedImages1.push_back(src1);
        }
        //Reconnaissance
        //Detection des caracteres
        chars1.initNetwork();
        test1 = chars1.detect(src1);
        chars_box1 = test1.box;
        class_id1 = test1.classID;
        ids1 = test1.id;

        string classesFile = "/media/hp/LENOVO_USB_HDD/ubuntu_Zainab/Travail/weights/data6/PL.names";
        ifstream ifs(classesFile.c_str());
        string line;
        while (getline(ifs, line)) classes1.push_back(line);
        for (int j = 0; j < ids1.size(); ++j) {
            int idx1 = ids1[j];
            int idx2 = class_id1[idx1];
            vect1.push_back({classes1[idx2],chars_box1[idx1].x});
        }

        //Trier dans le bon ordre les caracteres detecter
        sort(vect1.begin(), vect1.end(), [](const Solution& a, const Solution& b) {
              return a.box < b.box;
        });

        //Reconnaitre les caracteres detecter
        string a= "أ";
        for(auto it = vect1.begin(); it != vect1.end(); it++) {
            if(it->classe == "ch_0") {
                //cout << "0";
                strs+= "0";
            }
            if(it->classe == "ch_1") strs+= "1";//cout << "1";
            if(it->classe == "ch_2") strs+= "2";//cout << "2";
            if(it->classe == "ch_3") strs+= "3";//cout << "3";
            if(it->classe == "ch_4") strs+= "4";//cout << "4";
            if(it->classe == "ch_5") strs+= "5";//cout << "5";
            if(it->classe == "ch_6") strs+= "6";//cout << "6";
            if(it->classe == "ch_7") strs+= "7";//cout << "7";
            if(it->classe == "ch_8") strs+= "8";//cout << "8";
            if(it->classe == "ch_9") strs+= "9";//cout << "9";

            //Pour respecter le tri meme apres ajout d'une lettre Arabe
            if(it->classe == "lA_a") strs+= QString(QChar(0x202D)) + "أ" + QString(QChar(0x202C));
            //"أ";//cout << a.c_str();
            if(it->classe == "lA_b") strs+= QString(QChar(0x202D)) + "ب" + QString(QChar(0x202C));
            //"ب";//cout << "ب";
            if(it->classe == "lA_w") strs+= QString(QChar(0x202D)) + "و" + QString(QChar(0x202C));
            //"و";//cout << "و";
            if(it->classe == "lA_d") strs+= QString(QChar(0x202D)) + "د" + QString(QChar(0x202C));
            //"د";//cout << "د";
            if(it->classe == "lA_h") strs+= QString(QChar(0x202D)) + "ه" + QString(QChar(0x202C));
            //"ه";//cout << "ه";
            if(it->classe == "lA_maghrib") strs+= QString(QChar(0x202D)) + "المغرب" + QString(QChar(0x202C));
            //"المغرب";//cout << "المغرب";
            if(it->classe == "lA_j") strs+= QString(QChar(0x202D)) + "ج" + QString(QChar(0x202C));
            //"ج";//cout << "ج";
            if(it->classe == "lA_ch") strs+= QString(QChar(0x202D)) + "ش" + QString(QChar(0x202C));
            //"ش";//cout << "ش";
            if(it->classe == "lA_h_d_maghrib"){
                //str+= QString(QChar(0x202D)) + "ه د المغرب" + QString(QChar(0x202C));
                strs+= QString(QChar(0x202D)) + "ب" + QString(QChar(0x202C));
                strs+= QString(QChar(0x202D)) + "ر" + QString(QChar(0x202C));
                strs+= QString(QChar(0x202D)) + "غ" + QString(QChar(0x202C));
                strs+= QString(QChar(0x202D)) + "م" + QString(QChar(0x202C));
                strs+= QString(QChar(0x202D)) + "ل" + QString(QChar(0x202C));
                strs+= QString(QChar(0x202D)) + "ا" + QString(QChar(0x202C));
                strs+= QString(QChar(0x202D)) + "." + QString(QChar(0x202C));
                strs+= QString(QChar(0x202D)) + "د" + QString(QChar(0x202C));
                strs+= QString(QChar(0x202D)) + "." + QString(QChar(0x202C));
                strs+= QString(QChar(0x202D)) + "ه" + QString(QChar(0x202C));
            }
            //"ه.د. المغرب";//cout << "ه.د. المغرب";
            if(it->classe == "lA_h_k_maghrib"){
                //str+= QString(QChar(0x202D)) + "ه ك المغرب" + QString(QChar(0x202C));
                strs+= QString(QChar(0x202D)) + "ب" + QString(QChar(0x202C));
                strs+= QString(QChar(0x202D)) + "ر" + QString(QChar(0x202C));
                strs+= QString(QChar(0x202D)) + "غ" + QString(QChar(0x202C));
                strs+= QString(QChar(0x202D)) + "م" + QString(QChar(0x202C));
                strs+= QString(QChar(0x202D)) + "ل" + QString(QChar(0x202C));
                strs+= QString(QChar(0x202D)) + "ا" + QString(QChar(0x202C));
                strs+= QString(QChar(0x202D)) + "." + QString(QChar(0x202C));
                strs+= QString(QChar(0x202D)) + "ك" + QString(QChar(0x202C));
                strs+= QString(QChar(0x202D)) + "." + QString(QChar(0x202C));
                strs+= QString(QChar(0x202D)) + "ه" + QString(QChar(0x202C));
            }
            //"ه.ك. المغرب";//cout << "ه.ك. المغرب";
            if(it->classe == "lA_mat_maghrib"){
                //str+= QString(QChar(0x202D)) + "م أ ت المغرب" + QString(QChar(0x202C));
                strs+= QString(QChar(0x202D)) + "ب" + QString(QChar(0x202C));
                strs+= QString(QChar(0x202D)) + "ر" + QString(QChar(0x202C));
                strs+= QString(QChar(0x202D)) + "غ" + QString(QChar(0x202C));
                strs+= QString(QChar(0x202D)) + "م" + QString(QChar(0x202C));
                strs+= QString(QChar(0x202D)) + "ل" + QString(QChar(0x202C));
                strs+= QString(QChar(0x202D)) + "ا" + QString(QChar(0x202C));
                strs+= QString(QChar(0x202D)) + "." + QString(QChar(0x202C));
                strs+= QString(QChar(0x202D)) + "ت" + QString(QChar(0x202C));
                strs+= QString(QChar(0x202D)) + "." + QString(QChar(0x202C));
                strs+= QString(QChar(0x202D)) + "أ" + QString(QChar(0x202C));
                strs+= QString(QChar(0x202D)) + "." + QString(QChar(0x202C));
                strs+= QString(QChar(0x202D)) + "م" + QString(QChar(0x202C));
            }
            //"م.أ.ت. المغرب";//cout << "م.أ.ت. المغرب";
            if(it->classe == "lA_td_maghrib"){
                //str+= QString(QChar(0x202D)) + "ت د المغرب" + QString(QChar(0x202C));
                strs+= QString(QChar(0x202D)) + "ب" + QString(QChar(0x202C));
                strs+= QString(QChar(0x202D)) + "ر" + QString(QChar(0x202C));
                strs+= QString(QChar(0x202D)) + "غ" + QString(QChar(0x202C));
                strs+= QString(QChar(0x202D)) + "م" + QString(QChar(0x202C));
                strs+= QString(QChar(0x202D)) + "ل" + QString(QChar(0x202C));
                strs+= QString(QChar(0x202D)) + "ا" + QString(QChar(0x202C));
                strs+= QString(QChar(0x202D)) + "." + QString(QChar(0x202C));
                strs+= QString(QChar(0x202D)) + "د" + QString(QChar(0x202C));
                strs+= QString(QChar(0x202D)) + "." + QString(QChar(0x202C));
                strs+= QString(QChar(0x202D)) + "ت" + QString(QChar(0x202C));
            }
            //"ت.د. المغرب";//cout << "ت.د. المغرب";
            if(it->classe == "lF_WW") strs+= "WW";//cout << "WW";
            if(it->classe == "lF_W") strs+= "W";//cout << "W";
            if(it->classe == "lF_MA") strs+= "MA";//cout << "MA";
            if(it->classe == "lF_CC_MAROC") strs+= "CC MAROC";//cout << "CC MAROC";
            if(it->classe == "lF_CD_MAROC") strs+= "CD MAROC";//cout << "CD MAROC";
            if(it->classe == "lF_CI_MAROC") strs+= "CI MAROC";//cout << "CI MAROC";
            if(it->classe == "lF_FAT_MAROC") strs+= "FAT MAROC";//cout << "FAT MAROC";
            if(it->classe == "separateur") {
                //str+= " | ";//cout << " | ";
                strs+= QString(QChar(0x202D)) + " | " + QString(QChar(0x202C));
            }
            }
        ui->lbl_contenu_image->setText(strs);
        if (!ui->lbl_contenu_image->pixmap()) {
            ui->lbl_contenu_2->show();
        }
        ContextSTR1.push_back(strs);
        vect1.clear();
        strs.clear();
        degree1.clear();
        name1.str("");
    }
    vect_Images1.clear();
//    }
    cout<< "The detection took : " << timer1.elapsed() << " milliseconds"<<endl;
}
