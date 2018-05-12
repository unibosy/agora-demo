#include <cstdint>
#include <cinttypes>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "base/log.h"
#include "base/opt_parser.h"
#include "robot.h"
#include "json_parser.h"
#include "base/atomic.h"

// using namespace std;
using namespace agora;
using namespace agora::base;
using namespace agora::pstn;

void print_usage() {
  const char *usage =
    "AgoraPremiumRobot --global_settings <string> --robot_settings <string>\n"
    "OPTIONS:\n"
    "  --global_settings      String which specify the global settings with json format\n"
    "  --robot_settings       string which specify the robot settings with json format\n";
  std::cerr << usage << std::endl;
}

int main(int argc, char *argv[]) {
  if(argc != 5 ||
  std::string("--global_settings") != std::string(argv[1]) ||
  std::string(argv[2]).empty() ||
  std::string("--robot_settings") != std::string(argv[3]) ||
  std::string(argv[4]).empty()
  )
  {
    std::cerr << "Illegal arguments\n";
    print_usage();
    return -1;
  }

  json_parser parameter_parser(argv[2], argv[4]);
  // robot AgoraPremiumRobot(parameter_parser.get_settings());
  robot AgoraPremiumRobot(json_parser(argv[2], argv[4]).get_settings());

  return AgoraPremiumRobot.run();
}

