#include "input.hpp"

Input::Input(GLFWwindow *window) : _win_h(window) {}
Input::~Input() {}

void Input::update() {
    int state = glfwGetKey(_win_h, GLFW_KEY_W);
    if (state == GLFW_PRESS)
        _w_p = true;
    else if (state == GLFW_RELEASE)
        _w_p = false;
    
    state = glfwGetKey(_win_h, GLFW_KEY_A);
    if (state == GLFW_PRESS)
        _a_p = true;
    else if (state == GLFW_RELEASE)
        _a_p = false;

    state = glfwGetKey(_win_h, GLFW_KEY_S);
    if (state == GLFW_PRESS)
        _s_p = true;
    else if (state == GLFW_RELEASE)
        _s_p = false;
    
    state = glfwGetKey(_win_h, GLFW_KEY_D);
    if (state == GLFW_PRESS)
        _d_p = true;
    else if (state == GLFW_RELEASE)
        _d_p = false;

    state = glfwGetMouseButton(_win_h, GLFW_MOUSE_BUTTON_LEFT);
    if (state == GLFW_PRESS)
        _m1_p = true;
    else if (state == GLFW_RELEASE)
        _m1_p = false;

    state = glfwGetMouseButton(_win_h, GLFW_MOUSE_BUTTON_RIGHT);
    if (state == GLFW_PRESS)
        _m2_p = true;
    else if (state == GLFW_RELEASE)
        _m2_p = false;

    glfwGetCursorPos(_win_h, &_mousex, &_mousey);
};

void Input::setsticky(bool value) {
    if (value)
        glfwSetInputMode(_win_h, GLFW_STICKY_KEYS, GLFW_TRUE);
    else
        glfwSetInputMode(_win_h, GLFW_STICKY_KEYS, GLFW_FALSE);
}

bool Input::get_w() { return _w_p; }
bool Input::get_a() { return _a_p; }
bool Input::get_s() { return _s_p; }
bool Input::get_d() { return _d_p; }
bool Input::get_m1() { return _m1_p; }
bool Input::get_m2() { return _m2_p; }

glm::vec2 Input::inputdir() {
    float vertical = float(_w_p) + (-1.0f * float(_s_p));
    float horizontal = float(_d_p) + (-1.0f * float(_a_p));
    float angle = (horizontal != 0) ? glm::atan(glm::abs(vertical) / glm::abs(horizontal)) : (PI / 2.0f);
    return glm::vec2(
        horizontal * glm::cos(angle),
        vertical * glm::sin(angle)
    );
}

glm::vec2 Input::mousepos() {
    return glm::vec2(_mousex, _mousey);
}