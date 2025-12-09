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

QImage loadImage()
{
    QString imagePath = QFileDialog::getOpenFileName(
        nullptr,
        "Выберите изображение",
        "",
        "Изображения (*.png *.jpg *.jpeg *.bmp *.gif *.tiff *.webp)"
        );

    if (imagePath.isEmpty())
        return {};

    return QImage(imagePath);
}

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
    image_1_ = loadImage();

    setImage(ui->imageLabel_1, image_1_);
    resetImage(ui->imageLabel_3);
}

void MainWindow::on_selectImageButton_2_clicked()
{
    image_2_ = loadImage();

    setImage(ui->imageLabel_2, image_2_);
    resetImage(ui->imageLabel_3);
}

void MainWindow::on_runButton_clicked()
{
    resetImage(ui->imageLabel_3);

    if (image_1_.isNull())
    {
        qWarning() << "At least one of two images is null";
        return;
    }

    image_2_ = utils::subImage(image_1_);
    setImage(ui->imageLabel_2, image_2_);

    auto correlationComplex = utils::correlationalTable(image_1_, image_2_);
    auto correlationMagnitude = utils::magnitudeSpectrum(correlationComplex);
    auto correlationImage = utils::fromTable(correlationMagnitude);
    auto correlationHeatmap = utils::toHeatmap(correlationImage);
    image_3_ = correlationHeatmap;
    setImage(ui->imageLabel_3, image_3_);

    auto peakPoint = utils::findMaxCorrelation(correlationComplex);
    int dx = peakPoint.x() - (image_2_.width() - 1);
    int dy = peakPoint.y() - (image_2_.height() - 1);

    // Ограничиваем координаты прямоугольника границами исходного изображения
    dx = std::clamp(dx, 0, image_1_.width() - image_2_.width());
    dy = std::clamp(dy, 0, image_1_.height() - image_2_.height());

    auto rectPosition = QPoint{dx, dy};
    auto rectSize = image_2_.size();
    auto imageWithRect = utils::addRectangle(image_1_, QRect(rectPosition, rectSize));
    setImage(ui->imageLabel_1, imageWithRect);
}

void MainWindow::on_methodComboBox_currentIndexChanged(int index)
{
    setCurrentMethod(kMethods[index]);
}

void MainWindow::on_clearButton_clicked()
{
    image_1_ = {};
    image_3_ = {};
    resetImage(ui->imageLabel_1);
    resetImage(ui->imageLabel_3);
}


void MainWindow::on_clearButton_2_clicked()
{
    image_2_ = {};
    image_3_ = {};
    resetImage(ui->imageLabel_2);
    resetImage(ui->imageLabel_3);
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

    image_2_.save(fileName);
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
