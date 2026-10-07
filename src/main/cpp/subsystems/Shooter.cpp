#include <subsystems/Shooter.h>

Shooter::Shooter(){
    m_leftFeederConfig.OpenLoopRampRate(0.1);
    m_rightFeederConfig.OpenLoopRampRate(0.1);

    m_rightFeeder.Configure(m_rightFeederConfig, rev::ResetMode::kResetSafeParameters, rev::PersistMode::kPersistParameters);

    m_leftFeeder.Configure(m_leftFeederConfig, rev::ResetMode::kResetSafeParameters, rev::PersistMode::kPersistParameters);
}

void Shooter::Periodic(){

}

void Shooter::Run(){
    m_leftShooter.Set(0.75);
    m_rightShooter.Set(-0.75);
    m_leftFeeder.Set(-0.5);
    m_rightFeeder.Set(0.5);
    m_rotor.Set(-0.12);
}

void Shooter::Stop(){
    m_leftShooter.Set(0);
    m_rightShooter.Set(0);
    m_leftFeeder.Set(0);
    m_rightFeeder.Set(0);
    m_rotor.Set(0);
}