#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "utils.h"

#include <QColorSpace>
#include <QFileDialog>
#include <algorithm>

namespace {

using Method = utils::Method;

const std::array<Method, 4> kMethods = {{
    Method::BOX_BLUR,
    Method::GAUSS_BLUR,
    Method::MEDIAN_FILTER,
    Method::SOBEL_OPERATOR
}};

const std::array<QString, 4> kMethodNames = {{
    "Коробочное размытие",
    "Размытие по Гауссу",
    "Медианный фильтр",
    "Оператор Собеля"
}};

} // namespace

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    auto& comboBox = ui->methodComboBox;
    std::ranges::for_each(kMethodNames, [&](const auto& name) {
        comboBox->addItem(name);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_selectImageButton_clicked()
{
    QString imagePath = QFileDialog::getOpenFileName(
        nullptr,
        "Выберите изображение",
        "",
        "Изображения (*.png *.jpg *.jpeg *.bmp *.gif *.tiff *.webp)"
    );

    if (imagePath.isEmpty())
        return;

    inputImage_ = QImage(imagePath);
    outputImage_ = {};

    setImage(ui->inputImageLabel, inputImage_);
    resetImage(ui->outputImageLabel);
}

void MainWindow::on_runButton_clicked()
{
    outputImage_ = {};
    resetImage(ui->outputImageLabel);

    if (inputImage_.isNull())
        return;

    int operation = ui->methodComboBox->currentIndex();
    Method method = kMethods[operation];

    outputImage_ = utils::processImage(inputImage_, method);
    setImage(ui->outputImageLabel, outputImage_);
}

void MainWindow::on_saveImageButton_clicked()
{
    QPixmap pixmap = ui->outputImageLabel->pixmap();
    if (pixmap.isNull())
        return;

    QString fileName = QFileDialog::getSaveFileName(
        this,
        "Сохранить изображение",
        "",
        "PNG (*.png);;JPEG (*.jpg *.jpeg);;BMP (*.bmp);;Все файлы (*)"
    );

    if (fileName.isEmpty())
        return;

    outputImage_.save(fileName);
}

void MainWindow::setImage(QLabel* label, const QImage& image)
{
    int labelWidth = label->width();
    int labelHeight = label->height();

    QPixmap pixmap = QPixmap::fromImage(image);
    QPixmap scaledPixmap = pixmap.scaled(labelWidth, labelHeight,
                                         Qt::KeepAspectRatio,
                                         Qt::SmoothTransformation);
    label->setPixmap(scaledPixmap);
    QApplication::processEvents();
}

void MainWindow::resetImage(QLabel* label)
{
    label->setPixmap(QPixmap{});
    QApplication::processEvents();
}

