#ifndef SCHEDULER_SCHEDULER_HPP
#define SCHEDULER_SCHEDULER_HPP

#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/unordered_map.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/labeled_graph.hpp>
#include <boost/graph/graphviz.hpp>

class Scheduler {
    typedef boost::adjacency_list<boost::listS, boost::vecS, boost::directedS> Graph;
    typedef boost::labeled_graph<Graph, int> LabeledGraph;

    friend std::ostream& operator<<(std::ostream& output, const Scheduler& scheduler);

public:
    Scheduler();
    Scheduler(const std::string& graph, const std::string& timing, const std::string& constraints);

    [[nodiscard]] std::string printOperations() const;
    [[nodiscard]] std::string printTiming() const;
    [[nodiscard]] std::string printConstraints() const;

private:
    void parseGraph(const std::string& file, LabeledGraph& graph, boost::unordered_map<int, std::string>& map);
    void buildDependencyGraph(const std::vector<std::vector<std::string>> &text, LabeledGraph &graph);
    void makeDot(std::ofstream ofstream);
    static void log(const std::string& msg);
    static void parseResource(const std::string& file, boost::unordered_map<std::string, int>& map);
    static void buildOperationMap(const std::vector<std::vector<std::string>> &text, boost::unordered_map<int, std::string> &map);
    static int getNumNodes(const std::string &file);
    static void getText(const std::string &file, std::vector<std::vector<std::string>> &text);


    static std::ofstream logger;
    LabeledGraph dependencyGraph;
    boost::unordered_map<int, std::string> operationMap;
    boost::unordered_map<std::string, int> timingMap;
    boost::unordered_map<std::string, int> constraintsMap;
};

#endif //SCHEDULER_SCHEDULER_HPP
