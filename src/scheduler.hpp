#ifndef SCHEDULER_SCHEDULER_HPP
#define SCHEDULER_SCHEDULER_HPP

#include <boost/unordered_map.hpp>
#include <boost/graph/directed_graph.hpp>

class Scheduler {
    friend std::ostream& operator<<(std::ostream& output, const Scheduler& scheduler);


public:
    Scheduler();
    Scheduler(const std::string& graph, const std::string& timing, const std::string& constraints);
    [[nodiscard]] std::string printOperations() const;
    [[nodiscard]] std::string printTiming() const;
    [[nodiscard]] std::string printConstraints() const;

private:
    static void parseGraph(const std::string& file, boost::directed_graph<>& graph, boost::unordered_map<int, std::string>& map);
    static void parseResource(const std::string& file, boost::unordered_map<std::string, int>& map);

    boost::directed_graph<> dependencyGraph;
    boost::unordered_map<int, std::string> operationMap;
    boost::unordered_map<std::string, int> timingMap;
    boost::unordered_map<std::string, int> constraintsMap;
};

#endif //SCHEDULER_SCHEDULER_HPP
