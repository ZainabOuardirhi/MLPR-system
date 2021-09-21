#include "mainwindow.h"
#include <QApplication>
#include "firstwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
/*    MainWindow w;
    w.show();
*/
    FirstWindow fw;
    fw.setModal(true);
//    if (fw.exec() == QDialog::Accepted) {
//        w.show();
//    }
    fw.exec();
    return a.exec();
}
