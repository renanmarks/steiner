#include <iostream>
#include <fstream>
#include "data.h"
#include "graph.h"
#include "thomsonconstrutive.h"
#include "modthomsonconstrutive.h"
#include "edgeorientedlocalsearch.h"
#include "grasp.h"
#include "tabusearch.h"

st::Graph runConstrutive(const std::string& option, bool isRandom, const st::Data& data)
{
    if (option != "modthomsom-random")
    {
        return st::ThomsonConstrutive(data, isRandom).run();
    }

    return st::ModThomsonConstrutive(data).run();
}

st::Graph runLocalSearch(const std::string& option, bool isRandom, const st::Data& data)
{
    st::Graph tree;

    if (option != "modthomsom-random")
    {
        tree = st::ThomsonConstrutive(data, isRandom).run();
    }
    else
    {
        tree = st::ModThomsonConstrutive(data).run();
    }

    return st::EdgeOrientedLocalSearch(tree).run();
}

st::Graph runGrasp(const std::string& option, bool isRandom, const st::Data& data)
{
    const std::int32_t numberIterations = 50;

    if (option != "modthomsom-random")
    {
        st::ThomsonConstrutive c(data, isRandom);
        return st::Grasp(c, numberIterations).run();
    }

    st::ModThomsonConstrutive c(data);
    return st::Grasp(c, numberIterations).run();
}

st::Graph runTabu(const std::string& option, bool isRandom, const st::Data& data)
{
    const std::int32_t numberIterations = 50;
    const std::int32_t tabuListSize = 10;
    const std::int32_t incumbentCost = 20;

    if (option != "modthomsom-random")
    {
        st::ThomsonConstrutive c(data, isRandom);
        return st::TabuSearch(c, numberIterations, tabuListSize, incumbentCost).run();
    }

    st::ModThomsonConstrutive c(data);
    return st::TabuSearch(c, numberIterations, tabuListSize, incumbentCost).run();
}

void printHelp(std::string executableName)
{
    std::cout << "Usage:\n"
              << "\t" << executableName << " [option] [heuristic] filename\n"
              << "Options:\n"
              << "\t--help                 : Prints this help message.\n"
              << "\t--only-constructive    : Runs only construtive heuristic.\n"
              << "\t--localsearch          : Runs the constructive heuristic with edge transformation local search.\n"
              << "\t--grasp                : Runs the constructive heuristic with edge transformation local search using GRASP metaheuristic.\n"
              << "\t--tabu                 : Runs the constructive heuristic with edge transformation local search using Tabu Search metaheuristic.\n"
              << "Heuristics:\n"
              << "\t  thomsom                  Thomsom (Kruskal based) Construtive Heuristic\n"
              << "\t  thomsom-random           Thomsom (Kruskal based with random start) Construtive Heuristic\n"
              << "\t  modthomsom-random        Thomsom (Prim based with random start) Construtive Heuristic\n"
              << std::endl;
}

int main(int argc, char* argv[])
{
    if (argc <= 3)
    {
        std::cout << "Error: Please inform options and filename." << std::endl;
        TODO: printHelp(argv[0]);
        return 1;
    }

    std::string option = argv[1];

    if (option == "--help")
    {
        printHelp(argv[0]);
        return 0;
    }

    std::string filename = argv[3];

    st::Data data;
    std::ifstream file(filename);

    if (!file.is_open())
    {
        std::cout << "Error: cannot open file." << std::endl;
        printHelp(argv[0]);
        return 0;
    }

    data.load(file);

    std::string algorithm = argv[2];
    const bool isRandom = algorithm.find("random") != std::string::npos;

    if (option == "--only-constructive")
    {
        st::Graph tree = runConstrutive(algorithm, isRandom, data);
        std::cout << tree.getDistance() << std::endl;
        tree.printGraphviz(data.comment.name + "_c.dot");
        return 0;
    }

    if (option == "--localsearch")
    {
        st::Graph tree = runLocalSearch(algorithm, isRandom, data);
        std::cout << tree.getDistance() << std::endl;
        tree.printGraphviz(data.comment.name + "_ls.dot");
        return 0;
    }

    if (option == "--grasp")
    {
        st::Graph tree = runGrasp(algorithm, isRandom, data);
        std::cout << tree.getDistance() << std::endl;
        tree.printGraphviz(data.comment.name + "_g.dot");
        return 0;
    }

    if (option == "--tabu")
    {
        st::Graph tree = runTabu(algorithm, isRandom, data);
        std::cout << tree.getDistance() << std::endl;
        tree.printGraphviz(data.comment.name + "_t.dot");
        return 0;
    }

    return 0;
}
