#include <algorithm>
#include <exception>
#include <iostream>

#include <exec/single_thread_context.hpp>
#include <exec/when_any.hpp>
#include <stdexec/execution.hpp>
#include <tbbexec/tbb_thread_pool.hpp>

#include "application.h"
#include "widget.h"

int main(int, char **) {
    exec::single_thread_context ctx;

    auto app = Application();

    app.init();

    auto root = std::make_unique<ApplicationWindow>("root", &app);
    root->set_title("Hello World");

    app.set_root(std::move(root));

    auto label = std::make_unique<Label>("label");

    label->set_text("Hello World");
    label->set_border(1.0f);

    auto box = std::make_unique<Boxes>("box");

    box->set_capacity(2);

    box->set_widget(0, std::move(label));

    auto button = std::make_unique<Button>("button");

    button->set_text("Click Me");

    box->set_widget(1, std::move(button));

    dynamic_cast<ApplicationWindow *>(app.get_root())->set_widget(std::move(box));

    while (!app.should_close()) {
        app.poll_events();
        app.frame_move();
    }

    app.clean();

    return 0;
}