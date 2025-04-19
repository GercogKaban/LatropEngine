#pragma once

#include <glm/glm.hpp>
#include <gtest/gtest.h>

::testing::AssertionResult Vec3Equal(const glm::vec3& a, const glm::vec3& b, float epsilon = 1e-5f) {
    if (glm::all(glm::lessThan(glm::abs(a - b), glm::vec3(epsilon)))) {
        return ::testing::AssertionSuccess();
    }
    return ::testing::AssertionFailure()
           << "Vectors not equal. \nGot:      (" << a.x << ", " << a.y << ", " << a.z << ") "
           << "\nExpected: (" << b.x << ", " << b.y << ", " << b.z << ")";
}