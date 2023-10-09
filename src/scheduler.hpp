#ifndef SCHEDULER_SCHEDULER_HPP
#define SCHEDULER_SCHEDULER_HPP

#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/unordered_map.hpp>
#include "dag.hpp"

class Scheduler {
    friend std::ostream& operator<<(std::ostream& output, const Scheduler& scheduler);

public:
    Scheduler();
    Scheduler(const std::string& graph, const std::string& timing, const std::string& constraints);

    [[nodiscard]] std::string printOperations() const;
    [[nodiscard]] std::string printTiming() const;
    [[nodiscard]] std::string printConstraints() const;

private:
    static void parseGraph(const std::string& file, DAG<int>& graph, boost::unordered_map<int, std::string>& map);
    static void buildDependencyGraph(const std::vector<std::vector<std::string>> &text, DAG<int> &graph);
    static void log(const std::string& msg);
    static void parseResource(const std::string& file, boost::unordered_map<std::string, int>& map);
    static void buildOperationMap(const std::vector<std::vector<std::string>> &text, boost::unordered_map<int, std::string> &map);
    static int getNumNodes(const std::string &file);
    static void getText(const std::string &file, std::vector<std::vector<std::string>> &text);
    boost::tuple<int, std::vector<int>> findCriticalPathHelper(const boost::tuple<int, std::vector<int>>& path);
    void findCriticalPath();


    static std::ofstream logger;
    DAG<int> dependencyGraph;
    std::vector<int> criticalPath;
    boost::unordered_map<int, std::string> operationMap;
    boost::unordered_map<std::string, int> timingMap;
    boost::unordered_map<std::string, int> constraintsMap;


};

#endif //SCHEDULER_SCHEDULER_HPP
