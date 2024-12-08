// Based on cifar10_reader.hpp

#ifndef CIFAR_100_READER_HPP
#define CIFAR_100_READEAR_HPP

#include <vector>
#include <fstream>
#include <iostream>
#include <memory>
#include <cstdint>

namespace cifar {

typedef std::vector<double> Image;
typedef std::vector<Image> Images;
typedef std::vector<double> Labels;

struct CIFAR100_dataset {
    Images training_images;
    Images test_images;
    Labels training_labels;
    Labels test_labels;
};

int read_cifar100_file(Images &images, Labels &labels, const std::string &path, bool fine_labels=true);

}

#endif
