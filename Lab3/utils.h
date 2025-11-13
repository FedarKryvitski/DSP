#ifndef UTILS_H
#define UTILS_H

#include <QImage>

namespace utils {

enum class Method {
    BOX_BLUR,
    GAUSS_BLUR,
    MEDIAN_FILTER,
    SOBEL_OPERATOR
};

QImage processImage(const QImage& image, Method method);

} // namespace utils

#endif // UTILS_H
