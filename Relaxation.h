#pragma once
#include "MSComplex.h"
#include <Eigen/Sparse>
#include<Eigen/SparseLU>

namespace Relaxation {

	std::shared_ptr<std::vector<int>> node_index;
	std::shared_ptr<HE> he;

	std::shared_ptr<std::vector<int>> mtx_index;
	std::shared_ptr<std::vector<int>> partitions;
	std::shared_ptr<std::vector<MSComplex::ms_region_t>> ms_regions;

	// mass matrix
	std::shared_ptr<Eigen::SparseMatrix<double>> L;
	std::shared_ptr<Eigen::SparseMatrix<double>> calc_u_mtx;
	// U
	std::shared_ptr<Eigen::VectorXd> us;

	// need to know neighbors of vertices
	void setHE(std::shared_ptr<HE> halfEdge) {
		he = halfEdge;
	}

	// need to know which vertices are nodes of the complex
	void setNodeIndex(std::shared_ptr<std::vector<int>> p_node_index) {
		node_index = p_node_index;
	}

	// need to know which partition each vertex belongs to
	void setPartitions(std::shared_ptr<std::vector<int>> p_partitions) {
		partitions = p_partitions;
	}

	// need to know the weight of each edge
	void setMassMtx(std::shared_ptr<Eigen::SparseMatrix<double>> p_M) {
		L = p_M;
	}

	// need to know the nodes that each patch contains
	void setMSregions(std::shared_ptr<std::vector<MSComplex::ms_region_t>> p_ms_regions) {
		ms_regions = p_ms_regions;
	}

	void setInfos(std::shared_ptr<HE> halfEdge, std::shared_ptr<std::vector<int>> p_node_index,
		std::shared_ptr<std::vector<int>> p_partitions,
		std::shared_ptr<Eigen::SparseMatrix<double>> p_M, 
		std::shared_ptr<std::vector<MSComplex::ms_region_t>> p_ms_regions) {
		setHE(halfEdge);
		setNodeIndex(p_node_index);
		setPartitions(p_partitions);
		setMassMtx(p_M);
		setMSregions(p_ms_regions);
	}

	void setMtxIndices() {
		mtx_index = std::make_shared<std::vector<int>>(he->getNumVerts() - node_index->size());
		int cnt = 0;
		for (int i = 0; i < he->getNumVerts(); i++) {
			// vertex not a node
			if (std::find(node_index->begin(), node_index->end(), i) == node_index->end()) {
				mtx_index->at(cnt) = i;
				cnt++;
			}
		}
	}

	double getWeight(int v1, int v2) {
		return -(L->coeff(v1, v2));
	}

	void solveU() {
		Eigen::SparseMatrix<double> calc_u_mtx = Eigen::SparseMatrix<double>(mtx_index->size(), mtx_index->size());
		Eigen::VectorXd known_us = Eigen::VectorXd(mtx_index->size());

		for (int row = 0; row < mtx_index->size(); row++) {
			calc_u_mtx.insert(row, row) = -1.0;
			// get neighbors of vertex
			int v_index = mtx_index->at(row);
			// the patch vertex is in
			int region_index = partitions->at(v_index);

			auto neighbors = he->getNeighbors(v_index);
			std::vector<double> n_weights(neighbors->size());
			double sum = 0;
			for (int i = 0; i < neighbors->size(); i++) {
				n_weights[i] = getWeight(v_index, neighbors->at(i));
				sum += n_weights[i];
			}
			// normalize weights
			for (int i = 0; i < neighbors->size(); i++) {
				n_weights[i] /= sum;
			}
			for (int i = 0; i < neighbors->size(); i++) {
				int nb = neighbors->at(i);
				// if nb is a node of a patch
				auto node_it = std::find(node_index->begin(), node_index->end(), nb);
				if (node_it != node_index->end()) {
					std::vector<int> region_nodes = std::vector<int>(4);
					for (int j = 0; j < 4; j++) {
						region_nodes[j] = node_index->at(ms_regions->at(region_index).nodes[j]);
					}
					auto patch_node_it = std::find(region_nodes.begin(), region_nodes.end(), nb);
					// if nb is a node of this patch
					if (patch_node_it != region_nodes.end()) {
						int node_type = std::distance(region_nodes.begin(), patch_node_it);
						// if up-left or bot-left node, u is 0, otherwise it's 1
						double known_u = 1;
						if (node_type == 2 || node_type == 3) {
							known_u = 0;
						}
						// put it in the knowns
						known_us(row) -= n_weights[i] * known_u;
					}
					// if nb is a node of another patch
					// but it should not happen??
					else {
						int nb_region = partitions->at(nb);
						// fill region nodes with the nodes of that patch
						for (int j = 0; j < 4; j++) {
							region_nodes[j] = node_index->at(ms_regions->at(nb_region).nodes[j]);
						}
						auto nb_path_node_it = std::find(region_nodes.begin(), region_nodes.end(), nb);
						glm::vec2 trans_func = MSComplex::findTransFunc(region_index, nb_region);
						double known_u = 1;
						// didn't find nb as a node of the patch it is in, smth wrong
						if (nb_path_node_it == region_nodes.end()) {
							std::cout << "something wrong" << std::endl;
						}
						int node_type = std::distance(region_nodes.begin(), nb_path_node_it);
						if (node_type == 2 || node_type == 3) {
							known_u = 0;
						}
						known_us(row) -= n_weights[i] * (trans_func[0] + known_u);
					}
				}
				// nb is not a node of a patch
				else {
					int nb_region = partitions->at(nb);
					// nb in a different patch
					if (nb_region != region_index) {
						glm::vec2 trans_func = MSComplex::findTransFunc(region_index, nb_region);
						known_us(row) -= -n_weights[i] * trans_func[0];
					}
					int nb_index = std::distance(mtx_index->begin(), std::find(mtx_index->begin(), mtx_index->end(), nb));
					calc_u_mtx.coeffRef(row, nb_index) += n_weights[i];
				}
			}
		}
		calc_u_mtx.makeCompressed();
		Eigen::SparseLU<Eigen::SparseMatrix<double> > solver;
		solver.analyzePattern(calc_u_mtx);
		solver.factorize(calc_u_mtx);

		us = std::make_shared<Eigen::VectorXd>(known_us.rows());
		*us = solver.solve(known_us);
		for (int u_index = 0; u_index < us->rows(); u_index++) {
			std::cout << (*us)(u_index) << std::endl;
		}
	}

}