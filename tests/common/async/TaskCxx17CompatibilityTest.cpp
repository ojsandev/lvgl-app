#include <gtest/gtest.h>

#include "common/async/Task.h"

TEST(TaskCxx17CompatibilityTest, CompletesTaskWithValue)
{
  const common::async::TaskCompletionSource<int> completion;
  const auto task = completion.task();

  ASSERT_FALSE(task.isReady());

  completion.set_value(7);

  ASSERT_TRUE(task.isReady());
  EXPECT_EQ(task.result(), 7);
}

TEST(TaskCxx17CompatibilityTest, CancelsTaskAndRequestsCancellation)
{
  const common::async::TaskCompletionSource<int> completion;
  const auto task = completion.task();

  EXPECT_TRUE(task.requestCancel());
  EXPECT_TRUE(task.isReady());
  EXPECT_TRUE(task.isCanceled());
  EXPECT_TRUE(completion.isCancellationRequested());
}

TEST(TaskCxx17CompatibilityTest, ThrowsWhenGettingCancelledTaskResult)
{
  const common::async::TaskCompletionSource<int> completion;
  const auto task = completion.task();

  ASSERT_TRUE(task.requestCancel());

  EXPECT_THROW(
      static_cast<void>(task.result()),
      common::async::TaskCanceledException);
}
