#pragma once
#include <vector>
#include <Eigen/Dense>
#include <map>
#include <tuple>
#include <memory>

class HE {
public:
	HE(Eigen::MatrixXi &faces, int N);
	void buildHEs();
	std::shared_ptr<std::vector<int>> getNeighbors(int v_index);

private:
	class Edge {
	public:
		int he_vert;
		int he_pair = -1;
		int he_face;
		int he_next;
	};

	Eigen::MatrixXi &faces;
	std::vector<int> he_verts;
	std::vector<Edge> he_edges;
	std::vector<int> he_faces;
	std::map<std::pair<int, int>, int> edges_map;
};
