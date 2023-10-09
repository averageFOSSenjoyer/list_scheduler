#ifndef SCHEDULER_PARSER_HPP
#define SCHEDULER_PARSER_HPP

#include <string>
#include <vector>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/unordered_map.hpp>
#include "dag.hpp"

class Parser {
public:
    static boost::unordered_map<int, std::string> buildOperationMap(const std::vector<std::vector<std::string>>& text) {
        boost::unordered_map<int, std::string> operationMap;

        for (const auto& t : text) {
            operationMap[std::stoi(t[0])] = t[2];
        }

        return operationMap;
    }

    static DAG<int> buildDependencyGraph(const std::vector<std::vector<std::string>>& text) {
        DAG<int> dependencyGraph;

        for (const auto& t : text) {
            dependencyGraph.addVertex(std::stoi(t[0]));
        }

        std::stringstream ss;
        int child;
        for (const auto& t : text) {
            ss = std::stringstream(t[1]);
            while(ss >> child) {
                dependencyGraph.addEdge(std::stoi(t[0]), child);
            }
        }

        return dependencyGraph;
    }


    static std::vector<std::vector<std::string>> parseGraphText(const std::string &file) {
        return getText(file, getNumNodes(file));
    }

    static boost::unordered_map<std::string, int> parseTiming(const std::string &file) {
        boost::unordered_map<std::string, int> timingMap;

        std::ifstream stream(file);
        std::string resource;
        int num;

        while (stream >> resource >> num) {
            timingMap[resource] = num;
        }

        return timingMap;
    }

    static boost::unordered_map<std::string, int> parseConstraints(const std::string &file) {
        boost::unordered_map<std::string, int> constraintsMap;

        std::ifstream stream(file);
        std::string resource;
        int num;

        while (stream >> resource >> num) {
            constraintsMap[resource] = num;
        }

        return constraintsMap;
    }

private:
    static int getNumNodes(const std::string &file) {
        std::ifstream stream(file);
        std::string line;
        std::getline(stream, line);
        return std::stoi(line);
    }

    static std::vector<std::vector<std::string>> getText(const std::string &file, int totalNodes) {
        std::vector<std::vector<std::string>> text(totalNodes);

        std::ifstream stream(file);
        std::string line;
        std::getline(stream, line);
        for (auto & vec : text) {
            std::getline(stream, line);
            std::stringstream ss(line);
            std::string str;

            while(std::getline(ss, str, ',')) {
                vec.push_back(str);
            }

            boost::erase_all(vec[0], ",");
            boost::erase_all(vec[1], "[");
            boost::erase_all(vec[1], "]");
            boost::erase_all(vec[1], ",");
            boost::erase_all(vec[2], " ");
            boost::erase_all(vec[2], "\r");
        }

        return text;
    }
};

#endif //SCHEDULER_PARSER_HPP
