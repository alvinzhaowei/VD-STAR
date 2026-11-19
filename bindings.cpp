#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <vector>
#include <utility>
#include "MyLib/MyVector.h"
#include "graph/Vertex.h"
#include "graph/Graph.h"

namespace py = pybind11;

class VDStarEngine {
private:
    int n_;
    Graph* graph_;

public:
    // Accepts 0-indexed edges [(u, v), ...] where u, v in [0, n - 1]
    VDStarEngine(int n, const std::vector<std::pair<int, int>>& edges, double rho = 0.05)
        : n_(n), graph_(nullptr) {
        dynscan::Vertex::reset_intersection_cnt_manager();

        MyVector<dynscan::Vertex *> vList;
        vList.reserve(n);
        for (int i = 0; i < n; i++) {
            vList.push_back(new dynscan::Vertex(i + 1)); // 1-based IDs internally
        }

        graph_ = new Graph(vList, rho);
        vList.release_space();

        for (const auto& edge : edges) {
            int u = edge.first + 1;
            int v = edge.second + 1;
            if (u >= 1 && u <= n_ && v >= 1 && v <= n_ && u != v) {
                graph_->insertEdge(u, v);
            }
        }
    }

    ~VDStarEngine() {
        delete graph_;
    }

    // Dynamic update: u, v in [0, n - 1]; op = 1 (insert), op = 0 (remove)
    void update_edge(int u, int v, int op) {
        int u1 = u + 1;
        int v1 = v + 1;
        if (u1 < 1 || u1 > n_ || v1 < 1 || v1 > n_ || u1 == v1) return;
        if (op == 1) {
            graph_->insertEdge(u1, v1);
        } else {
            graph_->removeEdge(u1, v1);
        }
    }

    // Returns clusters of 0-based vertex IDs [[u0, u1, ...], ...]
    std::vector<std::vector<int>> query(double eps, int mu) {
        std::vector<std::vector<int>> raw_clusters = graph_->queryClusters(eps, mu);
        // Convert internal 1-based IDs back to 0-based IDs for Python
        for (auto& cluster : raw_clusters) {
            for (int& vid : cluster) {
                vid -= 1;
            }
        }
        return raw_clusters;
    }
};

PYBIND11_MODULE(vdstar, m) {
    m.doc() = "VD-STAR Dynamic Structural Clustering C++ Extension";

    py::class_<VDStarEngine>(m, "VDStarEngine")
        .def(py::init<int, const std::vector<std::pair<int, int>>&, double>(),
             py::arg("n"),
             py::arg("edges"),
             py::arg("rho") = 0.05)
        .def("update_edge", &VDStarEngine::update_edge,
             py::arg("u"), py::arg("v"), py::arg("op"))
        .def("query", &VDStarEngine::query,
             py::arg("epsilon"), py::arg("mu"));
}