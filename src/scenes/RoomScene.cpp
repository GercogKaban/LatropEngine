#include "scenes/Shared.h"
#include "scenes/RoomScene.h"
#include "LEngine.h"

void RoomScene::createFloor(int width, int depth)
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
                physicsComponent->material.restitution = 0.05f; 
                physicsComponent->material.frictionCombinator = LP::Material::CombinationMode::Maximum;
                physicsComponent->material.restitutionCombinator = LP::Material::CombinationMode::Minimum;
            });
            floor->graphicsComponent->setColorTexture("textures/smile.jpg");
        }
    }
}

void RoomScene::createWall(glm::ivec2 normal, int width, int height)
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
                physicsComponent->material.frictionCombinator = LP::Material::CombinationMode::Maximum;
                physicsComponent->material.restitutionCombinator = LP::Material::CombinationMode::Multiply;
            });
            floor->graphicsComponent->setColorTexture("textures/smile.jpg");
        }
    }
}

void RoomScene::createSmoothStairs(int height, glm::vec3 origin, float radius, float YStep, float angleStep)
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
                physicsComponent->collider = cubeOBBCollider;
                physicsComponent->transform.position = origin + position;
                physicsComponent->transform.scale.y = YStep;
                physicsComponent->material = LP::Material::Concrete; 
            });

        step->graphicsComponent->setColorTexture("textures/PavingStones138.jpg");

        angle += angleStep; // smooth curve
    }
}

void RoomScene::createPerfectlyBouncyPuddle(glm::vec3 origin, bool withCube)
{
    auto puddle = ObjectBuilder::construct<LActor>().lock();
    puddle->loadComponent<LG::LCube>();
    puddle->loadComponent<LP::RigidBody>([origin](LP::RigidBody* physicsComponent)
        {
            physicsComponent->setIsSimulated(false);
            
            physicsComponent->collider = cubeOBBCollider;
            physicsComponent->transform.position = origin;
            physicsComponent->transform.scale = glm::vec3(5.0f, 1.0f, 5.0f);
            physicsComponent->material.restitution = 1.0; 
            physicsComponent->material.restitutionCombinator = LP::Material::CombinationMode::Maximum;
        });
    puddle->graphicsComponent->setColorTexture("textures/Tiles133D.jpg");

    if (withCube)
    {
        auto cube = ObjectBuilder::construct<LActor>().lock();
        cube->loadComponent<LG::LCube>();
        cube->graphicsComponent->setColorTexture("textures/Tiles133D.jpg");

        cube->loadComponent<LP::RigidBody>([origin](LP::RigidBody* physicsComponent)
            {
                physicsComponent->setIsSimulated(true);
                physicsComponent->setMass(10.0f);
                physicsComponent->takesGravity = true;
                
                physicsComponent->collider = cubeOBBCollider;
                physicsComponent->transform.position = origin + glm::vec3(0.0f, 5.0f, 0.0f);
                physicsComponent->material = LP::Material::HumanBody;
            });
    }
}

void RoomScene::create()
{
    SharedScene::createPlayer();
    createFloor();
    createWall({ 1, 0 });
    createWall({ -1, 0 });
    createWall({ 0, 1 });
    createWall({ 0, -1 });
    createSmoothStairs(1000, { 5, 0, 2 }, 2.0, 0.05);
    createSmoothStairs(200, { -5, 0, 2 }, 1.4, 0.05);
    createPerfectlyBouncyPuddle({ 7.0f, 0.5f, -3.0f }, false);
}