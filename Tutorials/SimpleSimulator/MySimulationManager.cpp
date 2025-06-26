#include "MySimulationManager.h"
#include <Stonefish/entities/statics/Plane.h>
#include <Stonefish/entities/solids/Sphere.h>

MySimulationManager::MySimulationManager(sf::Scalar stepsPerSecond) : SimulationManager(stepsPerSecond)
{
}

void MySimulationManager::BuildScenario()
{
    //Physical materials
    CreateMaterial("Aluminium", 2700.0, 0.8);
    CreateMaterial("Steel", 7810.0, 0.9);
    SetMaterialsInteraction("Aluminium", "Aluminium", 0.7, 0.5);
    SetMaterialsInteraction("Steel", "Steel", 0.4, 0.2);
    SetMaterialsInteraction("Aluminium", "Steel", 0.6, 0.4);

    //Graphical materials (looks)
    CreateLook("gray", sf::Color::Gray(0.5f), 0.3f, 0.2f);
    CreateLook("red", sf::Color::RGB(1.f,0.f,0.f), 0.1f, 0.f);

    //Create environment
    sf::Plane* plane = new sf::Plane("Ground", 10000.0, "Steel", "gray");
    AddStaticEntity(plane, sf::I4());

    //Create object
    sf::Sphere* sph = new sf::Sphere("Sphere", 0.1, sf::I4(), "Aluminium", sf::BodyPhysicsMode::SURFACE, "red");
    AddSolidEntity(sph, sf::Transform(sf::IQ(), sf::Vector3(0.0,0.0,-1.0)));
}