/**
* @file main.cpp
* @author Maciej Perek
* @brief main
**/
#include "Funkcje.h" 

int main(int argv, char* argc[]) {
    std::map<std::string, std::string> mapa;
    for (int i = 1; i < argv; i = i + 2) {
        mapa[argc[i]] = argc[i + 1];
    }
    std::string fileName(mapa["-i"]);
    std::pair<Graph, Edges> graphCombined = LoadFromFile(fileName);
    Graph graph = graphCombined.first;
    Edges graphAsEdges = graphCombined.second;
    std::cout << "Graf z pliku w postaci <wierzcholek> <id sasiada i odległość do niego>:\n";
    ShowGraph(graph);
    std::pair<Graph, std::list<Path>> graphs = dijkstraBetweenAllOdds(graph);
    Graph oddNodesGraphDirect = graphs.first;
    std::list<Path> paths = graphs.second;
    std::cout << "Graf nieparzystych wierzcholkow BEZPOSREDNI (w postaci <wierzcholek> <id wezla docelowego i odległosc do niego>):\n";
    ShowGraph(oddNodesGraphDirect);
    std::cout << "Sciezki w grafie nieparzystych wierzcholkow:" << std::endl;
    for (Path& p : paths) {
        for (int nodeId : p) {
            std::cout << nodeId << ' ';
        }
        std::cout << '\n';
    }
    std::cout << std::endl;
    Edges oddEdgesDirect = transformGraphFromNodesToEdges(oddNodesGraphDirect);
    std::pair<Matching, double> minimumMatchingWithLength = findMinimumPerfectMatchingWithLength(oddEdgesDirect);
    std::cout << "[Znalezione minimalne pe�ne skojarzenie]\nKrawedzie:" << std::endl;
    for (Edge e : minimumMatchingWithLength.first) {
        std::cout << '\t' << e.node1 << " -> " << e.node2 << " (odleglosc = " << e.length << ")" << std::endl;
    }
    std::cout << "Calkowita dlugosc skojarzenia: " << minimumMatchingWithLength.second << std::endl;
    duplicateEdges(minimumMatchingWithLength.first, paths, graphAsEdges);
    std::cout << "Wszystkie krawedzie grafu z niezbednymi duplikatami:" << std::endl;
    for (Edge e : graphAsEdges) {
        std::cout << '\t' << e.node1 << " -> " << e.node2 << " (odleglosc = " << e.length << ")" << std::endl;
    }
    std::cout << std::endl;
    int startingNodeId = stoi(mapa["-p"]);
    Edges traversedEdges = fleury(startingNodeId, graphAsEdges);
    std::cout << "Wynik dzialania algorytmu Fleury'ego:" << std::endl;
    for (Edge e : traversedEdges) {
        std::cout << "ulica:\t" << e.node1 << " <--> " << e.node2 << ", odleglosc = " << e.length << ", nazwa ulicy = '" << e.streetName << "'\n";
    }
    std::cout << std::endl;
    std::ofstream File(mapa["-o"]);
    for (Edge e : traversedEdges) {
        File << e.node1 << " " << e.node2 << " " << e.streetName << "\n";
    }
    File.close();
}