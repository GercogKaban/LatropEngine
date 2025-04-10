#include <LWindow.h>
#include "LEngine.h"
#include "scenes/Shared.h"

// MARK: Main

int main()
{
	LWindow::LWindowSpecs wndSpecs{ LWindow::WindowMode::Windowed,"LGWindow",false, 1920, 1080 };
	LEngine engine(std::make_unique<LWindow>(wndSpecs));

	// MARK: Samples
	SharedScene::createPlayer();
	SharedScene::createPortals();
	
	// MARK: RunLoop
	engine.loop();
}