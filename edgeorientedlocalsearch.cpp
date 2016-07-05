#include "edgeorientedlocalsearch.h"
#include <random>
#include <iostream>

st::EdgeOrientedLocalSearch::EdgeOrientedLocalSearch(const st::Graph &_tree)
    : tree(_tree)
{

}

void st::EdgeOrientedLocalSearch::removeVertex(st::Graph::Vertex s)
{
    auto edges = this->tree.getIncidentEdges(s);

    for (auto e : edges)
    {
        this->tree.removeEdge(e);
    }

    this->tree.removeVertex(s);
}

void st::EdgeOrientedLocalSearch::removeSteinerLeafs()
{
    auto vertices = this->tree.getVertices();
    bool changed = true;

    while(changed == true)
    {
        changed = false;

        for (auto v : vertices)
        {
            if ((v.type == Vertex::Type::STEINER || v.type == Vertex::Type::CORNER) && (this->tree.getVertexDegree(v) == 1))
            {
                removeVertex(v);
                changed = true;
            }
        }
    }
}

void st::EdgeOrientedLocalSearch::checkAndCrystalize(const Vertex& steinerNode, Graph& graph)
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

void st::EdgeOrientedLocalSearch::connect(const st::EdgeOrientedLocalSearch::VertexPair &pair, Graph& graph)
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

void st::EdgeOrientedLocalSearch::buildLocalTrees()
{
    Graph bestTree = this->tree;

    // First node
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<> dist(0, this->tree.getNumberOfEdges()-1);
    std::int32_t randomIndex = dist(mt);

    Edge randomEdge = this->tree.getEdge(randomIndex);

    this->tree.removeEdge(randomEdge);
    this->removeSteinerLeafs();

    auto components = this->tree.getComponents();
    std::vector<Graph> localSearchedTrees;
    std::int32_t numberOfVariantTrees = components[0].size() * components[1].size();
    localSearchedTrees.reserve(numberOfVariantTrees);

    for (auto vertexFromC0 : components[0])
    {
        for (auto vertexFromC1 : components[1])
        {
            Graph newTree = this->tree;

            this->connect({vertexFromC0, vertexFromC1}, newTree);

            if (newTree.getDistance() < bestTree.getDistance())
            {
                bestTree = newTree;
            }
        }
    }

    this->minDistanceTrees.push(bestTree);
}

st::Graph st::EdgeOrientedLocalSearch::run()
{
    this->buildLocalTrees();

    return this->minDistanceTrees.top();
}
