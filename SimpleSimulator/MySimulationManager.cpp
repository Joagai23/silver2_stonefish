#include "MySimulationManager.h"
#include <Stonefish/entities/statics/Plane.h>
#include <entities/solids/Box.h>
#include <entities/solids/Polyhedron.h>
#include <actuators/Light.h>
#include <entities/statics/Terrain.h>
#include <entities/forcefields/Jet.h>
#include <entities/forcefields/Uniform.h>
#include <core/FeatherstoneRobot.h>
#include <core/NED.h>

MySimulationManager::MySimulationManager(sf::Scalar stepsPerSecond) : SimulationManager(stepsPerSecond)
{
}

void MySimulationManager::BuildScenario()
{   
    // Should not do this!
    const std::string DataPath = "/home/jorge/Documents/Code/silver2_stonefish/SimpleSimulator/data/";
    //Physical materials
    CreateMaterial("LightAluminium", 100.0, 0.8);
    CreateMaterial("Steel", 7810.0, 0.9);
    SetMaterialsInteraction("LightAluminium", "LightAluminium", 0.7, 0.5);
    SetMaterialsInteraction("Steel", "Steel", 0.4, 0.2);
    SetMaterialsInteraction("LightAluminium", "Steel", 0.6, 0.4);

    //Graphical materials (looks)
    CreateLook("gray", sf::Color::Gray(0.5f), 0.3f, 0.2f);
    CreateLook("red", sf::Color::RGB(1.f,0.f,0.f), 0.1f, 0.f);
    CreateLook("seabed", sf::Color::RGB(0.7f, 0.7f, 0.5f), 0.9f, 0.f, 0.f, "", DataPath + "sand_normal.png");
    CreateLook("silver_uv", sf::Color::RGB(1.0f, 1.0f, 1.0f), 1.0f, 0.f, 0.f, DataPath + "silver_uv.png");

    //Create environment
    /*EnableOcean(0.0);
    getOcean()->setWaterType(0.2);
    getOcean()->AddVelocityField(new sf::Jet(sf::Vector3(0,0,1.0), sf::VY(), 0.3, 5.0));
    getOcean()->AddVelocityField(new sf::Uniform(sf::Vector3(1.0,0.0,0.0)));
    //getOcean()->EnableCurrents();
    getAtmosphere()->SetSunPosition(0.0, 60.0);
    getNED()->Init(41.77737, 3.03376, 0.0);*/

    sf::Plane* plane = new sf::Plane("Ground", 10000.0, "Steel", "gray");
    AddStaticEntity(plane, sf::I4());

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
    sf::Polyhedron* body = new sf::Polyhedron("MainBody", phy_settings, DataPath + "Body.obj", sf::Scalar(1), sf::I4(), "LightAluminium", "silver_uv");
    sf::Polyhedron* test_body = new sf::Polyhedron("Test", phy_settings, DataPath + "Body.obj", sf::Scalar(1), sf::I4(), "LightAluminium", "silver_uv");
    // Leg 0
    sf::Polyhedron* coxa_L0 = new sf::Polyhedron("Coxa_L0", phy_settings, DataPath + "Coxa.obj", sf::Scalar(1), sf::I4(), "LightAluminium", "silver_uv");
    sf::Polyhedron* femur_L0 = new sf::Polyhedron("Femur_L0", phy_settings, DataPath + "Femur.obj", sf::Scalar(1), sf::I4(), "LightAluminium", "silver_uv");
    sf::Polyhedron* tibia_L0 = new sf::Polyhedron("Tibia_L0", phy_settings, DataPath + "Tibia.obj", sf::Scalar(1), sf::I4(), "LightAluminium", "silver_uv");
    // Leg 1
    sf::Polyhedron* coxa_L1 = new sf::Polyhedron("Coxa_L1", phy_settings, DataPath + "Coxa.obj", sf::Scalar(1), sf::I4(), "LightAluminium", "silver_uv");
    sf::Polyhedron* femur_L1 = new sf::Polyhedron("Femur_L1", phy_settings, DataPath + "Femur.obj", sf::Scalar(1), sf::I4(), "LightAluminium", "silver_uv");
    sf::Polyhedron* tibia_L1 = new sf::Polyhedron("Tibia_L1", phy_settings, DataPath + "Tibia.obj", sf::Scalar(1), sf::I4(), "LightAluminium", "silver_uv");
    // Leg 2
    sf::Polyhedron* coxa_L2 = new sf::Polyhedron("Coxa_L2", phy_settings, DataPath + "Coxa_Mirrored.obj", sf::Scalar(1), sf::I4(), "LightAluminium", "silver_uv"); 
    sf::Polyhedron* femur_L2 = new sf::Polyhedron("Femur_L2", phy_settings, DataPath + "Femur_Mirrored.obj", sf::Scalar(1), sf::I4(), "LightAluminium", "silver_uv");
    sf::Polyhedron* tibia_L2 = new sf::Polyhedron("Tibia_L2", phy_settings, DataPath + "Tibia_Mirrored.obj", sf::Scalar(1), sf::I4(), "LightAluminium", "silver_uv");
    // Leg 3
    sf::Polyhedron* coxa_L3 = new sf::Polyhedron("Coxa_L3", phy_settings, DataPath + "Coxa_Mirrored.obj", sf::Scalar(1), sf::I4(), "LightAluminium", "silver_uv"); 
    sf::Polyhedron* femur_L3 = new sf::Polyhedron("Femur_L3", phy_settings, DataPath + "Femur_Mirrored.obj", sf::Scalar(1), sf::I4(), "LightAluminium", "silver_uv");
    sf::Polyhedron* tibia_L3 = new sf::Polyhedron("Tibia_L3", phy_settings, DataPath + "Tibia_Mirrored.obj", sf::Scalar(1), sf::I4(), "LightAluminium", "silver_uv");
    // Leg 4
    sf::Polyhedron* coxa_L4 = new sf::Polyhedron("Coxa_L4", phy_settings, DataPath + "Coxa_Mirrored.obj", sf::Scalar(1), sf::I4(), "LightAluminium", "silver_uv"); 
    sf::Polyhedron* femur_L4 = new sf::Polyhedron("Femur_L4", phy_settings, DataPath + "Femur_Mirrored.obj", sf::Scalar(1), sf::I4(), "LightAluminium", "silver_uv");
    sf::Polyhedron* tibia_L4 = new sf::Polyhedron("Tibia_L4", phy_settings, DataPath + "Tibia_Mirrored.obj", sf::Scalar(1), sf::I4(), "LightAluminium", "silver_uv");
    // Leg 5
    sf::Polyhedron* coxa_L5 = new sf::Polyhedron("Coxa_L5", phy_settings, DataPath + "Coxa.obj", sf::Scalar(1), sf::I4(), "LightAluminium", "silver_uv");
    sf::Polyhedron* femur_L5 = new sf::Polyhedron("Femur_L5", phy_settings, DataPath + "Femur.obj", sf::Scalar(1), sf::I4(), "LightAluminium", "silver_uv");
    sf::Polyhedron* tibia_L5 = new sf::Polyhedron("Tibia_L5", phy_settings, DataPath + "Tibia.obj", sf::Scalar(1), sf::I4(), "LightAluminium", "silver_uv");

    //Create Robot
    sf::Robot* silver2 = new sf::FeatherstoneRobot("SILVER2", false);

    // Define Robot Links
    std::vector<sf::SolidEntity*> leg_links;
    // Leg 0
    leg_links.push_back(coxa_L0);
    //leg_links.push_back(femur_L0);
    //leg_links.push_back(tibia_L0);
    // Leg 1
    leg_links.push_back(coxa_L1);
    //leg_links.push_back(femur_L1);
    //leg_links.push_back(tibia_L1);
    // Leg 2
    leg_links.push_back(coxa_L2);
    //leg_links.push_back(femur_L2);
    //leg_links.push_back(tibia_L2);
    // Leg 3
    leg_links.push_back(coxa_L3);
    //leg_links.push_back(femur_L3);
    //leg_links.push_back(tibia_L3);
    // Leg 4
    leg_links.push_back(coxa_L4);
    //leg_links.push_back(femur_L4);
    //leg_links.push_back(tibia_L4);
    // Leg 5
    leg_links.push_back(coxa_L5);
    //leg_links.push_back(femur_L5);
    //leg_links.push_back(tibia_L5);
    // Append Links to Robot
    silver2->DefineLinks(body, leg_links, false);

    // Define Joints
    // -- LEG 0 --
    silver2->DefineRevoluteJoint("Joint_L0_Coxa", "MainBody", "Coxa_L0", 
        sf::Transform(sf::IQ(), sf::Vector3(1.62, 0.98, 0.28)), 
        sf::Vector3(0.0, 0.0, 1.0));
    /*silver2->DefineRevoluteJoint("Joint_L0_Femur", "Coxa_L0", "Femur_L0", 
        sf::Transform(sf::IQ(), sf::Vector3(0.55, 0.01, 0.7)), 
        sf::Vector3(0.0, 1.0, 0.0));
    silver2->DefineRevoluteJoint("Joint_L0_Tibia", "Femur_L0", "Tibia_L0", 
        sf::Transform(sf::IQ(), sf::Vector3(1.85, -0.04, 0.23)), 
        sf::Vector3(0.0, 1.0, 0.0));*/
    // -- LEG 1 --
    silver2->DefineRevoluteJoint("Joint_L1_Coxa", "MainBody", "Coxa_L1", 
        sf::Transform(sf::IQ(), sf::Vector3(1.62, -0.15, 0.28)), 
        sf::Vector3(0.0, 0.0, 1.0));
    /*silver2->DefineRevoluteJoint("Joint_L1_Femur", "Coxa_L1", "Femur_L1", 
        sf::Transform(sf::IQ(), sf::Vector3(0.55, 0.01, 0.7)), 
        sf::Vector3(0.0, 1.0, 0.0));
    silver2->DefineRevoluteJoint("Joint_L1_Tibia", "Femur_L1", "Tibia_L1", 
        sf::Transform(sf::IQ(), sf::Vector3(1.85, -0.04, 0.23)), 
        sf::Vector3(0.0, 1.0, 0.0));*/
    // -- LEG 2 --
    silver2->DefineRevoluteJoint("Joint_L2_Coxa", "MainBody", "Coxa_L2", 
        sf::Transform(sf::IQ(), sf::Vector3(1.62, -1.27, 0.28)), 
        sf::Vector3(0.0, 0.0, 1.0));
    /*silver2->DefineRevoluteJoint("Joint_L2_Femur", "Coxa_L2", "Femur_L2", 
        sf::Transform(sf::IQ(), sf::Vector3(0.55, 0.05, 0.7)), 
        sf::Vector3(0.0, 1.0, 0.0));
    silver2->DefineRevoluteJoint("Joint_L2_Tibia", "Femur_L2", "Tibia_L2", 
        sf::Transform(sf::IQ(), sf::Vector3(1.85, -0.03, 0.23)), 
        sf::Vector3(0.0, 1.0, 0.0));*/
    // -- LEG 3 --
    silver2->DefineRevoluteJoint("Joint_L3_Coxa", "MainBody", "Coxa_L3", 
        sf::Transform(sf::Quaternion(0,0,1,0), sf::Vector3(-1.62, 0.98, 0.28)), 
        sf::Vector3(0.0, 0.0, 1.0));
    /*silver2->DefineRevoluteJoint("Joint_L3_Femur", "Coxa_L3", "Femur_L3", 
        sf::Transform(sf::IQ(), sf::Vector3(0.55, 0.05, 0.7)), 
        sf::Vector3(0.0, 1.0, 0.0));
    silver2->DefineRevoluteJoint("Joint_L3_Tibia", "Femur_L3", "Tibia_L3", 
        sf::Transform(sf::IQ(), sf::Vector3(1.85, -0.03, 0.23)), 
        sf::Vector3(0.0, 1.0, 0.0));*/
    // -- LEG 4 --
    silver2->DefineRevoluteJoint("Joint_L4_Coxa", "MainBody", "Coxa_L4", 
        sf::Transform(sf::Quaternion(0,0,1,0), sf::Vector3(-1.62, -0.15, 0.28)), 
        sf::Vector3(0.0, 0.0, 1.0));
    /*silver2->DefineRevoluteJoint("Joint_L4_Femur", "Coxa_L4", "Femur_L4", 
        sf::Transform(sf::IQ(), sf::Vector3(0.55, 0.05, 0.7)), 
        sf::Vector3(0.0, 1.0, 0.0));
    silver2->DefineRevoluteJoint("Joint_L4_Tibia", "Femur_L4", "Tibia_L4", 
        sf::Transform(sf::IQ(), sf::Vector3(1.85, -0.03, 0.23)), 
        sf::Vector3(0.0, 1.0, 0.0));*/
    // -- LEG 5 --
    silver2->DefineRevoluteJoint("Joint_L5_Coxa", "MainBody", "Coxa_L5", 
        sf::Transform(sf::Quaternion(0,0,1,0), sf::Vector3(-1.62, -1.27, 0.28)), 
        sf::Vector3(0.0, 0.0, 1.0));
    /*silver2->DefineRevoluteJoint("Joint_L5_Femur", "Coxa_L5", "Femur_L5", 
        sf::Transform(sf::IQ(), sf::Vector3(0.55, 0.01, 0.7)), 
        sf::Vector3(0.0, 1.0, 0.0));
    silver2->DefineRevoluteJoint("Joint_L5_Tibia", "Femur_L5", "Tibia_L5", 
        sf::Transform(sf::IQ(), sf::Vector3(1.85, -0.04, 0.23)), 
        sf::Vector3(0.0, 1.0, 0.0));*/

    //Mechanical structure
    silver2->BuildKinematicStructure();

    // Add Robot to Scene
    AddRobot(silver2, sf::Transform(sf::IQ(), sf::Vector3(0.0,0.0,-3.0))); 
    
    // Instantiate Models
    sf::Box* box = new sf::Box("Box", phy_settings, sf::Vector3(0.5, 0.75, 0.3), sf::I4(), "LightAluminium", "red");
    AddSolidEntity(box, sf::Transform(sf::IQ(), sf::Vector3(10.0, 0.0, -1.0)));
    AddSolidEntity(test_body, sf::Transform(sf::IQ(), sf::Vector3(-10.0, 0.0, -1.0)));
}