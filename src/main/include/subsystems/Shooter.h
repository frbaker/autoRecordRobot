#pragma once

#include <frc2/command/SubsystemBase.h>
#include <rev/SparkMax.h>
#include <rev/config/SparkMaxConfig.h>
#include "Constants.h"

using namespace rev::spark;
using namespace ShooterConstants;
class Shooter : public frc2::SubsystemBase { 
    public:
        Shooter();
        void Periodic() override;
        void Run();
        void Stop();
    private:
        SparkMax m_leftShooter{kLeftShooterCanId, SparkLowLevel::MotorType::kBrushless};
        SparkMax m_rightShooter{kRightShooterCanId, SparkLowLevel::MotorType::kBrushless};
        SparkMax m_leftFeeder{kLeftFeederCanId, SparkLowLevel::MotorType::kBrushless};
        SparkMax m_rightFeeder{kRightFeederCanId, SparkLowLevel::MotorType::kBrushless};
        SparkMax m_rotor{kRotorCanId, SparkLowLevel::MotorType::kBrushless};

        SparkMaxConfig m_leftFeederConfig;
        SparkMaxConfig m_rightFeederConfig;
        //SparkMaxConfig m_rotorConfig;
};