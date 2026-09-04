// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "RobotContainer.h"

#include <frc/controller/PIDController.h>
#include <frc/geometry/Translation2d.h>
#include <frc/shuffleboard/Shuffleboard.h>
#include <frc/trajectory/Trajectory.h>
#include <frc/trajectory/TrajectoryGenerator.h>
#include <frc2/command/InstantCommand.h>
#include <frc2/command/SequentialCommandGroup.h>
#include <frc2/command/SwerveControllerCommand.h>
#include <frc2/command/button/JoystickButton.h>
#include <units/angle.h>
#include <units/velocity.h>
#include <frc2/command/Commands.h>

#include <utility>
#include <fstream>
#include <filesystem>
#include <iostream>

#include "Constants.h"
#include "subsystems/DriveSubsystem.h"
//#include "ControllerSnapshot.h"
#include "frc2/command/WaitCommand.h"
#include <frc/smartdashboard/SmartDashboard.h>
#include <frc2/command/Subsystem.h>
#include <autoRecordLib/RoutineHandler.h>

using namespace DriveConstants;

RobotContainer::RobotContainer() {
  // Initialize all of your commands and subsystems here
  recordingAutonomous = false;
  doneRecordingAutonomous = false;

  m_chooser.SetDefaultOption("test", "/home/lvuser/controllerRecordings/test.csv");
  m_chooser.AddOption("hehe", "hehe");
  frc::SmartDashboard::PutData("auto", &m_chooser);

  autoRecordLib::RegisterAutoCommands([this](autoRecordLib::ControllerSnapshot snapshot){
    m_drive.Drive(
        -units::meters_per_second_t{frc::ApplyDeadband(snapshot.leftY, OIConstants::kDriveDeadband)},
        -units::meters_per_second_t{frc::ApplyDeadband(snapshot.leftX, OIConstants::kDriveDeadband)},
        -units::radians_per_second_t{frc::ApplyDeadband(snapshot.rightX, OIConstants::kDriveDeadband)},
        false
    );
  });

  // Configure the button bindings
  ConfigureButtonBindings();

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
                autoRecordLib::writeRoutineToDisk(m_recorder);
                m_recorder.ClearSnapshots();
                //recordedSnapshots.clear();
                doneRecordingAutonomous = false;
            }
            else if(recordingAutonomous){
                m_recorder.RecordSnapshot(m_driverController);
            }
        }

      },
      {&m_drive}));
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

    std::string path = m_chooser.GetSelected();
    autoRecordLib::Routine routine = autoRecordLib::getRoutineFromDisk(path);
    controllerPlaybackAuto = true;

    if(controllerPlaybackAuto){
        return autoRecordLib::CreateAutonomousRoutine(routine);
    }

  return frc2::InstantCommand([this] {}).ToPtr();
}
