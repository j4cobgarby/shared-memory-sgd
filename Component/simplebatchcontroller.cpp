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
    : BatchController(exec), rng(1), bs(batch_size) {

    this->_dataset_name = dataset;

    if (dataset == "CIFAR10") {
        /* num categories */
        this->_y_dim = 10;

        auto dset = cifar::read_dataset<std::vector, std::vector, double, double>();
        long n_training = (long)dset.training_images.size();
        long n_test = (long)dset.test_images.size();

        /* image rows * image columns * colour channels */
        this->_x_dim = (int)dset.training_images.at(0).size();

        _train_x = Matrix::Zero(this->_x_dim, n_training);
        _train_y = Matrix::Zero(this->_y_dim, n_training);

        for (int i = 0; i < n_training; i++) {
            _train_x.col(i) = Vector::Map(
                &dset.training_images.at(i).at(0),
                dset.training_images.at(i).size()
            );

            int lbl = (int)dset.training_labels.at(i);

            if (0 <= lbl && lbl < this->_y_dim) {
                _train_y(lbl, i) = 1.0;
            } else {
                std::cerr << "[CIFAR10] Label value error. Got: " << lbl << std::endl;
                std::exit(-1);
            }
        }

        _test_x = Matrix::Zero(dset.test_images.at(0).size(), n_test);
        _test_y = Matrix::Zero(_y_dim, n_test);

        for (int i = 0; i < n_test; i++) {
            _test_x.col(i) = Vector::Map(
                &dset.test_images.at(i).at(0),
                dset.test_images.at(i).size()
            );

            int lbl = dset.test_labels.at(i);

            if (0 <= lbl && lbl < _y_dim) {
                _test_y(lbl, i) = 1.0;
            } else {
                std::cerr << "[CIFAR10] Label value error. Got: " << lbl << std::endl;
                std::exit(-1);
            }
        }

        /* normalise pixel values to 0.0-1.0 */
        _train_x /= 255;
        _test_x /= 255;

        std::cout << "[CIFAR10] Successfully loaded samples and labels.\n";
        std::cout << "\tn_training = " << n_training << std::endl;
        std::cout << "\tn_test = " << n_test << std::endl;
        std::cout << "\tx columns = " << _train_x.cols() << std::endl;
        std::cout << "\ttest_x columns = " << _test_x.cols() << std::endl;
    } else if (dataset == "CIFAR100") {
        /* num categories */
        this->_y_dim = 100;

        cifar::CIFAR100_dataset dset;
        cifar::read_cifar100_file(dset.training_images, dset.training_labels, "data/cifar-100/train.bin", true);
        cifar::read_cifar100_file(dset.test_images, dset.test_labels, "data/cifar-100/test.bin", true);

        long n_training = dset.training_images.size();
        long n_test = dset.test_images.size();

        std::cout << "[cifar100] " << n_training << " training images, " << n_test << " test images\n";

        /* image rows * image columns * colour channels */
        this->_x_dim = dset.training_images.at(0).size();

        _train_x = Matrix::Zero(this->_x_dim, n_training);
        _train_y = Matrix::Zero(this->_y_dim, n_training);

        for (int i = 0; i < n_training; i++) {
            _train_x.col(i) = Vector::Map(
                &dset.training_images.at(i).at(0),
                dset.training_images.at(i).size()
            );

            int lbl = dset.training_labels.at(i);

            if (0 <= lbl && lbl < this->_y_dim) {
                _train_y(lbl, i) = 1.0;
            } else {
                std::cerr << "[CIFAR10] Label value error. Got: " << lbl << std::endl;
                std::exit(-1);
            }
        }

        _test_x = Matrix::Zero(this->_x_dim, n_training);
        _test_y = Matrix::Zero(this->_y_dim, n_training);

        for (int i = 0; i < n_test; i++) {
            _test_x.col(i) = Vector::Map(
                &dset.test_images.at(i).at(0),
                dset.test_images.at(i).size()
            );

            int lbl = dset.test_labels.at(i);

            if (0 <= lbl && lbl < this->_y_dim) {
                _test_y(lbl, i) = 1.0;
            } else {
                std::cerr << "[CIFAR10] Label value error. Got: " << lbl << std::endl;
                std::exit(-1);
            }
        }

        /* normalise pixel values to 0.0-1.0 */
        _train_x /= 255;
        _test_x /= 255;

        std::cout << "[CIFAR10] Successfully loaded samples and labels.\n";
        std::cout << "\tn_training = " << n_training << std::endl;
        std::cout << "\tx columns = " << _train_x.cols() << std::endl;
        std::cout << "\ttest_x columns = " << _test_x.cols() << std::endl;
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
        internal::create_shuffled_batches(_train_x, _train_y, batch_size, rng, _x_batches, _y_batches);

    std::cout << "[batch] Created " << nbatch << " batches.\n";
}

} // namespace MiniDNN
