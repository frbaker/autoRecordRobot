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
#include "ControllerSnapshot.h"

using namespace DriveConstants;

RobotContainer::RobotContainer() {
  // Initialize all of your commands and subsystems here
  recordingAutonomous = false;
  doneRecordingAutonomous = false;

  m_chooser.SetDefaultOption("haha", "/home/lvuser/controllerRecordings/cool.csv");
  m_chooser.AddOption("hehe", "hehe");

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
                m_routineHandler.writeRoutineToDisk(recordedSnapshots);
                recordedSnapshots.clear();
                doneRecordingAutonomous = false;
            }
            else if(recordingAutonomous){
                ControllerSnapshot snapshot{m_driverController.GetLeftX(), m_driverController.GetLeftY(), m_driverController.GetRightX(), m_driverController.GetRightY(), m_driverController.GetLeftTriggerAxis(), m_driverController.GetRightTriggerAxis(), m_driverController.GetAButton(), m_driverController.GetBButton(), m_driverController.GetXButton(), m_driverController.GetYButton(), m_driverController.GetLeftBumper(), m_driverController.GetRightBumper(), m_driverController.GetPOV()};
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

  if(controllerPlaybackAuto){
    std::vector<frc2::CommandPtr> commands;
    for(const auto& snapshot : routine){
        commands.push_back(frc2::cmd::RunOnce([this, snapshot] {
            m_drive.Drive(
            -units::meters_per_second_t{frc::ApplyDeadband(snapshot.leftY, OIConstants::kDriveDeadband)},
            -units::meters_per_second_t{frc::ApplyDeadband(snapshot.leftX, OIConstants::kDriveDeadband)},
            -units::radians_per_second_t{frc::ApplyDeadband(snapshot.rightX, OIConstants::kDriveDeadband)},
            true);
            //m_drive.drive(stuff lol)
            //m_haha.hehe(hohe)
        }));
    }
    return frc2::cmd::Sequence(std::move(commands));
  }

  return frc2::InstantCommand([this] {}).ToPtr();
}


