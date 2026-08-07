#include <iomanip>
#include <subsystems/RoutineHandler.h>

std::string RoutineHandler::writeRoutineToDisk(const std::vector<ControllerSnapshot>& snapshots){
  std::filesystem::create_directories("/home/lvuser/controllerRecordings");

  auto now = std::chrono::system_clock::now();
  auto time = std::chrono::system_clock::to_time_t(now);
  auto tm = *std::localtime(&time);
  char filename[80];
  std::strftime(filename, sizeof(filename), "/home/lvuser/controllerRecordings/recording_%Y%m%d_%H%M%S.csv", &tm);

  std::ofstream file(filename);
  if (!file.is_open()) {
    return std::string();
  }
  for (const auto& snapshot : snapshots){
    file << snapshot.toString() << "\n";
  }
  file.close();
  return filename;
}

std::string RoutineHandler::writeRoutineToCpp(const std::vector<ControllerSnapshot>& snapshots, const std::string& csvPath) {
  std::filesystem::path cppPath = std::filesystem::path(csvPath).replace_extension(".cpp");
  std::filesystem::create_directories(cppPath.parent_path());

  std::ofstream file(cppPath);
  if (!file.is_open()) {
    return std::string();
  }

  file << "#include <frc2/command/Commands.h>\n";
  file << "#include <frc2/command/WaitCommand.h>\n";
  file << "#include <units/angle.h>\n";
  file << "#include <units/velocity.h>\n";
  file << "#include <units/time.h>\n";
  file << "#include \"subsystems/DriveSubsystem.h\"\n";
  file << "#include \"Constants.h\"\n";
  file << "\n";
  file << "frc2::CommandPtr BuildRecordedAuto(DriveSubsystem& drive) {\n";
  file << "  std::vector<frc2::CommandPtr> commands;\n";

  double previousTimestamp = snapshots.empty() ? 0.0 : snapshots.front().timestampSeconds;
  for (std::size_t index = 0; index < snapshots.size(); ++index) {
    const auto& snapshot = snapshots[index];
    double waitSeconds = 0.0;
    if (index > 0) {
      waitSeconds = snapshot.timestampSeconds - previousTimestamp;
      if (waitSeconds < 0.0) {
        waitSeconds = 0.0;
      }
    }
    previousTimestamp = snapshot.timestampSeconds;

    file << "  commands.push_back(frc2::cmd::Sequence(\n";
    file << "      frc2::RunCommand([&drive] {\n";
    file << "          drive.Drive(-units::meters_per_second_t{frc::ApplyDeadband(" << snapshot.leftY << ", OIConstants::kDriveDeadband)},\n";
    file << "                     -units::meters_per_second_t{frc::ApplyDeadband(" << snapshot.leftX << ", OIConstants::kDriveDeadband)},\n";
    file << "                     -units::radians_per_second_t{frc::ApplyDeadband(" << snapshot.rightX << ", OIConstants::kDriveDeadband)},\n";
    file << "                     true);\n";
    file << "      }, {&drive}).ToPtr(),\n";
    file << "      frc2::WaitCommand(units::second_t{" << std::fixed << std::setprecision(6) << waitSeconds << "}).ToPtr()));\n";
  }

  file << "  return frc2::cmd::Sequence(std::move(commands));\n";
  file << "}\n";
  file.close();
  return cppPath.string();
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
        
        std::getline(iss, token, ','); snapshot.timestampSeconds = std::stod(token);
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