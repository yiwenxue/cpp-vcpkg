#include "widget.h"
#include "application.h"

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <tuple>

void ApplicationWindow::set_title(std::string title) {
    WindowWidget::set_title(title);
    glfwSetWindowTitle(this->application->get_window(), title.c_str());
}

void ApplicationWindow::set_size(ImVec2 pos, ImVec2 size) noexcept {
    std::ignore  = pos;
    auto *window = application->get_window();
    if (window) {
        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        size.x = static_cast<float>(w);
        size.y = static_cast<float>(h);

        ImGui::SetNextWindowSize(size, ImGuiCond_Always);
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    }
}