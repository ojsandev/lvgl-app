#include <gtest/gtest.h>

#include "common/async/WorkerPool.h"

TEST(WorkerPoolTest, SubmitTaskIncrementsRunningTaskCount)
{
  common::async::WorkerPool pool(1);

  pool.submit([]() {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  });

  EXPECT_EQ(pool.getRunningWorkerCount(), 1);
  EXPECT_EQ(pool.getRunningTaskCount(), 1);
}
