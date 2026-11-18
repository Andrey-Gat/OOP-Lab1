
#include <iostream>
#include <vector>
#include <array>
#include <queue>
#include <stack>
#include <algorithm>
#include <random>
#include <cassert>
#include <stdexcept>
#include <functional>
#include <numeric>

using namespace std;

enum Direction {
    UP = 0,
    RIGHT = 1,
    DOWN = 2,
    LEFT = 3
};

Direction opposite(Direction d) {
    return static_cast<Direction>((d + 2) % 4);
}

template <typename T>
class Maze {
private:
    struct Cell {
        T data{};
        array<bool, 4> passages{ false, false, false, false };
    };

    int rows;
    int cols;
    vector<Cell> cells;

    int index(int row, int col) const {
        return row * cols + col;
    }

public:
    Maze(int r, int c) : rows(r), cols(c) {
        if (r < 2 || c < 2) {
            throw invalid_argument("Размер лабиринта должен быть не менее 2x2");
        }

        cells.resize(rows * cols);
    }

    int getRows() const { return rows; }
    int getCols() const { return cols; }
    int size() const { return rows * cols; }


    bool getNeighbor(int cell, Direction d, int& neighbor) const {
        int row = cell / cols;
        int col = cell % cols;

        switch (d) {
        case UP:    row--; break;
        case RIGHT: col++; break;
        case DOWN:  row++; break;
        case LEFT:  col--; break;
        }

        if (row < 0 || row >= rows || col < 0 || col >= cols) {
            return false;
        }

        neighbor = index(row, col);
        return true;
    }

    
    void createPassage(int cell, Direction d) {
        int neighbor;

        if (cell < 0 || cell >= size() ||
            !getNeighbor(cell, d, neighbor)) {
            throw out_of_range("Нельзя создать проход за границами лабиринта");
        }

        cells[cell].passages[d] = true;
        cells[neighbor].passages[opposite(d)] = true;
    }

    
    bool canMove(int cell, Direction d) const {
        int neighbor;

        return cell >= 0 && cell < size() &&
            getNeighbor(cell, d, neighbor) &&
            cells[cell].passages[d];
    }


    T& getData(int cell) {
        return cells.at(cell).data;
    }

    const T& getData(int cell) const {
        return cells.at(cell).data;
    }

 
    int passageCount() const {
        int count = 0;

        for (int i = 0; i < size(); i++) {
            if (canMove(i, RIGHT)) count++;
            if (canMove(i, DOWN)) count++;
        }

        return count;
    }
};


template <typename T, typename Generator>
void generateMaze(Maze<T>& maze, Generator& gen) {
    vector<bool> visited(maze.size(), false);
    stack<int> path;

    uniform_int_distribution<int> startDist(0, maze.size() - 1);

    int start = startDist(gen);

    visited[start] = true;
    path.push(start);

    while (!path.empty()) {
        int current = path.top();

        vector<Direction> directions = { UP, RIGHT, DOWN, LEFT };
        shuffle(directions.begin(), directions.end(), gen);

        bool moved = false;

        for (Direction d : directions) {
            int neighbor;

            if (maze.getNeighbor(current, d, neighbor) &&
                !visited[neighbor]) {

                maze.createPassage(current, d);

                visited[neighbor] = true;
                path.push(neighbor);

                moved = true;
                break;
            }
        }

        if (!moved) {
            path.pop();
        }
    }
}

template <typename T, typename NeighborOrder>
vector<int> findPath(
    const Maze<T>& maze,
    int start,
    int finish,
    NeighborOrder order
) {
    if (start < 0 || start >= maze.size() ||
        finish < 0 || finish >= maze.size()) {
        throw out_of_range("Некорректная начальная или конечная клетка");
    }

    vector<int> parent(maze.size(), -1);
    vector<bool> visited(maze.size(), false);
    queue<int> q;

    q.push(start);
    visited[start] = true;

    while (!q.empty()) {
        int current = q.front();
        q.pop();

        if (current == finish) {
            break;
        }

        vector<Direction> directions = { UP, RIGHT, DOWN, LEFT };
        order(directions);

        for (Direction d : directions) {
            int neighbor;

            if (maze.canMove(current, d) &&
                maze.getNeighbor(current, d, neighbor) &&
                !visited[neighbor]) {

                visited[neighbor] = true;
                parent[neighbor] = current;
                q.push(neighbor);
            }
        }
    }


    if (!visited[finish]) {
        return {};
    }


    vector<int> result;

    for (int v = finish; v != -1; v = parent[v]) {
        result.push_back(v);
    }

    reverse(result.begin(), result.end());

    return result;
}


template <typename T>
void printMaze(
    const Maze<T>& maze,
    const vector<int>& route,
    int start,
    int finish
) {
    vector<bool> onRoute(maze.size(), false);

    for (int cell : route) {
        onRoute[cell] = true;
    }

    for (int r = 0; r < maze.getRows(); r++) {

      
        for (int c = 0; c < maze.getCols(); c++) {
            int cell = r * maze.getCols() + c;

            cout << "+";
            cout << (maze.canMove(cell, UP) ? "   " : "---");
        }

        cout << "+\n";


        for (int c = 0; c < maze.getCols(); c++) {
            int cell = r * maze.getCols() + c;

            cout << (maze.canMove(cell, LEFT) ? " " : "|");

            char symbol = ' ';

            if (cell == start) symbol = 'S';
            else if (cell == finish) symbol = 'F';
            else if (onRoute[cell]) symbol = '*';

            cout << " " << symbol << " ";
        }

        cout << "|\n";
    }

   
    for (int c = 0; c < maze.getCols(); c++) {
        cout << "+---";
    }

    cout << "+\n";
}


template <typename T>
bool isConnected(const Maze<T>& maze) {
    vector<bool> visited(maze.size(), false);
    queue<int> q;

    q.push(0);
    visited[0] = true;

    int count = 0;

    while (!q.empty()) {
        int current = q.front();
        q.pop();
        count++;

        for (Direction d : {UP, RIGHT, DOWN, LEFT}) {
            int neighbor;

            if (maze.canMove(current, d) &&
                maze.getNeighbor(current, d, neighbor) &&
                !visited[neighbor]) {

                visited[neighbor] = true;
                q.push(neighbor);
            }
        }
    }

    return count == maze.size();
}

int main() {
    mt19937 gen(random_device{}());


    auto neighborOrder = [](vector<Direction>& directions) {
        
        };

   
    Maze<int> maze1(5, 7);

    generateMaze(maze1, gen);

    int start = 0;
    int finish = maze1.size() - 1;

    vector<int> route = findPath(
        maze1, start, finish, neighborOrder
    );

    cout << "LABYRINTH 1 (5x7)\n\n";

    printMaze(maze1, route, start, finish);

    cout << "\nRoute: ";

    for (int cell : route) {
        cout << "(" << cell / maze1.getCols()
            << "," << cell % maze1.getCols() << ") ";
    }

    cout << "\nPath length: " << route.size() << "\n";


    Maze<string> maze2(4, 4);

    generateMaze(maze2, gen);

    vector<int> route2 = findPath(
        maze2, 0, maze2.size() - 1, neighborOrder
    );

    cout << "\nLABYRINTH 2 (4x4)\n\n";

    printMaze(maze2, route2, 0, maze2.size() - 1);


    assert(isConnected(maze1));
    assert(isConnected(maze2));

   
    assert(maze1.passageCount() == maze1.size() - 1);
    assert(maze2.passageCount() == maze2.size() - 1);

    assert(!route.empty());
    assert(!route2.empty());

    assert(route.front() == start);
    assert(route.back() == finish);

    assert(route2.front() == 0);
    assert(route2.back() == maze2.size() - 1);


    bool invalidSizeCaught = false;

    try {
        Maze<int> invalidMaze(1, 3);
    }
    catch (const invalid_argument&) {
        invalidSizeCaught = true;
    }

    assert(invalidSizeCaught);

    cout << "\nAll tests passed!\n";

    return 0;
}
