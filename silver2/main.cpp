#include "SilverApp.h"
#include "SilverManager.h"
#include <cfenv>

int main(int argc, const char * argv[])
{
    //feenableexcept(FE_ALL_EXCEPT & ~FE_INEXACT);
    //feenableexcept(FE_INVALID | FE_OVERFLOW);
    
    sf::RenderSettings s;
    s.windowW = 1200;
    s.windowH = 900;
    s.aa = sf::RenderQuality::HIGH;
    s.shadows = sf::RenderQuality::HIGH;
    s.ao = sf::RenderQuality::HIGH;
    s.atmosphere = sf::RenderQuality::MEDIUM;
    s.ocean = sf::RenderQuality::HIGH;
    s.ssr = sf::RenderQuality::HIGH;
    
    sf::HelperSettings h;
    h.showFluidDynamics = false;
    h.showCoordSys = false;
    h.showBulletDebugInfo = false;
    h.showSensors = false;
    h.showActuators = false;
    h.showForces = false;
    
    SilverManager simulationManager(200.0);
    simulationManager.setRealtimeFactor(1.0);
    SilverApp app(std::string(DATA_DIR_PATH), s, h, &simulationManager);
    app.Run();
    
    return 0;
}

