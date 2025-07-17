#include <Stonefish/core/GraphicalSimulationApp.h>
#include "SilverManager.h"

int main(int argc, char **argv)
{
    //Using default settings
    sf::RenderSettings s;
    s.windowW = 1200;
    s.windowH = 900;
    sf::HelperSettings h;

    SilverManager manager(500.0);
    manager.setRealtimeFactor(1.0);
    sf::GraphicalSimulationApp app("Stonefish Silver2 Simulator", "path_to_data", s, h, &manager);
    app.Run();

    return 0;
}