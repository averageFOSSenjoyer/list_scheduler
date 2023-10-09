#include <iostream>
#include <format>
#include "scheduler.hpp"

Scheduler::Scheduler() = default;

Scheduler::Scheduler(const std::string &graph, const std::string &timing, const std::string &constraints) {
    auto graphText = Parser::parseGraphText(graph);
    dependencyGraph = Parser::buildDependencyGraph(graphText);
    operationMap = Parser::buildOperationMap(graphText);
    timingMap = Parser::parseTiming(timing);
    constraintsMap = Parser::parseConstraints(constraints);
}

boost::tuple<int, std::vector<int>> Scheduler::findCriticalPathHelper(const boost::tuple<int, std::vector<int>>& path) {
    boost::unordered_set<int> children = dependencyGraph.getChildren(path.get<1>().back());

    if (children.empty()) {
        return path;
    }

    boost::tuple<int, std::vector<int>> ret;
    int criticalLength = 0;
    for (const auto& v : children) {
        auto potPath = path;
        potPath.get<0>() += timingMap[operationMap[v]];
        potPath.get<1>().push_back(v);
        auto nextPath = findCriticalPathHelper(potPath);
        if (nextPath.get<0>() > criticalLength) {
            ret = nextPath;
            criticalLength = nextPath.get<0>();
        }
    }

    return ret;
}

int Scheduler::getCriticalPathLength() {
    int length = 0;

    for (const auto& node : criticalPath) {
        length += timingMap[operationMap[node]];
    }

    return length;
}

void Scheduler::findCriticalPath() {
    auto startingVertices = dependencyGraph.getStartingVertices();

    int criticalLength = 0;
    for (const auto& v : startingVertices) {
        auto nextPath = findCriticalPathHelper(boost::tuple<int, std::vector<int>>(timingMap[operationMap[v]], {v}));
        if (nextPath.get<0>() > criticalLength) {
            criticalLength = nextPath.get<0>();
            criticalPath = nextPath.get<1>();
        }
    }
}

void Scheduler::makeDot() {
    dependencyGraph.makeDot(std::ofstream("graph.dot"));
}

void Scheduler::exec() {
    findCriticalPath();
}