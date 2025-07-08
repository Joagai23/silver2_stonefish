#ifndef __Stonefish__SilverApp__
#define __Stonefish__SilverApp__

#include <core/GraphicalSimulationApp.h>
#include <graphics/OpenGLPrinter.h>
#include "SilverManager.h"

class SilverApp : public sf::GraphicalSimulationApp
{
public:
    SilverApp(std::string dataDirPath, sf::RenderSettings s, sf::HelperSettings h, SilverManager* sim);
    
    void DoHUD();
    void InitializeGUI();
    
private:
    sf::OpenGLPrinter* largePrint;
};

#endif
