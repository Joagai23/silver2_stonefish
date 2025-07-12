#include "SilverManager.h"

#include "SilverApp.h"
#include <core/FeatherstoneRobot.h>
#include <entities/statics/Plane.h>
#include <entities/statics/Obstacle.h>
#include <entities/solids/Polyhedron.h>
#include <entities/solids/Box.h>
#include <entities/solids/Sphere.h>
#include <entities/solids/Torus.h>
#include <entities/solids/Cylinder.h>
#include <entities/solids/Compound.h>
#include <entities/solids/Wing.h>
#include <graphics/OpenGLPointLight.h>
#include <graphics/OpenGLSpotLight.h>
#include <graphics/OpenGLTrackball.h>
#include <utils/SystemUtil.hpp>
#include <entities/statics/Obstacle.h>
#include <entities/statics/Terrain.h>
#include <actuators/Thruster.h>
#include <actuators/Servo.h>
#include <actuators/VariableBuoyancy.h>
#include <sensors/scalar/Pressure.h>
#include <sensors/scalar/Odometry.h>
#include <sensors/scalar/DVL.h>
#include <sensors/scalar/Compass.h>
#include <sensors/scalar/IMU.h>
#include <sensors/scalar/GPS.h>
#include <sensors/Contact.h>
#include <sensors/vision/ColorCamera.h>
#include <sensors/vision/DepthCamera.h>
#include <sensors/vision/Multibeam2.h>
#include <sensors/vision/FLS.h>
#include <sensors/vision/SSS.h>
#include <sensors/vision/MSIS.h>
#include <comms/AcousticModem.h>
#include <sensors/Sample.h>
#include <actuators/Light.h>
#include <sensors/scalar/RotaryEncoder.h>
#include <sensors/scalar/Accelerometer.h>
#include <entities/FeatherstoneEntity.h>
#include <entities/forcefields/Trigger.h>
#include <entities/forcefields/Pipe.h>
#include <entities/forcefields/Jet.h>
#include <entities/forcefields/Uniform.h>
#include <entities/AnimatedEntity.h>
#include <sensors/scalar/Profiler.h>
#include <sensors/scalar/Multibeam.h>
#include <utils/UnitSystem.h>
#include <core/ScenarioParser.h>
#include <core/NED.h>

SilverManager::SilverManager(sf::Scalar stepsPerSecond)
: SimulationManager(stepsPerSecond, sf::SolverType::SOLVER_SI, sf::CollisionFilteringType::COLLISION_EXCLUSIVE)
{
}

void SilverManager::BuildScenario()
{
#ifdef PARSED_SCENARIO
    sf::ScenarioParser parser(this);
    bool success = parser.Parse(sf::GetDataPath() + "underwater_test.scn");
    if(!success)
        cCritical("Scenario parser: Parsing failed!");
#else
    ///////MATERIALS////////
    CreateMaterial("Dummy", sf::UnitSystem::Density(sf::CGS, sf::MKS, 0.4), 0.3);
    CreateMaterial("Fiberglass", sf::UnitSystem::Density(sf::CGS, sf::MKS, 1.5), 0.9);
    CreateMaterial("Rock", sf::UnitSystem::Density(sf::CGS, sf::MKS, 3.0), 0.6);
    CreateMaterial("Steel", 7810.0, 0.9);
    SetMaterialsInteraction("Dummy", "Dummy", 0.5, 0.2);
    SetMaterialsInteraction("Fiberglass", "Fiberglass", 0.5, 0.2);
    SetMaterialsInteraction("Rock", "Rock", 0.9, 0.7);
    SetMaterialsInteraction("Fiberglass", "Dummy", 0.5, 0.2);
    SetMaterialsInteraction("Rock", "Dummy", 0.6, 0.4);
    SetMaterialsInteraction("Rock", "Fiberglass", 0.6, 0.4);
    SetMaterialsInteraction("Steel", "Steel", 0.4, 0.2);
    SetMaterialsInteraction("Dummy", "Steel", 0.6, 0.4);
    
    ///////LOOKS///////////
    CreateLook("seabed", sf::Color::RGB(0.7f, 0.7f, 0.5f), 0.9f, 0.f, 0.f, "", sf::GetDataPath() + "sand_normal.png");
    CreateLook("silver", sf::Color::RGB(1.f, 1.f, 1.f), 0.3f, 0.4f, 0.f, sf::GetDataPath() + "SILVER_UV.png");
    CreateLook("gray", sf::Color::Gray(0.5f), 0.3f, 0.2f);

    ////////OBJECTS    
    //Create environment
    /*EnableOcean(0.0);
    getOcean()->setWaterType(0.2);
    getOcean()->AddVelocityField(new sf::Jet(sf::Vector3(0,0,1.0), sf::VY(), 0.3, 5.0));
    getOcean()->AddVelocityField(new sf::Uniform(sf::Vector3(1.0,0.0,0.0)));
    getOcean()->EnableCurrents();
    getAtmosphere()->SetSunPosition(0.0, 60.0);
    getNED()->Init(41.77737, 3.03376, 0.0);*/
    sf::Plane* plane = new sf::Plane("Ground", 10000.0, "Steel", "gray");
    AddStaticEntity(plane, sf::I4());
    
    //sf::Terrain* seabed = new sf::Terrain("Seabed", sf::GetDataPath() + "terrain.png", 1.0, 1.0, 5.0, "Rock", "seabed", 5.f);
    //AddStaticEntity(seabed, sf::Transform(sf::IQ(), sf::Vector3(0,0,15.0)));
	
	sf::Light* spot = new sf::Light("Spot", 0.02, 50.0, sf::Color::BlackBody(5000.0), 100.0);
	spot->AttachToWorld(sf::Transform(sf::Quaternion(0,0,M_PI/3.0), sf::Vector3(0.0,0.0,1.0)));
	AddActuator(spot);
    
    sf::Light* omni = new sf::Light("Omni", 0.02, sf::Color::BlackBody(5000.0), 10000.0);
	omni->AttachToWorld(sf::Transform(sf::Quaternion(0,0,M_PI/3.0), sf::Vector3(2.0,2.0,0.5)));
	AddActuator(omni);

    // Define Physics Settings
    sf::BodyPhysicsSettings phy;
    phy.mode = sf::BodyPhysicsMode::SURFACE;
    phy.collisions = true;
    phy.buoyancy = false;

    // Instantiate all physical parts
    //sf::Polyhedron* body = new sf::Polyhedron("MainBody", phy, sf::GetDataPath() + "Body.obj", sf::Scalar(0.01), sf::I4(), "Dummy", "gray");
    sf::Box* body = new sf::Box("MainBody", phy, sf::Vector3(1.0, 1.0, 1.0), sf::I4(), "Fiberglass", "gray");
    body->ScalePhysicalPropertiesToArbitraryMass(20.0);
    
    // Leg 0
    /*sf::Polyhedron* coxa_L0 = new sf::Polyhedron("Coxa_L0", phy, sf::GetDataPath() + "Coxa.obj", sf::Scalar(1), sf::I4(), "Dummy", "silver");
    coxa_L0->ScalePhysicalPropertiesToArbitraryMass(2.0);
    sf::Polyhedron* femur_L0 = new sf::Polyhedron("Femur_L0", phy, sf::GetDataPath() + "Femur.obj", sf::Scalar(1), sf::I4(), "Dummy", "silver");
    femur_L0->ScalePhysicalPropertiesToArbitraryMass(2.0);
    sf::Polyhedron* tibia_L0 = new sf::Polyhedron("Tibia_L0", phy, sf::GetDataPath() + "Tibia.obj", sf::Scalar(1), sf::I4(), "Dummy", "silver");
    tibia_L0->ScalePhysicalPropertiesToArbitraryMass(2.0);
    // Leg 1
    sf::Polyhedron* coxa_L1 = new sf::Polyhedron("Coxa_L1", phy, sf::GetDataPath() + "Coxa.obj", sf::Scalar(1), sf::I4(), "Dummy", "silver");
    coxa_L1->ScalePhysicalPropertiesToArbitraryMass(2.0);
    sf::Polyhedron* femur_L1 = new sf::Polyhedron("Femur_L1", phy, sf::GetDataPath() + "Femur.obj", sf::Scalar(1), sf::I4(), "Dummy", "silver");
    femur_L1->ScalePhysicalPropertiesToArbitraryMass(2.0);
    sf::Polyhedron* tibia_L1 = new sf::Polyhedron("Tibia_L1", phy, sf::GetDataPath() + "Tibia.obj", sf::Scalar(1), sf::I4(), "Dummy", "silver");
    tibia_L1->ScalePhysicalPropertiesToArbitraryMass(2.0);
    // Leg 2
    sf::Polyhedron* coxa_L2 = new sf::Polyhedron("Coxa_L2", phy, sf::GetDataPath() + "Coxa_Mirrored.obj", sf::Scalar(1), sf::I4(), "Dummy", "silver"); 
    coxa_L2->ScalePhysicalPropertiesToArbitraryMass(2.0); 
    sf::Polyhedron* femur_L2 = new sf::Polyhedron("Femur_L2", phy, sf::GetDataPath() + "Femur_Mirrored.obj", sf::Scalar(1), sf::I4(), "Dummy", "silver");
    femur_L2->ScalePhysicalPropertiesToArbitraryMass(2.0);
    sf::Polyhedron* tibia_L2 = new sf::Polyhedron("Tibia_L2", phy, sf::GetDataPath() + "Tibia_Mirrored.obj", sf::Scalar(1), sf::I4(), "Dummy", "silver");
    tibia_L2->ScalePhysicalPropertiesToArbitraryMass(2.0);
    // Leg 3
    sf::Polyhedron* coxa_L3 = new sf::Polyhedron("Coxa_L3", phy, sf::GetDataPath() + "Coxa_Mirrored.obj", sf::Scalar(1), sf::I4(), "Dummy", "silver");
    coxa_L3->ScalePhysicalPropertiesToArbitraryMass(2.0); 
    sf::Polyhedron* femur_L3 = new sf::Polyhedron("Femur_L3", phy, sf::GetDataPath() + "Femur_Mirrored.obj", sf::Scalar(1), sf::I4(), "Dummy", "silver");
    femur_L3->ScalePhysicalPropertiesToArbitraryMass(2.0);
    sf::Polyhedron* tibia_L3 = new sf::Polyhedron("Tibia_L3", phy, sf::GetDataPath() + "Tibia_Mirrored.obj", sf::Scalar(1), sf::I4(), "Dummy", "silver");
    tibia_L3->ScalePhysicalPropertiesToArbitraryMass(2.0);
    // Leg 4
    sf::Polyhedron* coxa_L4 = new sf::Polyhedron("Coxa_L4", phy, sf::GetDataPath() + "Coxa_Mirrored.obj", sf::Scalar(1), sf::I4(), "Dummy", "silver");
    coxa_L4->ScalePhysicalPropertiesToArbitraryMass(2.0);  
    sf::Polyhedron* femur_L4 = new sf::Polyhedron("Femur_L4", phy, sf::GetDataPath() + "Femur_Mirrored.obj", sf::Scalar(1), sf::I4(), "Dummy", "silver");
    femur_L4->ScalePhysicalPropertiesToArbitraryMass(2.0);
    sf::Polyhedron* tibia_L4 = new sf::Polyhedron("Tibia_L4", phy, sf::GetDataPath() + "Tibia_Mirrored.obj", sf::Scalar(1), sf::I4(), "Dummy", "silver");
    tibia_L4->ScalePhysicalPropertiesToArbitraryMass(2.0);
    // Leg 5
    sf::Polyhedron* coxa_L5 = new sf::Polyhedron("Coxa_L5", phy, sf::GetDataPath() + "Coxa.obj", sf::Scalar(1), sf::I4(), "Dummy", "silver");
    coxa_L5->ScalePhysicalPropertiesToArbitraryMass(2.0);
    sf::Polyhedron* femur_L5 = new sf::Polyhedron("Femur_L5", phy, sf::GetDataPath() + "Femur.obj", sf::Scalar(1), sf::I4(), "Dummy", "silver");
    femur_L5->ScalePhysicalPropertiesToArbitraryMass(2.0);
    sf::Polyhedron* tibia_L5 = new sf::Polyhedron("Tibia_L5", phy, sf::GetDataPath() + "Tibia.obj", sf::Scalar(1), sf::I4(), "Dummy", "silver");
    tibia_L5->ScalePhysicalPropertiesToArbitraryMass(2.0);*/

    //Create Robot
    sf::Robot* silver2 = new sf::FeatherstoneRobot("SILVER2", false);

    // Define Robot Links
    std::vector<sf::SolidEntity*> leg_links;
    // Leg 0
    /*leg_links.push_back(coxa_L0);
    leg_links.push_back(femur_L0);
    leg_links.push_back(tibia_L0);
    // Leg 1
    leg_links.push_back(coxa_L1);
    leg_links.push_back(femur_L1);
    leg_links.push_back(tibia_L1);
    // Leg 2
    leg_links.push_back(coxa_L2);
    leg_links.push_back(femur_L2);
    leg_links.push_back(tibia_L2);
    // Leg 3
    leg_links.push_back(coxa_L3);
    leg_links.push_back(femur_L3);
    leg_links.push_back(tibia_L3);
    // Leg 4
    leg_links.push_back(coxa_L4);
    leg_links.push_back(femur_L4);
    leg_links.push_back(tibia_L4);
    // Leg 5
    leg_links.push_back(coxa_L5);
    leg_links.push_back(femur_L5);
    leg_links.push_back(tibia_L5);*/
    // Append Links to Robot
    //silver2->DefineLinks(body, leg_links, false);
    silver2->DefineLinks(body);

    // Define Joints
    // -- LEG 0 --
    /*silver2->DefineRevoluteJoint("Joint_L0_Coxa", "MainBody", "Coxa_L0", 
        sf::Transform(sf::IQ(), sf::Vector3(1.62, -0.98, -0.28)), 
        sf::Vector3(0.0, 0.0, 1.0));
    silver2->DefineRevoluteJoint("Joint_L0_Femur", "Coxa_L0", "Femur_L0", 
        sf::Transform(sf::IQ(), sf::Vector3(0.55, -0.01, -0.7)), 
        sf::Vector3(0.0, 1.0, 0.0));
    silver2->DefineRevoluteJoint("Joint_L0_Tibia", "Femur_L0", "Tibia_L0", 
        sf::Transform(sf::IQ(), sf::Vector3(1.85, 0.04, -0.23)), 
        sf::Vector3(0.0, 1.0, 0.0));
    // -- LEG 1 --
    silver2->DefineRevoluteJoint("Joint_L1_Coxa", "MainBody", "Coxa_L1", 
        sf::Transform(sf::IQ(), sf::Vector3(1.62, 0.15, -0.28)), 
        sf::Vector3(0.0, 0.0, 1.0));
    silver2->DefineRevoluteJoint("Joint_L1_Femur", "Coxa_L1", "Femur_L1", 
        sf::Transform(sf::IQ(), sf::Vector3(0.55, -0.01, -0.7)), 
        sf::Vector3(0.0, 1.0, 0.0));
    silver2->DefineRevoluteJoint("Joint_L1_Tibia", "Femur_L1", "Tibia_L1", 
        sf::Transform(sf::IQ(), sf::Vector3(1.85, 0.04, -0.23)), 
        sf::Vector3(0.0, 1.0, 0.0));
    // -- LEG 2 --
    silver2->DefineRevoluteJoint("Joint_L2_Coxa", "MainBody", "Coxa_L2", 
        sf::Transform(sf::IQ(), sf::Vector3(1.62, 1.27, -0.28)), 
        sf::Vector3(0.0, 0.0, 1.0));
    silver2->DefineRevoluteJoint("Joint_L2_Femur", "Coxa_L2", "Femur_L2", 
        sf::Transform(sf::IQ(), sf::Vector3(0.55, -0.05, -0.7)), 
        sf::Vector3(0.0, 1.0, 0.0));
    silver2->DefineRevoluteJoint("Joint_L2_Tibia", "Femur_L2", "Tibia_L2", 
        sf::Transform(sf::IQ(), sf::Vector3(1.85, 0.03, -0.23)), 
        sf::Vector3(0.0, 1.0, 0.0));
    // -- LEG 3 --
    silver2->DefineRevoluteJoint("Joint_L3_Coxa", "MainBody", "Coxa_L3", 
        sf::Transform(sf::Quaternion(0,0,M_PI), sf::Vector3(-1.62, -0.98, -0.28)), 
        sf::Vector3(0.0, 0.0, 1.0));
    silver2->DefineRevoluteJoint("Joint_L3_Femur", "Coxa_L3", "Femur_L3", 
        sf::Transform(sf::IQ(), sf::Vector3(0.55, -0.05, -0.7)), 
        sf::Vector3(0.0, 1.0, 0.0));
    silver2->DefineRevoluteJoint("Joint_L3_Tibia", "Femur_L3", "Tibia_L3", 
        sf::Transform(sf::IQ(), sf::Vector3(1.85, 0.03, -0.23)), 
        sf::Vector3(0.0, 1.0, 0.0));
    // -- LEG 4 --
    silver2->DefineRevoluteJoint("Joint_L4_Coxa", "MainBody", "Coxa_L4", 
        sf::Transform(sf::Quaternion(0,0,M_PI), sf::Vector3(-1.62, 0.15, -0.28)), 
        sf::Vector3(0.0, 0.0, 1.0));
    silver2->DefineRevoluteJoint("Joint_L4_Femur", "Coxa_L4", "Femur_L4", 
        sf::Transform(sf::IQ(), sf::Vector3(0.55, -0.05, -0.7)), 
        sf::Vector3(0.0, 1.0, 0.0));
    silver2->DefineRevoluteJoint("Joint_L4_Tibia", "Femur_L4", "Tibia_L4", 
        sf::Transform(sf::IQ(), sf::Vector3(1.85, 0.03, -0.23)), 
        sf::Vector3(0.0, 1.0, 0.0));
    // -- LEG 5 --
    silver2->DefineRevoluteJoint("Joint_L5_Coxa", "MainBody", "Coxa_L5", 
        sf::Transform(sf::Quaternion(0,0,M_PI), sf::Vector3(-1.62, 1.27, -0.28)), 
        sf::Vector3(0.0, 0.0, 1.0));
    silver2->DefineRevoluteJoint("Joint_L5_Femur", "Coxa_L5", "Femur_L5", 
        sf::Transform(sf::IQ(), sf::Vector3(0.55, -0.01, -0.7)), 
        sf::Vector3(0.0, 1.0, 0.0));
    silver2->DefineRevoluteJoint("Joint_L5_Tibia", "Femur_L5", "Tibia_L5", 
        sf::Transform(sf::IQ(), sf::Vector3(1.85, 0.04, -0.23)), 
        sf::Vector3(0.0, 1.0, 0.0));*/
    
    //Mechanical structure
    silver2->BuildKinematicStructure();

    // Add Robot to Scene
    AddRobot(silver2, sf::Transform(sf::Quaternion(0,0,0), sf::Vector3(0.0,0.0,5.0)));

    #endif
} 

void SilverManager::SimulationStepCompleted(sf::Scalar timeStep){}