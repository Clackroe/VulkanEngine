#ifndef VKE_INPUTH
#define VKE_INPUTH
#include <KeyCodes.hpp>

#include <core.hpp>

namespace VKE {

class Input {
public:
    static Input& getInstance()
    {
        static Input s_Instance;
        return s_Instance;
    };

    static bool isKeyDown(KeyCode key);
    static bool isKeyReleased(KeyCode key);

    static bool isMouseDown(KeyCode key = Key::MOUSE_BUTTON_1);
    static bool isMouseReleased(KeyCode key = Key::MOUSE_BUTTON_1);

    static float getMouseX();
    static float getMouseY();

    static void Init();

    void keyPressed(KeyCode key);
    void keyReleased(KeyCode key);

    void mousePressed(MouseCode key);
    void mouseReleased(MouseCode key);

    void mouseMoved(float x, float y);
    void mouseButtonPressed(MouseCode button);

    Input(const Input&) = delete;
    Input& operator=(const Input&) = delete;

private:
    ~Input() = default;
    Input() = default;

    UMap<KeyCode, bool> m_KeyStates;
    UMap<MouseCode, bool> m_MouseButtonStates;

    float m_MouseX;
    float m_MouseY;
};

}

#endif
