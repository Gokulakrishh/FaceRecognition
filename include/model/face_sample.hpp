// Author: Gokulakrishnan Sivakumar

#pragma once

#include <string>
#include <vector>
#include <eigen3/Eigen/Dense>


struct FaceSample {
    std::string id;
    std::string subject;
    int label {0};
    Eigen::VectorXd pixels;
    Eigen::VectorXd embedding;
};

