#ifndef SCHEDULER_DAG_HPP
#define SCHEDULER_DAG_HPP

#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/labeled_graph.hpp>
#include <boost/graph/graphviz.hpp>
#include <unordered_set>

class DAG {
    typedef boost::adjacency_list<boost::listS, boost::vecS, boost::directedS> Graph;
    typedef boost::labeled_graph<Graph, int> LabeledGraph;

public:
    DAG();
    void addVertex(int node);
    void addEdge(int from, int to);
    boost::unordered_set<int> getVertices();
    boost::unordered_set<boost::tuple<int, int>> getEdges();
    boost::unordered_set<int> getEdges(int node);
    void makeDot(std::ofstream of);

private:
    boost::unordered_map<int, boost::unordered_set<int>> g;
};

#endif //SCHEDULER_DAG_HPP
