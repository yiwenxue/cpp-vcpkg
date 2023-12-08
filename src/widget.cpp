#include "widget.h"
#include "application.h"

#include <GLFW/glfw3.h>

void ApplicationWindow::set_title(std::string title) {
    WindowWidget::set_title(title);
    glfwSetWindowTitle(this->application->get_window(), title.c_str());
}
