#ifndef SCHEDULER_SCHEDULER_HPP
#define SCHEDULER_SCHEDULER_HPP

#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/unordered_map.hpp>
#include "dag.hpp"
#include "parser.hpp"

class Scheduler {
public:
    Scheduler();
    Scheduler(const std::string& graph, const std::string& timing, const std::string& constraints);
    void exec();
    void makeDot();
    int getCriticalPathLength();

private:
    boost::tuple<int, std::vector<int>> findCriticalPathHelper(const boost::tuple<int, std::vector<int>>& path);
    void findCriticalPath();

    DAG<int> dependencyGraph;
    std::vector<int> criticalPath;
    boost::unordered_map<int, std::string> operationMap;
    boost::unordered_map<std::string, int> timingMap;
    boost::unordered_map<std::string, int> constraintsMap;
};

#endif //SCHEDULER_SCHEDULER_HPP
