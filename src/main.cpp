#include <LWindow.h>
#include "LEngine.h"

class StaticCube : public LObject
{

};

int main()
{
	LWindow::LWindowSpecs wndSpecs{ LWindow::WindowMode::Windowed,"LGWindow",false, 1920, 1080 };
	LWindow wnd(wndSpecs);

	LEngine engine(wnd);

	auto character = ObjectBuilder::construct<PlayerCharacter>();
	auto testcube = ObjectBuilder::construct<StaticCube, LG::LCube>();

	engine.loop();
}