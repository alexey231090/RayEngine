#pragma once

#include "raylib.h"
#include <vector>

namespace REngine {

struct ParticleProps {
    Vector3 position = { 0.0f, 0.0f, 0.0f };
    Vector3 velocity = { 0.0f, 0.0f, 0.0f };
    Vector3 velocityVariation = { 3.0f, 4.0f, 3.0f };
    Vector3 gravity = { 0.0f, -9.8f, 0.0f };
    Color colorBegin = GOLD;
    Color colorEnd = RED;
    float sizeBegin = 0.25f;
    float sizeEnd = 0.02f;
    float lifeTime = 0.8f;
};

class ParticleSystem3D {
public:
    ParticleSystem3D(size_t maxParticles = 1000);
    ~ParticleSystem3D() = default;

    static ParticleSystem3D& Get();

    void Emit(const ParticleProps& props, int count = 1);
    void EmitBurst(const Vector3& position, Color color, int count = 30);

    void OnUpdate(float dt);
    void OnRender3D();
    void Clear();

private:
    struct Particle {
        Vector3 position;
        Vector3 velocity;
        Vector3 gravity;
        Color colorBegin;
        Color colorEnd;
        float sizeBegin;
        float sizeEnd;
        float lifeTime;
        float lifeRemaining;
        bool active = false;
    };

    std::vector<Particle> m_particlePool;
    size_t m_poolIndex = 0;
};

} // namespace REngine
