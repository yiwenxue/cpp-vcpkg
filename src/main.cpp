#include <algorithm>
#include <exception>
#include <iostream>

#include <exec/async_scope.hpp>
#include <exec/repeat_effect_until.hpp>
#include <exec/single_thread_context.hpp>
#include <exec/when_any.hpp>
#include <stdexec/__detail/__execution_fwd.hpp>
#include <stdexec/execution.hpp>
#include <tbbexec/tbb_thread_pool.hpp>
#include <thread>

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

    auto progress = std::make_unique<ProgressBar>("progress");

    auto button = std::make_unique<Button>("button");

    button->set_text("Click Me");

    std::function<void()> callback
        = [&, l = label.get(), b = button.get(), p = progress.get()]() -> void {
        l->set_text("Clicked");

        p->set_progress(0.0f);

        auto task = ex::schedule(ctx.get_scheduler()) | ex::then([&] {
                        auto progress = p->get_progress();
                        progress += 0.01f;
                        std::this_thread::sleep_for(std::chrono::milliseconds(100));
                        p->set_progress(progress);
                        return progress >= 1.0f;
                    })
                    | exec::repeat_effect_until() | ex::then([&] { l->set_text("Done"); });

        scope.spawn(std::move(task));
    };

    button->set_callback(callback);

    auto boxes = std::make_unique<Boxes>("boxes");

    boxes->set_size(3);
    boxes->set_widget(0, std::move(label));
    boxes->set_widget(1, std::move(progress));
    boxes->set_widget(2, std::move(button));

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