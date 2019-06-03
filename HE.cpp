#include "HE.h"

HE::HE(Eigen::MatrixXi &faces, int N) :
	faces(faces),
	he_edges(std::vector<Edge>(faces.rows() * 3)),
	he_verts(std::vector<int>(N)),
	he_faces(std::vector<int>(faces.rows())) {
}

void HE::buildHEs() {
	for (int i = 0; i < faces.rows(); i++) {
		for (int j = 0; j < 3; j++) {
			int v = faces(i, j);
			int v_next = faces(i, (j + 1) % 3);
			int edges_index = i * 3 + j;
			Edge he = Edge();
			he.he_vert = v_next;
			he.he_face = i;
			he.he_next = i * 3 + (j + 1) % 3;
			edges_map[std::pair<int, int>(v, v_next)] = edges_index;
			he_edges[edges_index] = he;
			he_verts[v] = edges_index;
		}
		he_faces[i] = i * 3;
	}
	// get pairs
	std::map<std::pair<int, int>, int>::iterator it;
	for (it = edges_map.begin(); it != edges_map.end(); ++it) {
		int edge_index = it->second;
		// not yet have a pair
		if (he_edges[edge_index].he_pair < 0) {
			int start_v = it->first.first;
			int end_v = it->first.second;
			auto pair_index_it = edges_map.find(std::pair<int, int>(end_v, start_v));
			if (pair_index_it != edges_map.end()) {
				he_edges[edge_index].he_pair = pair_index_it->second;
			}
		}
	}
}

std::shared_ptr<std::vector<int>> HE::getNeighbors(int v_index) {
	std::shared_ptr<std::vector<int>> neighbors = std::make_shared<std::vector<int>>();
	int he_index = he_verts[v_index];
	// append first neighbor
	neighbors->push_back(he_edges[he_index].he_vert);
	
	int next_he_indx = he_edges[he_edges[he_index].he_pair].he_next;
	int nb_indx;
	while (next_he_indx != he_index) {
		nb_indx = he_edges[next_he_indx].he_vert;
		neighbors->push_back(nb_indx);
		next_he_indx = he_edges[he_edges[next_he_indx].he_pair].he_next;
	} 
	return neighbors;
}