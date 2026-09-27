#pragma once
#include "raylib.h"
#include "raymath.h"
#include <algorithm>
#include <cmath>

namespace REngine {

/**
 * @brief Physics3D: Lightweight header-only arcade 3D collision resolution utilities.
 * Handles penetration push-out, velocity reflection, and collision response.
 */
class Physics3D {
public:
    /**
     * @brief Reflect a velocity vector off a surface normal with an optional bounciness factor.
     */
    static inline Vector3 Reflect(Vector3 vel, Vector3 normal, float bounciness = 1.0f) {
        float dot = Vector3DotProduct(vel, normal);
        Vector3 reflected = Vector3Subtract(vel, Vector3Scale(normal, (1.0f + bounciness) * dot));
        return reflected;
    }

    /**
     * @brief Resolves collision between a moving sphere and an axis-aligned bounding box (AABB).
     * Pushes the sphere out of penetration and reflects velocity along the collision normal.
     * 
     * @param spherePos [in,out] Current sphere center position (modified if penetrated)
     * @param sphereVel [in,out] Current sphere velocity (reflected upon collision)
     * @param radius Sphere radius
     * @param boxMin Minimum corner of the AABB
     * @param boxMax Maximum corner of the AABB
     * @param bounciness Restitution coefficient (1.0 = elastic bounce, 0.0 = stops)
     * @return true if collision occurred and was resolved, false otherwise.
     */
    static inline bool ResolveSphereAABB(Vector3& spherePos, Vector3& sphereVel, float radius,
                                         Vector3 boxMin, Vector3 boxMax, float bounciness = 1.0f) {
        // Find closest point on AABB to sphere center
        Vector3 closest;
        closest.x = std::max(boxMin.x, std::min(spherePos.x, boxMax.x));
        closest.y = std::max(boxMin.y, std::min(spherePos.y, boxMax.y));
        closest.z = std::max(boxMin.z, std::min(spherePos.z, boxMax.z));

        Vector3 diff = Vector3Subtract(spherePos, closest);
        float distSq = Vector3LengthSqr(diff);

        // Check if sphere center is inside the box
        bool inside = (spherePos.x >= boxMin.x && spherePos.x <= boxMax.x) &&
                      (spherePos.y >= boxMin.y && spherePos.y <= boxMax.y) &&
                      (spherePos.z >= boxMin.z && spherePos.z <= boxMax.z);

        if (inside) {
            // Find minimal push-out axis
            float pushLeft = spherePos.x - boxMin.x;
            float pushRight = boxMax.x - spherePos.x;
            float pushDown = spherePos.y - boxMin.y;
            float pushUp = boxMax.y - spherePos.y;
            float pushBack = spherePos.z - boxMin.z;
            float pushFront = boxMax.z - spherePos.z;

            float minPush = pushLeft;
            Vector3 pushNormal = { -1.0f, 0.0f, 0.0f };

            if (pushRight < minPush) { minPush = pushRight; pushNormal = { 1.0f, 0.0f, 0.0f }; }
            if (pushDown < minPush)  { minPush = pushDown;  pushNormal = { 0.0f, -1.0f, 0.0f }; }
            if (pushUp < minPush)    { minPush = pushUp;    pushNormal = { 0.0f, 1.0f, 0.0f }; }
            if (pushBack < minPush)  { minPush = pushBack;  pushNormal = { 0.0f, 0.0f, -1.0f }; }
            if (pushFront < minPush) { minPush = pushFront; pushNormal = { 0.0f, 0.0f, 1.0f }; }

            // Push sphere completely out
            spherePos = Vector3Add(spherePos, Vector3Scale(pushNormal, minPush + radius + 0.001f));
            sphereVel = Reflect(sphereVel, pushNormal, bounciness);
            return true;
        }

        if (distSq < radius * radius && distSq > 0.000001f) {
            float dist = std::sqrt(distSq);
            Vector3 normal = Vector3Scale(diff, 1.0f / dist);
            float penetration = radius - dist;

            // Push out of collision
            spherePos = Vector3Add(spherePos, Vector3Scale(normal, penetration + 0.001f));

            // Only reflect if moving towards the box
            if (Vector3DotProduct(sphereVel, normal) < 0.0f) {
                sphereVel = Reflect(sphereVel, normal, bounciness);
            }
            return true;
        }

        return false;
    }

    /**
     * @brief Resolves collision between a moving sphere and a center/size defined box.
     */
    static inline bool ResolveSphereBox(Vector3& spherePos, Vector3& sphereVel, float radius,
                                        Vector3 boxCenter, Vector3 boxSize, float bounciness = 1.0f) {
        Vector3 halfSize = Vector3Scale(boxSize, 0.5f);
        Vector3 boxMin = Vector3Subtract(boxCenter, halfSize);
        Vector3 boxMax = Vector3Add(boxCenter, halfSize);
        return ResolveSphereAABB(spherePos, sphereVel, radius, boxMin, boxMax, bounciness);
    }

    /**
     * @brief Resolves collision between two moving spheres (elastic collision response).
     */
    static inline bool ResolveSphereSphere(Vector3& posA, Vector3& velA, float radA,
                                           Vector3& posB, Vector3& velB, float radB,
                                           float bounciness = 1.0f) {
        Vector3 diff = Vector3Subtract(posA, posB);
        float distSq = Vector3LengthSqr(diff);
        float totalRad = radA + radB;

        if (distSq < totalRad * totalRad && distSq > 0.000001f) {
            float dist = std::sqrt(distSq);
            Vector3 normal = Vector3Scale(diff, 1.0f / dist);
            float penetration = totalRad - dist;

            // Separate spheres proportionally (50/50)
            posA = Vector3Add(posA, Vector3Scale(normal, penetration * 0.5f + 0.001f));
            posB = Vector3Subtract(posB, Vector3Scale(normal, penetration * 0.5f + 0.001f));

            // Relative velocity
            Vector3 relVel = Vector3Subtract(velA, velB);
            float velAlongNormal = Vector3DotProduct(relVel, normal);

            if (velAlongNormal < 0.0f) {
                float impulse = -(1.0f + bounciness) * velAlongNormal * 0.5f;
                velA = Vector3Add(velA, Vector3Scale(normal, impulse));
                velB = Vector3Subtract(velB, Vector3Scale(normal, impulse));
            }
            return true;
        }
        return false;
    }

    /**
     * @brief Resolves collision between a sphere and an infinite plane.
     */
    static inline bool ResolveSpherePlane(Vector3& spherePos, Vector3& sphereVel, float radius,
                                          Vector3 planePoint, Vector3 planeNormal, float bounciness = 1.0f) {
        planeNormal = Vector3Normalize(planeNormal);
        float dist = Vector3DotProduct(Vector3Subtract(spherePos, planePoint), planeNormal);

        if (dist < radius) {
            float penetration = radius - dist;
            spherePos = Vector3Add(spherePos, Vector3Scale(planeNormal, penetration + 0.001f));

            if (Vector3DotProduct(sphereVel, planeNormal) < 0.0f) {
                sphereVel = Reflect(sphereVel, planeNormal, bounciness);
            }
            return true;
        }
        return false;
    }
};

} // namespace REngine
