#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <QBoxLayout>
#include <QElapsedTimer>
#include <QFileDialog>
#include <iomanip> // to format image names using setw() and setfill()
#include <unistd.h>.
#include <set>
#include <numeric>
#include <track.h>
#include <QtCore>
#include <QtGui>
#include <QColor>

#include "vehicledetector.h"
#include "charactersdetector.h"
#include "videowindow.h"
#include "ui_videowindow.h"


using namespace cv;
using namespace std;

//Initialiser les variables utilisees
char* warp_rotate_window2 = "License Plate";
Mat frame2;
QImage imgIn2;
vector<Mat> RedressedImages2;
vector<QString> ContextSTR2;
QStringList strlDir2;
QString absolute2;

vector<Rect> box2;
vector<string> classes2;
Doub test2;
plate car2;
vector<float> conf2;
vector<Solution> vect2;
vector<Rect> chars_box2;
vector<int> class_id2;
vector<int> ids2;
VehicleDetector detector2;
charactersDetector chars2;
Mat img2;
float confidence2;
vector<Image> vect_Images2;
QString str2;
Mat newImage2;
vector<double> degree2;
Mat rot_mat2( 2, 3, CV_32FC1 );
Mat warp_mat2( 2, 3, CV_32FC1 );
Mat src2, warp_dst2, warp_rotate_dst2;
vector<LP> licensePlate2;
std::ostringstream name2;
int iterate2;
VideoCapture cap2;

Track track;
vector<TrackBox> detData;


VideoWindow::VideoWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VideoWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Système ALPR de MAScIR");

    QPixmap pix("/home/hp/Pictures/Mascir.png");
    pix = pix.scaledToWidth(ui->lbl_mascir_video->width(),Qt::SmoothTransformation);
    ui->lbl_mascir_video->setPixmap(pix);

    //Creation des Layout et ajout des widgets.
    ui->contenu_horizontalLayout->addWidget(ui->lbl_contenu_2);
    ui->contenu_horizontalLayout->addWidget(ui->lbl_contenu_video);
    ui->contenu_horizontalLayout->setAlignment(Qt::AlignHCenter);
    ui->lbl_contenu_2->hide();
}

VideoWindow::~VideoWindow()
{
    delete ui;
}

void VideoWindow::on_btn_parcourir_video_clicked()
{
    detector2.initNetwork();
    chars2.initNetwork();

    //Ouverture de l'image
    QString filename = QFileDialog::getOpenFileName(this,
                                                    tr("Choose"),
                                                    QDir::homePath(),
                                                    tr("Videos(*.mp4 *.avi *.gif *.webm)"));
//    ui->lbl_detect->setText(filename);

    String path = filename.toStdString().c_str();
    cap2.open(path);
    frame2=imread(path);

    //Creation d'un Timer pour connaitre le temps d'execution de la detection.
    QElapsedTimer timer1;
    timer1.start();
    while (true)
    {
        waitKey(1);
        cap2 >> frame2;
        if (frame2.empty())
        {
            waitKey();
            break;
        }
        cv::resize(frame2,frame2,cv::Size(1280,720));

//        imshow("frame",frame);
//        waitKey(1);
        //Detection de la PI
        int left = 500;
        int top = 100;
        int height = 359;
        int width = 617;
        int baseLine = 1000;
        //rectangle(frame, Point(left, top - round(1.5*labelSize.height)), Point(left + round(1.5*labelSize.width), top + baseLine), Scalar(255, 255, 255), FILLED);
        cout<<frame2.size()<<endl;
        cout<<ui->lbl_video_frame->size().width()<<endl;
//        rectangle(frame2, Point(222,222), Point(0,0), Scalar(255, 255, 255), 3);
        //rectangle(frame2, Point(1500,650), Point(0,650), Scalar(255, 255, 255), 3);
        Rect myView = Rect(Point(1100, 600),Point(500, 400));//Rect(1300, 500, 100, 650);
//        cout<<"Width : ";
//        cout<<myView.width<<endl;
//        cout<<"Height : ";
//        cout<<myView.height<<endl;

//        rectangle(frame2, Point(1500,500), Point(16,650), Scalar(255, 255, 255), 3);
        rectangle(frame2, Point(1100, 600),Point(500, 400), Scalar(255, 255, 255), 3);

        Mat chosenView;
//        cv::Mat submat = frame2(myView).clone();
        frame2(myView).copyTo(chosenView);
//        rectangle(frame2, Point(133, 133), Point(133, 133), Scalar(255, 255, 255), 3);
        car2 = detector2.detect(frame2);
        box2 = car2.box;

        if (!box2.empty()) {
            detData = track.Trackbox(box2);
            for(int i =0; i<detData.size();i++){
                if (   (detData.at(i).box.x+detData.at(i).box.width) < (myView.x+myView.width)
                        && (detData.at(i).box.x) > (myView.x)
                        && (detData.at(i).box.y) > (myView.y)
                        && (detData.at(i).box.y+detData.at(i).box.height) < (myView.y+myView.height)
                        )
                {
                    int idx = detData.at(i).id;
                    Rect_<float> boxPlate = detData.at(i).box;
                    frame2(boxPlate).copyTo(img2);
                    vect_Images2.push_back({img2});
                    cout<<"Nowwwwwwwwwwwwwwwwwwwwwwwwww"<<endl;

                    imshow("",img2);
                    waitKey(1);
                    cv::rectangle(frame2,detData.at(i).box, cv::Scalar(255, 0, 0), 2, 8, 0);
                    cv::putText(frame2, std::to_string(detData.at(i).id),
                                cv::Point(detData.at(i).box.x , detData.at(i).box.y  ), 1, 3, cv::Scalar(255, 0, 0), 5);
                }
            }
            conf2 = car2.conf;
            vector<int> indices = car2.id;
        //    cout<<indices.size()<<endl;

            //Boucler sur les PI detecter.
//            for (int j = 0; j < indices.size(); ++j) {
//                int idx = indices[j];
//                Rect boxPlate = box2[idx];
//                if (   (box2[idx].x+box2[idx].width) < (myView.x+myView.width)
//                        && (box2[idx].x) > (myView.x)
//                        && (box2[idx].y) > (myView.y)
//                        && (box2[idx].y+box2[idx].height) < (myView.y+myView.height)
//                        )
//                {

//                    if (boxPlate.x>0 && boxPlate.y>0 && boxPlate.x+boxPlate.width<frame2.cols && boxPlate.y+boxPlate.height<frame2.rows){
//                        frame2(boxPlate).copyTo(img2);
//                        confidence2 = conf2[idx];
//                        vect_Images2.push_back({img2,confidence2});
//                    }
//                    cout<<"Nowwwwwwwwwwwwwwwwwwwwwwwwww"<<endl;
//                }
//            }
            imshow("View",chosenView);
            waitKey(1);
        }

    //    cout<<vect_Images.size()<<endl;

        //for (vector<Image>::iterator iter = vect_Images.begin() ; iter != vect_Images.end(); ++iter){
//        if (QString::compare(filename,QString()) != 0) {
            QImage image;
            image = QImage((uchar*) frame2.data, frame2.cols, frame2.rows, frame2.step, QImage::Format_RGB888);
//            bool valid = image.load(filename);
//            if (valid) {

                //Afficher l'image du vehicule dans l'IG (UI)
                image = image.scaledToWidth(ui->lbl_video_frame->width(),Qt::SmoothTransformation);
                ui->lbl_video_frame->setPixmap(QPixmap::fromImage(image));

                //Recuperer le chemin absolut de cette image
                QDir fileDir = QFileInfo(filename).absoluteDir();
                absolute2=fileDir.absolutePath();
                QStringList filters;
                filters << "*.png" << "*.xpm" << "*.jpg" << "*.jpeg" << "*.webm" << "*.mp4" << "*.avi" << "*.gif";
                fileDir.setNameFilters(filters);
                strlDir2 = fileDir.entryList();
                iterate2 = strlDir2.indexOf(filename);
//            }
//            else {
//                //Erreur
//            }
        //}
        //}

                for (vector<Image>::iterator iter = vect_Images2.begin() ; iter != vect_Images2.end(); ++iter){
                    cv::resize(iter->image,iter->image,cv::Size(400,100));
                    newImage2 = iter->image;//image;
            //        imshow("LP",newImage);
            //        waitKey(0);

                    cv::cvtColor(newImage2,newImage2, CV_BGR2GRAY);
                    cv::Canny(newImage2,newImage2,100,50);

                    // Standard Hough Line Transform
                    vector<Vec4i> lines;
                    cv::HoughLinesP(newImage2, lines, 1, CV_PI/180, 100,100,100);
                    cout<< "Number of lines = ";
                    cout << lines.size() << endl;

                    /// Load the image
                    src2 = iter->image;//image; //newImage
                    if (!lines.empty()) {
                        for( size_t i = 0; i < lines.size(); i++ )
                          {
                            Vec4i l = lines[i];
                            cv::line( newImage2, Point(l[0], l[1]), Point(l[2], l[3]), cv::Scalar(94.0, 206.0, 165.0), 3, CV_AA);
                            Point p1, p2;
                            p1=Point(l[0], l[1]);
                            p2=Point(l[2], l[3]);
                            degree2.push_back(fmodf((atan2(p1.y - p2.y, p1.x - p2.x) * (180.0 / 3.14159265)) + 360, 360));
                          }

                        double MaxDegree = degree2[0];
                        for (int var = 1; var < degree2.size(); ++var) {
                            if(MaxDegree < degree2[var]){
                                MaxDegree = degree2[var];
                            }
                        }
                        cout<< "Degree of the angle = ";
                        cout<<MaxDegree<<endl;
//                        if (box[idx].x>0 && box[idx].y>0 && box[idx].x+box[idx].width<frame.cols && box[idx].y+box[idx].height<frame.rows){
                            /// Set the dst image the same type and size as src
                            warp_dst2 = Mat::zeros( src2.rows, src2.cols, src2.type() );

                            cout<<"Hello"<<endl;
                            /// Compute a rotation matrix with respect to the center of the image
                            Point center = Point( src2.cols/2, src2.rows/2 );

                            //Easy way to keeping angles between -179 and 180 degrees
                            int newAngle = MaxDegree;
                            while (newAngle <= -180) newAngle += 360;
                            while (newAngle > 180) newAngle -= 360;

                            double angle2 = newAngle - 180;
                            double scale = 1;
                            cout<< "Angle of rotation = ";
                            cout<<angle2<<endl;

                            /// Get the rotation matrix with the specifications above
                            rot_mat2 = getRotationMatrix2D( center, angle2 , scale );

                            /// Rotate the warped image
                            cv::warpAffine( src2, src2, rot_mat2, src2.size() );

                            ///Show redressed image
    //                        warp_rotate_window2 = "Warp + Rotate License Plate";
    //                        namedWindow( warp_rotate_window2, CV_WINDOW_AUTOSIZE );
                //            imshow( warp_rotate_window, src );
                //            waitKey(0);
//                        }
                    }

                    //Recognition
                    chars2.initNetwork();
                    cout<<"test 1"<<endl;
                    test2 = chars2.detect(src2);
                    cout<<"test 2"<<endl;
                    chars_box2 = test2.box;
                    class_id2 = test2.classID;
                    ids2 = test2.id;

                    string classesFile = "/media/hp/LENOVO_USB_HDD/ubuntu_Zainab/Travail/weights/data8/CR.names";
                    ifstream ifs(classesFile.c_str());
                    string line;
                    while (getline(ifs, line)) classes2.push_back(line);

                    for (int j = 0; j < ids2.size(); ++j) {
                        int idx1 = ids2[j];
                        int idx2 = class_id2[idx1];
                        vect2.push_back({classes2[idx2],chars_box2[idx1].x});
                    }
                    sort(vect2.begin(), vect2.end(), [](const Solution& a, const Solution& b) {
                          return a.box < b.box;
                    });
                    string a= "أ";
                    for(auto it = vect2.begin(); it != vect2.end(); it++) {
                        if(it->classe == "ch_0") {
                            //cout << "0";
                            str2+= "0";
                        }
                        if(it->classe == "ch_1") str2+= "1";//cout << "1";
                        if(it->classe == "ch_2") str2+= "2";//cout << "2";
                        if(it->classe == "ch_3") str2+= "3";//cout << "3";
                        if(it->classe == "ch_4") str2+= "4";//cout << "4";
                        if(it->classe == "ch_5") str2+= "5";//cout << "5";
                        if(it->classe == "ch_6") str2+= "6";//cout << "6";
                        if(it->classe == "ch_7") str2+= "7";//cout << "7";
                        if(it->classe == "ch_8") str2+= "8";//cout << "8";
                        if(it->classe == "ch_9") str2+= "9";//cout << "9";
                        //Pour respecter le tri meme apres ajout d'une lettre Arabe
                        if(it->classe == "lA_a") str2+= QString(QChar(0x202D)) + "أ" + QString(QChar(0x202C));
                        //"أ";//cout << a.c_str();
                        if(it->classe == "lA_b") str2+= QString(QChar(0x202D)) + "ب" + QString(QChar(0x202C));
                        //"ب";//cout << "ب";
                        if(it->classe == "lA_w") str2+= QString(QChar(0x202D)) + "و" + QString(QChar(0x202C));
                        //"و";//cout << "و";
                        if(it->classe == "lA_d") str2+= QString(QChar(0x202D)) + "د" + QString(QChar(0x202C));
                        //"د";//cout << "د";
                        if(it->classe == "lA_h") str2+= QString(QChar(0x202D)) + "ه" + QString(QChar(0x202C));
                        //"ه";//cout << "ه";
                        if(it->classe == "lA_maghrib") str2+= QString(QChar(0x202D)) + "المغرب" + QString(QChar(0x202C));
                        //"المغرب";//cout << "المغرب";
                        if(it->classe == "lA_j") str2+= QString(QChar(0x202D)) + "ج" + QString(QChar(0x202C));
                        //"ج";//cout << "ج";
                        if(it->classe == "lA_ch") str2+= QString(QChar(0x202D)) + "ش" + QString(QChar(0x202C));
                        //"ش";//cout << "ش";
                        if(it->classe == "lA_h_d_maghrib"){
                            //str+= QString(QChar(0x202D)) + "ه د المغرب" + QString(QChar(0x202C));
                            str2+= QString(QChar(0x202D)) + "ب" + QString(QChar(0x202C));
                            str2+= QString(QChar(0x202D)) + "ر" + QString(QChar(0x202C));
                            str2+= QString(QChar(0x202D)) + "غ" + QString(QChar(0x202C));
                            str2+= QString(QChar(0x202D)) + "م" + QString(QChar(0x202C));
                            str2+= QString(QChar(0x202D)) + "ل" + QString(QChar(0x202C));
                            str2+= QString(QChar(0x202D)) + "ا" + QString(QChar(0x202C));
                            str2+= QString(QChar(0x202D)) + "." + QString(QChar(0x202C));
                            str2+= QString(QChar(0x202D)) + "د" + QString(QChar(0x202C));
                            str2+= QString(QChar(0x202D)) + "." + QString(QChar(0x202C));
                            str2+= QString(QChar(0x202D)) + "ه" + QString(QChar(0x202C));
                        }
                        //"ه.د. المغرب";//cout << "ه.د. المغرب";
                        if(it->classe == "lA_h_k_maghrib"){
                            //str+= QString(QChar(0x202D)) + "ه ك المغرب" + QString(QChar(0x202C));
                            str2+= QString(QChar(0x202D)) + "ب" + QString(QChar(0x202C));
                            str2+= QString(QChar(0x202D)) + "ر" + QString(QChar(0x202C));
                            str2+= QString(QChar(0x202D)) + "غ" + QString(QChar(0x202C));
                            str2+= QString(QChar(0x202D)) + "م" + QString(QChar(0x202C));
                            str2+= QString(QChar(0x202D)) + "ل" + QString(QChar(0x202C));
                            str2+= QString(QChar(0x202D)) + "ا" + QString(QChar(0x202C));
                            str2+= QString(QChar(0x202D)) + "." + QString(QChar(0x202C));
                            str2+= QString(QChar(0x202D)) + "ك" + QString(QChar(0x202C));
                            str2+= QString(QChar(0x202D)) + "." + QString(QChar(0x202C));
                            str2+= QString(QChar(0x202D)) + "ه" + QString(QChar(0x202C));
                        }
                        //"ه.ك. المغرب";//cout << "ه.ك. المغرب";
                        if(it->classe == "lA_mat_maghrib"){
                            //str+= QString(QChar(0x202D)) + "م أ ت المغرب" + QString(QChar(0x202C));
                            str2+= QString(QChar(0x202D)) + "ب" + QString(QChar(0x202C));
                            str2+= QString(QChar(0x202D)) + "ر" + QString(QChar(0x202C));
                            str2+= QString(QChar(0x202D)) + "غ" + QString(QChar(0x202C));
                            str2+= QString(QChar(0x202D)) + "م" + QString(QChar(0x202C));
                            str2+= QString(QChar(0x202D)) + "ل" + QString(QChar(0x202C));
                            str2+= QString(QChar(0x202D)) + "ا" + QString(QChar(0x202C));
                            str2+= QString(QChar(0x202D)) + "." + QString(QChar(0x202C));
                            str2+= QString(QChar(0x202D)) + "ت" + QString(QChar(0x202C));
                            str2+= QString(QChar(0x202D)) + "." + QString(QChar(0x202C));
                            str2+= QString(QChar(0x202D)) + "أ" + QString(QChar(0x202C));
                            str2+= QString(QChar(0x202D)) + "." + QString(QChar(0x202C));
                            str2+= QString(QChar(0x202D)) + "م" + QString(QChar(0x202C));
                        }
                        //"م.أ.ت. المغرب";//cout << "م.أ.ت. المغرب";
                        if(it->classe == "lA_td_maghrib"){
                            //str+= QString(QChar(0x202D)) + "ت د المغرب" + QString(QChar(0x202C));
                            str2+= QString(QChar(0x202D)) + "ب" + QString(QChar(0x202C));
                            str2+= QString(QChar(0x202D)) + "ر" + QString(QChar(0x202C));
                            str2+= QString(QChar(0x202D)) + "غ" + QString(QChar(0x202C));
                            str2+= QString(QChar(0x202D)) + "م" + QString(QChar(0x202C));
                            str2+= QString(QChar(0x202D)) + "ل" + QString(QChar(0x202C));
                            str2+= QString(QChar(0x202D)) + "ا" + QString(QChar(0x202C));
                            str2+= QString(QChar(0x202D)) + "." + QString(QChar(0x202C));
                            str2+= QString(QChar(0x202D)) + "د" + QString(QChar(0x202C));
                            str2+= QString(QChar(0x202D)) + "." + QString(QChar(0x202C));
                            str2+= QString(QChar(0x202D)) + "ت" + QString(QChar(0x202C));
                        }
                        //"ت.د. المغرب";//cout << "ت.د. المغرب";
                        if(it->classe == "lF_WW") str2+= "WW";//cout << "WW";
                        if(it->classe == "lF_W") str2+= "W";//cout << "W";
                        if(it->classe == "lF_MA") str2+= "MA";//cout << "MA";
                        if(it->classe == "lF_CC_MAROC") str2+= "CC MAROC";//cout << "CC MAROC";
                        if(it->classe == "lF_CD_MAROC") str2+= "CD MAROC";//cout << "CD MAROC";
                        if(it->classe == "lF_CI_MAROC") str2+= "CI MAROC";//cout << "CI MAROC";
                        if(it->classe == "lF_FAT_MAROC") str2+= "FAT MAROC";//cout << "FAT MAROC";
                        if(it->classe == "separateur") {
                            //str+= " | ";//cout << " | ";
                            str2+= QString(QChar(0x202D)) + " | " + QString(QChar(0x202C));
                        }
                        if(it->classe == "lF_A") str2+= "A";
                        if(it->classe == "lF_B") str2+= "B";
                        if(it->classe == "lF_C") str2+= "C";
                        if(it->classe == "lF_D") str2+= "D";
                        if(it->classe == "lF_E") str2+= "E";
                        if(it->classe == "lF_F") str2+= "F";
                        if(it->classe == "lF_G") str2+= "G";
                        if(it->classe == "lF_H") str2+= "H";
                        if(it->classe == "lF_I") str2+= "I";
                        if(it->classe == "lF_J") str2+= "J";
                        if(it->classe == "lF_K") str2+= "K";
                        if(it->classe == "lF_L") str2+= "L";
                        if(it->classe == "lF_M") str2+= "M";
                        if(it->classe == "lF_N") str2+= "N";
                        if(it->classe == "lF_O") str2+= "O";
                        if(it->classe == "lF_P") str2+= "P";
                        if(it->classe == "lF_Q") str2+= "Q";
                        if(it->classe == "lF_R") str2+= "R";
                        if(it->classe == "lF_S") str2+= "S";
                        if(it->classe == "lF_T") str2+= "T";
                        if(it->classe == "lF_U") str2+= "U";
                        if(it->classe == "lF_V") str2+= "V";
                        if(it->classe == "lF_X") str2+= "X";
                        if(it->classe == "lF_Y") str2+= "Y";
                        if(it->classe == "lF_Z") str2+= "Z";
                        }
//                    cout << str2;
//                    cout << '\n';
                    ui->lbl_contenu_video->setText(str2);
                    if (!ui->lbl_contenu_video->pixmap()) {
                        ui->lbl_contenu_2->show();
                    }
//                    if (lines.empty()) {
//            //            imshow(warp_rotate_window,src);
//            //            waitKey(0);
//                    }
//                    imwrite(format("/home/hp/Videos/test2/LP_%d.jpg",i), src );
//                    img.release();
//                    i++;
//                    licensePlate2.push_back({name2.str(),str2,src2});//,fn[i]
                    vect2.clear();
                    str2.clear();
                    degree2.clear();
                }
        //        for (int var = 0; var < licensePlate.size(); ++var) {
        //            if(!licensePlate[var].name.empty()){
        //                imwrite(format("/home/hp/Videos/test2/LP_%d.jpg",i), licensePlate[var].image );
        //                img.release();
        //                i++;
        //            }
        //        }
                vect_Images2.clear();
    }
    cout<< "The detection took : " << timer1.elapsed() << " milliseconds"<<endl;
}
