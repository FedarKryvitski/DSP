#ifndef UTILS_H
#define UTILS_H

#include <QImage>

#include <vector>
#include <complex>

namespace utils {

enum class Method {
    CROSS_CORRELATION,
    AUTO_CORRELATION
};

using Row = std::vector<double>;
using Table = std::vector<Row>;

using Complex = std::complex<double>;
using ComplexRow = std::vector<Complex>;
using ComplexTable = std::vector<ComplexRow>;

QImage processImage(const QImage& image, Method method);
QImage addRectangle(const QImage& image, const QRect& rect);
QImage subImage(const QImage& image);

Table DCT(const QImage& image);
QImage IDCT(const Table& image);

ComplexTable DFT(const QImage& image);
ComplexTable IDFT(const ComplexTable& image);

ComplexTable correlationalTable(const QImage& first, const QImage& second);

QPoint findMaxCorrelation(const ComplexTable& correlation);

QImage toGrayscale(const QImage& image);
QImage toHeatmap(const QImage& image);
QImage toSize(const QImage& image, QSize size);
QImage fromTable(const Table& table);

Table logMagnitudeSpectrum(const ComplexTable& spectrum);
Table phaseSpectrum(const ComplexTable& spectrum);
Table magnitudeSpectrum(const ComplexTable& spectrum);

} // namespace utils

#endif // UTILS_H
