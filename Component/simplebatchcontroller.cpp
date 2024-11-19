#include "Utils/Random.h"
#include "cifar100_reader.hpp"
#include "cifar10_reader.hpp"
#include "minidnn/Component/BatchController.hpp"
#include "modular_components.hpp"
#include <cstdlib>
#include "utils.h"

namespace MiniDNN {

SimpleBatchController::SimpleBatchController(SystemExecutor &exec, std::string dataset,
                                             int batch_size)
    : BatchController(exec, batch_size), rng(1) {

    this->dataset_name = dataset;

    Matrix x, y;

    if (dataset == "CIFAR10") {
        /* num categories */
        this->y_dim = 10;

        auto dset = cifar::read_dataset<std::vector, std::vector, double, double>();
        long n_training = dset.training_images.size();

        /* image rows * image columns * colour channels */
        this->x_dim = dset.training_images.at(0).size();

        x = Matrix::Zero(this->x_dim, n_training);
        y = Matrix::Zero(this->y_dim, n_training);

        for (int i = 0; i < n_training; i++) {
            x.col(i) = Vector::Map(
                &dset.training_images.at(i).at(0),
                dset.training_images.at(i).size()
            );

            int lbl = dset.training_labels.at(i);

            if (0 <= lbl && lbl < this->y_dim) {
                y(lbl, i) = 1.0;
            } else {
                std::cerr << "[CIFAR10] Label value error. Got: " << lbl << std::endl;
                std::exit(-1);
            }
        }

        /* normalise pixel values to 0.0-1.0 */
        x /= 255;

        std::cout << "[CIFAR10] Successfully loaded samples and labels.\n";
        std::cout << "\tn_training = " << n_training << std::endl;
        std::cout << "\tx columns = " << x.cols() << std::endl;
    } else if (dataset == "CIFAR100") {
        /* num categories */
        this->y_dim = 100;

        cifar::CIFAR100_dataset dset;
        cifar::read_cifar100_file(dset.training_images, dset.training_labels, "data/cifar-100/train.bin", true);
        cifar::read_cifar100_file(dset.test_images, dset.test_labels, "data/cifar-100/test.bin", true);

        long n_training = dset.training_images.size();

        /* image rows * image columns * colour channels */
        this->x_dim = dset.training_images.at(0).size();

        x = Matrix::Zero(this->x_dim, n_training);
        y = Matrix::Zero(this->y_dim, n_training);

        for (int i = 0; i < n_training; i++) {
            x.col(i) = Vector::Map(
                &dset.training_images.at(i).at(0),
                dset.training_images.at(i).size()
            );

            int lbl = dset.training_labels.at(i);

            if (0 <= lbl && lbl < this->y_dim) {
                y(lbl, i) = 1.0;
            } else {
                std::cerr << "[CIFAR10] Label value error. Got: " << lbl << std::endl;
                std::exit(-1);
            }
        }

        /* normalise pixel values to 0.0-1.0 */
        x /= 255;

        std::cout << "[CIFAR100] Successfully loaded samples and labels.\n";
        std::cout << "\tn_training = " << n_training << std::endl;
        std::cout << "\tx columns = " << x.cols() << std::endl;
    } else if (dataset == "MNIST") {
        std::cerr << "MNIST is not supported right now...\n";
        std::exit(-1);
    } else if (dataset == "FASHION-MNIST") {
        std::cerr << "FASHION-MNIST is not supported right now...\n";
        std::exit(-1);
    } else {
        std::cerr << "Unsupported dataset '" << dataset << "'\n";
        std::exit(-1);
    }

    const int nbatch =
        internal::create_shuffled_batches(x, y, batch_size, rng, x_batches, y_batches);

    std::cout << "[batch] Created " << nbatch << " batches.\n";
}

} // namespace MiniDNN
