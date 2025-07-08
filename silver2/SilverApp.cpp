#include "SilverApp.h"

#include <actuators/Servo.h>
#include <actuators/Thruster.h>
#include <actuators/VariableBuoyancy.h>
#include <core/Robot.h>
#include <sensors/scalar/Accelerometer.h>
#include <sensors/scalar/IMU.h>
#include <sensors/scalar/DVL.h>
#include <sensors/vision/FLS.h>
#include <sensors/vision/SSS.h>
#include <graphics/IMGUI.h>
#include <utils/SystemUtil.hpp>
#include <comms/USBL.h>
#include <core/Console.h>

SilverApp::SilverApp(std::string dataDirPath, sf::RenderSettings s, sf::HelperSettings h, SilverManager* sim)
    : GraphicalSimulationApp("SILVER2 Stonefish", dataDirPath, s, h, sim)
{
}

void SilverApp::InitializeGUI()
{
    largePrint = new sf::OpenGLPrinter(sf::GetShaderPath() + std::string(STANDARD_FONT_NAME), 64.0);
}

void SilverApp::DoHUD()
{
    GraphicalSimulationApp::DoHUD();
}
