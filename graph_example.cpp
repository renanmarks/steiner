//=======================================================================
// Copyright 2001 Jeremy G. Siek, Andrew Lumsdaine, Lie-Quan Lee,
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/kruskal_min_spanning_tree.hpp>
#include <iostream>
#include <fstream>

int
main()
{
    using namespace boost;
    using Graph = adjacency_list < vecS, vecS, undirectedS, no_property, property < edge_weight_t, int > >;
    using Edge = graph_traits < Graph >::edge_descriptor;
    using Vertex = graph_traits < Graph >::vertex_descriptor;
    using E = std::pair<int, int>;

    const int num_nodes = 5;

    E edge_array[] = { E(0, 2), E(1, 3), E(1, 4), E(2, 1), E(2, 3), E(3, 4), E(4, 0), E(4, 1) };

    int weights[] = { 1, 1, 2, 7, 3, 1, 1, 1 };

    std::size_t num_edges = sizeof(edge_array) / sizeof(E);

    Graph g(edge_array, edge_array + num_edges, weights, num_nodes);

    property_map < Graph, edge_weight_t >::type weight = get(edge_weight, g);
    std::vector < Edge > spanning_tree;

    kruskal_minimum_spanning_tree(g, std::back_inserter(spanning_tree));

    std::cout << "Print the edges in the MST:" << std::endl;

    for (std::vector < Edge >::iterator ei = spanning_tree.begin(); ei != spanning_tree.end(); ++ei)
    {
        std::cout << source(*ei, g) << " <--> " << target(*ei, g) << " with weight of " << weight[*ei] << std::endl;
    }

    std::ofstream fout("figs/kruskal-eg.dot");

    fout << "graph A {\n"
         << " rankdir=LR\n"
         << " size=\"3,3\"\n"
         << " ratio=\"filled\"\n"
         << " edge[style=\"bold\"]\n" << " node[shape=\"circle\"]\n";

    graph_traits<Graph>::edge_iterator eiter, eiter_end;

    for (boost::tie(eiter, eiter_end) = edges(g); eiter != eiter_end; ++eiter)
    {
        fout << source(*eiter, g) << " -- " << target(*eiter, g);

        if (std::find(spanning_tree.begin(), spanning_tree.end(), *eiter) != spanning_tree.end())
        {
            fout << "[color=\"black\", label=\"" << get(edge_weight, g, *eiter) << "\"];\n";
        }
        else
        {
            fout << "[color=\"gray\", label=\"" << get(edge_weight, g, *eiter) << "\"];\n";
        }
    }
    fout << "}\n";
    return EXIT_SUCCESS;
}