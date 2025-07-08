#ifndef __Stonefish__SilverManager__
#define __Stonefish__SilverManager__

#include <core/SimulationManager.h>

//#define PARSED_SCENARIO

class SilverManager : public sf::SimulationManager
{
public:
    SilverManager(sf::Scalar stepsPerSecond);
    
    void BuildScenario();
    void SimulationStepCompleted(sf::Scalar timeStep);
};

#endif
