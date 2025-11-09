#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_selectImageButton_clicked();

    void on_runButton_clicked();

    void on_saveImageButton_clicked();

private:
    QImage processImage(const QImage& image);

    void setImage(QLabel* label, const QImage& image);

    void resetImage(QLabel* label);

private:
    Ui::MainWindow *ui;

    QImage inputImage_;
    QImage outputImage_;
};
#endif // MAINWINDOW_H
