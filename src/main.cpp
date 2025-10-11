#include <LWindow.h>
#include "LEngine.h"
#include "scenes/Shared.h"
#include "scenes/RotationalScene.h"
#include "scenes/RoomScene.h"
#include "scenes/PortalsSandbox.h"

// MARK: Main

int main()
{
	LWindow::LWindowSpecs wndSpecs{ LWindow::WindowMode::Windowed,"LGWindow", false, 1920, 1080 };
	LEngine engine(std::make_unique<LWindow>(wndSpecs));

	// - Warning: Portals are required by the engine at the moment.
	//RoomScene::create();
	PortalsSandbox::createRotatedScenario();
	
	// MARK: RunLoop
	engine.loop();
}