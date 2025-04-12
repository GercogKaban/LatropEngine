#pragma once

namespace RotationalScene
{
    /// @brief Verify that the physics world is applying rotations correctly.
    /// @param isDiagonal Rotates around Y if false, around Y and Z otherwise.
    void createSanityCheck(bool isDiagonal = false);

    /// @brief Cube B falls on the edge of a static Cube A, friction is not enough
    /// to stabilise B on A and the impulse gained from free fall is too much.
    /// Cube B spins away down into the void.
    void createFreeFallScenarioI();

    /// @brief Cube B falls with its edge on the center of a static Cube A.
    /// There is more friction between the objects, which results in a slightly
    /// different trajectory of B, but overall the result is simillar to the
    /// FreeFallScenarioI.
    void createFreeFallScenarioII();

    /// @brief Cube B falls with its min edge to the location between min edge 
    /// and the center of a static Cube A.
    /// There is even more more friction between the objects compared to previous
    /// scenarious. It's still not enough to stabilises B on top of A, but its
    /// rotation is noticibely slowed down after colliding with A.
    void createFreeFallScenarioIII();

    /// @brief Cube B falls with its min edge closer to the min edge of a static Cube A.
    /// The friction is so great that Cube B balances on its edge, but still not
    /// enough to stabilise B, thus it eventually rotates and falls down to the void.
    void createFreeFallScenarioIV();

    /// @brief Cube B falls with its min edge even closer to the min edge of a static Cube A.
    /// The friction is large enough to stabilise B on top of A. 
    /// No rotation is happening, B is not moving.
    void createFreeFallScenarioV();

    /// @brief Creates all free fall scenarios with aligned on the Z-axis.
    void createAllFreeFallScenarios();
} // namespace RotationalScene