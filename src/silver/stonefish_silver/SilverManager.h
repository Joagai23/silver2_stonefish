#include <Stonefish/core/SimulationManager.h>

class SilverManager : public sf::SimulationManager
{
public:
    SilverManager(sf::Scalar stepsPerSecond);
    void BuildScenario();
};