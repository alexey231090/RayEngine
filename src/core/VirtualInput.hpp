#pragma once
#include "raylib.h"
#include <unordered_map>
#include <vector>

namespace REngine {

/**
 * @brief VirtualInput system allowing both physical Raylib inputs and simulated inputs for automated testing.
 */
class VirtualInput {
public:
    static VirtualInput& Get() {
        static VirtualInput instance;
        return instance;
    }

    // Set virtual key state
    void SetKeyPressed(int key, bool pressed) {
        m_keysPressed[key] = pressed;
    }

    void SetKeyDown(int key, bool down) {
        m_keysDown[key] = down;
    }

    // Check if key was pressed this frame (physical OR virtual)
    bool IsKeyPressed(int key) const {
        if (::IsKeyPressed(key)) return true;
        auto it = m_keysPressed.find(key);
        return it != m_keysPressed.end() && it->second;
    }

    // Check if key is being held down (physical OR virtual)
    bool IsKeyDown(int key) const {
        if (::IsKeyDown(key)) return true;
        auto it = m_keysDown.find(key);
        return it != m_keysDown.end() && it->second;
    }

    // Auto-clear single-frame triggers at end of frame
    void EndFrame() {
        m_keysPressed.clear();
    }

    // Clear all virtual inputs
    void Reset() {
        m_keysPressed.clear();
        m_keysDown.clear();
    }

private:
    VirtualInput() = default;
    std::unordered_map<int, bool> m_keysPressed;
    std::unordered_map<int, bool> m_keysDown;
};

// Global helper functions
inline bool IsActionPressed(int key) {
    return VirtualInput::Get().IsKeyPressed(key);
}

inline bool IsActionDown(int key) {
    return VirtualInput::Get().IsKeyDown(key);
}

} // namespace REngine
