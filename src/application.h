#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <memory>

class Widget;

class Application {
public:
    Application()  = default;
    ~Application() = default;

    void init();

    void clean();

    bool should_close();

    void poll_events();

    void frame_move();

    GLFWwindow *get_window();

    void set_root(std::unique_ptr<Widget> root);

    Widget *get_root();

private:
    static ImVec4           clear_color;
    GLFWwindow             *window;
    std::unique_ptr<Widget> root;
};