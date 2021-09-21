#ifndef IMAGEWINDOW_H
#define IMAGEWINDOW_H

#include <QDialog>

namespace Ui {
class ImageWindow;
}

class ImageWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ImageWindow(QWidget *parent = 0);
    ~ImageWindow();

private slots:
    void on_btn_parcourir_image_clicked();

private:
    Ui::ImageWindow *ui;
};

#endif // IMAGEWINDOW_H
