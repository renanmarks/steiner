#include "data.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <regex>
#include <algorithm>

namespace
{
    std::vector<std::string> getMatches(std::istream &stream, std::string regexStr)
    {
        std::string fileLine;
        std::getline(stream, fileLine);

        /* Matches all the specification tags */
        std::regex specsRegex(regexStr);
        std::smatch specsMatches;

        /* Regex search */
        std::regex_search(fileLine, specsMatches, specsRegex);

        if (specsMatches.ready() && specsMatches.empty() && specsMatches.size() == 0)
        {
            return std::vector<std::string>();
        }

        return std::vector<std::string>(specsMatches.begin()+1, specsMatches.end());
    }

    std::string getString(std::istream &stream, std::string regexStr)
    {
        return getMatches(stream, regexStr)[1];
    }

    std::string getStringAndRewind(std::istream &stream, std::string regexStr)
    {
        std::string stringValue = getString(stream, regexStr);
        stream.seekg(0, stream.beg);

        return stringValue;
    }

    std::string toLower(const std::string& str)
    {
        std::string returnStr = str;

        std::transform(returnStr.begin(), returnStr.end(), returnStr.begin(), ::tolower);

        return returnStr;
    }
}

bool st::Data::parseHeader(std::istream &file)
{
    std::string header;
    std::getline(file, header);

    return (toLower(header) == toLower("33D32945 STP File, STP Format Version 1.00"));
}

void st::Data::parseCommentSection(std::istream &file)
{
    std::string line;

    for (std::getline(file, line);
         toLower(line) != "end";
         std::getline(file, line))
    {
        std::istringstream stream(line);
        std::vector<std::string> matches = getMatches(stream, "^\\s*(\\w*)\\s*\"(.*)\"\\s*$");
        stream.seekg(0, stream.beg);

        const std::string itemName  = toLower(matches[0]);
        const std::string itemValue = matches[1];

        if (itemName == "name")
        {
            this->comment.name = itemValue;
        }
        else if (itemName == "date")
        {
            this->comment.date = itemValue;
        }
        else if (itemName == "creator")
        {
            this->comment.creator = itemValue;
        }
        else if (itemName == "remark")
        {
            this->comment.remark = itemValue;
        }
        else if (itemName == "problem")
        {
            this->comment.problem = itemValue;
        }
    }

    return;
}

void st::Data::parseGraphSection(std::istream &file)
{
    std::string line;

    for (std::getline(file, line);
         toLower(line) != "end";
         std::getline(file, line))
    {
        std::istringstream stream(line);
        std::vector<std::string> matches = getMatches(stream, "^\\s*(\\w*)\\s*\"(.*)\"\\s*$");
        stream.seekg(0, stream.beg);

        const std::string itemName  = toLower(matches[0]);
        const std::string itemValue = matches[1];

        if (itemName == "obstacles")
        {
            this->graph.numberOfObstacles = std::stoi(itemValue);
        }
        else if (itemName == "nodes")
        {
            this->graph.numberOfNodes = std::stoi(itemValue);
        }
        else if (itemName == "edges")
        {
            this->graph.numberOfEdges = std::stoi(itemValue);
        }
        else if (itemName == "arcs")
        {
            this->graph.numberOfArcs = std::stoi(itemValue);
        }
        else if (itemName == "e")
        {
            stream.seekg(0, stream.beg);
            matches = getMatches(stream, "^\\s*E\\s*(\\d+)\s*(\\d+)\s*(\\d+)\s*$");

            //TODO: this->graph.edges.push_back();
        }
        else if (itemName == "a")
        {
            //TODO: this->graph.arcs.push_back();
        }
    }

    return;
}

void st::Data::parseTerminalsSection(std::istream &file)
{

}

void st::Data::parseCoordinatesSection(std::istream &file)
{

}

st::Data::Data()
{

}

void st::Data::load(std::istream &file)
{
    if (parseHeader(file) == false)
    {
        std::cout << "Error." << std::endl;
        return;
    }

    std::string line;

    for (std::getline(file, line);
         file.eof() == false;
         std::getline(file, line))
    {
        std::istringstream stream(line);
        std::vector<std::string> matches = getMatches(stream, "^\\s*([sS][eE][cC][tT][iI][oO][nN])\\s*(.*)\\s*$");
        stream.seekg(0, stream.beg);

        if (line.empty() || matches.size() == 0)
        {
            continue;
        }

        const std::string itemValue = toLower(matches[1]);

        if (itemValue == "comment")
        {
            parseCommentSection(file);
        }
        else if (itemValue == "graph")
        {
            parseGraphSection(file);
        }
        else if (itemValue == "terminals")
        {
            parseTerminalsSection(file);
        }
        else if (itemValue == "coordinates")
        {
            parseCoordinatesSection(file);
        }
    }
}

void st::Data::print(std::ostream &stream)
{
    this->comment.print(stream);
}

void st::Data::Comment::print(std::ostream& out) const
{
    out << "Name    : " << this->name    << '\n'
        << "Creator : " << this->creator << '\n'
        << "Date    : " << this->date    << '\n'
        << "Problem : " << this->problem << '\n'
        << "Remark  : " << this->remark  << '\n'
        << std::endl;
}
