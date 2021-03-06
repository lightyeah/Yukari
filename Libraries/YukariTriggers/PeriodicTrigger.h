/** @file */

#pragma once

#include "ITrigger.h"

#include <atomic>
#include <chrono>
#include <thread>

#include <YukariCommon/LoggingService.h>

namespace Yukari
{
namespace Triggers
{
  class PeriodicTrigger : public ITrigger
  {
  public:
    PeriodicTrigger(std::chrono::milliseconds duration);
    virtual ~PeriodicTrigger();

    virtual void enable() override;
    virtual void disable() override;

  private:
    void workerFunc();

  private:
    Common::LoggingService::Logger m_logger;

    std::chrono::milliseconds m_duration;

    std::atomic_bool m_enabled;
    std::thread m_worker;
  };
}
}
