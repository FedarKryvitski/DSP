#ifndef UTILS_H
#define UTILS_H

#include <QImage>

#include <vector>

namespace utils {

enum class Method {
    CROSS_CORRELATION,
    AUTO_CORRELATION
};

using Table = std::vector<std::vector<double>>;

QImage processImage(const QImage& image, Method method);
QImage addRectangle(const QImage& image, const QRect& rect);

Table DCT(const QImage& image);
QImage IDCT(const Table& image, QImage::Format format);

QImage fromTable(const Table& table, QImage::Format format);
QImage toGrayscale(const QImage& image);
QImage toSize(const QImage& image, QSize size);

} // namespace utils

#endif // UTILS_H
