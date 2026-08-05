#include <subsystems/RoutineHandler.h>

void RoutineHandler::writeRoutineToDisk(std::vector<ControllerSnapshot>& snapshots){
  std::filesystem::create_directories("/home/lvuser/controllerRecordings");

  auto now = std::chrono::system_clock::now();
  auto time = std::chrono::system_clock::to_time_t(now);
  auto tm = *std::localtime(&time);
  char filename[80];
  std::strftime(filename, sizeof(filename), "/home/lvuser/controllerRecordings/recording_%Y%m%d_%H%M%S.csv", &tm);

  std::ofstream file(filename);
  for (const auto& snapshot : snapshots){
    file << snapshot.toString() << "\n";
  }
  file.close();
}

std::vector<ControllerSnapshot> RoutineHandler::getRoutineFromDisk(std::string path){
    std::vector<ControllerSnapshot> snapshots;
    std::ifstream file(path);

    if(!file.is_open()){
        return snapshots;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string token;
        ControllerSnapshot snapshot;
        
        std::getline(iss, token, ','); snapshot.leftX = std::stod(token);
        std::getline(iss, token, ','); snapshot.leftY = std::stod(token);
        std::getline(iss, token, ','); snapshot.rightX = std::stod(token);
        std::getline(iss, token, ','); snapshot.rightY = std::stod(token);
        std::getline(iss, token, ','); snapshot.LT = std::stod(token);
        std::getline(iss, token, ','); snapshot.RT = std::stod(token);
        std::getline(iss, token, ','); snapshot.A = std::stoi(token);
        std::getline(iss, token, ','); snapshot.B = std::stoi(token);
        std::getline(iss, token, ','); snapshot.X = std::stoi(token);
        std::getline(iss, token, ','); snapshot.Y = std::stoi(token);
        std::getline(iss, token, ','); snapshot.LB = std::stoi(token);
        std::getline(iss, token, ','); snapshot.RB = std::stoi(token);
        std::getline(iss, token, ','); snapshot.POV = std::stoi(token);
        
        snapshots.push_back(snapshot);
    }
    
    file.close();
    return snapshots;
}