#pragma once

namespace OriginalScene
{
    void createFloor(float scale = 20.0f);

    void createOriginalSample(bool isSimulated = false);

    void createStairs(int height, int maxLength = 3);

    void createPerfectlyBouncyPuddle();

    void createStairsStressTest(int height, int maxLength = 3, float YStep = 0.01f);

    void createStairsStressTest2(int height, int maxLength = 3, float YStep = 0.01f);

    void createPerfectlyBouncyPuddleNearHeavenlyOrbit(int height, int maxLength = 3, float YStep = 0.01f);
} // namespace OriginalScene