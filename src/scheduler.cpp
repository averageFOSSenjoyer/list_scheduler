#include <iostream>
#include <numeric>
#include "scheduler.hpp"

Scheduler::Scheduler() = default;

Scheduler::Scheduler(const std::string &graph, const std::string &timing, const std::string &constraints) {
    auto graphText = Parser::parseGraphText(graph);
    dependencyGraph = Parser::buildDependencyGraph(graphText);
    operationMap = Parser::buildOperationMap(graphText);
    timingMap = Parser::parseTiming(timing);
    constraintsMap = Parser::parseConstraints(constraints);
}

int Scheduler::getNodeTiming(int node) const {
    return timingMap.at(operationMap.at(node));
}

int Scheduler::getCriticalPathLength() const {
    return std::accumulate(criticalPath.begin(), criticalPath.end(), 0,
                           [this](int a, int b) {
        return a + getNodeTiming(b);
    });
}

boost::tuple<int, std::vector<int>> Scheduler::findCriticalPathHelper(const boost::tuple<int, std::vector<int>>& path) const {
    boost::unordered_set<int> children = dependencyGraph.getChildren(path.get<1>().back());

    if (children.empty()) {
        return path;
    }

    boost::tuple<int, std::vector<int>> ret;
    int criticalLength = 0;
    for (const auto& v : children) {
        auto potPath = path;
        potPath.get<0>() += getNodeTiming(v);
        potPath.get<1>().push_back(v);
        auto nextPath = findCriticalPathHelper(potPath);
        if (nextPath.get<0>() > criticalLength) {
            ret = nextPath;
            criticalLength = nextPath.get<0>();
        }
    }

    return ret;
}

void Scheduler::findCriticalPath() {
    auto startingVertices = dependencyGraph.getStartingVertices();

    int criticalLength = 0;
    for (const auto& v : startingVertices) {
        auto nextPath
        = findCriticalPathHelper(boost::tuple<int, std::vector<int>>(getNodeTiming(v), {v}));
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


    boost::promise<boost::container::map<int, int>> asapSchedulePromise;
    boost::promise<boost::container::map<int, int>> alapSchedulePromise;
    auto asapScheduleFuture = asapSchedulePromise.get_future();
    auto alapScheduleFuture = alapSchedulePromise.get_future();

    boost::thread asapTh([this, &asapSchedulePromise] { findASAP(boost::ref(asapSchedulePromise)); });
    boost::thread alapTh([this, &alapSchedulePromise] { findALAP(boost::ref(alapSchedulePromise)); });
    asapTh.join();
    alapTh.join();

    auto asapSchedule = asapScheduleFuture.get();
    auto alapSchedule = alapScheduleFuture.get();

    boost::container::map<int, int> slack = findSlack(asapSchedule, alapSchedule);
    boost::container::map<int, boost::tuple<int, int, int>> listSchedule = findListSchedule(slack);

    printSchedule(asapSchedule, std::ofstream("asap.txt"));
    printSchedule(alapSchedule, std::ofstream("alap.txt"));
    printSlack(slack, std::ofstream("slack.txt"));
    printListSchedule(listSchedule, std::ofstream("list_scheduling.txt"));
}

bool Scheduler::areAllScheduled(const boost::unordered_set<int>& nodes,
                                boost::container::map<int, int>& schedule) {
    return std::ranges::all_of(nodes, [&schedule](int node) {
        return schedule.at(node) != -1;
    });
}

bool Scheduler::areAllScheduled(const boost::unordered_set<int>& nodes,
                                boost::container::map<int, boost::tuple<int, int, int>>& listSchedule) {
    return std::ranges::all_of(nodes, [&listSchedule](int node) {
        return listSchedule.at(node).get<FINISHED>() != -1;
    });
}

int Scheduler::earliestSchedule(const boost::unordered_set<int>& parents,
                                const boost::container::map<int, int>& schedule) const {
    int maxNode = *(std::ranges::max_element(parents,
                                             [&schedule, this](const int a, const int b) {
        return schedule.find(a)->second + getNodeTiming(a) < schedule.find(b)->second + getNodeTiming(b);
    }));

    return schedule.find(maxNode)->second + getNodeTiming(maxNode);
}

int Scheduler::latestSchedule(int self, const boost::unordered_set<int>& children,
                              const boost::container::map<int, int>& schedule) const {
    int minNode =  *(std::ranges::min_element(children,
                                              [&schedule](const int a, const int b) {
        return schedule.find(a)->second < schedule.find(b)->second;
    }));

    return schedule.find(minNode)->second - getNodeTiming(self);
}

void Scheduler::findASAP(boost::promise<boost::container::map<int, int>>& asapSchedule) const {
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

    asapSchedule.set_value(schedule);
}

void Scheduler::findALAP(boost::promise<boost::container::map<int, int>>& alapSchedule) const {
    boost::unordered_set<int> workVertices = dependencyGraph.getVertices();
    boost::unordered_set<int> scheduled;
    boost::container::map<int, int> schedule;

    for (const auto& v : workVertices) {
        auto children = dependencyGraph.getChildren(v);
        if (children.empty()) {
            schedule[v] = getCriticalPathLength() - getNodeTiming(v);
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

    alapSchedule.set_value(schedule);
}

void Scheduler::printSchedule(const boost::container::map<int, int>& schedule, std::ofstream of) const {
    for (auto [node, time] : schedule) {
        of << "Node " << node << ": t=" << time << std::endl;
    }

    of << "Finished t=" << getCriticalPathLength() << std::endl;
}

boost::container::map<int, int>
Scheduler::findSlack(boost::container::map<int, int>& asapSchedule,
                     boost::container::map<int, int>& alapSchedule) {
    boost::container::map<int, int> slack;

    for (auto [k, v] : asapSchedule) {
        slack[k] = alapSchedule.at(k) - asapSchedule.at(k);
    }

    return slack;
}

void Scheduler::printSlack(boost::container::map<int, int>& slack, std::ofstream of) {
    for (auto [node, time] : slack) {
        of << "Node " << node << ": slack=" << time << std::endl;
    }
}

boost::unordered_map<std::string, boost::unordered_set<int>>
Scheduler::hasResourceAndNode(const boost::unordered_map<std::string, int>& resources,
                              boost::container::map<int, boost::tuple<int, int, int>>& listSchedule) const {
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

    for (auto [node, v] : listSchedule) {
        if (v.get<READY>() != -1 && v.get<RUNNING>() == -1) {
            availableNode.insert(node);
        }
    }

    for (auto node : availableNode) {
        if (availableRes.find(operationMap.at(node)) != availableRes.end()) {
            if (result.find(operationMap.at(node)) != result.end()) {
                result.at(operationMap.at(node)).insert(node);
            } else {
                result[operationMap.at(node)] = {node};
            }
        }
    }

    return result;
}

boost::container::map<int, boost::tuple<int, int, int>>
Scheduler::findListSchedule(boost::container::map<int, int>& slack) const {
    boost::unordered_set<int> workVertices = dependencyGraph.getVertices();
    boost::unordered_map<std::string, int> resources = constraintsMap;
    boost::container::map<int, boost::tuple<int, int, int>> listSchedule;
    boost::unordered_set<int> scheduled;
    int time = 0;

    for (auto node : workVertices) {
        listSchedule[node] = boost::tuple<int, int, int>(-1, -1, -1);
    }

    while (!workVertices.empty()) {
        for (auto node : workVertices) {
            auto parents = dependencyGraph.getParents(node);
            if (areAllScheduled(parents, listSchedule) && listSchedule.at(node).get<READY>() == -1) {
                listSchedule.at(node).get<READY>() = time;
            }
        }

        auto possibleNodes = hasResourceAndNode(resources, listSchedule);
        while (!possibleNodes.empty()) {
            for (auto [res, nodes] : possibleNodes) {
                int lowestNode = *std::ranges::min_element(nodes, [&slack](int a, int b) {
                    return slack.at(a) < slack.at(b);
                });
                listSchedule.at(lowestNode).get<RUNNING>() = time;
                resources.at(res) -= 1;
                scheduled.insert(lowestNode);
            }

            possibleNodes = hasResourceAndNode(resources, listSchedule);
        }

        for (auto node : workVertices) {
            if (listSchedule.at(node).get<RUNNING>() != -1
                    && listSchedule.at(node).get<RUNNING>() + getNodeTiming(node) - 1 == time) {
                listSchedule.at(node).get<FINISHED>() = time;
                resources.at(operationMap.at(node)) += 1;
            }
        }

        for (auto [k, v] : listSchedule) {
            if (v.get<FINISHED>() != -1) {
                workVertices.erase(k);
            }
        }

        time++;
    }

    return listSchedule;
}

void Scheduler::printListSchedule(const boost::container::map<int, boost::tuple<int, int, int>>& listSchedule,
                                  std::ofstream of) const {
    for (const auto& [k, v] : listSchedule) {
        of << "Node " << k << ": Ready t=" << v.get<READY>()
                << "; Running t=" << v.get<RUNNING>() << "; Finished t=" << v.get<FINISHED>() << std::endl;
    }

    int runtime = listSchedule.at(criticalPath.back()).get<FINISHED>() + 1;
    of << "Finished t=" <<  runtime << std::endl;
}