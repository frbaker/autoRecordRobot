// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "RobotContainer.h"

#include <frc/Timer.h>
#include <frc/smartdashboard/SmartDashboard.h>
#include <frc/controller/PIDController.h>
#include <frc/geometry/Translation2d.h>
#include <frc/shuffleboard/Shuffleboard.h>
#include <frc/trajectory/Trajectory.h>
#include <frc/trajectory/TrajectoryGenerator.h>
#include <frc2/command/InstantCommand.h>
#include <frc2/command/SequentialCommandGroup.h>
#include <frc2/command/SwerveControllerCommand.h>
#include <frc2/command/WaitCommand.h>
#include <frc2/command/button/JoystickButton.h>
#include <units/angle.h>
#include <units/time.h>
#include <units/velocity.h>
#include <frc2/command/Commands.h>

#include <utility>
#include <fstream>
#include <filesystem>
#include <iostream>

#include "Constants.h"
#include "subsystems/DriveSubsystem.h"
#include "ControllerSnapshot.h"

using namespace DriveConstants;

RobotContainer::RobotContainer() {
  // Initialize all of your commands and subsystems here
  recordingAutonomous = false;
  doneRecordingAutonomous = false;
  controllerPlaybackAuto = true;

  m_chooser.SetDefaultOption("No Auto", "");

  const std::filesystem::path recordingsDir{"/home/lvuser/controllerRecordings"};
  if (std::filesystem::exists(recordingsDir)) {
    for (const auto& entry : std::filesystem::directory_iterator(recordingsDir)) {
      if (!entry.is_regular_file()) {
        continue;
      }
      const auto path = entry.path();
      if (path.extension() == ".csv") {
        m_chooser.AddOption(path.filename().string(), path.string());
      }
    }
  }

  // Configure the button bindings
  ConfigureButtonBindings();
  frc::SmartDashboard::PutData("Auto Mode", &m_chooser);

  // Set up default drive command
  // The left stick controls translation of the robot.
  // Turning is controlled by the X axis of the right stick.
  m_drive.SetDefaultCommand(frc2::RunCommand(
      [this] {
        m_drive.Drive(
            -units::meters_per_second_t{frc::ApplyDeadband(
                m_driverController.GetLeftY(), OIConstants::kDriveDeadband)},
            -units::meters_per_second_t{frc::ApplyDeadband(
                m_driverController.GetLeftX(), OIConstants::kDriveDeadband)},
            -units::radians_per_second_t{frc::ApplyDeadband(
                m_driverController.GetRightX(), OIConstants::kDriveDeadband)},
            true);
        if(AutoConstants::CanRecordAuto){
            if(doneRecordingAutonomous){
                std::cout << "Routine written to disk" << std::endl;
                const std::string csvPath = m_routineHandler.writeRoutineToDisk(recordedSnapshots);
                if (!csvPath.empty()) {
                  m_chooser.AddOption(std::filesystem::path(csvPath).filename().string(), csvPath);
                  frc::SmartDashboard::PutData("Auto Mode", &m_chooser);
                  m_routineHandler.writeRoutineToCpp(recordedSnapshots, csvPath);
                }
                recordedSnapshots.clear();
                doneRecordingAutonomous = false;
            }
            else if(recordingAutonomous){
                ControllerSnapshot snapshot;
                snapshot.timestampSeconds = frc::Timer::GetFPGATimestamp().value();
                snapshot.leftX = m_driverController.GetLeftX();
                snapshot.leftY = m_driverController.GetLeftY();
                snapshot.rightX = m_driverController.GetRightX();
                snapshot.rightY = m_driverController.GetRightY();
                snapshot.LT = m_driverController.GetLeftTriggerAxis();
                snapshot.RT = m_driverController.GetRightTriggerAxis();
                snapshot.A = m_driverController.GetAButton();
                snapshot.B = m_driverController.GetBButton();
                snapshot.X = m_driverController.GetXButton();
                snapshot.Y = m_driverController.GetYButton();
                snapshot.LB = m_driverController.GetLeftBumper();
                snapshot.RB = m_driverController.GetRightBumper();
                snapshot.POV = m_driverController.GetPOV();
                recordedSnapshots.push_back(snapshot);
            }
        }

      },
      {&m_drive}));
    //
}

void RobotContainer::ConfigureButtonBindings() {
  frc2::JoystickButton(&m_driverController,
                       frc::XboxController::Button::kRightBumper)
      .WhileTrue(new frc2::RunCommand([this] { m_drive.SetX(); }, {&m_drive}));

  frc2::JoystickButton(&m_driverController, frc::XboxController::Button::kLeftStick).OnTrue(
    new frc2::InstantCommand([this] {
        if(recordingAutonomous){
            doneRecordingAutonomous = true;
            recordingAutonomous = false;
        }
        else{
            recordingAutonomous = true;
        }
    })
  );
}

frc2::CommandPtr RobotContainer::GetAutonomousCommand() {

  std::vector<ControllerSnapshot> routine = m_routineHandler.getRoutineFromDisk(m_chooser.GetSelected());

  if(controllerPlaybackAuto && !routine.empty()){
    std::vector<frc2::CommandPtr> commands;
    double previousTimestamp = routine.front().timestampSeconds;

    for(std::size_t index = 0; index < routine.size(); ++index){
      const auto& snapshot = routine[index];
      double waitSeconds = index == 0 ? 0.0 : snapshot.timestampSeconds - previousTimestamp;
      if (waitSeconds < 0.0) {
        waitSeconds = 0.0;
      }
      previousTimestamp = snapshot.timestampSeconds;

      commands.push_back(frc2::cmd::Sequence(
          frc2::RunCommand([this, snapshot] {
              m_drive.Drive(
                  -units::meters_per_second_t{frc::ApplyDeadband(snapshot.leftY, OIConstants::kDriveDeadband)},
                  -units::meters_per_second_t{frc::ApplyDeadband(snapshot.leftX, OIConstants::kDriveDeadband)},
                  -units::radians_per_second_t{frc::ApplyDeadband(snapshot.rightX, OIConstants::kDriveDeadband)},
                  true);
          }, {&m_drive}).ToPtr(),
          frc2::WaitCommand(units::second_t{waitSeconds}).ToPtr()));
    }
    return frc2::cmd::Sequence(std::move(commands));
  }

  return frc2::InstantCommand([this] {}).ToPtr();
}


