#ifndef MINIDNN_H_
#define MINIDNN_H_

//
// disable eigen parallelization for fair comparison
// remove the following line for more efficient training/inference in a practical setting
//#define EIGEN_DONT_PARALLELIZE
// INFO: We have moved this definition to CMakeLists.txt, so that it can be set more consistently.
//

#include <Eigen/Core>

#include "Config.h"

#include "RNG.h"

#include "Layer.h"
#include "Layer/FullyConnected.h"
#include "Layer/Convolutional.h"
#include "Layer/MaxPooling.h"

#include "EigenStream.h"

#include "Activation/ReLU.h"
#include "Activation/Identity.h"
#include "Activation/Sigmoid.h"
#include "Activation/Softmax.h"

#include "Output.h"
#include "Output/RegressionMSE.h"
#include "Output/BinaryClassEntropy.h"
#include "Output/MultiClassEntropy.h"

#include "Optimizer.h"
#include "Optimizer/SGD.h"
#include "Optimizer/SGDM.h"
#include "Optimizer/AdaGrad.h"
#include "Optimizer/RMSProp.h"

#include "Callback.h"
#include "Callback/VerboseCallback.h"

#include "Network.h"


#endif /* MINIDNN_H_ */
