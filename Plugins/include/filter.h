#ifndef FILTER_H
#define FILTER_H

#include "utils.h"

#include <span>
#include <vector>

namespace filter {

using namespace utils;

std::vector<complex> filter(std::span<const complex> input, std::span<const float> freqs, int lowerBound, int upperBound);

}

#endif //FILTER_H
