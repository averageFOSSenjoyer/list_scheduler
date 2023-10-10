#include <iostream>
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
        auto nextPath
        = findCriticalPathHelper(boost::tuple<int, std::vector<int>>(timingMap[operationMap[v]], {v}));
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

    boost::container::map<int, int> asapSchedule = findASAP();
    boost::container::map<int, int> alapSchedule = findALAP();
    boost::container::map<int, int> slack = findSlack(asapSchedule, alapSchedule);
    boost::container::map<int, boost::tuple<int, int, int>> listSchedule = findListSchedule(slack);

    printSchedule(asapSchedule, std::ofstream("asap.txt"));
    printSchedule(alapSchedule, std::ofstream("alap.txt"));
    printSlack(slack, std::ofstream("slack.txt"));
    printListSchedule(listSchedule, std::ofstream("list_scheduling.txt"));
}

bool Scheduler::areAllScheduled(const boost::unordered_set<int>& nodes,
                                const boost::container::map<int, int>& schedule) {
    for (auto node : nodes) {
        if (schedule.find(node)->second == -1) {
            return false;
        }
    }

    return true;
}

int Scheduler::earliestSchedule(const boost::unordered_set<int>& parents,
                                const boost::container::map<int, int>& schedule) {
    int max = 0;

    for (auto node : parents) {
        int nodeTiming = timingMap[operationMap[node]];
        if (schedule.find(node)->second + nodeTiming > max) {
            max = schedule.find(node)->second + nodeTiming;
        }
    }

    return max;
}

int Scheduler::latestSchedule(int self, const boost::unordered_set<int>& children,
                              const boost::container::map<int, int>& schedule) {
    int min = getCriticalPathLength();
    int nodeTiming = timingMap[operationMap[self]];

    for (auto node : children) {
        if (schedule.find(node)->second - nodeTiming < min) {
            min = schedule.find(node)->second - nodeTiming;
        }
    }

    return min;
}

boost::container::map<int, int> Scheduler::findASAP() {
    boost::unordered_set<int> workVertices = dependencyGraph.getVertices();
    boost::unordered_set<int> scheduled;
    boost::container::map<int, int> schedule;

    for (const auto& v : workVertices) {
        auto parents = dependencyGraph.getParents(v);
        if (parents.empty()) {
            schedule[v] = 0;
            scheduled.insert(v);
        } else {
            schedule[v] = -1;
        }
    }

    for (const auto& v : scheduled) {
        workVertices.erase(v);
    }

    while (!workVertices.empty()) {
        for (const auto& v : workVertices) {
            auto parents = dependencyGraph.getParents(v);
            if (areAllScheduled(parents, schedule)) {
                schedule[v] = earliestSchedule(parents, schedule);
                scheduled.insert(v);
            }
        }
        for (const auto& v : scheduled) {
            workVertices.erase(v);
        }
    }

    return schedule;
}

boost::container::map<int, int> Scheduler::findALAP() {
    boost::unordered_set<int> workVertices = dependencyGraph.getVertices();
    boost::unordered_set<int> scheduled;
    boost::container::map<int, int> schedule;

    for (const auto& v : workVertices) {
        auto children = dependencyGraph.getChildren(v);
        if (children.empty()) {
            schedule[v] = getCriticalPathLength() - timingMap[operationMap[v]];
            scheduled.insert(v);
        } else {
            schedule[v] = -1;
        }
    }

    for (const auto& v : scheduled) {
        workVertices.erase(v);
    }

    while (!workVertices.empty()) {
        for (const auto& v : workVertices) {
            auto children = dependencyGraph.getChildren(v);
            if (areAllScheduled(children, schedule)) {
                schedule[v] = latestSchedule(v, children, schedule);
                scheduled.insert(v);
            }
        }
        for (const auto& v : scheduled) {
            workVertices.erase(v);
        }
    }

    return schedule;
}

void Scheduler::printSchedule(const boost::container::map<int, int>& schedule, std::ofstream of) {
    for (auto [node, time] : schedule) {
        of << "Node " << node << ": t=" << time << std::endl;
    }

    of << "Finished t=" << getCriticalPathLength() << std::endl;
}

boost::container::map<int, int>
        Scheduler::findSlack(boost::container::map<int, int> asapSchedule
                             , boost::container::map<int, int> alapSchedule) {
    boost::container::map<int, int> slack;

    for (auto [k, v] : asapSchedule) {
        slack[k] = alapSchedule[k] - asapSchedule[k];
    }

    return slack;
}

void Scheduler::printSlack(boost::container::map<int, int> slack, std::ofstream of) {
    for (auto [node, time] : slack) {
        of << "Node " << node << ": slack=" << time << std::endl;
    }
}

boost::unordered_map<std::string, boost::unordered_set<int>>
Scheduler::hasResourceAndNode(const boost::unordered_map<std::string, int>& resources,
                                  boost::container::map<int, int>& ready,
                                  boost::container::map<int, int>& running) {
    boost::unordered_set<int> availableNode;
    boost::unordered_set<std::string> availableRes;
    boost::unordered_map<std::string, boost::unordered_set<int>> result;

    for (auto [res, num] : resources) {
        if (num > 0) {
            availableRes.insert(res);
        }
    }
    if (availableRes.empty()) {
        return {};
    }

    for (auto [node, v] : ready) {
        if (v != -1 && running[node] == -1) {
            availableNode.insert(node);
        }
    }

    for (auto node : availableNode) {
        if (availableRes.contains(operationMap[node])) {
            if (result.contains(operationMap[node])) {
                result[operationMap[node]].insert(node);
            } else {
                result[operationMap[node]] = {node};
            }
        }
    }

    return result;
}

boost::container::map<int, boost::tuple<int, int, int>>
Scheduler::findListSchedule(boost::container::map<int, int>& slack) {
    boost::unordered_set<int> workVertices = dependencyGraph.getVertices();
    boost::unordered_map<std::string, int> resources = constraintsMap;
    boost::container::map<int, boost::tuple<int, int, int>> listSchedule;
    boost::container::map<int, int> ready;
    boost::container::map<int, int> running;
    boost::container::map<int, int> finished;
    boost::unordered_set<int> scheduled;
    int time = 0;

    for (auto node : workVertices) {
        ready[node] = -1;
        running[node] = -1;
        finished[node] = -1;
    }

    while (!workVertices.empty()) {
        for (auto node : workVertices) {
            auto parents = dependencyGraph.getParents(node);
            if (areAllScheduled(parents, finished) && ready[node] == -1) {
                ready[node] = time;
            }
        }

        auto possibleNodes = hasResourceAndNode(resources, ready, running);
        while (!possibleNodes.empty()) {
            for (auto [res, nodes] : possibleNodes) {
                int lowestSlack = INT_MAX;
                int lowestNode;
                for (auto node : nodes) {
                    if (slack[node] < lowestSlack) {
                        lowestSlack = slack[node];
                        lowestNode = node;
                    }
                }
                running[lowestNode] = time;
                resources[res] -= 1;
                scheduled.insert(lowestNode);
            }

            possibleNodes = hasResourceAndNode(resources, ready, running);
        }

        for (auto node : workVertices) {
            if (running[node] != -1 && running[node] + timingMap[operationMap[node]] - 1 == time) {
                finished[node] = time;
                resources[operationMap[node]] += 1;
            }
        }

        for (auto [k, v] : finished) {
            if (v != -1) {
                workVertices.erase(k);
            }
        }

        time++;
    }

    for (auto node : scheduled) {
        listSchedule[node]
        = boost::tuple<int, int, int>(ready[node], running[node], finished[node]);

        std::cout << node << ' ' << ready[node] << ' ' << running[node] << ' ' << finished[node] << std::endl;
    }

    return listSchedule;
}

void Scheduler::printListSchedule(const boost::container::map<int, boost::tuple<int, int, int>>& listSchedule,
                                  std::ofstream of) {
    for (const auto& [k, v] : listSchedule) {
        of << "Node " << k << ": Ready t=" << v.get<0>()
                << "; Running t=" << v.get<1>() << "; Finished t=" << v.get<2>() << std::endl;
    }

    of << "Finished t=" << std::endl;
}