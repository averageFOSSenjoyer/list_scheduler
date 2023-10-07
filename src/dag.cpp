#include "dag.hpp"

DAG::DAG() = default;

void DAG::addVertex(int node) {
    g[node] = boost::unordered_set<int>();
}

void DAG::addEdge(int from, int to) {
    g[from].insert(to);
}

boost::unordered_set<int> DAG::getVertices() {
    boost::unordered_set<int> ret;

    for (const auto& v : g) {
        ret.insert(v.first);
    }

    return ret;
}

boost::unordered_set<boost::tuple<int, int>> DAG::getEdges() {
    boost::unordered_set<boost::tuple<int, int>> ret;

    for (const auto& v : g) {
        for (const auto& e : v.second) {
            ret.insert(boost::tuple<int, int>(v.first, e));
        }
    }

    return ret;
}

boost::unordered_set<int> DAG::getEdges(int node) {
    return g[node];
}

void DAG::makeDot(std::ofstream of) {
    LabeledGraph boostGraph;

    boost::unordered_set<int> vertices = getVertices();
    for (const auto& v : vertices) {
        boost::add_vertex(v, boostGraph);
    }

    boost::unordered_set<boost::tuple<int, int>> edges = getEdges();
    for (const auto& e : edges) {
        boost::add_edge(
                boost::vertex_by_label(e.get<0>(), boostGraph),
                boost::vertex_by_label(e.get<1>(), boostGraph),
                boostGraph);
    }

    boost::write_graphviz(of, boostGraph);
}

