// Author: Gokulakrishnan Sivakumar

#pragma once

#include <string>


struct RecognitionResult {
    bool matched {false};
    std::string sample_id;
    std::string subject;
    int label {-1};
    double distance {0.0};
};

