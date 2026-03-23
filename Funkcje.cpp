/**
* @file Funkcje.cpp
* @author Maciej Perek
* @brief Plik zawierający funkcje wykorzystywane w programie
**/
#include "Funkcje.h"
#define MISSING_NODE_ID -1
// funkcja pomocnicza - dodaje do wezla `id1` w grafie `graph` informacje o sasiedzie `id2`, ktory jest w odleglosci `distance`
void addNodeDistanceToGraph(Graph& graph, NodeId id1, NodeId id2, double distance) {
    if (graph.find(id1) == graph.end()) { // nie ma wezla o takim id
        graph[id1] = { id1 }; // skoro go nie ma, utworz go i dodaj
    }
    // teraz juz na pewno jest taki wezel - mozemy dodac odleglosc (od id1 do id2)
    graph[id1].distancesToNeighbourNodes[id2] = distance;
}

std::pair<Graph, Edges> LoadFromFile(const std::string& fileName) {
    std::ifstream in(fileName);
    Graph graph;
    Edges graphAsEdges;
    if (in) {
        std::string line;
        while (std::getline(in, line)) {
            std::stringstream ss(line);
            NodeId nodeId1, nodeId2;
            double length;
            std::string streetName;
            std::string ulica;
            if (!(ss >> nodeId1)) continue;
            if (!(ss >> nodeId2)) continue;
            if (!(ss >> length)) continue;
            while (ss >> ulica)
                streetName += ulica + " ";
            addNodeDistanceToGraph(graph, nodeId1, nodeId2, length);
            addNodeDistanceToGraph(graph, nodeId2, nodeId1, length);
            Edge edge = {
                nodeId1,
                nodeId2,
                streetName,
                length
            };
            graphAsEdges.push_back(edge);
        }
        in.close();
        for (auto& el : graph) {
            el.second.degree = el.second.distancesToNeighbourNodes.size() % 2 == 0 ? Degree::Even : Degree::Odd;
        }
        return { graph, graphAsEdges };
    }
    else {
        std::cout << "Nie znaleziono pliku o podanej nazwie:" << fileName << std::endl;
        exit(1);
    }
}

void ShowGraph(const Graph& graph) {
    for (const auto& row : graph) {
        const Node node = row.second;
        std::cout << node.id << '\t';
        for (const auto& el : node.distancesToNeighbourNodes) {
            std::cout << '[' << el.first << ", " << el.second << "]\t";
        }
        std::cout << '\n';
    }
    std::cout << std::endl;
}

DijkstraResult dijkstraFromNodeToAllOdds(const Graph& graph, const NodeId nodeId) {
    DijkstraResult result;
    // wezly, ktorych jeszcze nie sprawdzilismy
    std::set<NodeId> nodes;
    std::set<NodeId> oddDegreeNodes;
    for (const auto& el : graph) {
        result[el.first] = DijkstraNode{ std::numeric_limits<double>::infinity(), MISSING_NODE_ID };
        nodes.insert(el.first);
        if (el.second.degree == Degree::Odd)
            oddDegreeNodes.insert(el.first);
    }
    if (graph.find(nodeId) == graph.end()) // sprawdz, czy taki wezel istnieje w podanym grafie
        return result; // nie istnieje 

    result[nodeId] = DijkstraNode{ 0.0, nodeId }; // tu jestesmy, czyli odleglosc = 0, poprzednik = ten sam wezel
    while (!nodes.empty()) {
        // znajdz najblizszy element:
        NodeId i = *std::min_element(
            nodes.begin(),
            nodes.end(),
            [&result](const NodeId id1, const NodeId id2) {
                return result[id1] < result[id2];
            }
        );
        Node node_i = graph.at(i);
        nodes.erase(i);

        if (node_i.degree == Degree::Odd)
            oddDegreeNodes.erase(i);

        for (const auto& el : node_i.distancesToNeighbourNodes) {  // dla wszystkich sasiadow
            if (nodes.find(el.first) != nodes.end()) { // jesli ten sasiad nie zostal jeszcze sprawdzony (nie ma go w liscie wezlow do sprawdzenia)
                double alternativeDistance = result[i].first + el.second; // odleglosc do sasiada prowadzaca przez wezel 'i'
                if (result[el.first].first > alternativeDistance) {
                    result[el.first] = { alternativeDistance, i };
                }
            }
        }
    }
    return result;
}
std::pair<Graph, std::list<Path>> dijkstraBetweenAllOdds(const Graph& graph) {
    Graph oddNodesGraphDirect;
    std::list<Path> paths;
    for (const auto& el : graph) {
        const Node sourceNode = el.second;
        if (sourceNode.degree == Degree::Odd) {
            DijkstraResult result = dijkstraFromNodeToAllOdds(graph, sourceNode.id);
            for (const auto& r : result) {
                NodeId destinationNodeId = r.first;
                Node destinationNode = graph.at(destinationNodeId);
                if (destinationNode.degree == Degree::Odd) { // interesuja nas tylko nieparzyste wezly - zrodlowy juz sprawdzilismy, ze jest nieparzystego stopnia, teraz czas na docelowy
                    DijkstraNode destinationResult = r.second;
                    addNodeDistanceToGraph(oddNodesGraphDirect, sourceNode.id, destinationNodeId, destinationResult.first);
                    // liczenie sciezki
                    if (sourceNode.id != destinationNodeId) {
                        Path path;
                        path.push_front(destinationNodeId);
                        DijkstraNode nodeDijkstraResult = destinationResult;
                        NodeId predecessor = nodeDijkstraResult.second;
                        while (predecessor != sourceNode.id) {
                            path.push_front(predecessor);
                            nodeDijkstraResult = result.at(nodeDijkstraResult.second); // nowy poprzednik
                            predecessor = nodeDijkstraResult.second;
                        }
                        path.push_front(sourceNode.id);
                        paths.push_back(path);
                    }
                }
            }
        }
    }
    return { oddNodesGraphDirect, paths };
}
Edges transformGraphFromNodesToEdges(const Graph& graph) {
    Edges edges;
    for (auto& el : graph) {
        for (auto& d : el.second.distancesToNeighbourNodes) {
            NodeId nodeId1 = el.first,
                nodeId2 = d.first;

            if (nodeId1 != nodeId2) { // tylko jesli to nie jest petla
                // sprawdzenie czy nie ma juz dodanej takiej krawedzi
                bool hasEdge = false;
                for (Edge e : edges) {
                    if (
                        (e.node1 == nodeId1 && e.node2 == nodeId2)
                        ||
                        (e.node1 == nodeId2 && e.node2 == nodeId1)
                        ) {
                        hasEdge = true;
                        break;
                    }
                }
                if (!hasEdge) { // nie ma juz wczeniej dodanej, wiec dodaj teraz
                    Edge e = {
                        nodeId1,
                        nodeId2,
                        "",
                        d.second // distance
                    };
                    edges.push_back(e);
                }
            }

        }
    }
    return edges;
}

std::list<MatchingWithMetadata> findAllPerfectMathings(std::list<MatchingWithMetadata> matchingsWithMetadatas) {
    std::list<MatchingWithMetadata> newMatchingsWithMetadatas;
    for (MatchingWithMetadata& matchingWithMetadata : matchingsWithMetadatas) {
        if (matchingWithMetadata.remainingEdges.size() == 0) {
            newMatchingsWithMetadatas.push_back(matchingWithMetadata);
        }
        else {
            for (Edge edge : matchingWithMetadata.remainingEdges) {
                Matching newMatching(matchingWithMetadata.matching); // kopia za pomocą konstruktora kopiujacego
                newMatching.push_back(edge);
                // pozostale krawedzie z wezlami ktore nie sa w dopasowaniu
                Edges newRemainingEdges;
                for (Edge i : matchingWithMetadata.remainingEdges) {
                    if (
                        i.node1 != edge.node1 && i.node2 != edge.node1
                        && i.node1 != edge.node2 && i.node2 != edge.node2
                        ) {
                        newRemainingEdges.push_back(i);
                    }
                }
                MatchingWithMetadata newMatchingWithMetadata = {
                    newMatching,
                    newRemainingEdges
                };
                newMatchingsWithMetadatas.push_back(newMatchingWithMetadata);
            }
        }
    }
    if (newMatchingsWithMetadatas.size() == matchingsWithMetadatas.size())
        return newMatchingsWithMetadatas;
    else
        return findAllPerfectMathings(newMatchingsWithMetadatas);
}

std::pair<Matching, double> findMinimumPerfectMatchingWithLength(const Edges& edges) {
    std::list<MatchingWithMetadata> matchingsWithMetadatas;
    Matching matching; // pusty
    MatchingWithMetadata matchingWithMetadata = {
        matching,
        edges // remaining edges = all edges, bo nic jeszcze nie sprawdzilismy
    };
    matchingsWithMetadatas.push_back(matchingWithMetadata);

    std::list<MatchingWithMetadata> allPerfectMatchingsWithMetadatas = findAllPerfectMathings(matchingsWithMetadatas);

    Matching emptyMatching;
    std::pair<Matching, double> minimumMatchingWithLength = { emptyMatching, std::numeric_limits<double>::infinity() };
    for (MatchingWithMetadata matchingWithMetadata : allPerfectMatchingsWithMetadatas) {
        double totalDistance = 0.0;
        for (Edge e : matchingWithMetadata.matching) {
            totalDistance += e.length;
        }
        if (totalDistance < minimumMatchingWithLength.second) { // nowe minimum
            minimumMatchingWithLength = { matchingWithMetadata.matching, totalDistance };
        }
    }
    return minimumMatchingWithLength;
}

Path findPath(NodeId from, NodeId to, const std::list<Path>& allPaths) { // znalezienie polaczenia miedzy wezlami
    for (const Path p : allPaths) {
        if (p.front() == from && p.back() == to) {
            return p;
        }
    }
    std::cout << "Nie znaleziono sciezki od: " << from << " do: " << to << std::endl;
    exit(1);
}

Edge findEdge(NodeId from, NodeId to, const Edges& edges) {
    for (const Edge e : edges) {
        if (
            (e.node1 == from && e.node2 == to)
            ||
            (e.node2 == from && e.node1 == to)
            ) {
            return e;
        }
    }
    std::cout << "Nie znaleziono krawedzi od: " << from << " do: " << to << std::endl;
    exit(1);
}

NodeId nodeAt(int index, Path& path) {
    auto it = path.begin();
    std::advance(it, index);
    return *it;
}

bool findEdgeWithNodeId(const Edges& edges, NodeId nodeId, Edge& foundEdge) {
    for (const Edge& e : edges) {
        if (e.node1 == nodeId || e.node2 == nodeId) {
            foundEdge = e;
            return true;
        }
    }
    return false;
}
// funkcja modyfikuje graphAsEdges
void duplicateEdges(const Matching& minimumMatching, const std::list<Path>& paths, Edges& graphAsEdges) {
    for (Edge e : minimumMatching) {
        Path path = findPath(e.node1, e.node2, paths);
        for (int i = 1; i < path.size(); i++) {
            // znajdz krawedz ze sciezki:
            Edge e = findEdge(
                nodeAt(i - 1, path),
                nodeAt(i, path),
                graphAsEdges
            );
            //zduplikuj krawedz w grafie:
            graphAsEdges.push_back(e);
        }
    }
}

Edges fleury(NodeId startingNodeId, const Edges& allEdges) {
    std::stack<NodeId> visitedNodes;
    NodeId currentNode = startingNodeId;
    NodeId nextNode;
    Edges remainingEdges(allEdges);
    Edges traversedEdges;
    Edge foundEdge;
    Edges backtrackEdges;
    bool endAlgorithm = false;
    while (!endAlgorithm) {
        while (findEdgeWithNodeId(remainingEdges, currentNode, foundEdge)) {
            if (!backtrackEdges.empty()) { // jesli sie cofalismy
                std::cout << "Backtrack! ";
                for (Edge& e : backtrackEdges) {
                    std::cout << "[" << e.node1 << " -> " << e.node2 << "]\t";
                }
                std::cout << std::endl;
                // znalezlismy inna droge to trzeba zwrocic te krawedzie, po ktorych sie cofalismy do remainingEdges i usunac je z backtrackEdges `splice()` dodaje elementy do remainingEdges i usuwa z backtrackEdges. remainingEdges.end dodaje krawedz na koniec
                remainingEdges.splice(remainingEdges.end(), backtrackEdges);
            }
            std::cout << "v: " << currentNode << ", foundEdge: " << foundEdge.node1 << " -> " << foundEdge.node2 << " (distance = " << foundEdge.length << ")" << std::endl;
            nextNode = (foundEdge.node1 == currentNode) ? foundEdge.node2 : foundEdge.node1;
            visitedNodes.push(currentNode);
            bool removed = false;
            for (auto it = remainingEdges.begin(); !removed && it != remainingEdges.end();) {
                const Edge& e = *it;
                if (foundEdge == e) {
                    it = remainingEdges.erase(it);
                    removed = true;
                }
                else ++it;
            }
            traversedEdges.push_back(foundEdge); // zapisuje jakie krawedzie przeszlismy
            currentNode = nextNode;
        }
        if (remainingEdges.empty()) {
            endAlgorithm = true;
        }
        else {
            currentNode = visitedNodes.top();   //zwraca wartosc z gory stosu
            visitedNodes.pop();
            // tutaj wracamy sie o jeden wezel do tylu, wiec krawedz, przez ktora wlasnie przeszlismy musimy usunac z listy krawedzi,po ktorych juz przeszlismy i dodac ja do krawedzi, po ktorych sie cofamy
            Edge lastTraversedEdge = traversedEdges.back(); // pobieramy
            backtrackEdges.push_back(lastTraversedEdge); // dodajemy
            traversedEdges.pop_back(); //  usuwamy
        }
    }
    return traversedEdges;
}