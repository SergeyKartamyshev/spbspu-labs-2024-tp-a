#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <limits>
#include <map>
#include <string>
#include <vector>
#include "polygonCommands.hpp"

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    std::cerr << "Required filename argument\n";
    return 1;
  }
  std::ifstream file(argv[1]);
  if (!file.is_open())
  {
    std::cerr << "File cannot be opened\n";
    return 2;
  }
  using namespace kravchenko;

  std::vector< Polygon > polygons;
  using inputIt = std::istream_iterator< Polygon >;
  while (!file.eof())
  {
    std::copy(inputIt{ file }, inputIt{}, std::back_inserter(polygons));
    file.clear();
    file.ignore(std::numeric_limits< std::streamsize >::max(), '\n');
  }

  std::map< std::string, std::function< void(CmdStreams) > > cmds;
  {
    using namespace std::placeholders;
    cmds["AREA"] = std::bind(Area{}, std::cref(polygons), _1);
    cmds["MIN"] = std::bind(MinMax{}, std::cref(polygons), _1, true);
    cmds["MAX"] = std::bind(MinMax{}, std::cref(polygons), _1, false);
    cmds["COUNT"] = std::bind(Count{}, std::cref(polygons), _1);
    cmds["RMECHO"] = std::bind(RmEcho{}, std::ref(polygons), _1);
    cmds["RIGHTSHAPES"] = std::bind(RightShapes{}, std::cref(polygons), _1);
  }

  std::string command;
  std::cin >> command;
  while (!std::cin.eof())
  {
    try
    {
      cmds.at(command)(CmdStreams{ std::cin, std::cout });
      std::cout << '\n';
    }
    catch (const std::out_of_range&)
    {
      std::cout << "<INVALID COMMAND>" << '\n';
    }
    catch (const InvalidCommand& e)
    {
      std::cout << e.what() << '\n';
    }
    std::cin.clear();
    std::cin.ignore(std::numeric_limits< std::streamsize >::max(), '\n');
    std::cin >> command;
  }

  file.close();
  return 0;
}
