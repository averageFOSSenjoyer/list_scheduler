#include <iostream>
#include <format>
#include "scheduler.hpp"

std::ofstream Scheduler::logger;

Scheduler::Scheduler() = default;

Scheduler::Scheduler(const std::string &graph, const std::string &timing, const std::string &constraints) {
    Scheduler::logger = std::ofstream("log.txt");
    parseGraph(graph, this->dependencyGraph, this->operationMap);
    parseResource(timing, this->timingMap);
    parseResource(constraints, this->constraintsMap);
}

int Scheduler::getNumNodes(const std::string &file) {
    std::ifstream stream(file);
    std::string line;
    std::getline(stream, line);
    return std::stoi(line);
}

void Scheduler::getText(const std::string &file, std::vector<std::vector<std::string>>& text) {
    std::ifstream stream(file);
    std::string line;
    std::getline(stream, line);
    for (auto & vec : text) {
        std::getline(stream, line);
        std::stringstream ss(line);
        std::string str;

        while(std::getline(ss, str, ',')) {
            vec.push_back(str);
        }

        boost::erase_all(vec[0], ",");
        boost::erase_all(vec[1], "[");
        boost::erase_all(vec[1], "]");
        boost::erase_all(vec[1], ",");
        boost::erase_all(vec[2], " ");
        boost::erase_all(vec[2], "\r");
    }
}

void Scheduler::buildOperationMap(const std::vector<std::vector<std::string>>& text, boost::unordered_map<int, std::string> &map) {
    log("Building operation map...");
    for (const auto& t : text) {
        map[std::stoi(t[0])] = t[2];
    }
}

void Scheduler::buildDependencyGraph(const std::vector<std::vector<std::string>>& text, LabeledGraph& graph) {
    log("Building dependency graph...");
    for (const auto& t : text) {
        boost::add_vertex(std::stoi(t[0]), graph);
    }

    std::stringstream ss;
    int child;
    for (const auto& t : text) {
        ss = std::stringstream(t[1]);
        while(ss >> child) {
            boost::add_edge(
                    boost::vertex_by_label(std::stoi(t[0]), graph),
                    boost::vertex_by_label(child, graph),
                    graph);
        }
    }

    log("Creating dot file...");
    makeDot(std::ofstream("graph.dot"));
}

void Scheduler::parseGraph(const std::string &file, LabeledGraph &graph, boost::unordered_map<int, std::string> &map) {
    log(std::format("Reading {}", file));

    int totalNodes = getNumNodes(file);
    log(std::format("Total Nodes: {}", totalNodes));

    std::vector<std::vector<std::string>> text(totalNodes);
    getText(file, text);

    buildOperationMap(text, map);

    buildDependencyGraph(text, graph);
}

void Scheduler::parseResource(const std::string &file, boost::unordered_map<std::string, int> &map) {
    log(std::format("Reading {}", file));
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

void Scheduler::log(const std::string &msg) {
    logger << msg << std::endl;
}

void Scheduler::makeDot(std::ofstream ofstream) {
    boost::write_graphviz(ofstream, dependencyGraph);
}
