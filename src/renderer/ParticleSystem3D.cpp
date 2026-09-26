#include "ParticleSystem3D.hpp"
#include "raymath.h"
#include <cstdlib>

namespace REngine {

static float RandomFloat(float min, float max) {
    float r = (float)std::rand() / (float)RAND_MAX;
    return min + r * (max - min);
}

ParticleSystem3D::ParticleSystem3D(size_t maxParticles) {
    m_particlePool.resize(maxParticles);
}

ParticleSystem3D& ParticleSystem3D::Get() {
    static ParticleSystem3D instance(1000);
    return instance;
}

void ParticleSystem3D::Emit(const ParticleProps& props, int count) {
    for (int i = 0; i < count; ++i) {
        Particle& p = m_particlePool[m_poolIndex];
        p.active = true;
        p.position = props.position;
        p.gravity = props.gravity;

        p.velocity.x = props.velocity.x + RandomFloat(-props.velocityVariation.x, props.velocityVariation.x);
        p.velocity.y = props.velocity.y + RandomFloat(-props.velocityVariation.y, props.velocityVariation.y);
        p.velocity.z = props.velocity.z + RandomFloat(-props.velocityVariation.z, props.velocityVariation.z);

        p.colorBegin = props.colorBegin;
        p.colorEnd = props.colorEnd;
        p.sizeBegin = props.sizeBegin + RandomFloat(-props.sizeBegin * 0.2f, props.sizeBegin * 0.2f);
        p.sizeEnd = props.sizeEnd;
        p.lifeTime = props.lifeTime;
        p.lifeRemaining = props.lifeTime;

        m_poolIndex = (m_poolIndex + 1) % m_particlePool.size();
    }
}

void ParticleSystem3D::EmitBurst(const Vector3& position, Color color, int count) {
    ParticleProps props;
    props.position = position;
    props.velocity = (Vector3){ 0.0f, 3.5f, 0.0f };
    props.velocityVariation = (Vector3){ 4.0f, 3.0f, 4.0f };
    props.gravity = (Vector3){ 0.0f, -9.8f, 0.0f };
    props.colorBegin = color;
    props.colorEnd = (Color){ color.r, color.g, color.b, 0 };
    props.sizeBegin = 0.25f;
    props.sizeEnd = 0.02f;
    props.lifeTime = 0.8f;

    Emit(props, count);
}

void ParticleSystem3D::OnUpdate(float dt) {
    for (auto& p : m_particlePool) {
        if (!p.active) continue;

        p.lifeRemaining -= dt;
        if (p.lifeRemaining <= 0.0f) {
            p.active = false;
            continue;
        }

        p.velocity = Vector3Add(p.velocity, Vector3Scale(p.gravity, dt));
        p.position = Vector3Add(p.position, Vector3Scale(p.velocity, dt));
    }
}

void ParticleSystem3D::OnRender3D() {
    for (const auto& p : m_particlePool) {
        if (!p.active) continue;

        float life = p.lifeRemaining / p.lifeTime;
        float progress = 1.0f - life;

        // Size interpolation
        float currentSize = p.sizeBegin + (p.sizeEnd - p.sizeBegin) * progress;
        if (currentSize <= 0.001f) continue;

        // Color interpolation
        Color currentColor = {
            (unsigned char)(p.colorBegin.r + (p.colorEnd.r - p.colorBegin.r) * progress),
            (unsigned char)(p.colorBegin.g + (p.colorEnd.g - p.colorBegin.g) * progress),
            (unsigned char)(p.colorBegin.b + (p.colorEnd.b - p.colorBegin.b) * progress),
            (unsigned char)(p.colorBegin.a + (p.colorEnd.a - p.colorBegin.a) * progress)
        };

        DrawCube(p.position, currentSize, currentSize, currentSize, currentColor);
    }
}

void ParticleSystem3D::Clear() {
    for (auto& p : m_particlePool) {
        p.active = false;
    }
}

} // namespace REngine
