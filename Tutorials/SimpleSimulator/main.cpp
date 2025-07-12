#include <Stonefish/core/GraphicalSimulationApp.h>
#include "MySimulationManager.h"

int main(int argc, char **argv)
{
    //Using default settings
    sf::RenderSettings s;
    s.windowW = 1200;
    s.windowH = 900;
    sf::HelperSettings h;

    MySimulationManager manager(500.0);
    manager.setRealtimeFactor(1.0);
    sf::GraphicalSimulationApp app("Simple simulator", "path_to_data", s, h, &manager);
    app.Run();

    return 0;
}