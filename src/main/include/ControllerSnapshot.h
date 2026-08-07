#pragma once

#include <sstream>
#include <string>

struct ControllerSnapshot{
    double timestampSeconds = 0.0;
    double leftX = 0.0, leftY = 0.0, rightX = 0.0, rightY = 0.0, LT = 0.0, RT = 0.0;
    bool A = false, B = false, X = false, Y = false, LB = false, RB = false;
    int POV = 0;

    std::string toString() const {
        std::ostringstream oss;
        oss << timestampSeconds << ',' << leftX << ',' << leftY << ',' << rightX
            << ',' << rightY << ',' << LT << ',' << RT << ',' << (A ? 1 : 0)
            << ',' << (B ? 1 : 0) << ',' << (X ? 1 : 0) << ',' << (Y ? 1 : 0)
            << ',' << (LB ? 1 : 0) << ',' << (RB ? 1 : 0) << ',' << POV;
        return oss.str();
    }
};