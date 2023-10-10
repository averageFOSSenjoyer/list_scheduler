#ifndef SCHEDULER_SCHEDULER_HPP
#define SCHEDULER_SCHEDULER_HPP

#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/unordered_map.hpp>
#include <boost/container/map.hpp>
#include <boost/container/set.hpp>
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
    void printSchedule(const boost::container::map<int, int>& schedule, std::ofstream);
    bool areAllScheduled(const boost::unordered_set<int>& nodes, const boost::container::map<int, int> &schedule);
    int earliestSchedule(const boost::unordered_set<int>& parents, const boost::container::map<int, int> &schedule);
    int latestSchedule(int self, const boost::unordered_set<int>& children, const boost::container::map<int, int> &schedule);
    boost::tuple<int, std::vector<int>> findCriticalPathHelper(const boost::tuple<int, std::vector<int>>& path);
    void findCriticalPath();
    boost::container::map<int, int> findASAP();
    boost::container::map<int, int> findALAP();
    boost::container::map<int, int> findSlack(boost::container::map<int, int> asapSchedule, boost::container::map<int, int> alapSchedule);
    void printSlack(boost::container::map<int, int> map1, std::basic_ofstream<char> ofstream);
    boost::container::map<int, boost::tuple<int, int, int>>
    findListSchedule(boost::container::map<int, int>& slack);
    void
    printListSchedule(const boost::container::map<int, boost::tuple<int, int, int>> &listSchedule, std::ofstream of);
    boost::unordered_map<std::string, boost::unordered_set<int>>
    hasResourceAndNode(const boost::unordered_map<std::string, int>& resources,
                                  boost::container::map<int, int>& ready,
                                  boost::container::map<int, int>& running);

    DAG<int> dependencyGraph;
    std::vector<int> criticalPath;
    boost::unordered_map<int, std::string> operationMap;
    boost::unordered_map<std::string, int> timingMap;
    boost::unordered_map<std::string, int> constraintsMap;
};

#endif //SCHEDULER_SCHEDULER_HPP
