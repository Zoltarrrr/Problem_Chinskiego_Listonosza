/**
* @file Funkcje.h
* @author Maciej Perek
* @brief Plik zawieraj¹cy funkcje wykorzystywane w programie
**/
#include <iostream>
#include <string>
#include <map>
#include <set>
#include <list>
#include <stack>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <limits>

#define MISSING_NODE_ID -1

/**@brief Degree opisuje stopieñ wierzcho³ka
* @param Even ozncza, ¿e wierzcho³ek jest pazrzysty
* @param Odd oznacza, ¿e wierzcho³ek jest nieparzysty
**/
enum class Degree {
    Even,
    Odd
};
/**@brief tworzê typedef int NodeId ¿eby numer wierzcho³ka mia³ typ int
**/
typedef int NodeId;
/** @brief tworzê strukturê opisuj¹c¹ wêze³
* @param NodeId id oznacza numer wierzcho³ka
* @param distancesToNeighbourNodes opisuje id wierzcholka i odleglosc do niego
* @param Degree opisuje stopieñ wierzcho³ka
**/
struct Node
{
    NodeId id; // id wierzcholka -> odleglosc do niego
    std::map<NodeId, double> distancesToNeighbourNodes;
    Degree degree;
};
/**@brief jest to mapa, z numjerami wierzcho³ków
**/
typedef std::map<NodeId, Node> Graph;
/**@brief Graph tworzy listê ze œcie¿kami miêdzy wêz³ami
**/
typedef std::list<NodeId> Path;
/** @brief struktura opisuje skojarzenia za pomoc¹ odleg³oœci z metadanych w postaci œcie¿ki, prowadz¹cych do innego wêz³a
* @param distance opisuje odleg³oœæ miêdzy wêz³ami
* @param path opisuje œcie¿kê ³¹cz¹c¹ wêz³y
**/
struct DistanceAndPath {
    double distance;
    Path path;
};
/** @brief  struktura opisuje krawêdŸ grafu
* @param node1 opisuje numer wêz³a
* @param node2 opisuje numer drugiego wêz³a
* @param streetName opisuje nazwy ulicy ³¹cz¹c¹ dwa wêz³y
* @param lenght przyjmuje d³ugoœæ drogi
* @param operator==(const Edge& other) definijemy operator porównania, ¿eby potem ³atwo robiæ porównanie "edge1 == edge2"
* @return zwraca odpowiedni krawêdŸ z waroœci¹ obu wierzcho³ków oraz odleg³oœci¹ miêdzy nimi
**/
struct Edge {
    NodeId node1;
    NodeId node2;
    std::string streetName;
    double length;
    bool operator==(const Edge& other) 
    {
        return this->node1 == other.node1 && this->node2 == other.node2 && this->length == other.length;
    }
};
/**@brief tworzê parê odlegloœci do wez³a docelowego i poprzednika
**/
typedef std::pair<double, NodeId> DijkstraNode; 
/**@brief tworzê mapê opisuj¹c¹ numer wêz³a docelowego i pary sk³¹daj¹cej siê z odleg³oœci do wêz³a docelowego i poprzednika
**/
typedef std::map<NodeId, DijkstraNode> DijkstraResult;
/**@brief tworzê listê krawêdzi
*
**/
typedef std::list<Edge> Edges;
/**@brief tworzê listê skojarzeñ w grafie
*
**/
typedef std::list<Edge> Matching;
/**@brief struktura zawiera czêœciowe skojarzenie z metadanymi w postaci krawêdzi, które zawieraj¹ nieskojarzone wierzcho³ki
*
**/
struct MatchingWithMetadata
{
    Matching matching;
    Edges remainingEdges;
};
/**@brief funkcja pomocnicza dodaj¹ca do wêz³a "id1" w grafie "graph" informacje o s¹siedzie "id2", który jest w odleg³oœci "distance"
* @param graph garph z numerami wierzcho³ków
* @param id1 numer wierzcho³ka pierwszego
* @param id2 numer wierzcho³ka drugiego
* @param distance odleg³oœæmiêdzy wêz³ami
**/
void addNodeDistanceToGraph(Graph& graph, NodeId id1, NodeId id2, double distance);
/**@brief Funkcja ³¹duj¹ca z pliku dane
* @param Graph graph z numerami wierzcho³ków
* @param Edges lista z krawêdziami miêdzy weirzcho³kami
* @return funkcja zwraca graph z numerami wierzcho³ków i krawêdŸ je ³¹cz¹c¹
**/
std::pair<Graph, Edges> LoadFromFile(const std::string& fileName);
/**@brief funkcja pokazuje zapisany graph z pliku
* @param graph graph z numerami wierzcho³ków
**/
void ShowGraph(const Graph& graph);
/**@brief funkcja szuka najkrótszych po³¹czeñ pomiêdzy wszystkimi wêz³ami nieparzystymi
* @param graph graph z numerami wierzcho³ków
* @param nodeId numer wierzcho³ka
* @return funkcja zwraca najkrótsze odleg³oœci miêdzy wszystkimi wêz³ami nieparzystymi
**/
DijkstraResult dijkstraFromNodeToAllOdds(const Graph& graph, const NodeId nodeId);
/**@brief funkcja tworzy graf zawierajacy wszystkie nieparzyste wierzcholki grafu z najkrotszymi odleglosciami pomiedzy nimi, wykorzystujac algorytm Dijkstry
* @param graph  graph z numerami wierzcho³ków
* @return funkcja zwraca najkrótsze odleg³oœci miêdzy wêz³ami nieparzystymi
**/
std::pair<Graph, std::list<Path>> dijkstraBetweenAllOdds(const Graph& graph);
/**@brief funkcja przeksztalca nasz graf z wezlami o stopniu nieparzystym na krawedzie
* @param graph graph z numerami wierzcho³ków
* @return funkcja zwraca najkrótsze krawêdzie ³¹cz¹ce wierzcho³ki o nieparzystym stopniu
**/
Edges transformGraphFromNodesToEdges(const Graph& graph);
/**@brief funkcja znajduje wszystkie skojarzenia w graphie
* @param matchingsWithMetadatas jest to lista skojarzeñ miêdzy wierzcho³kami
* @return funkcja zwraca wszystkie skojarzenia  w graphie
**/
std::list<MatchingWithMetadata> findAllPerfectMathings(std::list<MatchingWithMetadata> matchingsWithMetadatas);
/**@brief znajduje najkrótsze skojarzenia w graphie
* @param edges lista krawêdzi ³¹cz¹cych wierzcho³ki
* @return funkcja zwraca najkrótsze skojarzenia w graphie
**/
std::pair<Matching, double> findMinimumPerfectMatchingWithLength(const Edges& edges);
/**@brief funkcja znajduje œcie¿kê miêdzy dwoma wierzcho³kami
* @param from to numer wierzcho³ka
* @param to to numer docelowy wierzcho³ka
* @param allPaths to lista œcie¿ek
* @return funkcja zwraca œcie¿kê miêdzy dwoma wierzcho³kami
**/
Path findPath(NodeId from, NodeId to, const std::list<Path>& allPaths);
/**@brief funkcja znajduje krawêdŸ miêdzy dwoma wierzcho³kami na podstawie œcie¿ki
* @param from to numer wierzcho³ka
* @param to to numer docelowy wierzcho³ka
* @param edges lista krawêdzi ³¹cz¹cych wierzcho³ki
* @return funkcja zwraca krawêdŸ miêdzy dwoma wierzcho³kami na podstawie œcie¿ki
**/
Edge findEdge(NodeId from, NodeId to, const Edges& edges);
/**@brief funkcja sprawdzaj¹ca gdzie jest siê aktualnie
* @param index numer aktualnego wierzcho³ka
* @param path lista œcie¿ek
* @return funkcja zwraca wierzcho³ek w,którym aktualnie siê znajdujemy
*
**/
NodeId nodeAt(int index, Path& path);
/**@brief funkcja sprawdza czy znaleŸliœmy krawêdŸ
* @param edges lista krawêdzi ³¹cz¹cych wierzcho³ki
* @param nodeId numer wierzcho³ka
* @param foundEdge znaleziona krawêdŸ
* @return funkcja zwraca krawêdŸ
**/
bool findEdgeWithNodeId(const Edges& edges, NodeId nodeId, Edge& foundEdge);
/**@brief funkcja duplikuje i dodaje zduplikowan¹ krawêdŸ do graphAsEdges
* @param minimumMatching minimalne skojarzenie w graphie
* @param paths lista œcie¿ek
* @param graphAsEdges lista krawêdzi ³¹cz¹cych wierzcho³ki
**/
void duplicateEdges(const Matching& minimumMatching, const std::list<Path>& paths, Edges& graphAsEdges);
/**@brief funkcja znajduje cykl Eulera w graphie za pomoc¹ algorytmu Fleurego
* @param startingNodeId numer wêz³a startowego
* @param paths lista œcie¿ek
* @param graphAsEdges lista krawêdzi ³¹cz¹cych wierzcho³ki
* @return funkcja zwraca cykl Eulera
**/
Edges fleury(NodeId startingNodeId, const Edges& allEdges);

