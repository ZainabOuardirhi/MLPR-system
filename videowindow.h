#ifndef VIDEOWINDOW_H
#define VIDEOWINDOW_H

#include <QDialog>

namespace Ui {
class VideoWindow;
}

class VideoWindow : public QDialog
{
    Q_OBJECT

public:
    explicit VideoWindow(QWidget *parent = 0);
    ~VideoWindow();

private slots:
    void on_btn_parcourir_video_clicked();

private:
    Ui::VideoWindow *ui;
};

#endif // VIDEOWINDOW_H
