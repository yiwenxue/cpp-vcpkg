#include <algorithm>
#include <exception>
#include <iostream>

#include <exec/async_scope.hpp>
#include <exec/single_thread_context.hpp>
#include <exec/when_any.hpp>
#include <stdexec/execution.hpp>
#include <tbbexec/tbb_thread_pool.hpp>

#include "application.h"
#include "widget.h"

namespace ex = stdexec;

int main(int, char **) {
    exec::single_thread_context ctx;
    exec::async_scope           scope;

    auto app = Application();

    app.init();

    auto root = std::make_unique<ApplicationWindow>("root", &app);
    root->set_title("Hello World");

    app.set_root(std::move(root));

    auto label = std::make_unique<Label>("label");

    label->set_text("Hello World");
    label->set_border(1.0f);

    auto button = std::make_unique<Button>("button");

    button->set_text("Click Me");

    std::function<void()> callback = [&, l = label.get(), b = button.get()]() -> void {
        l->set_text("Clicked");

        scope.spawn(ex::schedule(ctx.get_scheduler()) | ex::then([=] {
                        std::this_thread::sleep_for(std::chrono::seconds(3));
                        l->set_text("Done");
                    }));
    };

    button->set_callback(callback);

    auto boxes = std::make_unique<Boxes>("boxes");

    boxes->set_size(2);
    boxes->set_widget(0, std::move(label));
    boxes->set_widget(1, std::move(button));

    dynamic_cast<ApplicationWindow *>(app.get_root())->set_widget(std::move(boxes));

    while (!app.should_close()) {
        app.poll_events();
        app.frame_move();
    }

    scope.request_stop();

    ex::sync_wait(scope.on_empty());

    app.clean();

    return 0;
}