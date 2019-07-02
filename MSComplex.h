#pragma once
#include "SteepLine.h"
#include "HE.h"
#include <memory>
#include <algorithm>
#include <tuple>

namespace MSComplex {

	std::shared_ptr<SteepLine> SL;
	std::shared_ptr<HE> he;
	// vertex connected to, index of steep line, forward
	std::shared_ptr<std::vector<std::vector<std::tuple<int, int, bool>>>> adj_list;
	std::shared_ptr<std::vector<int>> adj_index;
	std::map<int, std::vector<int>> sl_to_region;

	struct ms_region_t {
		std::vector<int> nodes;
		std::vector<int> region_verts;
	};

	std::shared_ptr<std::vector<ms_region_t>> ms_regions;

	bool onSteepLine(int v_index, int &sl_index);
	void DFS_rec(int start_index, std::vector<int> &sl_indices, std::vector<bool> &visited);
	int findAVertInRegion(ms_region_t &ms_region);
	void fillMsComplex();
	void DFS(int start_index, std::vector<int> &region_verts);

	void setSL(std::shared_ptr<SteepLine> steepLine) {
		SL = steepLine;
	}

	void setHE(std::shared_ptr<HE> halfEdge) {
		he = halfEdge;
	}

	int v2AdjIndex(int v) {
		int v_index = std::distance(adj_index->begin(),
			(std::find(adj_index->begin(), adj_index->end(), v)));
		return v_index;
	}

	void buildAdjMatrix() {
		// build index. saddles - mins - maxs
		std::shared_ptr<std::vector<int>> mins = SL->getMins();
		std::shared_ptr<std::vector<int>> maxs = SL->getMaxs();
		std::shared_ptr<std::vector<int>> saddles = SL->getSaddles();
		adj_index = std::make_shared<std::vector<int>>(mins->size()+maxs->size()+saddles->size());
		for (int i = 0; i < saddles->size(); i++) {
			(*adj_index)[i] = (*saddles)[i];
		}
		for (int i = 0; i <  mins->size(); i++) {
			(*adj_index)[saddles->size() + i] = (*mins)[i];
		}
		for (int i = 0; i < maxs->size(); i++) {
			(*adj_index)[saddles->size() + mins->size() + i] = (*maxs)[i];
		}
		// build adj_list 
		auto steepLines = SL->getSteepLines();
		adj_list = std::make_shared<std::vector<std::vector<std::tuple<int, int, bool>>>>(adj_index->size());
		for (int i = 0; i < steepLines->size(); i++) {
			int start = (*steepLines)[i][0];
			int end = (*steepLines)[i].back();
			int start_index = v2AdjIndex(start);
			int end_index = v2AdjIndex(end);
			(adj_list->at(start_index)).push_back(std::make_tuple(end_index, i, true));
			(adj_list->at(end_index)).push_back(std::make_tuple(start_index, i, false));
		}
		// order adj_list so that neighbors are in cw order
		for (int i = 0; i < adj_list->size(); i++) {
			std::vector<int> next_in_sl(adj_list->at(i).size());
			std::vector<std::tuple<int, int, bool>> ordered_list;
			int v_index = adj_index->at(i);
			auto neighbors = he->getNeighbors(v_index);
			for (int j = 0; j < next_in_sl.size(); j++) {
				int sl_index = std::get<1>(adj_list->at(i)[j]);
				bool forward = std::get<2>(adj_list->at(i)[j]);
				if (forward) {
					next_in_sl[j] = steepLines->at(sl_index)[1];
				}
				else {
					next_in_sl[j] = steepLines->at(sl_index).end()[-2];
				}
			}
			// order steepline indices in the order of neighbors
			for (int j = 0; j < neighbors->size(); j++) {
				for (int k = 0; k < next_in_sl.size(); k++) {
					// get a neighbor that's also a next_in_sl 
					if (next_in_sl[k] == neighbors->at(j)) {
						ordered_list.push_back(adj_list->at(i)[k]);
					}
				}
			}
			// swap ordered list into adj_list
			adj_list->at(i) = ordered_list;
		}
	}

	std::tuple<int, int, bool> getNextSL(int start, int end) {
		int i = 0;
		int nb_index = -1;
		int nb_num = adj_list->at(start).size();
		for (i = 0; i < nb_num; i++) {
			if (std::get<0>(adj_list->at(start)[i]) == end) {
				nb_index = i;
				break;
			}
		}
		return adj_list->at(start)[(nb_index + 1) % nb_num];
	}
	
	std::tuple<int, int, bool> getSL(int start, int end) {
		int i = 0;
		int nb_index = -1;
		int nb_num = adj_list->at(start).size();
		for (i = 0; i < nb_num; i++) {
			if (std::get<0>(adj_list->at(start)[i]) == end) {
				nb_index = i;
				return adj_list->at(start)[nb_index];
			}
		}
	}

	void connectSL2Region() {
		for (int i = 0; i < ms_regions->size(); i++) {
			for (int j = 0; j < 4; j++) {
				int node_index = ms_regions->at(i).nodes[j];
				int next_node_index = ms_regions->at(i).nodes[(j+1)%4];
				auto sl_info = getSL(node_index, next_node_index);

			}
		}
	}
	std::shared_ptr<std::vector<ms_region_t>> buildMSComplex() {
		// if already built
		if (ms_regions) {
			return ms_regions;
		}
		ms_regions = std::make_shared<std::vector<ms_region_t>>();
		//int N = adj_index->size();
		auto steepLines = SL->getSteepLines();
		// sl1(forward) sl1(backward) sl2(forward) sl2(backward) ...
		std::vector<bool> visited(2* steepLines->size());
		// iterate over all steep lines
		for (int i = 0; i < steepLines->size(); i++) {
			// forward and backward
			for (int k = 0; k < 2; k++) {
				ms_region_t ms_region;
				int cur_index, next_index, initial_index;
				bool is_visited;
				if (k == 0) {
					cur_index = v2AdjIndex(steepLines->at(i)[0]);
					next_index = v2AdjIndex(steepLines->at(i).end()[-1]);
					is_visited = visited[2 * i];
				}
				else {
					cur_index = v2AdjIndex(steepLines->at(i).end()[-1]);
					next_index = v2AdjIndex(steepLines->at(i)[0]);
					is_visited = visited[2 * i + 1];
				}
				if (is_visited) {
					continue;
				}
				initial_index = cur_index;
				ms_region.nodes.push_back(cur_index);
				//ms_region.nodes.push_back(next_index);
				do {
					int prev_cur_index = cur_index;
					cur_index = next_index;
					next_index = std::get<0>(getNextSL(next_index, prev_cur_index));
					ms_region.nodes.push_back(cur_index);
					std::tuple<int, int, bool> sl_data = getSL(cur_index, next_index);
					if (std::get<2>(sl_data)) {
						visited[2 * std::get<1>(sl_data)] = true;
					}
					else {
						visited[2 * std::get<1>(sl_data) + 1] = true;
					}
				} while (next_index != initial_index);
				ms_regions->push_back(ms_region);
			}
		}
		fillMsComplex();
		return ms_regions;
	}

	void fillMsComplex() {
		for (int i = 0; i < ms_regions->size(); i++) {
			int v_in_region = findAVertInRegion(ms_regions->at(i));
			DFS(v_in_region, (ms_regions->at(i)).region_verts);
		}
	}

	int findAVertInRegion(ms_region_t &ms_region) {
		// try each steep line
		for (int i = 0; i < 4; i++) {
			auto steepLines = SL->getSteepLines();
			int node1 = ms_region.nodes[i];
			int node2 = ms_region.nodes[(i+1)%4];
			std::tuple<int, int, bool> sl_data = getSL(node1, node2);
			int sl_index = std::get<1>(sl_data);
			bool forward = std::get<2>(sl_data);
			int sl_mid_index = steepLines->at(sl_index).size() / 2;
			int sl_mid_v = steepLines->at(sl_index)[sl_mid_index];
			// the next 
			int sl_mid_next, sl_mid_prev;
			if (forward) {
				sl_mid_next = steepLines->at(sl_index)[sl_mid_index + 1];
				sl_mid_prev = steepLines->at(sl_index)[sl_mid_index - 1];
			}
			else {
				sl_mid_prev = steepLines->at(sl_index)[sl_mid_index + 1];
				sl_mid_next = steepLines->at(sl_index)[sl_mid_index - 1];
			}
			// get the neighbor on the right
			auto neighbors = he->getNeighbors(sl_mid_v);
			int prev_index = std::distance( neighbors->begin(), std::find(neighbors->begin(), neighbors->end(), sl_mid_prev));
			int next_index = std::distance( neighbors->begin(), std::find(neighbors->begin(), neighbors->end(), sl_mid_next));
			int nbs_size = neighbors->size();
			// if there are neighbors after next, before prev
			if ((next_index + 1) % nbs_size != prev_index) {
				return neighbors->at((next_index + 1) % neighbors->size());
			}
		}
		return -1;
	}

	void DFS(int start_index, std::vector<int> &region_verts) {
		int N = he->getNumVerts();
		std::vector<bool> visited(N);
		std::vector<int> sl_indices;
		DFS_rec(start_index, sl_indices, visited);
		for (int i = 0; i < N; i++) {
			if (visited.at(i)) {
				region_verts.push_back(i);
			}
		}
	}

	void DFS_rec(int start_index, std::vector<int> &sl_indices, std::vector<bool> &visited) {
		// mark start index visited
		visited.at(start_index) = true;
		// get neighbors
		std::shared_ptr<std::vector<int>> neighbors = he->getNeighbors(start_index);
		// recursively visit neightbors
		for (int i = 0; i < neighbors->size(); i++) {
			// if not visited
			int nb = neighbors->at(i);
			if (!visited.at(nb)) {
				int touched_sl_index;
				if (onSteepLine(nb, touched_sl_index)) {
					visited.at(nb) = true;
					// something related to boundary
					if (std::find(sl_indices.begin(), sl_indices.end(), touched_sl_index) == sl_indices.end()) {
						sl_indices.push_back(touched_sl_index);
					}
				}
				else {
					DFS_rec(nb, sl_indices, visited);
				}
			}
		}
	}

	bool onSteepLine(int v_index, int &sl_index) {
		auto steepLines = SL->getSteepLines();
		for (int i = 0; i < steepLines->size(); i++) {
			if (std::find(steepLines->at(i).begin(), steepLines->at(i).end(), v_index) != steepLines->at(i).end()) {
				sl_index = i;
				return true;
			}
		}
		return false;
	}
}