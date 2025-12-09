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
    return image.convertToFormat(QImage::Format_Grayscale8);
}

QImage subImage(const QImage& image)
{
    if (image.isNull())
        return QImage();

    int subWidth  = std::max(image.width()  / 2, 10);
    int subHeight = std::max(image.height() / 2, 10);

    int x = (image.width()  - subWidth)  / 2;
    int y = (image.height() - subHeight) / 2;

    return image.copy(x, y, subWidth, subHeight);
}

QRgb heatmapColor(int value)
{
    value = std::clamp(value, 0, 255);
    double t = static_cast<double>(value) / 255.0;

    int r, g, b;

    if (t <= 0.25) {
        double f = t / 0.25;
        r = 0;
        g = static_cast<int>(255 * f);
        b = 255;
    }
    else if (t <= 0.5) {
        double f = (t - 0.25) / 0.25;
        r = 0;
        g = 255;
        b = static_cast<int>(255 * (1.0 - f));
    }
    else if (t <= 0.75) {
        double f = (t - 0.5) / 0.25;
        r = static_cast<int>(255 * f);
        g = 255;
        b = 0;
    }
    else {
        double f = (t - 0.75) / 0.25;
        r = 255;
        g = static_cast<int>(255 * (1.0 - f));
        b = 0;
    }

    return qRgb(r, g, b);
}

QImage toHeatmap(const QImage& image)
{
    const auto width = image.width();
    const auto height = image.height();
    constexpr auto format = QImage::Format_RGB32;

    QImage result(width, height, format);
    for (int x = 0; x < width; ++x)
    {
        for (int y = 0; y < height; ++y)
        {
            auto pixel = image.pixel(x, y);
            int gray = qGray(pixel);
            auto heatPixel = heatmapColor(gray);
            result.setPixel(x, y, heatPixel);
        }
    }

    return result;
}

QImage toSize(const QImage& image, QSize size)
{
    QImage result(size, image.format());
    result.fill(0);

    const int copyWidth = std::min(size.width(), image.width());
    const int copyHeight = std::min(size.height(), image.height());

    for (int y = 0; y < copyHeight; ++y) {
        for (int x = 0; x < copyWidth; ++x) {
            result.setPixel(x, y, image.pixel(x, y));
        }
    }

    return result;
}

QPoint findMaxCorrelation(const ComplexTable& correlation)
{
    double maxVal = std::real(correlation[0][0]);
    int maxY = 0;
    int maxX = 0;

    for (size_t y = 0; y < correlation.size(); ++y)
    {
        for (size_t x = 0; x < correlation[y].size(); ++x)
        {
            const auto elem = std::real(correlation[y][x]);
            if (elem > maxVal)
            {
                maxVal = elem;
                maxY = static_cast<int>(y);
                maxX = static_cast<int>(x);
            }
        }
    }

    return {maxX, maxY};
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

ComplexTable correlationalTable(const QImage& A, const QImage& B)
{
    int W = A.width()  + B.width()  - 1;
    int H = A.height() + B.height() - 1;

    const auto size = QSize{W, H};
    QImage Ap = toSize(toGrayscale(A), size);
    QImage Bp = toSize(toGrayscale(B), size);

    auto FA = DFT(Ap);
    auto FB = DFT(Bp);

    ComplexTable C(H, ComplexRow(W));
    for (int y = 0; y < H; ++y)
        for (int x = 0; x < W; ++x)
            C[y][x] = FA[y][x] * std::conj(FB[y][x]);

    return IDFT(C);
}

Table DCT(const QImage& image)
{
    auto grayImage = toGrayscale(image);
    auto N = grayImage.height();
    auto M = grayImage.width();

    Table result(N, std::vector<double>(M));

    std::vector<std::pair<int, int>> indices;
    indices.reserve(N * M);

    for (int n = 0; n < N; ++n)
    {
        for (int m = 0; m < M; ++m)
        {
            indices.emplace_back(n, m);
        }
    }

    std::for_each(indices.begin(), indices.end(),
                  [&](const std::pair<int, int>& idx) {
        int n = idx.first;
        int m = idx.second;

        double grayValue = 0.0;

        std::vector<double> cosX(M);
        std::vector<double> cosY(N);

        for (int x = 0; x < M; ++x)
        {
            cosX[x] = std::cos(((2 * x + 1) * m * pi) / (2 * M));
        }
        for (int y = 0; y < N; ++y)
        {
            cosY[y] = std::cos(((2 * y + 1) * n * pi) / (2 * N));
        }

        for (int y = 0; y < N; ++y)
        {
            double cos_n = cosY[y];
            
            for (int x = 0; x < M; ++x)
            {
                QRgb pixel = grayImage.pixel(x, y);
                int gray = qGray(pixel);
                
                grayValue += gray * cosX[x] * cos_n;
            }
        }

        double alpha_n = (n == 0) ? 1.0 / std::sqrt(2.0) : 1.0;
        double alpha_m = (m == 0) ? 1.0 / std::sqrt(2.0) : 1.0;

        result[n][m] = (2.0 / std::sqrt(M * N)) * alpha_n * alpha_m * grayValue;
    });

    return result;
}

QImage IDCT(const Table& table)
{
    auto N = table.size();
    auto M = table[0].size();

    QImage result(M, N, QImage::Format_Grayscale8);

    std::vector<std::pair<int, int>> pixels;
    pixels.reserve(N * M);

    for (int n = 0; n < N; ++n)
    {
        for (int m = 0; m < M; ++m)
        {
            pixels.emplace_back(n, m);
        }
    }

    std::for_each(pixels.begin(),
                  pixels.end(),
                  [&](const std::pair<int, int>& pixel)
    {

        int n = pixel.first;
        int m = pixel.second;

        double grayValue = 0.0;

        std::vector<double> cosX(M);
        std::vector<double> cosY(N);

        for (int x = 0; x < M; ++x)
        {
            cosX[x] = std::cos((x * (2 * m + 1) * pi) / (2 * M));
        }
        for (int y = 0; y < N; ++y)
        {
            cosY[y] = std::cos((y * (2 * n + 1) * pi) / (2 * N));
        }

        for (int y = 0; y < N; ++y)
        {
            double alpha_y = (y == 0) ? 1.0 / std::sqrt(2.0) : 1.0;
            double cos_n = cosY[y];

            for (int x = 0; x < M; ++x)
            {
                double alpha_x = (x == 0) ? 1.0 / std::sqrt(2.0) : 1.0;
                double cos_m = cosX[x];

                grayValue += alpha_x * alpha_y * table[y][x] * cos_m * cos_n;
            }
        }

        double normalized = (2.0 / std::sqrt(static_cast<double>(M * N))) * grayValue;

        int pixelValue = static_cast<int>(std::round(normalized));
        pixelValue = std::clamp(pixelValue, 0, 255);

        result.setPixel(m, n, qRgb(pixelValue, pixelValue, pixelValue));
    });

    return result;
}

ComplexTable DFT(const QImage& image)
{
    QImage grayImage = toGrayscale(image);
    const int N = grayImage.height();
    const int M = grayImage.width();

    ComplexTable result(N, ComplexRow(M));

    const double two_pi = 2.0 * pi;

    std::vector<double> gray(N * M);
    for (int y = 0; y < N; ++y) {
        for (int x = 0; x < M; ++x)
            gray[y * M + x] = qGray(grayImage.pixel(x, y)) / 255.0;
    }

    ComplexTable expY;
    expY.resize(N);
    for (int i = 0; i < N; ++i)
        expY[i].resize(N);

    ComplexTable expX;
    expX.resize(M);
    for (int i = 0; i < M; ++i)
        expX[i].resize(M);

    for (int k = 0; k < N; ++k)
        for (int y = 0; y < N; ++y)
            expY[k][y] = std::polar(1.0, -two_pi * k * y / N);

    for (int l = 0; l < M; ++l)
        for (int x = 0; x < M; ++x)
            expX[l][x] = std::polar(1.0, -two_pi * l * x / M);

    for (int k = 0; k < N; ++k)
    {
        for (int l = 0; l < M; ++l)
        {
            Complex sum(0, 0);

            const auto& expY_k = expY[k];
            const auto& expX_l = expX[l];

            for (int y = 0; y < N; ++y)
            {
                Complex ey = expY_k[y];
                const double* row = &gray[y*M];

                for (int x = 0; x < M; ++x)
                {
                    sum += row[x] * expX_l[x] * ey;
                }
            }

            result[k][l] = sum;
        }
    }

    return result;
}

ComplexTable IDFT(const ComplexTable& spectrum)
{
    int N = spectrum.size();
    int M = spectrum[0].size();

    ComplexTable result(N, ComplexRow(M));

    const double two_pi = 2.0 * pi;

    ComplexTable expY(N, ComplexRow(N));
    for (int k = 0; k < N; ++k)
        for (int y = 0; y < N; ++y)
            expY[k][y] = std::polar(1.0, two_pi * k * y / N);

    ComplexTable expX(M, ComplexRow(M));
    for (int l = 0; l < M; ++l)
        for (int x = 0; x < M; ++x)
            expX[l][x] = std::polar(1.0, two_pi * l * x / M);

    const double norm = 1.0 / double(M * N);

    for (int y = 0; y < N; ++y)
    {
        for (int x = 0; x < M; ++x)
        {
            Complex sum(0, 0);

            for (int k = 0; k < N; ++k)
            {
                Complex ey = expY[k][y];

                for (int l = 0; l < M; ++l)
                {
                    Complex ex = expX[l][x];
                    sum += spectrum[k][l] * ex * ey;
                }
            }

            result[y][x] = sum * norm;
        }
    }

    return result;
}

QImage fromTable(const Table& table)
{
    auto N = table.size();
    auto M = table[0].size();

    QImage result(M, N, QImage::Format_Grayscale8);

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

Table magnitudeSpectrum(const ComplexTable& spectrum)
{
    auto N = spectrum.size();
    auto M = spectrum[0].size();

    Table magnitude(N, Row(M));

    for (int k = 0; k < N; ++k)
    {
        for (int l = 0; l < M; ++l)
        {
            magnitude[k][l] = std::abs(spectrum[k][l]);
        }
    }

    return magnitude;
}

Table phaseSpectrum(const ComplexTable& spectrum)
{
    auto N = spectrum.size();
    auto M = spectrum[0].size();

    Table phase(N, std::vector<double>(M));

    for (int k = 0; k < N; ++k)
    {
        for (int l = 0; l < M; ++l)
        {
            phase[k][l] = std::arg(spectrum[k][l]);
        }
    }

    return phase;
}

Table logMagnitudeSpectrum(const ComplexTable& spectrum)
{
    auto mag = magnitudeSpectrum(spectrum);
    auto N = mag.size();
    auto M = mag[0].size();

    Table logMag(N, std::vector<double>(M));

    for (int k = 0; k < N; ++k)
    {
        for (int l = 0; l < M; ++l)
        {
            logMag[k][l] = std::log(1.0 + mag[k][l]) * 255.0;
        }
    }

    return logMag;
}

} // namespace utils
