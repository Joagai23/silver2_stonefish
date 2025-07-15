#include "MySimulationManager.h"
#include <Stonefish/entities/statics/Plane.h>
#include <entities/solids/Box.h>
#include <entities/solids/Polyhedron.h>
#include <actuators/Light.h>
#include <entities/statics/Terrain.h>
#include <entities/forcefields/Jet.h>
#include <entities/forcefields/Uniform.h>
#include <core/NED.h>

MySimulationManager::MySimulationManager(sf::Scalar stepsPerSecond) : SimulationManager(stepsPerSecond)
{
}

void MySimulationManager::BuildScenario()
{   
    // Should not do this!
    const std::string DataPath = "/home/jorge/Documents/Code/silver2_stonefish/SimpleSimulator/data/";
    //Physical materials
    CreateMaterial("LightAluminium", 1250.0, 0.8);
    CreateMaterial("Steel", 7810.0, 0.9);
    SetMaterialsInteraction("LightAluminium", "LightAluminium", 0.7, 0.5);
    SetMaterialsInteraction("Steel", "Steel", 0.4, 0.2);
    SetMaterialsInteraction("LightAluminium", "Steel", 0.6, 0.4);

    //Graphical materials (looks)
    CreateLook("gray", sf::Color::Gray(0.5f), 0.3f, 0.2f);
    CreateLook("red", sf::Color::RGB(1.f,0.f,0.f), 0.1f, 0.f);
    CreateLook("seabed", sf::Color::RGB(0.7f, 0.7f, 0.5f), 0.9f, 0.f, 0.f, "", DataPath + "sand_normal.png");
    CreateLook("silver", sf::Color::RGB(1.0f, 1.0f, 1.0f), 0.9f, 0.f, 0.f, "", DataPath + "SILVER_UV.png");

    //Create environment
    EnableOcean(0.0);
    getOcean()->setWaterType(0.2);
    getOcean()->AddVelocityField(new sf::Jet(sf::Vector3(0,0,1.0), sf::VY(), 0.3, 5.0));
    getOcean()->AddVelocityField(new sf::Uniform(sf::Vector3(1.0,0.0,0.0)));
    //getOcean()->EnableCurrents();
    getAtmosphere()->SetSunPosition(0.0, 60.0);
    getNED()->Init(41.77737, 3.03376, 0.0);

    // Create Lights
    sf::Light* spot = new sf::Light("Spot", 0.02, 50.0, sf::Color::BlackBody(5000.0), 100.0);
	spot->AttachToWorld(sf::Transform(sf::Quaternion(0,0,M_PI/3.0), sf::Vector3(0.0,0.0,1.0)));
	AddActuator(spot);
    sf::Light* omni = new sf::Light("Omni", 0.02, sf::Color::BlackBody(5000.0), 10000.0);
	omni->AttachToWorld(sf::Transform(sf::Quaternion(0,0,M_PI/3.0), sf::Vector3(2.0,2.0,0.5)));
	AddActuator(omni);

    // Create Terrain
    sf::Terrain* seabed = new sf::Terrain("Seabed", DataPath + "terrain.png", 1.0, 1.0, 5.0, "Steel", "seabed", 5.f);
    AddStaticEntity(seabed, sf::Transform(sf::IQ(), sf::Vector3(0,0,15.0)));

    // Define Physics
    sf::BodyPhysicsSettings phy_settings;
    phy_settings.mode = sf::BodyPhysicsMode::SUBMERGED;

    // Create Objects
    sf::Polyhedron* body = new sf::Polyhedron("MainBody", phy_settings, DataPath + "Body_Modify.obj", sf::Scalar(1), sf::I4(), "LightAluminium", "silver");
    //body->ScalePhysicalPropertiesToArbitraryMass(140.0);

    sf::Box* box = new sf::Box("Box", phy_settings, sf::Vector3(0.5, 0.75, 0.3), sf::I4(), "LightAluminium", "red");
    
    // Instantiate Models
    AddSolidEntity(body, sf::Transform(sf::IQ(), sf::Vector3(0.0, 0.0, 1.0)));
    AddSolidEntity(box, sf::Transform(sf::IQ(), sf::Vector3(2.0, 0.0, 1.0)));
}