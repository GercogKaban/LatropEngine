#include <LWindow.h>
#include "LEngine.h"
#include "scenes/Shared.h"
#include "scenes/RotationalScene.h"
#include "scenes/RoomScene.h"

// MARK: Main

int main()
{
	LWindow::LWindowSpecs wndSpecs{ LWindow::WindowMode::Windowed,"LGWindow",false, 1920, 1080 };
	LEngine engine(std::make_unique<LWindow>(wndSpecs));

	// MARK: Samples
	RotationalScene::createBouncingScenarioII();

	// Portals are required by the engine at the moment.
	SharedScene::createPortals();
	
	// MARK: RunLoop
	engine.loop();
}