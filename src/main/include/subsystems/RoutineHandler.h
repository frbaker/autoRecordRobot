#pragma once
#include <fstream>
#include <filesystem>
#include <vector>
#include "ControllerSnapshot.h"

class RoutineHandler{
    public:
    void writeRoutineToDisk(std::vector<ControllerSnapshot>& snapshots);
    std::vector<ControllerSnapshot> getRoutineFromDisk(std::string path);
};