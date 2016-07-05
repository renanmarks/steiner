#include "edgeorientedlocalsearch.h"
#include <random>
#include <iostream>

st::EdgeOrientedLocalSearch::EdgeOrientedLocalSearch(const st::Graph &_tree)
    : tree(_tree)
{

}

std::vector<st::Graph> st::EdgeOrientedLocalSearch::getValidNeighbourMoves() const
{
    std::vector<st::Graph> localNeighbours;
    localNeighbours.reserve(this->tree.getNumberOfEdges());

    Graph bestTree = this->tree;

    for (const Edge& edge : this->tree.getEdges())
    {
        Graph workTree = this->tree;
        std::uint32_t oldDistance = workTree.getDistance();
        workTree.removeEdge(edge);
        this->removeSteinerLeafs(workTree);

        auto components = workTree.getComponents();

        for (const auto& vertexFromC0 : components[0])
        {
            for (const auto& vertexFromC1 : components[1])
            {
                std::uint32_t newDistance = workTree.getDistance() + Edge(vertexFromC0, vertexFromC1).getDistance();

                if (newDistance < oldDistance)
                {
                    Graph newTree = workTree;
                    this->connect({vertexFromC0, vertexFromC1}, newTree);
                    bestTree = newTree;
                    localNeighbours.push_back(bestTree);
                }
            }
        }
    }

    return localNeighbours;
}

void st::EdgeOrientedLocalSearch::removeVertex(st::Graph::Vertex s, Graph& tree) const
{
    auto edges = tree.getIncidentEdges(s);

    for (auto e : edges)
    {
        tree.removeEdge(e);
    }

    tree.removeVertex(s);
}

void st::EdgeOrientedLocalSearch::removeSteinerLeafs(Graph& tree) const
{
    bool changed = true;

    while(changed == true)
    {
        changed = false;

        for (auto v : tree.getVertices())
        {
            if ((v.type == Vertex::Type::STEINER || v.type == Vertex::Type::CORNER) && (tree.getVertexDegree(v) == 1))
            {
                removeVertex(v, tree);
                changed = true;
            }
        }
    }
}

void st::EdgeOrientedLocalSearch::checkAndCrystalize(const Vertex& steinerNode, Graph& graph) const
{
    Vertex updated = graph.getVertex(steinerNode.index);

    if (updated.type == Vertex::Type::CORNER)
    {
        /* Cristalization to Steiner Nodes */
        if ( graph.getVertexDegree(updated) > 2 )
        {
            Vertex temp = updated;
            temp.type = Vertex::Type::STEINER;
            graph.updateVertex(updated, temp);
        }
    }
    else if (updated.type == Vertex::Type::STEINER)
    {
        /* Cristalization to Steiner Nodes */
        if ( graph.getVertexDegree(updated) == 2 )
        {
            Vertex temp = updated;
            temp.type = Vertex::Type::CORNER;
            graph.updateVertex(updated, temp);
        }
    }
}

void st::EdgeOrientedLocalSearch::connect(const st::EdgeOrientedLocalSearch::VertexPair &pair, Graph& graph) const
{
    /* Collinear vertices */
    if ((pair.first.x == pair.second.x) || (pair.first.y == pair.second.y))
    {
        Graph::Edge edge(pair.first, pair.second);

        graph.addEdge(edge);

        /* Need to check the "crystalization" of corner node to steiner node*/
        checkAndCrystalize(pair.first, graph);
        checkAndCrystalize(pair.second, graph);

        return;
    }

    /* Need to make a corner */
    Vertex corner( pair.first.x, pair.second.y, Vertex::Type::CORNER );
    corner = graph.addVertex(corner);

    connect({corner, pair.first}, graph);
    connect({corner, pair.second}, graph);
}

st::Graph st::EdgeOrientedLocalSearch::buildLocalTrees() const
{
    Graph bestTree = this->tree;

    for (const Edge& edge : this->tree.getEdges())
    {
        Graph workTree = this->tree;
        std::uint32_t oldDistance = workTree.getDistance();
        workTree.removeEdge(edge);
        this->removeSteinerLeafs(workTree);

        auto components = workTree.getComponents();

        for (const auto& vertexFromC0 : components[0])
        {
            for (const auto& vertexFromC1 : components[1])
            {
                std::uint32_t newDistance = workTree.getDistance() + Edge(vertexFromC0, vertexFromC1).getDistance();

                if (newDistance < oldDistance)
                {
                    Graph newTree = workTree;
                    this->connect({vertexFromC0, vertexFromC1}, newTree);
                    bestTree = newTree;
                }
            }
        }
    }

    return bestTree;
}

st::Graph st::EdgeOrientedLocalSearch::run()
{
    return this->buildLocalTrees();
}
