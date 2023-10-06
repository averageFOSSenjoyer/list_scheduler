#include <iostream>
#include <fstream>
#include <format>
#include <boost/algorithm/string.hpp>
#include "scheduler.hpp"

Scheduler::Scheduler() = default;

Scheduler::Scheduler(const std::string &graph, const std::string &timing, const std::string &constraints) {
    parseGraph(graph, this->dependencyGraph, this->operationMap);
    parseResource(timing, this->timingMap);
    parseResource(constraints, this->constraintsMap);
}

void Scheduler::parseGraph(const std::string &file, boost::directed_graph<> &graph, boost::unordered_map<int, std::string> &map) {
    std::ifstream stream(file);
    std::string line;
    int totalNodes;

    std::getline(stream, line);
    totalNodes = std::stoi(line);

    while(std::getline(stream, line)) {
        std::vector<std::string> vec;
        std::stringstream ss(line);
        std::string str;
        while(std::getline(ss, str, ',')) {
            vec.push_back(str);
        }

        if (vec.size() != 3) {
            throw std::invalid_argument(std::format("Invalid format at line: \"{}\"", line));
        }

        boost::erase_all(vec[0], ",");
        boost::erase_all(vec[2], " ");
        map[std::stoi(vec[0])] = vec[2];
    }

    if (map.size() != totalNodes) {
        throw std::invalid_argument(std::format("Potentially corrupt input \"{}\"", file));
    }
}

void Scheduler::parseResource(const std::string &file, boost::unordered_map<std::string, int> &map) {
    std::ifstream stream(file);
    std::string resource;
    int num;

    while (stream >> resource >> num) {
        map[resource] = num;
    }
}

std::string Scheduler::printOperations() const {
    std::stringstream ss;
    ss << "Operations:" << std::endl;
    for (const auto& pair : operationMap) {
        ss << pair.first << " " << pair.second << std::endl;
    }

    return ss.str();
}

std::string Scheduler::printTiming() const {
    std::stringstream ss;
    ss << "Timing:" << std::endl;
    for(const auto& pair : timingMap) {
        ss << pair.first << " " << pair.second << std::endl;
    }

    return ss.str();
}

std::string Scheduler::printConstraints() const {
    std::stringstream ss;
    ss << "Constraints:" << std::endl;
    for(const auto& pair : constraintsMap) {
        ss << pair.first << " " << pair.second << std::endl;
    }

    return ss.str();
}

std::ostream& operator<<(std::ostream &output, const Scheduler& scheduler) {
    output
        << scheduler.printOperations()
        << scheduler.printTiming()
        <<scheduler.printConstraints();
    return output;
}
