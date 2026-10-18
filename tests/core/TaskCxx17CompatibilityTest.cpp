#include "common/async/Task.h"

namespace {

bool completionWorks()
{
  common::async::TaskCompletionSource<int> completion;
  auto task = completion.task();

  if (task.isReady()) {
    return false;
  }

  completion.set_value(7);
  return task.isReady() && task.result() == 7;
}

bool cancellationWorks()
{
  common::async::TaskCompletionSource<int> completion;
  auto task = completion.task();

  if (!task.requestCancel() || !task.isReady() || !task.isCanceled() || !completion.isCancellationRequested()) {
    return false;
  }

  try {
    static_cast<void>(task.result());
    return false;
  } catch (const common::async::TaskCanceledException&) {
    return true;
  } catch (...) {
    return false;
  }
}

} // namespace

int main()
{
  if (!completionWorks()) {
    return 1;
  }

  return cancellationWorks() ? 0 : 1;
}
