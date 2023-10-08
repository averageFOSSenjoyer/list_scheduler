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

template <class LABEL>
class DAG {
    typedef boost::adjacency_list<boost::listS, boost::vecS, boost::directedS> Graph;
    typedef boost::labeled_graph<Graph, LABEL> LabeledGraph;

public:
    DAG() = default;

    void addVertex(LABEL node) {
        g[node] = boost::unordered_set<LABEL>();
    }

    void addEdge(LABEL from, LABEL to) {
        g[from].insert(to);
    }

    boost::unordered_set<LABEL> getVertices() {
        boost::unordered_set<LABEL> ret;

        for (const auto& v : g) {
            ret.insert(v.first);
        }

        return ret;
    }

    boost::unordered_set<boost::tuple<LABEL, LABEL>> getEdges() {
        boost::unordered_set<boost::tuple<LABEL, LABEL>> ret;

        for (const auto& v : g) {
            for (const auto& e : v.second) {
                ret.insert(boost::tuple<LABEL, LABEL>(v.first, e));
            }
        }

        return ret;
    }

    boost::unordered_set<LABEL> getEdges(LABEL node) {
        return g[node];
    }

    void makeDot(std::ofstream of) {
        LabeledGraph boostGraph;

        boost::unordered_set<LABEL> vertices = getVertices();
        for (const auto& v : vertices) {
            boost::add_vertex(v, boostGraph);
        }

        boost::unordered_set<boost::tuple<LABEL, LABEL>> edges = getEdges();
        for (const auto& e : edges) {
            boost::add_edge(
                    boost::vertex_by_label(boost::get<0>(e), boostGraph),
                    boost::vertex_by_label(boost::get<1>(e), boostGraph),
                    boostGraph);
        }

        boost::write_graphviz(of, boostGraph);
    }

private:
    boost::unordered_map<int, boost::unordered_set<int>> g;
};

#endif //SCHEDULER_DAG_HPP
