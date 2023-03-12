#include <iostream>

#include <unifex/just.hpp>
#include <unifex/scheduler_concepts.hpp>
#include <unifex/sync_wait.hpp>
#include <unifex/then.hpp>
#include <unifex/timed_single_thread_context.hpp>

#include <chrono>

using namespace std::chrono_literals;

int main() {

  using namespace unifex;
  timed_single_thread_context context;

  auto scheduler = context.get_scheduler();

  auto task = schedule_after(scheduler, 10ms) |
              then([]() { std::cout << "Hello, World!" << std::endl; });

  sync_wait(std::move(task));

  return 0;
}