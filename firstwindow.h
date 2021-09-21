#ifndef FIRSTWINDOW_H
#define FIRSTWINDOW_H

#include <QDialog>

namespace Ui {
class FirstWindow;
}

class FirstWindow : public QDialog
{
    Q_OBJECT

public:
    explicit FirstWindow(QWidget *parent = 0);
    ~FirstWindow();

private slots:
    void on_btn_video_clicked();

    void on_btn_image_clicked();

private:
    Ui::FirstWindow *ui;
};

#endif // FIRSTWINDOW_H
