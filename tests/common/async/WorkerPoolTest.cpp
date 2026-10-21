#include <gtest/gtest.h>

#include "common/async/WorkerPool.h"

TEST(WorkerPoolTest, SubmitTaskIncrementsRunningTaskCount)
{
  auto pool = std::make_shared<common::async::WorkerPool>(1);

  pool->submit([pool]() {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  });

  EXPECT_EQ(pool->getRunningWorkerCount(), 1);
  EXPECT_EQ(pool->getRunningTaskCount(), 1);
}
