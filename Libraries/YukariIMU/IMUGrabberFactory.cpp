/** @file */

#include "IMUGrabberFactory.h"

#include <YukariCommon/MapHelpers.h>
#include <YukariCommon/StringParsers.h>

#include "DummyIMUGrabber.h"
#include "FileIMUGrabber.h"
#include "MSPGrabberAttitude.h"
#include "STM32IMUDevice.h"
#include "TeensyIMUDevice.h"

using namespace Yukari::Common;

namespace Yukari
{
namespace IMU
{
  IIMUGrabber::Ptr IMUGrabberFactory::Create(const std::string &fullCommand)
  {
    std::string type;
    std::map<std::string, std::string> params;
    if (!StringParsers::ParseCommand(fullCommand, type, params))
      return nullptr;

    return Create(type, params);
  }

  IIMUGrabber::Ptr IMUGrabberFactory::Create(const std::string &type,
                                             std::map<std::string, std::string> &parameters)
  {
    auto logger = Common::LoggingService::Instance().getLogger("IMUGrabberFactory");

    std::string lowerType = type;
    StringParsers::CleanString(lowerType);

    IIMUGrabber::Ptr grabber;
    if (lowerType == "dummy")
    {
      grabber = std::make_shared<DummyIMUGrabber>();
    }
    else if (lowerType == "file")
    {
      boost::filesystem::path dir =
          MapHelpers::Get<std::string, std::string>(parameters, "dir", ".");
      std::string pattern = MapHelpers::Get<std::string, std::string>(parameters, "pattern");
      int delay = std::stoi(MapHelpers::Get<std::string, std::string>(parameters, "delay", "10"));
      grabber = std::make_shared<FileIMUGrabber>(dir, pattern, std::chrono::milliseconds(delay));
    }
    else if (lowerType == "attitude")
    {
      std::string port = MapHelpers::Get<std::string, std::string>(parameters, "port");
      int baud = std::stoi(MapHelpers::Get<std::string, std::string>(parameters, "baud", "115200"));

      try
      {
        grabber = std::make_shared<MSPGrabberAttitude>(port, baud);
      }
      catch (const serial::IOException &e)
      {
        grabber = nullptr;
        logger->error("Cannot create MSP attitude grabber, IO error: {}", e.what());
      }
    }
    else if (lowerType == "teensy")
    {
      std::string port = MapHelpers::Get<std::string, std::string>(parameters, "port");
      int baud = std::stoi(MapHelpers::Get<std::string, std::string>(parameters, "baud", "115200"));

      try
      {
        grabber = std::make_shared<TeensyIMUDevice>(port, baud);
      }
      catch (const serial::IOException &e)
      {
        grabber = nullptr;
        logger->error("Cannot create Teensey grabber, IO error: {}", e.what());
      }
    }
    else if (lowerType == "stm32")
    {
      std::string port = MapHelpers::Get<std::string, std::string>(parameters, "port");
      int baud = std::stoi(MapHelpers::Get<std::string, std::string>(parameters, "baud", "115200"));

      std::string positionParam =
          MapHelpers::Get<std::string, std::string>(parameters, "position", "true");
      StringParsers::CleanString(positionParam);
      bool position = positionParam == "true";

      try
      {
        grabber = std::make_shared<STM32IMUDevice>(port, baud, position);
      }
      catch (const serial::IOException &e)
      {
        grabber = nullptr;
        logger->error("Cannot create STM32 grabber, IO error: {}", e.what());
      }
    }

    return grabber;
  }
}
}
