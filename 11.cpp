#include <iostream> 
#include <vector>
#include <queue>
#include <map>

using namespace std;

const int inf = 1 << 28;

struct Energy { // faster than pairs
  int cost;
  int mask;
  Energy(int c, int m) : cost(c), mask(m) {}
};

struct Wormhole {
  int destination;
  int mask;
  Wormhole(int d, int m) : destination(d), mask(m) {}
};

vector<int> dijkstra(vector<vector<Energy> >& energies,
                     vector<vector<Wormhole> >& graph,
                     int numPrimaryColors) {
  
  int numGalaxies = energies.size();
  int maskColors = 1 << numPrimaryColors;
  int numStates = numGalaxies * maskColors;
  
  vector<int> distance(numStates, inf);
  vector<bool> visited(numStates, false);
  distance[0] = 0;
  priority_queue<pair<int, int> > queue;
  queue.push(pair<int, int>(0, 0));
  while (not queue.empty()) {
    int state = queue.top().second;
    queue.pop();
    
    if (visited[state]) continue;
    visited[state] = true;
    int galaxy = state / maskColors;
    int mask = state % maskColors;
    
    // Traversing wormholes
    for (int i = 0; i < graph[galaxy].size(); i++) {
      int destination = graph[galaxy][i].destination;
      int colorNeeded = graph[galaxy][i].mask;
      if (colorNeeded & ~mask) continue; // not a subset
      int colorAfter = mask & ~colorNeeded;
      int newState = destination * maskColors + colorAfter;
      int newDistance = distance[state]; // distance 0
      if (distance[newState] > newDistance) {
        distance[newState] = newDistance;
        queue.push(pair<int, int>(-newDistance, newState));
      }
    }
    
    // Recovering energy
    for (int i = 0; i < energies[galaxy].size(); i++) {
      int time = energies[galaxy][i].cost;
      int newState = state | energies[galaxy][i].mask;
      int newDistance = distance[state] + time;
      if (distance[newState] > newDistance) {
        distance[newState] = newDistance;
        queue.push(pair<int, int>(-newDistance, newState));
      }
    }
  }
  
  // Computing the answer
  vector<int> result(numGalaxies, -1);
  for (int i = 0; i < numStates; i++) {
    if (distance[i] < inf) {
      int galaxy = i / maskColors;
      if (result[galaxy] == -1 || result[galaxy] > distance[i])
        result[galaxy] = distance[i];
    }
  }
  return result;
}

int main() {
  int test_cases;
  cin >> test_cases;
  for (int case_num = 1; case_num <= test_cases; ++case_num) {
    
    // Although it's not clear from the statement, we assume that the
    // composed colors are composed of colors that appear before them.
    int numColors;
    cin >> numColors;
    int numPrimaryColors = 0;
    
    // From color name to bitmask of primary colors, in order of occurrence.
    map<string, int> nameToMask;
    for (int i = 0; i < numColors; i++) {
      string nameColor;
      int composed;
      cin >> nameColor >> composed;
      if (composed == 0) { // primary
        nameToMask[nameColor] = 1 << numPrimaryColors;
        numPrimaryColors++;
      } else { // composed
        int mask = 0;
        for (int j = 0; j < composed; j++) {
          string subcolorName;
          cin >> subcolorName;
          mask |= nameToMask[subcolorName];
        }
        nameToMask[nameColor] = mask;
      }
    }
    
    int numGalaxies;
    cin >> numGalaxies;
    vector<vector<Energy> > energies(numGalaxies);
    for (int i = 0; i < numGalaxies; i++) {
      int numEnergies;
      cin >> numEnergies;
      for (int j = 0; j < numEnergies; j++) {
        string colorName;
        int energyCost;
        cin >> colorName >> energyCost;
        energies[i].push_back(Energy(energyCost, nameToMask[colorName]));
      }
    }
    
    int numWormholes;
    cin >> numWormholes;
    vector<vector<Wormhole> > graph(numGalaxies);
    for (int i = 0; i < numWormholes; i++) {
      string colorName;
      int origin, destination;
      cin >> colorName >> origin >> destination;
      graph[origin].push_back(Wormhole(destination, nameToMask[colorName]));
    }
    
    // Shall the result fit in an integer? Fingers crossed.
    vector<int> result = dijkstra(energies, graph, numPrimaryColors);
    cout << "Case #" << case_num << ":";
    for (int i = 0; i < numGalaxies; i++)
      cout << " " << result[i];
    cout << endl;
  }
}
