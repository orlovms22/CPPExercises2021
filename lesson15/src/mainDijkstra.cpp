#include <vector>
#include <sstream>
#include <iostream>
#include <stdexcept>

int debugPoint(int line) {
    if (line < 0)
        return 0;

    // You can put breakpoint at the following line to catch any rassert failure:
    return line;
}

#define rassert(condition, message) if (!(condition)) { std::stringstream ss; (ss << "Assertion \"" << message << "\" failed at line " << debugPoint(__LINE__) << "!"); throw std::runtime_error(ss.str()); }


struct Edge {
    int u, v; // номера вершин которые это ребро соединяет
    int w; // длина ребра (т.е. насколько длинный путь предстоит преодолеть переходя по этому ребру между вершинами)

    Edge(int u, int v, int w) : u(u), v(v), w(w)
    {}
};

struct Vertex {
    int path_length;
    bool flag;
    int father;

    Vertex() : path_length(std::numeric_limits<int>::max()), flag(false), father(-1)
    {}
};

void run() {
    // https://codeforces.com/problemset/problem/20/C?locale=ru
    // Не требуется сделать оптимально быструю версию, поэтому если вы получили:
    //
    // Превышено ограничение времени на тесте 31
    //
    // То все замечательно и вы молодец.

    int nvertices, medges;
    std::cin >> nvertices;
    std::cin >> medges;

    std::vector<std::vector<Edge>> edges_by_vertex(nvertices);
    for (int i = 0; i < medges; ++i) {
        int ai, bi, w;
        std::cin >> ai >> bi >> w;
        rassert(ai >= 1 && ai <= nvertices, 23472894792020);
        rassert(bi >= 1 && bi <= nvertices, 23472894792021);

        ai -= 1;
        bi -= 1;
        rassert(ai >= 0 && ai < nvertices, 3472897424024);
        rassert(bi >= 0 && bi < nvertices, 3472897424025);

        Edge edgeAB(ai, bi, w);
        edges_by_vertex[ai].push_back(edgeAB);

        edges_by_vertex[bi].push_back(Edge(bi, ai, w)); // а тут - обратное ребро, можно конструировать объект прямо в той же строчке где он и потребовался
    }

    const int start = 0;
    const int finish = nvertices - 1;

    const int INF = std::numeric_limits<int>::max();

    std::vector<Vertex> vertices(nvertices);
    vertices[0].path_length = 0;

    while (true) {
        int min_path = INF;
        int idx = -1;
        for (int i = 0; i < vertices.size(); i++) {
            if (vertices[i].path_length != INF && !vertices[i].flag && vertices[i].path_length < min_path)
            {
                min_path = vertices[i].path_length;
                idx = i;
            }
        }
        if (idx == -1) break;

        for (int i = 0; i < edges_by_vertex[idx].size(); i++) {
            int new_path = vertices[idx].path_length + edges_by_vertex[idx][i].w;
            if (vertices[edges_by_vertex[idx][i].v].path_length > new_path) {
                vertices[edges_by_vertex[idx][i].v].path_length = new_path;
                vertices[edges_by_vertex[idx][i].v].father = idx;
            }
        }
        vertices[idx].flag = true;
    }

    if (vertices[finish].path_length != INF) {
        std::vector<int> path;
        for (int i = finish; i != start; i = vertices[i].father) {
            path.insert(path.begin(), i);
        }
        path.insert(path.begin(), start);
        for (int i = 0; i < path.size(); i++) {
            std::cout << (path[i] + 1) << " ";
        }
        std::cout << std::endl;
    } else {
        std::cout << -1 << std::endl;
    }
}

int main() {
    try {
        run();

        return 0;
    } catch (const std::exception &e) {
        std::cout << "Exception! " << e.what() << std::endl;
        return 1;
    }
}
