#include "cifar10_reader.hpp"
#include "cifar100_reader.hpp"

namespace cifar {

int read_cifar100_file(Images &images, Labels &labels, const std::string &path, bool fine_labels) {
    std::ifstream file;
    file.open(path, std::ios::in | std::ios::binary | std::ios::ate);

    if (!file) {
        std::cout << "Couldn't open " << path << std::endl;
        return -1;
    }

    size_t file_size = file.tellg();
    std::unique_ptr<char[]> buffer(new char[file_size]);

    file.seekg(0, std::ios::beg);
    file.read(buffer.get(), file_size);
    file.close();

    size_t img_plus_labels_size = 1 + 1 + 3072; // coarse label, fine label, 3072 pixels
    size_t num_images = file_size / img_plus_labels_size;

    images.reserve(num_images);
    labels.reserve(num_images);

    const size_t label_offset = fine_labels ? 1 : 0;

    for (size_t i = 0; i < num_images; i++) {
        labels.push_back(buffer[i * 3074 + label_offset]);

        images.push_back(Image(3072));

        for (size_t j = 0; j < 3072; j++) {
            images[i][j] = buffer[i * 3074 + (j + 2)];
        }
    }

    std::cout << "Successfully read CIFAR100 data file\n";

    return 0;
}

}