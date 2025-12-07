#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "utils.h"

#include <QColorSpace>
#include <QFileDialog>
#include <algorithm>

namespace {

using Method = utils::Method;

constexpr std::array<Method, 2> kMethods = {{
    Method::CROSS_CORRELATION,
    Method::AUTO_CORRELATION,
}};

const std::unordered_map<Method, QString> kMethodNames = {{
   { Method::CROSS_CORRELATION, "Взаимная корреляция" },
   { Method::AUTO_CORRELATION, "Автокорреляция"}
}};

} // namespace

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    auto& comboBox = ui->methodComboBox;
    std::ranges::for_each(kMethods, [&](const auto& method) {
        auto it = kMethodNames.find(method);
        comboBox->addItem(it->second);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setCurrentMethod(Method method)
{
    currentMethod_ = method;

    switch (currentMethod_)
    {
    case Method::CROSS_CORRELATION:
    {
        ui->clearButton_2->setVisible(true);
        ui->selectImageButton_2->setVisible(true);
        break;
    }
    case Method::AUTO_CORRELATION:
    {
        ui->clearButton_2->setVisible(false);
        ui->selectImageButton_2->setVisible(false);
        break;
    }
    default:
        break;
    }
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

    auto format = inputImage_.format();
    auto table = utils::DCT(inputImage_);

    auto inputGrayscale = utils::toGrayscale(inputImage_);
    outputImage_ = utils::fromTable(table, format);
    outputImage_2_ = utils::IDCT(table, format);

    setImage(ui->inputImageLabel, inputGrayscale);
    setImage(ui->outputImageLabel, outputImage_);
    setImage(ui->outputImageLabel_2, outputImage_2_);

    // auto temp = utils::addRectangle(inputImage_, QRect(100, 200, 50, 50));
    // setImage(ui->inputImageLabel, temp);
}

void MainWindow::on_methodComboBox_currentIndexChanged(int index)
{
    setCurrentMethod(kMethods[index]);
}

void MainWindow::on_clearButton_clicked()
{
    inputImage_ = {};
    resetImage(ui->inputImageLabel);
}

/*
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
*/

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
