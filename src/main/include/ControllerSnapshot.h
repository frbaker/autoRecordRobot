#pragma once

struct ControllerSnapshot{
    double leftX, leftY, rightX, rightY, LT, RT;
    bool A, B, X, Y, LB, RB;
    int POV;

    std::string toString() const {
        return fmt::format("{},{},{},{},{},{},{},{},{},{},{},{},{}",
        leftX, leftY, rightX, rightY, LT, RT, A, B, X, Y, LB, RB, POV
        );
    }
};