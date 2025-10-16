#include "scenes/Shared.h"
#include "scenes/RoomScene.h"
#include "scenes/PortalsSandbox.h"
#include "LEngine.h"
#include "LPlayerCharacter.h"

void PortalsSandbox::createSymmetricScenario()
{
    SharedScene::createPlayer({ -4, 2, -3.5 }, true);
    RoomScene::createFloor(400, 400);

    // MARK: Cubes
	SharedScene::createCube({ 6.0f, 1.0f, -2.0f }, false);
	SharedScene::createCube({ 3.0f, 1.0f, -1.0f }, false);
	SharedScene::createCube({ -3.0f, 1.0f, -9.0f }, false);
	SharedScene::createCube({ -6.0f, 1.0f, -8.0f }, false);

    // MARK: Portals
	SharedScene::createBluePortal({ 4.0f, 1.5f, -5.0f });
	SharedScene::createOrangePortal({ -4.0f, 1.5f, -5.0f });
}

void PortalsSandbox::createAsymmetricScenario()
{
    SharedScene::createPlayer({ -4, 2, -3.5 }, true);
    RoomScene::createFloor(400, 400);

    // MARK: Cubes
	SharedScene::createCube({ 6.0f, 1.0f, -2.0f }, false);
	SharedScene::createCube({ 3.0f, 1.0f, -1.0f }, false);
	SharedScene::createCube({ -6.0f, 1.0f, -8.0f }, false);

    // MARK: Portals
	SharedScene::createBluePortal({ 4.0f, 1.5f, -5.0f }, glm::angleAxis(glm::radians(0.0f), glm::vec3(0, 0, 1)));
	SharedScene::createOrangePortal({ -4.0f, 1.5f, -5.0f }, glm::angleAxis(glm::radians(0.0f), glm::vec3(0, 1, 0)));
}

void PortalsSandbox::createRotatedScenario()
{
	SharedScene::createPlayer();
    RoomScene::createFloor(400, 400);

	// MARK: Stairs
	RoomScene::createSmoothStairs(600, { 6, -1, 2 }, 4.5);
	RoomScene::createPerfectlyBouncyPuddle({ 6, 0.0, 2 }, true);

	// MARK: Cubes
    bool isSimulated = false;
	SharedScene::createCube({ 6.0f, 1.0f, 12.0f }, false);
	SharedScene::createCube({ 3.0f, 2.0f, 10.0f }, false);
	SharedScene::createCube({ -3.0f, 1.0f, -9.0f }, false);
	SharedScene::createCube({ -6.0f, 1.0f, -8.0f }, false);

	// MARK: Portals
	SharedScene::createBluePortal({ 4.0f, 1.6f, -5.0f }, glm::angleAxis(glm::radians(-45.0f), glm::vec3(0, 0, 1)));
	SharedScene::createOrangePortal({ -1.0f, 1.5f, -8.0f }, glm::angleAxis(glm::radians(-90.0f), glm::vec3(0, 1, 0)));
}

void PortalsSandbox::createStablePlayground()
{
	SharedScene::createPlayer();
    RoomScene::createFloor(400, 400);

	// MARK: Stairs
	RoomScene::createSmoothStairs(600, { 6, -1, 2 }, 4.5);
	RoomScene::createPerfectlyBouncyPuddle({ 6, 0.0, 2 }, true);

	// MARK: Cubes
    bool isSimulated = false;
	SharedScene::createCube({ 6.0f, 1.0f, 12.0f }, false);
	SharedScene::createCube({ 3.0f, 2.0f, 10.0f }, false);
	SharedScene::createCube({ -3.0f, 1.0f, -9.0f }, false);
	SharedScene::createCube({ -6.0f, 1.0f, -8.0f }, false);

	// MARK: Portals
	SharedScene::createBluePortal({ 4.0f, 1.5f, -5.0f }, glm::angleAxis(glm::radians(0.0f), glm::vec3(0, 0, 1)));
	SharedScene::createOrangePortal({ -4.0f, 1.5f, -5.0f }, glm::angleAxis(glm::radians(0.0f), glm::vec3(0, 1, 0)));
}