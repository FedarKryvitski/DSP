#include "utils.h"

#include <QColor>
#include <QPainter>

#include <algorithm>

namespace utils {

namespace {

constexpr double pi = std::numbers::pi;

} // namespace

QImage toGrayscale(const QImage& image)
{
    auto inputFormat = image.format();

    QImage buffered = image.convertToFormat(QImage::Format_Grayscale8);
    QImage output = buffered.convertToFormat(inputFormat);

    return output;
}

QImage toSize(const QImage& image, QSize size)
{
    auto format = image.format();
    QImage result(size, format);

    const int width = std::min(size.width(), image.width());
    const int height = std::min(size.height(), image.height());

    for (int i = 0; i < width; ++i)
    {
        for (int j = 0; j < height; ++j)
        {
            result.setPixel(i, j, image.pixel(i, j));
        }
    }

    return result;
}

std::pair<int, int> getCorrelationCoordinates(const QImage& image)
{
    int x, y = 0;
    return {};
}

QImage processImage(const QImage& image, Method method)
{
    switch (method)
    {
        case Method::CROSS_CORRELATION:
        {

        }
        case Method::AUTO_CORRELATION:
        {
            //return
        }
        default:
            break;
    }

    return {};
}

QImage addRectangle(const QImage& image, const QRect& rect)
{
    constexpr auto color = Qt::green;
    constexpr auto width = 3;

    QImage result = image;
    QPainter painter(&result);

    QPen pen(color);
    pen.setWidth(width);
    pen.setStyle(Qt::SolidLine);
    pen.setJoinStyle(Qt::MiterJoin);

    painter.setPen(pen);
    painter.drawRect(rect);

    return result;
}

Table DCT(const QImage& image)
{
    auto grayImage = toGrayscale(image);

    auto N = grayImage.height();
    auto M = grayImage.width();

    Table result(N, std::vector<double>(M));

    for (int n = 0; n < N; ++n)
    {
        for (int m = 0; m < M; ++m)
        {
            double grayValue{};
            for (int y = 0; y < N; ++y)
            {
                for (int x = 0; x < M; ++x)
                {
                    QRgb pixel = grayImage.pixel(x, y);
                    int gray = qGray(pixel);

                    double firstAngle = ((2 * x + 1) * m * pi) / (2 * M);
                    double secondAngle = ((2 * y + 1) * n * pi) / (2 * N);

                    grayValue += gray * std::cos(firstAngle) * std::cos(secondAngle);
                }
            }

            double alpha_n = (n == 0) ? 1.0 / std::sqrt(2.0) : 1.0;
            double alpha_m = (m == 0) ? 1.0 / std::sqrt(2.0) : 1.0;

            result[n][m] = (2.0 / std::sqrt(M * N)) * alpha_n * alpha_m * grayValue;
        }
    }

    return result;
}

QImage IDCT(const Table& table, QImage::Format format)
{
    auto N = table.size();
    auto M = table[0].size();

    QImage result(M, N, format);

    for (int n = 0; n < N; ++n)
    {
        for (int m = 0; m < M; ++m)
        {
            double grayValue{};
            for (int y = 0; y < N; ++y)
            {
                for (int x = 0; x < M; ++x)
                {
                    double gray = table[y][x];
                    double firstAngle = (x * (2 * m + 1) * pi) / (2 * M);
                    double secondAngle = (y * (2 * n + 1) * pi) / (2 * N);

                    double alpha_x = (x == 0) ? 1.0 / std::sqrt(2.0) : 1.0;
                    double alpha_y = (y == 0) ? 1.0 / std::sqrt(2.0) : 1.0;
                    grayValue += alpha_x * alpha_y * gray * std::cos(firstAngle) * std::cos(secondAngle);
                }
            }

            double normalized = (2.0 / std::sqrt(M * N)) * grayValue;

            auto pixelValue = static_cast<int>(std::clamp(normalized, 0.0, 255.0));
            auto pixel = qRgb(pixelValue, pixelValue, pixelValue);
            result.setPixel(m, n, pixel);
        }
    }

    return result;
}

QImage fromTable(const Table& table, QImage::Format format)
{
    auto N = table.size();
    auto M = table[0].size();

    QImage result(M, N, format);

    for (int k = 0; k < N; ++k)
    {
        for (int l = 0; l < M; ++l)
        {
            double gray = table[k][l];

            auto pixelValue = static_cast<int>(std::clamp(gray, 0.0, 255.0));
            auto pixel = qRgb(pixelValue, pixelValue, pixelValue);
            result.setPixel(l, k, pixel);
        }
    }

    return result;
}

} // namespace utils
