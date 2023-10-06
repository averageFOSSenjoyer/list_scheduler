#include <iostream>
#include <filesystem>
#include <format>
#include "scheduler.hpp"
#include <boost/program_options.hpp>

namespace po = boost::program_options;
namespace fs = std::filesystem;

int validateParams(int argc, char* argv[], po::variables_map& vm) {
    std::string usage("Usage:\n   ./scheduler -g <graph file> -t <timing file> -c <constraints file>");

    po::options_description desc("Options");
    desc.add_options()
            ("graph,g", po::value<std::string>(), "Graph file - Topological dependency for scheduling")
            ("timing,t", po::value<std::string>(),"Timing file - Latency of operators")
            ("constraints,c", po::value<std::string>(),"Constraints file - Number of operators")
            ("help,h", "Prints the help menu");


    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help") || !vm.count("graph") || !vm.count("timing") || !vm.count("constraints")) {
        std::cout << desc << usage << std::endl;
        return 1;
    }

    std::string graphFP(vm["graph"].as<std::string>());
    std::string timingFP(vm["timing"].as<std::string>());
    std::string constraintsFP(vm["constraints"].as<std::string>());

    if (!fs::exists(graphFP)) {
        std::cout << std::format("Provided graph file \"{}\" does not exist\n", graphFP);
        return 1;
    }

    if (!fs::exists(timingFP)) {
        std::cout << std::format("Provided timing file \"{}\" does not exist\n", timingFP);
        return 1;
    }

    if (!fs::exists(constraintsFP)) {
        std::cout << std::format("Provided constraints file \"{}\" does not exist\n", constraintsFP);
        return 1;
    }

    return 0;
}

int main(int argc, char* argv[]) {
    po::variables_map vm;
    if (validateParams(argc, argv, vm)) {
        return 1;
    }

    Scheduler scheduler(vm["graph"].as<std::string>(), vm["timing"].as<std::string>(), vm["constraints"].as<std::string>());
    std::cout << scheduler;

    return 0;
}

