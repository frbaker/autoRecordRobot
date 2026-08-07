#pragma once
#include <fstream>
#include <filesystem>
#include <vector>
#include "ControllerSnapshot.h"

class RoutineHandler{
    public:
    std::string writeRoutineToDisk(const std::vector<ControllerSnapshot>& snapshots);
    std::string writeRoutineToCpp(const std::vector<ControllerSnapshot>& snapshots, const std::string& csvPath);
    std::vector<ControllerSnapshot> getRoutineFromDisk(std::string path);
};