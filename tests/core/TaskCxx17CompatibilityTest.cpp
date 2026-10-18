#include "common/async/Task.h"

int main()
{
  common::async::TaskCompletionSource<int> completion;
  auto task = completion.task();

  if (task.isReady()) {
    return 1;
  }

  completion.set_value(7);
  return task.isReady() && task.result() == 7 ? 0 : 1;
}
