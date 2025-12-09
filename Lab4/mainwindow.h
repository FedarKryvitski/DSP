#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "utils.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
class QLabel;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    using Method = utils::Method;

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_selectImageButton_clicked();

    void on_runButton_clicked();

    void on_methodComboBox_currentIndexChanged(int index);

    void on_clearButton_clicked();

    void on_selectImageButton_2_clicked();

    void on_clearButton_2_clicked();

private:
    void setCurrentMethod(Method method);

    void setImage(QLabel* label, const QImage& image);

    void resetImage(QLabel* label);

private:
    Ui::MainWindow *ui;
    Method currentMethod_{Method::CROSS_CORRELATION};

    QImage image_1_;
    QImage image_2_;
    QImage image_3_;
};
#endif // MAINWINDOW_H
