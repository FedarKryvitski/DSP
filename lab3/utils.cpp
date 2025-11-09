#include "utils.h"

#include <algorithm>

namespace utils {

namespace {

constexpr double pi = std::numbers::pi;

// for Gaussian
constexpr int kKernelSize = 13;
constexpr double kSigma = 9;

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

QImage applyGaussian(const QImage& image)
{
    constexpr int kernelSize = kKernelSize;
    constexpr int kernelHalf = kKernelSize / 2;

    auto kernel = createGaussianKernel(kKernelSize, kSigma);

    const int width = image.width();
    const int height = image.height();
    const auto format = image.format();

    QImage resultImage(width, height, format);

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            double sumR = 0;
            double sumG = 0;
            double sumB = 0;

            for (int ky = 0; ky < kernelSize; ky++)
            {
                for (int kx = 0; kx < kernelSize; kx++)
                {
                    int pixelX = x - kernelHalf + kx;
                    int pixelY = y - kernelHalf + ky;

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

QImage applySobel(const QImage& image);

QImage processImage(const QImage& image, Method method)
{
    switch (method)
    {
        case Method::BOX_BLUR:
        {
            return {};
        }
        case Method::GAUSS_BLUR:
        {
            return applyGaussian(image);
        }
        case Method::MEDIAN_FILTER:
        {
            return {};
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

QImage applySobel(const QImage& image)
{
    QImage buffered = toGrayscale(image);
    QImage result(buffered);

    const int width = buffered.width();
    const int height = buffered.height();

    for (int y = 1; y < height - 1; ++y)
    {
        for (int x = 1; x < width - 1; ++x)
        {
            int pixelX = 0;
            int pixelY = 0;

            for (int i = 0; i < 3; ++i)
            {
                for (int j = 0; j < 3; ++j)
                {
                    int curr_x = x + i - 1;
                    int curr_y = y + j - 1;

                    QRgb pixel = buffered.pixel(curr_x, curr_y);
                    int gray = qGray(pixel);

                    pixelX += gray * sobelX[i][j];
                    pixelY += gray * sobelY[i][j];
                }
            }

            int magnitude = static_cast<int>(std::sqrt(pixelX * pixelX + pixelY * pixelY));
            QRgb newPixel = qRgb(magnitude, magnitude, magnitude);

            result.setPixel(x, y, newPixel);
        }
    }

    return result;
}

} // namespace utils
