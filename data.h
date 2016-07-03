#ifndef DATA_H
#define DATA_H

#include <string>
#include <vector>
#include <array>

namespace st
{

class Data
{
public:
    Data();

    void load(std::istream& file);
    void print(std::ostream& stream);

    bool operator==(const Data& other) const;
    bool operator!=(const Data& other) const;
//private:
    struct Comment
    {
        std::string name;
        std::string date;
        std::string creator;
        std::string remark;
        std::string problem;

        void print(std::ostream &out) const;
    } comment;

    struct Graph
    {
        struct Edge
        {
            std::uint32_t first;
            std::uint32_t second;
            std::int32_t weight;

            Edge();
            Edge(std::uint32_t _first, std::uint32_t _second, std::int32_t _weight);
            bool operator==(const Edge& other) const;
            bool operator!=(const Edge& other) const;
        };

        struct Arc
        {
            std::uint32_t first;
            std::uint32_t second;
            std::int32_t weight;

            Arc();
            Arc(std::uint32_t _first, std::uint32_t _second, std::int32_t _weight);
            bool operator==(const Arc& other) const;
            bool operator!=(const Arc& other) const;
        };

        std::uint32_t numberOfObstacles;
        std::uint32_t numberOfNodes;
        std::uint32_t numberOfEdges;
        std::vector<Edge> edges;
        std::vector<Arc> arcs;

        Graph();

        void print(std::ostream &out) const;
        bool operator==(const Graph& other) const;
        bool operator!=(const Graph& other) const;
    } graph;

    struct Terminal
    {
        std::uint32_t rootP;
        std::uint32_t root;
        std::vector<std::uint32_t> terminals;

        Terminal();

        void print(std::ostream &out) const;
        bool operator==(const Terminal& other) const;
        bool operator!=(const Terminal& other) const;
    } terminal;

    struct Vertex
    {
        enum Coord
        {
            x = 0, y, z
        };

        std::int32_t index;
        std::array<double, 3> values;
        using value_type = double;

        Vertex();
        Vertex(double x, double y, double z);
        Vertex(std::int32_t i, double x, double y, double z);
        value_type operator[](size_t i) const;
        bool operator==(const Vertex& other) const;
        bool operator!=(const Vertex& other) const;

        bool isCollinearWith(const Vertex& other) const;
        Vertex getPerpendicularProjectionOnEdge(const Vertex& e1, const Vertex& e2) const;
    };

    std::vector<Vertex> vertices;

    bool parseHeader(std::istream &file);
    void parseCommentSection(std::istream &file);
    void parseGraphSection(std::istream &file);
    void parseTerminalsSection(std::istream &file);
//TODO:    void parseMaximumDegressSection();
    void parseCoordinatesSection(std::istream &file);

};

}

#endif // DATA_H
