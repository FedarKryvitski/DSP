#include "utils.h"

#include <algorithm>

namespace utils {

namespace {

constexpr double pi = std::numbers::pi;

// for Box blur
constexpr int kBoxBlurRadius = 10;

// for Median
constexpr int kMedianBlurRadius = 2;

// for Gaussian
constexpr int kGaussianRadius = 5;
constexpr double kSigma = 9;

// for Sobel
constexpr std::array<std::array<int, 3>, 3> sobelX = {{
    {-1, 0, 1},
    {-2, 0, 2},
    {-1, 0, 1}
}};

constexpr std::array<std::array<int, 3>, 3> sobelY = {{
    {-1, -2, -1},
    { 0,  0,  0},
    { 1,  2,  1}
}};

QImage toGrayscale(const QImage& image)
{
    auto inputFormat = image.format();

    QImage buffered = image.convertToFormat(QImage::Format_Grayscale8);
    QImage output = buffered.convertToFormat(inputFormat);

    return output;
}

double gaussian(int x, int y, double sigma)
{
    const double left = 1 / (2 * pi * std::pow(sigma, 2));
    const double right = (x * x + y * y) / (2 * std::pow(sigma, 2));

    return left * std::exp(-right);
}

std::vector<std::vector<double>> createGaussianKernel(int kernelSize, double sigma)
{
    Q_ASSERT(kernelSize % 2 == 1);

    std::vector<std::vector<double>> kernel(kernelSize, std::vector<double>(kernelSize, 0.0));

    int indent = kernelSize / 2;
    double sum = 0.0;

    for (int y = -indent; y <= indent; ++y)
    {
        for (int x = -indent; x <= indent; ++x)
        {
            double value = gaussian(x, y, sigma);
            sum += value;
            kernel[y + indent][x + indent] = value;
        }
    }

    std::ranges::for_each(kernel, [sum](auto& row){
        std::ranges::for_each(row, [sum](auto& elem){
            elem = elem / sum;
        });
    });

    return kernel;
}

QImage applySobel(const QImage& image)
{
    QImage bufferedImage = toGrayscale(image);
    QImage resultImage(bufferedImage);

    const int width = bufferedImage.width();
    const int height = bufferedImage.height();

    for (int x = 1; x < width - 1; ++x)
    {
        for (int y = 1; y < height - 1; ++y)
        {
            int pixelX = 0;
            int pixelY = 0;

            for (int i = 0; i < 3; ++i)
            {
                for (int j = 0; j < 3; ++j)
                {
                    int curr_x = x + i - 1;
                    int curr_y = y + j - 1;

                    QRgb pixel = bufferedImage.pixel(curr_x, curr_y);

                    int gray = qGray(pixel);
                    pixelX += gray * sobelX[i][j];
                    pixelY += gray * sobelY[i][j];
                }
            }

            int magnitude = static_cast<int>(std::sqrt(pixelX * pixelX + pixelY * pixelY));

            QRgb newPixel = qRgb(magnitude, magnitude, magnitude);
            resultImage.setPixel(x, y, newPixel);
        }
    }

    return resultImage;
}

QImage applyBoxBlur(const QImage& image)
{
    constexpr int radius = kBoxBlurRadius;

    const int width = image.width();
    const int height = image.height();
    const auto format = image.format();

    QImage resultImage(width, height, format);

    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            int sumRed = 0;
            int sumGreen = 0;
            int sumBlue = 0;
            int count = 0;

            for (int curr_x = x - radius; curr_x <= x + radius; curr_x++)
            {
                for (int curr_y = y - radius; curr_y <= y + radius; curr_y++)
                {
                    if (0 <= curr_x && curr_x < width &&
                        0 <= curr_y && curr_y < height)
                    {
                        const auto pixel = image.pixel(curr_x, curr_y);

                        sumRed += qRed(pixel);
                        sumGreen += qGreen(pixel);
                        sumBlue += qBlue(pixel);
                        count++;
                    }
                }
            }

            int red = sumRed / count;
            int green = sumGreen / count;
            int blue = sumBlue / count;

            QRgb pixel = qRgb(red, green, blue);
            resultImage.setPixel(x, y, pixel);
        }
    }

    return resultImage;
}

QImage applyMedian(const QImage& image)
{
    constexpr int radius = kMedianBlurRadius;

    const int width = image.width();
    const int height = image.height();
    const auto format = image.format();

    QImage resultImage(width, height, format);

    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            std::vector<QRgb> valuesRed;
            std::vector<QRgb> valuesGreen;
            std::vector<QRgb> valuesBlue;

            for (int curr_x = x - radius; curr_x <= x + radius; curr_x++)
            {
                for (int curr_y = y - radius; curr_y <= y + radius; curr_y++)
                {
                    if (curr_x >= 0 && curr_x < width &&
                        curr_y >= 0 && curr_y < height)
                    {
                        const auto pixel = image.pixel(curr_x, curr_y);

                        valuesRed.push_back(qRed(pixel));
                        valuesGreen.push_back(qGreen(pixel));
                        valuesBlue.push_back(qBlue(pixel));
                    }
                }
            }

            std::ranges::sort(valuesRed);
            std::ranges::sort(valuesGreen);
            std::ranges::sort(valuesBlue);

            auto red = valuesRed[valuesRed.size() / 2];
            auto green = valuesGreen[valuesGreen.size() / 2];
            auto blue = valuesBlue[valuesBlue.size() / 2];

            const auto pixel = qRgb(red, green, blue);
            resultImage.setPixel(x, y, pixel);
        }
    }

    return resultImage;
}

QImage applyGaussian(const QImage& image)
{
    constexpr int radius = kGaussianRadius;
    constexpr int kernelSize = radius * 2 + 1;

    auto kernel = createGaussianKernel(kernelSize, kSigma);

    const int width = image.width();
    const int height = image.height();
    const auto format = image.format();

    QImage resultImage(width, height, format);

    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            double sumR = 0;
            double sumG = 0;
            double sumB = 0;

            for (int kx = 0; kx < kernelSize; kx++)
            {
                for (int ky = 0; ky < kernelSize; ky++)
                {
                    int pixelX = x - radius + kx;
                    int pixelY = y - radius + ky;

                    if (pixelX < 0)
                    {
                        pixelX = -pixelX;
                    }
                    else if (pixelX >= width)
                    {
                        pixelX = 2 * width - pixelX - 1;
                    }

                    if (pixelY < 0)
                    {
                        pixelY = -pixelY;
                    }
                    else if (pixelY >= height)
                    {
                        pixelY = 2 * height - pixelY - 1;
                    }

                    QRgb rgb = image.pixel(pixelX, pixelY);
                    auto coef = kernel[ky][kx];

                    sumR += qRed(rgb) * coef;
                    sumG += qGreen(rgb) * coef;
                    sumB += qBlue(rgb) * coef;
                }
            }

            int red = std::clamp(static_cast<int>(sumR), 0, 255);
            int green = std::clamp(static_cast<int>(sumG), 0, 255);
            int blue = std::clamp(static_cast<int>(sumB), 0, 255);

            QRgb color = qRgb(red, green, blue);
            resultImage.setPixel(x, y, color);
        }
    }

    return resultImage;
}

} // namespace


QImage processImage(const QImage& image, Method method)
{
    switch (method)
    {
        case Method::BOX_BLUR:
        {
            return applyBoxBlur(image);
        }
        case Method::GAUSS_BLUR:
        {
            return applyGaussian(image);
        }
        case Method::MEDIAN_FILTER:
        {
            return applyMedian(image);
        }
        case Method::SOBEL_OPERATOR:
        {
            return applySobel(image);
        }
        default:
            break;
    }

    return {};
}

} // namespace utils
