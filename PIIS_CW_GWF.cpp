#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <climits>
#include <fstream>
#include <sstream>

using namespace std;

struct Edge {
    int u, v;
};

void addEdge(vector<vector<int>>& incMatrix, int edgeIndex, int u, int v) {
    incMatrix[edgeIndex][u] = 1;
    incMatrix[edgeIndex][v] = 1;
}

void printMatrix(const vector<vector<int>>& matrix) {
    for (const auto& row : matrix) {
        for (int val : row) {
            cout << val << " ";
        }
        cout << endl;
    }
}

vector<int> bfs(const vector<vector<int>>& incMatrix, const vector<Edge>& edges, int src) {
    int V = incMatrix[0].size();
    vector<int> dist(V, INT_MAX);
    queue<int> q;

    q.push(src);
    dist[src] = 0;

    while (!q.empty()) {
        int u = q.front();
        q.pop();

        for (size_t i = 0; i < edges.size(); ++i) {
            if (incMatrix[i][u] == 1) {
                int v = (edges[i].u == u) ? edges[i].v : edges[i].u;
                if (dist[v] == INT_MAX) {
                    dist[v] = dist[u] + 1;
                    q.push(v);
                }
            }
        }
    }

    return dist;
}

vector<int> findEccentricities(const vector<vector<int>>& incMatrix, const vector<Edge>& edges) {
    int V = incMatrix[0].size();
    vector<int> ecc(V, 0);

    for (int i = 0; i < V; ++i) {
        vector<int> dist = bfs(incMatrix, edges, i);
        ecc[i] = *max_element(dist.begin(), dist.end());
    }

    return ecc;
}

int findRadius(const vector<int>& ecc) {
    return *min_element(ecc.begin(), ecc.end());
}

vector<int> findCentralVertices(const vector<int>& ecc, int radius) {
    vector<int> centralVertices;

    for (size_t i = 0; i < ecc.size(); ++i) {
        if (ecc[i] == radius) {
            centralVertices.push_back(i);
        }
    }

    return centralVertices;
}

vector<vector<int>> buildRadiusGraph(const vector<vector<int>>& incMatrix, const vector<Edge>& edges, int radius) {
    int V = incMatrix[0].size();
    vector<Edge> radiusEdges;

    for (int u = 0; u < V; ++u) {
        vector<int> dist = bfs(incMatrix, edges, u);
        for (int v = 0; v < V; ++v) {
            if (dist[v] <= radius && u != v) {
                radiusEdges.push_back({ u, v });
            }
        }
    }

    vector<vector<int>> radiusGraphIncMatrix(radiusEdges.size(), vector<int>(V, 0));
    for (size_t i = 0; i < radiusEdges.size(); ++i) {
        addEdge(radiusGraphIncMatrix, i, radiusEdges[i].u, radiusEdges[i].v);
    }

    return radiusGraphIncMatrix;
}

void parseGWF(ifstream& file, vector<Edge>& edges, int exampleNumber) {
    string line;
    bool readingEdges = false;
    int currentExample = 0;

    while (getline(file, line)) {
        if (line.find("# Example") != string::npos) {
            currentExample = stoi(line.substr(line.find("Example") + 8));
            edges.clear();
            readingEdges = false;
        }

        if (currentExample == exampleNumber) {
            if (line == "vertices:") {
                readingEdges = false;
            }
            else if (line == "edges:") {
                readingEdges = true;
            }
            else if (line == "end") {
                break;
            }
            else if (readingEdges) {
                stringstream ss(line);
                int u, v;
                ss >> u >> v;
                edges.push_back({ u, v });
            }
        }
    }
}

void runExampleFromGWF(ifstream& file, int exampleNumber, streampos& lastPosition) {
    file.clear();
    file.seekg(lastPosition);

    vector<Edge> edges;
    parseGWF(file, edges, exampleNumber);

    if (edges.empty()) {
        cerr << "Error: No edges found for example " << exampleNumber << endl;
        return;
    }

    int V = 5;
    vector<vector<int>> incMatrix(edges.size(), vector<int>(V, 0));

    for (size_t i = 0; i < edges.size(); ++i) {
        addEdge(incMatrix, i, edges[i].u, edges[i].v);
    }

    cout << "Example " << exampleNumber << ":" << endl;
    cout << "Original Incidence Matrix:" << endl;
    printMatrix(incMatrix);

    vector<int> ecc = findEccentricities(incMatrix, edges);
    int radius = findRadius(ecc);
    vector<int> centralVertices = findCentralVertices(ecc, radius);

    cout << "Eccentricities: ";
    for (int e : ecc) {
        cout << e << " ";
    }
    cout << endl;

    cout << "Radius: " << radius << endl;
    cout << "Central Vertices: ";
    for (int v : centralVertices) {
        cout << v << " ";
    }
    cout << endl;

    vector<vector<int>> radiusGraphIncMatrix = buildRadiusGraph(incMatrix, edges, radius);
    cout << "Radius Graph Incidence Matrix:" << endl;
    printMatrix(radiusGraphIncMatrix);
    cout << "--------------------------------" << endl;

    lastPosition = file.tellg();
}

int main() {
    ifstream file("graph.gwf");
    if (!file.is_open()) {
        cerr << "Error: Unable to open file graph.gwf" << endl;
        return 1;
    }

    streampos lastPosition = file.tellg();

    for (int i = 1; i <= 5; ++i) {
        runExampleFromGWF(file, i, lastPosition);
    }

    file.close();
    return 0;
}
