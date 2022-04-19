#include <vector>
#include <sstream>
#include <iostream>
#include <stdexcept>

#include <libutils/rasserts.h>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/types.hpp>


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

// Эта биективная функция по координате пикселя (строчка и столбик) + размерам картинки = выдает номер вершины
int encodeVertex(int row, int column, int nrows, int ncolumns) {
    rassert(row < nrows, 348723894723980017);
    rassert(column < ncolumns, 347823974239870018);
    int vertexId = row * ncolumns + column;
    return vertexId;
}

// Эта биективная функция по номеру вершины говорит какой пиксель этой вершине соовтетствует (эта функция должна быть симметрична предыдущей!)
cv::Point2i decodeVertex(int vertexId, int nrows, int ncolumns) {

    // TODO: придумайте как найти номер строки и столбика пикселю по номеру вершины (просто поймите предыдущую функцию и эта функция не будет казаться сложной)
    int row = vertexId / ncolumns;
    int column = vertexId % ncolumns;

    // сверим что функция симметрично сработала:
    rassert(encodeVertex(row, column, nrows, ncolumns) == vertexId, 34782974923035);

    rassert(row < nrows, 34723894720027);
    rassert(column < ncolumns, 3824598237592030);
    return cv::Point2i(column, row);
}

void run(int mazeNumber) {
    cv::Mat maze = cv::imread("../../../../lesson15/data/mazesImages/maze" + std::to_string(mazeNumber) + ".png");
    rassert(!maze.empty(), 324783479230019);
    rassert(maze.type() == CV_8UC3, 3447928472389020);
    std::cout << "Maze resolution: " << maze.cols << "x" << maze.rows << std::endl;

    int nvertices = maze.rows * maze.cols; // TODO

    const int INF = std::numeric_limits<int>::max();

    std::vector<std::vector<Edge>> edges_by_vertex(nvertices);
    for (int j = 0; j < maze.rows; ++j) {
        for (int i = 0; i < maze.cols; ++i) {
            cv::Vec3b color = maze.at<cv::Vec3b>(j, i);
            unsigned char blue = color[0];
            unsigned char green = color[1];
            unsigned char red = color[2];

            int vertex_id = encodeVertex(j, i, maze.rows, maze.cols);

            // TODO добавьте соотвтетсвующие этому пикселю ребра
            if (i > 0) {
                int side_vertex_id = encodeVertex(j, i - 1, maze.rows, maze.cols);
                int w = (color == maze.at<cv::Vec3b>(j, i - 1)) ? 1 : INF;
                edges_by_vertex[vertex_id].push_back(Edge(vertex_id, side_vertex_id, w));
            }
            if (j > 0) {
                int side_vertex_id = encodeVertex(j - 1, i, maze.rows, maze.cols);
                int w = (color == maze.at<cv::Vec3b>(j - 1, i)) ? 1 : INF;
                edges_by_vertex[vertex_id].push_back(Edge(vertex_id, side_vertex_id, w));
            }
            if (i < maze.cols - 1) {
                int side_vertex_id = encodeVertex(j, i + 1, maze.rows, maze.cols);
                int w = (color == maze.at<cv::Vec3b>(j, i + 1)) ? 1 : INF;
                edges_by_vertex[vertex_id].push_back(Edge(vertex_id, side_vertex_id, w));
            }
            if (j < maze.rows - 1) {
                int side_vertex_id = encodeVertex(j + 1, i, maze.rows, maze.cols);
                int w = (color == maze.at<cv::Vec3b>(j + 1, i)) ? 1 : INF;
                edges_by_vertex[vertex_id].push_back(Edge(vertex_id, side_vertex_id, w));
            }
        }
    }

    int start, finish;
    if (mazeNumber >= 1 && mazeNumber <= 3) { // Первые три лабиринта очень похожи но кое чем отличаются...
        start = encodeVertex(300, 300, maze.rows, maze.cols);
        finish = encodeVertex(0, 305, maze.rows, maze.cols);
    } else if (mazeNumber == 4) {
        start = encodeVertex(154, 312, maze.rows, maze.cols);
        finish = encodeVertex(477, 312, maze.rows, maze.cols);
    } else if (mazeNumber == 5) { // Лабиринт в большом разрешении, добровольный (на случай если вы реализовали быструю Дейкстру с приоритетной очередью)
        start = encodeVertex(1200, 1200, maze.rows, maze.cols);
        finish = encodeVertex(1200, 1200, maze.rows, maze.cols);
    } else if (mazeNumber == 6) { // Первые три лабиринта очень похожи но кое чем отличаются...
        start = encodeVertex(100, 100, maze.rows, maze.cols);
        finish = encodeVertex(0, 102, maze.rows, maze.cols);
    } else {
        rassert(false, 324289347238920081);
    }

    cv::Mat window = maze.clone(); // на этой картинке будем визуализировать до куда сейчас дошла прокладка маршрута

    //std::vector<int> distances(nvertices, INF);
    std::vector<Vertex> vertices(nvertices);
    vertices[start].path_length = 0;

    int count = 0;
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
        if (idx == finish) break;

        for (int i = 0; i < edges_by_vertex[idx].size(); i++) {
            if (edges_by_vertex[idx][i].w == INF) continue;
            int new_path = vertices[idx].path_length + edges_by_vertex[idx][i].w;
            if (vertices[edges_by_vertex[idx][i].v].path_length > new_path) {
                vertices[edges_by_vertex[idx][i].v].path_length = new_path;
                vertices[edges_by_vertex[idx][i].v].father = idx;
            }
        }
        cv::Point2i p = decodeVertex(idx, maze.rows, maze.cols);
        window.at<cv::Vec3b>(p.y, p.x) = cv::Vec3b(0, 255, 0);
        if (count % 100 == 0) {
            cv::imshow("Maze", window);
            cv::waitKey(1);
        }
        count++;
        vertices[idx].flag = true;
    }
    // TODO СКОПИРУЙТЕ СЮДА ДЕЙКСТРУ ИЗ ПРЕДЫДУЩЕГО ИСХОДНИКА

    // TODO в момент когда вершина становится обработанной - красьте ее на картинке window в зеленый цвет и показывайте картинку:
    //    cv::Point2i p = decodeVertex(the_chosen_one, maze.rows, maze.cols);
    //    window.at<cv::Vec3b>(p.y, p.x) = cv::Vec3b(0, 255, 0);
    //    cv::imshow("Maze", window);
    //    cv::waitKey(1);
    // TODO это может тормозить, в таком случае показывайте window только после обработки каждой сотой вершины

    // TODO обозначьте найденный маршрут красными пикселями

    if (vertices[finish].path_length != INF) {
        for (int i = finish; i != start; i = vertices[i].father) {
            cv::Point2i p = decodeVertex(i, maze.rows, maze.cols);
            window.at<cv::Vec3b>(p.y, p.x) = cv::Vec3b(0, 0, 255);
            std::cout << p.x << " " << p.y << std::endl;
            if (count % 100 == 0) {
                cv::imshow("Maze", window);
                cv::waitKey(1);
            }
            count++;
        }
        cv::Point2i p = decodeVertex(start, maze.rows, maze.cols);
        window.at<cv::Vec3b>(p.y, p.x) = cv::Vec3b(255, 0, 0);
        cv::imwrite("../../../../lesson15/data/mazesImages/maze" + std::to_string(mazeNumber) + "_solved.png", window);
    }

    // TODO сохраните картинку window на диск

    std::cout << "Finished!" << std::endl;

    // Показываем результат пока пользователь не насладиться до конца и не нажмет Escape
    while (cv::waitKey(10) != 27) {
        cv::imshow("Maze", window);
    }
}

int main() {
    try {
        int mazeNumber = 6;
        run(mazeNumber);

        return 0;
    } catch (const std::exception &e) {
        std::cout << "Exception! " << e.what() << std::endl;
        return 1;
    }
}
