/** @file */

#include <iostream>

#include <boost/program_options.hpp>
#include <boost/qvm/all.hpp>
#include <serial/serial.h>

#include <YukariTriggers/TriggerFactory.h>
#include <YukariCommon/LoggingService.h>

using namespace Yukari::Triggers;
using namespace Yukari::Common;
namespace po = boost::program_options;

int main(int argc, char **argv)
{
  auto logger = LoggingService::Instance().getLogger("main");

  /* Init command line */
  po::options_description desc("Allowed options");
  po::variables_map args;

  // clang-format off
  desc.add_options()
    ("help", "Show brief usage message")
    ("trigger", po::value<std::string>(), "Name of trigger to add");
  // clang-format on

  /* Parse command line args */
  try
  {
    po::store(po::parse_command_line(argc, argv, desc), args);
  }
  catch (po::error const &e)
  {
    logger->critical("{}", e.what());
    return 1;
  }

  /* Show usage */
  if (args.count("help"))
  {
    std::cout << desc << "\n";
    return 1;
  }

  /* Get trigger */
  ITrigger_sptr trigger = TriggerFactory::Create(args["trigger"].as<std::string>());
  if (!trigger)
  {
    logger->error("No trigger!");
    return 2;
  }

  /* Run trigger test */
  trigger->setHandler([&logger]() { logger->info("Triggered."); });
  trigger->enable();
  while (true)
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

  logger->info("Exiting.");
  trigger->disable();
  return 0;
}
