#include <LWindow.h>
#include "LEngine.h"
#include "LPlayerCharacter.h"
#include <collision/AABBCollider.h>
#include <collision/BoundedPlaneCollider.h>

// MARK: - Shared Colliders
auto cubeAABBCollider = std::make_shared<LP::AABBCollider>(LP::AABBCollider::makeCube());
auto planeYUPCollider = std::make_shared<LP::BoundedPlaneCollider>(LP::BoundedPlaneCollider());

void createPlayer() 
{
	auto weakPlayer = ObjectBuilder::construct<LPlayerCharacter>();
	auto playerCharacter = weakPlayer.lock();
	playerCharacter->loadComponent<LP::RigidBody>([weakPlayer](LP::RigidBody* physicsComponent)
		{
			physicsComponent->setIsSimulated(true);
			physicsComponent->setMass(LPlayerCharacter::mass);
			physicsComponent->takesGravity = true;

			physicsComponent->collider = cubeAABBCollider;
			physicsComponent->transform.position = glm::vec3(2.0f, 2.0f, 2.0f);
			physicsComponent->transform.scale = LPlayerCharacter::standingDimensions;
			physicsComponent->material = LP::Material::HumanBody;
			physicsComponent->onCollision = [weakPlayer](LP::Collision collision, float depth) {
				if (collision.points.normal.y > 0)
				{
					auto playerCharacter = weakPlayer.lock();
					playerCharacter->resetJump();
				}
			};
		});
}

void createFloor(float scale = 20.0f) 
{
	auto floor = ObjectBuilder::construct<LActor>().lock();
	floor->loadComponent<LG::LPlane>();
	floor->loadComponent<LP::RigidBody>([scale](LP::RigidBody* physicsComponent)
		{
			physicsComponent->setIsSimulated(false);

			physicsComponent->collider = planeYUPCollider;
			physicsComponent->transform.position = glm::vec3(0.0f, 0.0f, 0.0f);
			physicsComponent->transform.scale = glm::vec3(scale, 1.0f, scale);
			physicsComponent->material = LP::Material::Ice;
		});
	floor->graphicsComponent->setColorTexture("textures/smile.jpg");
}

void createOriginalSample(bool isSimulated = false)
{

	auto cubeA = ObjectBuilder::construct<LActor>().lock();
	cubeA->loadComponent<LG::LCube>();
	cubeA->loadComponent< LP::RigidBody>([isSimulated](LP::RigidBody* physicsComponent)
		{
			physicsComponent->setIsSimulated(isSimulated);
			if (isSimulated) physicsComponent->setMass(10.0f);

			physicsComponent->collider = cubeAABBCollider;
			physicsComponent->takesGravity = isSimulated;
			physicsComponent->material = LP::Material::Metal;
			physicsComponent->material.restitution = 1.0f;
			physicsComponent->transform.position = glm::vec3(2.0f, 1.0f, 0.0f);
		});

	auto cubeB = ObjectBuilder::construct<LActor>().lock();
	cubeB->loadComponent<LG::LCube>();
	cubeB->loadComponent< LP::RigidBody>([isSimulated](LP::RigidBody* physicsComponent)
		{
			physicsComponent->setIsSimulated(isSimulated);
			if (isSimulated) physicsComponent->setMass(10.0f);

			physicsComponent->collider = cubeAABBCollider;
			physicsComponent->takesGravity = isSimulated;
			physicsComponent->material = LP::Material::Metal;
			physicsComponent->material.restitution = 2.2f;
			physicsComponent->transform.position = glm::vec3(2.0f, 3.0f, 0.0f);
		});

	auto cubeC = ObjectBuilder::construct<LActor>().lock();
	cubeC->loadComponent<LG::LCube>();
	cubeC->loadComponent< LP::RigidBody>([isSimulated](LP::RigidBody* physicsComponent)
		{
			physicsComponent->setIsSimulated(isSimulated);
			if (isSimulated) physicsComponent->setMass(10.0f);

			physicsComponent->collider = cubeAABBCollider;
			physicsComponent->takesGravity = isSimulated;
			physicsComponent->material = LP::Material::Rubber;
			physicsComponent->transform.position = glm::vec3(2.0f, 5.0f, 0.0f);
		});
}

void createStairs(int height, int maxLength = 3)
{
    int directionIndex = 0;
    int stepCount = 0;
    int currentLength = 0;
    glm::ivec2 position(0, 0); // Starting position

    // Movement directions: forward, right, backward, left
    glm::ivec2 directions[4] = 
	{
        {0, 1},   // forward (up)
        {1, 0},   // right
        {0, -1},  // backward (down)
        {-1, 0}   // left
    };

    for (int i = 0; i < height; ++i)
    {
		auto step = ObjectBuilder::construct<LActor>().lock();
		step->loadComponent<LG::LCube>().
			loadComponent<LP::RigidBody>([position, i](LP::RigidBody* physicsComponent)
			{
				physicsComponent->setIsSimulated(false);
				
				physicsComponent->collider = cubeAABBCollider;
				physicsComponent->transform.position = glm::vec3((float)position.x * 0.85f, (float)i, (float)position.y * 0.85f);
			});

		if (i % 2 == 0)
		{
			step->graphicsComponent->setColorTexture("textures/smile.jpg");
		}
		else
		{
			step->graphicsComponent->setColorTexture("textures/smile2.jpg");
		}

        // Move to the next position
        position += directions[directionIndex];
        ++currentLength;

        // Change direction if reached max length
        if (currentLength >= maxLength)
        {
            currentLength = 0;
            directionIndex = (directionIndex + 1) % 4;  // Cycle through 0-3
        }
    }
}

void createPerfectlyBouncyPuddle()
{
	auto puddle = ObjectBuilder::construct<LActor>().lock();
	puddle->loadComponent<LG::LCube>();
	puddle->loadComponent<LP::RigidBody>([](LP::RigidBody* physicsComponent)
		{
			physicsComponent->setIsSimulated(false);
			
			physicsComponent->collider = cubeAABBCollider;
			physicsComponent->transform.position = glm::vec3(7.0f, 0.5f, 7.0f);
			physicsComponent->transform.scale = glm::vec3(5.0f, 1.0f, 5.0f);
			// Must be adjusted to accomodate the material of the object that wishes to bounce:
			// bouncyPuddle.restituion = 1 / object.restituion
			physicsComponent->material.restitution = 2.0; 
		});
	puddle->graphicsComponent->setColorTexture("textures/smile.jpg");
}

void createStairsStressTest(int height, int maxLength = 3, float YStep = 0.01f)
{
    int directionIndex = 0;
    int stepCount = 0;
    int currentLength = 0;
    glm::ivec2 position(0, 0); // Starting position

    // Movement directions: forward, right, backward, left
    glm::ivec2 directions[4] = 
	{
        {0, 1},   // forward (up)
        {1, 0},   // right
        {0, -1},  // backward (down)
        {-1, 0}   // left
    };

    for (int i = -maxLength; i < height - maxLength; ++i)
    {
		auto step = ObjectBuilder::construct<LActor>().lock();
		step->loadComponent<LG::LCube>();
		step->loadComponent<LP::RigidBody>([position, i, YStep](LP::RigidBody* physicsComponent)
			{
				physicsComponent->setIsSimulated(false);
				
				physicsComponent->collider = cubeAABBCollider;
				physicsComponent->transform.position = glm::vec3((float)position.x * 0.85f, (float)i * YStep, (float)position.y * 0.85f);
			});

        // Move to the next position
        position += directions[directionIndex];
        ++currentLength;

        // Change direction if reached max length
        if (currentLength >= maxLength)
        {
            currentLength = 0;
            directionIndex = (directionIndex + 1) % 4;  // Cycle through 0-3
        }

		step->graphicsComponent->setColorTexture("textures/smile.jpg");
    }
}

void createStairsStressTest2(int height, int maxLength = 3, float YStep = 0.01f)
{
    int directionIndex = 0;
    int stepCount = 0;
    int currentLength = 0;
    glm::ivec2 position(0, 0); // Starting position

    // Movement directions: forward, right, backward, left
    glm::ivec2 directions[4] = 
	{
        {0, 1},   // forward (up)
        {1, 0},   // right
        {0, -1},  // backward (down)
        {-1, 0}   // left
    };

    for (int i = -maxLength; i < height - maxLength; ++i)
    {
		auto step = ObjectBuilder::construct<LActor>().lock();
		step->loadComponent<LG::LCube>();
		step->loadComponent<LP::RigidBody>([position, i, YStep](LP::RigidBody* physicsComponent)
			{
				physicsComponent->setIsSimulated(false);
				
				physicsComponent->collider = cubeAABBCollider;
				physicsComponent->transform.position = glm::vec3((float)position.x, (float)i * YStep, (float)position.y);
			});

        // Move to the next position
        position += directions[directionIndex];
        ++currentLength;

        // Change direction if reached max length
        if (currentLength >= maxLength)
        {
            currentLength = 0;
            directionIndex = (directionIndex + 1) % 4;  // Cycle through 0-3
        }
    }
}

void createPerfectlyBouncyPuddleNearHeavenlyOrbit(int height, int maxLength = 3, float YStep = 0.01f)
{
	auto puddle = ObjectBuilder::construct<LActor>().lock();
	puddle->loadComponent<LG::LCube>();
	puddle->loadComponent<LP::RigidBody>([height, maxLength, YStep](LP::RigidBody* physicsComponent)
		{
			physicsComponent->setIsSimulated(false);
			
			physicsComponent->collider = cubeAABBCollider;
			physicsComponent->transform.position = glm::vec3(7.0f, (height - maxLength) * YStep, 7.0f);
			physicsComponent->transform.scale = glm::vec3(5.0f, 1.0f, 5.0f);
			// Must be adjusted to accomodate the material of the object that wishes to bounce:
			// bouncyPuddle.restituion = 1 / object.restituion
			physicsComponent->material.restitution = 5.0; 
		});
}

void createPortals()
{
	const glm::vec3 portalScale = glm::vec3(1.6f, 0.01f, 2.8f);
	glm::quat rotationY = glm::angleAxis(glm::radians(90.0f), glm::vec3(1, 0, 0));

	const glm::vec3 pos1 = glm::vec3(7.0f, 1.0f, -5.0f);
	const glm::vec3 pos2 = glm::vec3(-5.0f, 1.0f, -5.0f);

	auto bluePortal = ObjectBuilder::construct<LActor>().lock();
	bluePortal->loadComponent<LG::LPortal>([pos1](LG::LGraphicsComponent* graphicsComponent)
		{
			const glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
			const glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
			dynamic_cast<LG::LPortal*>(graphicsComponent)->setPortalView(glm::lookAt(pos1, pos1 + cameraFront, cameraUp));

			auto p1View = glm::lookAt(pos1, pos1 + cameraFront, cameraUp);

			auto playerView = LRenderer::get()->getView();

			//glm::mat4 model(1.0f);
			//model = glm::translate(model, {0,0,5});

			auto modifiedModel = p1View * playerView;

			return;
		});
	bluePortal->loadComponent<LP::RigidBody>([pos1, portalScale, rotationY](LP::RigidBody* physicsComponent)
		{
			physicsComponent->setIsSimulated(false);

			auto player = LPlayerCharacter::get();
			player->bluePortal = physicsComponent;

			physicsComponent->collider = cubeAABBCollider;
			physicsComponent->transform.position = pos1;
			physicsComponent->transform.scale = portalScale;
			physicsComponent->transform.rotation *= rotationY;
			physicsComponent->isTrigger = true;

			physicsComponent->onCollision = [player](LP::Collision collision, float dt) {
				if (collision.points.normal.z == 1)
				{
					// std::cout << "Colliding: ";
					// std::cout << "x: " << collision.points.normal.x << " ";
					// std::cout << "y: " << collision.points.normal.y << " ";
					// std::cout << "z: " << collision.points.normal.z << " ";
					// std::cout << "d: " << collision.points.depth << " ";
					// std::cout << std::endl;
					// auto portal = player->orangePortal;
					// // Get destination portal's transform
					// auto destinationTransform = portal->transform;
					// // destinationTransform.position.y -= 1.0f;

					// // Compute teleport position: Move to portal position & offset slightly along normal
					// glm::vec3 portalNormal = glm::normalize(destinationTransform.rotation * glm::vec3 { 0.0f, 1.0f, 0.0f });
					// // Teleport position
					// player->physicsComponent->transform.position = destinationTransform.position + portalNormal/* * 1.1f */; // Offset slightly to prevent instant re-trigger
					// player->physicsComponent->linearVelocity = glm::reflect(player->physicsComponent->linearVelocity, portalNormal);

					// // Rotate player 180 degrees around Y-axis
					// player->setOrientation(player->orientation * glm::angleAxis(glm::radians(180.0f) ,glm::vec3(0, 1, 0)));
				}
			};
		});

	auto orangePortal = ObjectBuilder::construct<LActor>().lock();
	orangePortal->loadComponent<LG::LPortal>([pos2](LG::LGraphicsComponent* graphicsComponent)
		{
			const glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
			const glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
			dynamic_cast<LG::LPortal*>(graphicsComponent)->setPortalView(glm::lookAt(pos2, pos2 + cameraFront, cameraUp));
		});
	orangePortal->loadComponent<LP::RigidBody>([pos2, portalScale, rotationY](LP::RigidBody* physicsComponent)
		{
			physicsComponent->setIsSimulated(false);
			auto player = LPlayerCharacter::get();
			player->orangePortal = physicsComponent;

			physicsComponent->collider = cubeAABBCollider;
			physicsComponent->transform.position = pos2;
			physicsComponent->transform.scale = portalScale;
			physicsComponent->transform.rotation *= rotationY;
			physicsComponent->isTrigger = true;

			physicsComponent->onCollision = [player](LP::Collision collision, float dt) {
				if (collision.points.normal.z == -1)
				{
					// std::cout << "Colliding: ";
					// std::cout << "x: " << collision.points.normal.x << " ";
					// std::cout << "y: " << collision.points.normal.y << " ";
					// std::cout << "z: " << collision.points.normal.z << " ";
					// std::cout << "d: " << collision.points.depth << " ";
					// std::cout << std::endl;
					// auto portal = player->bluePortal;
					// // Get destination portal's transform
					// auto destinationTransform = portal->transform;
					// // destinationTransform.position.y -= 1.0f;

					// // Compute teleport position: Move to portal position & offset slightly along normal
					// glm::vec3 portalNormal = glm::normalize(destinationTransform.rotation * glm::vec3{ 0.0f, 1.0f, 0.0f });
					// // Teleport position
					// player->physicsComponent->transform.position = destinationTransform.position + portalNormal/* * 1.1f */; // Offset slightly to prevent instant re-trigger
					// player->physicsComponent->linearVelocity = glm::reflect(player->physicsComponent->linearVelocity, portalNormal);

					// // Rotate player 180 degrees around Y-axis
					// player->setOrientation(player->orientation * glm::angleAxis(glm::radians(180.0f) ,glm::vec3(0, 1, 0)));
				}
			};
		});
}

// MARK: Complex Scene - Room

namespace RoomScene
{
	void createFloor(int width = 20, int depth = 20)
	{
		int halfWidth = width / 2;
		int halfDepth = depth / 2;

		for (int i = -halfWidth; i < halfWidth; ++i)
		{
			for (int k = -halfDepth; k < halfDepth; ++k)
			{
				// float sign = (i + k) % 3;
				float sign = rand() % 3;
				glm::quat rotation = glm::angleAxis(sign * glm::radians(90.0f), glm::vec3(0, 1, 0));

				auto floor = ObjectBuilder::construct<LActor>().lock();
				floor->loadComponent<LG::LPlane>();
				floor->loadComponent<LP::RigidBody>([i, k, rotation](LP::RigidBody *physicsComponent) 
				{
					physicsComponent->setIsSimulated(false);

					physicsComponent->collider = planeYUPCollider;
					physicsComponent->transform.rotation *= rotation;
					physicsComponent->transform.position = glm::vec3(float(i), 0.0f, float(k));
					physicsComponent->material = LP::Material::Concrete; 
				});
				floor->graphicsComponent->setColorTexture("textures/smile.jpg");
			}
		}
	}

	void createWall(glm::ivec2 normal, int width = 20, int height = 10)
	{
		// glm::vec3 normal3 = { normal.x, 0.0, normal.y };

		int halfWidth = width / 2;

		for (int i = -halfWidth; i < halfWidth; ++i)
		{
			for (int j = 0; j < height; ++j)
			{
				auto floor = ObjectBuilder::construct<LActor>().lock();
				floor->loadComponent<LG::LPlane>();
				floor->loadComponent<LP::RigidBody>([normal, i, j, halfWidth](LP::RigidBody *physicsComponent)
				{
					physicsComponent->setIsSimulated(false);

					physicsComponent->collider = planeYUPCollider;
					physicsComponent->transform.position = glm::vec3(
						normal.x != 0 ? float(i) : normal.y * float(halfWidth - 0.5f),
						float(j), 
						normal.y != 0 ? float(i) : normal.x * float(halfWidth - 0.5f)
					);
					glm::quat rotation = glm::angleAxis(glm::radians(90.0f), glm::vec3(-normal.x, 0.0f, normal.y));
					physicsComponent->transform.rotation *= rotation;
					// float sign = (i + j) % 3;
					float sign = rand() % 3;
					glm::quat styler = glm::angleAxis(sign * glm::radians(90.0f), glm::vec3(0, 1, 0));
					physicsComponent->transform.rotation *= styler; 

					physicsComponent->material = LP::Material::Concrete; 
				});
				floor->graphicsComponent->setColorTexture("textures/smile.jpg");
			}
		}
	}

	void createSmoothStairs(int height, glm::vec3 origin, float radius = 2.0f, float YStep = 0.1f, float angleStep = 0.1f)
	{
		// int stepCount = float(height) / YStep;
		float angle = 0.0f;

		for (int i = 0; i < height; ++i)
		{
			glm::vec2 dir(glm::cos(angle), glm::sin(angle));
			glm::vec2 pos2D = dir * radius;
			glm::vec3 position(pos2D.x, i * YStep, pos2D.y);

			auto step = ObjectBuilder::construct<LActor>().lock();
			step->loadComponent<LG::LCube>().
				loadComponent<LP::RigidBody>([position, YStep, origin](LP::RigidBody* physicsComponent)
				{
					physicsComponent->setIsSimulated(false);
					physicsComponent->collider = cubeAABBCollider;
					physicsComponent->transform.position = origin + position;
					physicsComponent->transform.scale.y = YStep;
					physicsComponent->material = LP::Material::Concrete; 
				});

			step->graphicsComponent->setColorTexture("textures/Tiles133D.jpg");

			angle += angleStep; // smooth curve
		}
	}

	void createPerfectlyBouncyPuddle(glm::vec3 origin, bool withCube = false)
	{
		auto puddle = ObjectBuilder::construct<LActor>().lock();
		puddle->loadComponent<LG::LCube>();
		puddle->loadComponent<LP::RigidBody>([origin](LP::RigidBody* physicsComponent)
			{
				physicsComponent->setIsSimulated(false);
				
				physicsComponent->collider = cubeAABBCollider;
				physicsComponent->transform.position = origin;
				physicsComponent->transform.scale = glm::vec3(5.0f, 1.0f, 5.0f);
				// Must be adjusted to accomodate the material of the object that wishes to bounce:
				// bouncyPuddle.restituion = 1 / object.restituion
				physicsComponent->material.restitution = 2.0; 
			});
		puddle->graphicsComponent->setColorTexture("textures/Tiles133D.jpg");

		if (withCube)
		{
			auto cube = ObjectBuilder::construct<LActor>().lock();
			cube->loadComponent<LG::LCube>();
			cube->graphicsComponent->setColorTexture("textures/smile.jpg");

			cube->loadComponent<LP::RigidBody>([origin](LP::RigidBody* physicsComponent)
				{
					physicsComponent->setIsSimulated(true);
					physicsComponent->setMass(10.0f);
					physicsComponent->takesGravity = true;
					
					physicsComponent->collider = cubeAABBCollider;
					physicsComponent->transform.position = origin + glm::vec3(0.0f, 3.0f, 0.0f);
					physicsComponent->material = LP::Material::HumanBody;
				});
		}
	}

	void create()
	{
		createFloor();
		createWall({ 1, 0 });
		createWall({ -1, 0 });
		createWall({ 0, 1 });
		createWall({ 0, -1 });
		createSmoothStairs(1000, { 5, 0, 2 }, 2.0, 0.05);
		createSmoothStairs(200, { -5, 0, 2 }, 1.4, 0.05);
		createPerfectlyBouncyPuddle({ 7.0f, 0.5f, 7.0f }, false);
	}
} // namespace RoomScene

// MARK: Main

int main()
{
	LWindow::LWindowSpecs wndSpecs{ LWindow::WindowMode::Windowed,"LGWindow",false, 1920, 1080 };
	LEngine engine(std::make_unique<LWindow>(wndSpecs));

	// MARK: Samples
	createPlayer();
	// createFloor();
	// createOriginalSample(true);
	// createStairs(100);
	//createStairsStressTest(600000, 40, 0.00625f);
	//createPerfectlyBouncyPuddleNearHeavenlyOrbit(600000, 40, 0.00625f);
	createPortals();
	// createPerfectlyBouncyPuddle();

	RoomScene::create();
	
	// MARK: RunLoop
	engine.loop();
}