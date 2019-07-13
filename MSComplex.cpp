#include "MSComplex.h"

namespace MSComplex {

	std::shared_ptr<SteepLine> SL;
	std::shared_ptr<HE> he;
	// vertex connected to, index of steep line, forward
	std::shared_ptr<std::vector<std::vector<std::tuple<int, int, bool>>>> adj_list;
	std::shared_ptr<std::vector<int>> adj_index;
	std::map<int, std::vector<int>> sl_to_region;
	// from patch, to patch: index in transfer functions
	std::map<std::tuple<int, int>, int> trnsfr_fnctns;
	std::shared_ptr<std::vector<ms_region_t>> ms_regions;
	std::shared_ptr<std::vector<int>> partitions;

	// possible transfer functions
	std::vector<glm::vec2> trans_funcs = std::vector<glm::vec2>({
		glm::vec2(-1, 0),
		glm::vec2(0, 1),
		glm::vec2(1, 0),
		glm::vec2(0, -1) });

	void setSL(std::shared_ptr<SteepLine> steepLine) {
		SL = steepLine;
	}

	void setHE(std::shared_ptr<HE> halfEdge) {
		he = halfEdge;
	}

	std::shared_ptr<std::vector<int>> getPartitions() {
		return partitions;
	}

	std::shared_ptr<std::vector<int>> getAdjIndex() {
		return adj_index;
	}

	std::shared_ptr<std::vector<ms_region_t>> getMSRegions() {
		return ms_regions;
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
		adj_index = std::make_shared<std::vector<int>>(mins->size() + maxs->size() + saddles->size());
		for (int i = 0; i < saddles->size(); i++) {
			(*adj_index)[i] = (*saddles)[i];
		}
		for (int i = 0; i < mins->size(); i++) {
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

	int getSLIndex(int start_v, int end_v) {
		auto steepLines = SL->getSteepLines();
		for (int i = 0; i < steepLines->size(); i++) {
			if ((steepLines->at(i)[0] == start_v  && steepLines->at(i).end()[-1] == end_v) ||
				(steepLines->at(i)[0] == end_v  && steepLines->at(i).end()[-1] == start_v)) {
				return i;
			}
		}
		return -1;
	}

	void connectSL2Region() {
		for (int r_index = 0; r_index < ms_regions->size(); r_index++) {
			for (int j = 0; j < 4; j++) {
				int node_index = ms_regions->at(r_index).nodes[j];
				int next_node_index = ms_regions->at(r_index).nodes[(j + 1) % 4];
				int sl_index = getSLIndex(adj_index->at(node_index), adj_index->at(next_node_index));
				if (sl_to_region.find(sl_index) != sl_to_region.end()) {
					sl_to_region.at(sl_index).push_back(r_index);
				}
				else {
					sl_to_region.insert(std::pair<int, std::vector<int>>(sl_index, std::vector<int>{r_index}));
				}

			}
		}
	}

	std::vector<int> getPatchSlIndices(ms_region_t patch) {
		std::vector<int> sl_indices(4);
		for (int j = 0; j < 4; j++) {
			int node_index = patch.nodes[j];
			int next_node_index = patch.nodes[(j + 1) % 4];
			sl_indices[j] = getSLIndex(adj_index->at(node_index), adj_index->at(next_node_index));
		}
		return sl_indices;
	}

	std::vector<int> getNbPatches(int patch_index) {
		std::vector<int> sl_indices = getPatchSlIndices(ms_regions->at(patch_index));
		std::vector<int> nb_regions(4);
		for (int j = 0; j < 4; j++) {
			auto adj_regions = sl_to_region.at(sl_indices[j]);
			if (adj_regions[0] == patch_index) {
				nb_regions[j] = adj_regions[1];
			}
			else {
				nb_regions[j] = adj_regions[0];
			}
		}
		return nb_regions;
	}

	void buildTransFuncsEntries() {
		for (int r_index = 0; r_index < ms_regions->size(); r_index++) {
			std::vector<int> nb_patches = getNbPatches(r_index);
			for (int j = 0; j < 4; j++) {
				int nb_region = nb_patches[j];

				if (trnsfr_fnctns.find(std::make_pair(r_index, nb_region)) == trnsfr_fnctns.end()) {
					// set origin and u direction
					if (r_index == 0) {
						trnsfr_fnctns[std::make_pair(r_index, nb_region)] = j;
					}
					else {
						trnsfr_fnctns[std::make_pair(r_index, nb_region)] = -1;
					}
				}
				else {
					std::cout << "two patches are connected by more than 1 edge" << std::endl;
				}
			}
		}
	}

	void buildTransFuncs() {
		std::shared_ptr<std::vector<bool>> built = std::make_shared<std::vector<bool>>(ms_regions->size(), false);
		std::vector<int> nb_patches = getNbPatches(0);
		// set the first patch as built
		built->at(0) = true;
		// go over neighbors, if nb not built, build trans functions from nb
		for (int nb_patch = 0; nb_patch < 4; nb_patch++) {
			if (!built->at(nb_patches[nb_patch])) {
				buildTransFuncsRec(0, nb_patches[nb_patch], built);
			}
		}
	}

	int getOppositeTransFunc(int func_index) {
		switch (func_index) {
		case 0:
			return 2;
		case 1:
			return 3;
		case 2:
			return 0;
		case 3:
			return 1;
		default:
			std::cout << "function index out of range, cannot find opposite" << std::endl;
			return -1;
		}
	}

	int validateTransFuncs() {
		for (int i = 0; i < ms_regions->size(); i++) {
			std::vector<int> nb_patches = getNbPatches(i);
			for (int j = 0; j < 4; j++) {
				int forward_func_index = trnsfr_fnctns[std::make_pair(i, nb_patches[j])];
				int backward_func_index = trnsfr_fnctns[std::make_pair(nb_patches[j], i)];
				if (getOppositeTransFunc(forward_func_index) != backward_func_index) {
					return 1;
				}
			}
		}
		return 0;
	}

	// to be removed later, just going over the degrees of each node
	int validateComplex() {
		int res = 0;
		std::map<int, int> node_degree;
		for (int i = 0; i < ms_regions->size(); i++) {
			for (int j = 0; j < 4; j++) {
				int node_index = ms_regions->at(i).nodes[j];
				if (node_degree.find(node_index) != node_degree.end()) {
					node_degree[node_index] += 1;
				}
				else {
					node_degree[node_index] = 1;
				}
			}
		}

		for (auto& x: node_degree) {
			if (x.second != 4) {
				res = 1;
			}
			std::cout << x.first << ": " << x.second << std::endl;
		}
		return res;
	}

	void buildTransFuncsRec(int from_patch, int to_patch, std::shared_ptr<std::vector<bool>> built) {
		std::vector<int> nb_patches = getNbPatches(to_patch);
		std::vector<int>::iterator it = std::find(nb_patches.begin(), nb_patches.end(), from_patch);
		// the index of from_patch in neighbors of to_patch
		int from_index = std::distance(nb_patches.begin(), it);
		// index (type) of trans functions from from_patch to to_patch
		int from_function = trnsfr_fnctns[std::make_pair(from_patch, to_patch)];
		// the opposite of trans func above
		int func_index = getOppositeTransFunc(from_function);
		// set to_patch as built
		built->at(to_patch) = true;
		// need to re-order the nodes
		std::vector<int> ordered_nodes(4);
		// fill in the transfunction from this patch to its neighbors
		for (int i = 0; i < 4; i++) {
			ordered_nodes[(from_index + i) % 4] = ms_regions->at(to_patch).nodes[(func_index + i) % 4];
			int nb = nb_patches[(from_index + i) % 4];
			trnsfr_fnctns[std::make_pair(to_patch, nb)] = (func_index + i) % 4;
		}
		// set ordered nodes
		ms_regions->at(to_patch).nodes = ordered_nodes;
		// recursively find trans functions starting from neighbors
		for (int i = 0; i < 4; i++) {
			if (!built->at(nb_patches[i])) {
				buildTransFuncsRec(to_patch, nb_patches[i], built);
			}
		}
	}

	glm::vec2 findTransFunc(int from_patch, int to_patch) {
		auto trans_map_it = trnsfr_fnctns.find(std::make_pair(from_patch, to_patch));
		// if can be found directly
		if (trans_map_it != trnsfr_fnctns.end()) {
			return trans_funcs[trans_map_it->second];
		}
		// if two patches connected by a point
		else {
			std::vector<int> nb_patches = getNbPatches(from_patch);
			// check if there's path from nb
			for (int i = 0; i < 4; i++) {
				std::vector<int> nb_nb_patches = getNbPatches(nb_patches[i]);
				auto p_it = std::find(nb_nb_patches.begin(), nb_nb_patches.end(), to_patch);
				if (p_it != nb_nb_patches.end()) {
					trans_map_it = trnsfr_fnctns.find(std::make_pair(from_patch, nb_patches[i]));
					auto trans_map_it_snd = trnsfr_fnctns.find(std::make_pair(nb_patches[i], to_patch));
					glm::vec2 trans_to_nb = trans_funcs[trans_map_it->second];
					glm::vec2 trans_from_nb = trans_funcs[trans_map_it_snd->second];
					return trans_to_nb + trans_from_nb;
				}
			}
		}
		// should have found it
		std::cout << "trans function from " << from_patch << ", to " << to_patch << " not found" << std::endl;
		return glm::vec2(0, 0);
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
		std::vector<bool> visited(2 * steepLines->size());
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

	int findMsRegion(std::set<int> &sl_indices) {
		std::set<int> nodes_v;
		for (int sl_index : sl_indices) {
			nodes_v.insert(SL->getSteepLines()->at(sl_index)[0]);
			nodes_v.insert(SL->getSteepLines()->at(sl_index).end()[-1]);
		}
		std::vector<int> nodes_index;
		for (int n : nodes_v) {
			int index = v2AdjIndex(n);
			nodes_index.push_back(index);
		}
		for (int i = 0; i < ms_regions->size(); i++) {
			std::sort(nodes_index.begin(), nodes_index.end());
			// make a copy
			std::vector<int> ms_region_nodes = ms_regions->at(i).nodes;
			std::sort(ms_region_nodes.begin(), ms_region_nodes.end());
			if (ms_region_nodes == nodes_index) {
				return i;
			}
		}
		return -1;
	}

	bool ltzero(int i) {
		return i < 0;
	}

	// might not partition the nodes completely
	void fillMsComplex() {
		//for (int i = 0; i < ms_regions->size(); i++) {
		//	int v_in_region = findAVertInRegion(ms_regions->at(i));
		//	DFS(v_in_region, (ms_regions->at(i)).region_verts);
		//}
		// change the way to do that
		partitions = std::make_shared<std::vector<int>>(he->getNumVerts(), -1);
		int start_index = 0;
		while (true) {
			// get next unassigned
			auto unassigned_it = std::find_if(partitions->begin()+ start_index, partitions->end(), [](int x) {return x < 0; });
			//[](int x) {return x < 0; }
			if (unassigned_it == partitions->end()) {
				break;
			}
			// if it's on the boundry, skip
			int unassigned_v = std::distance(partitions->begin(), unassigned_it);
			int sl_index;
			if (onSteepLine(unassigned_v, sl_index)) {
				start_index = unassigned_v + 1;
				continue;
			}
			// do a search
			std::set<int> sl_indices;
			std::vector<int> region_verts;
			DFS(unassigned_v, region_verts, sl_indices);
			// find the region those vertices belong to
			int ms_region_id = findMsRegion(sl_indices);
			if (ms_region_id >= 0) {
				ms_regions->at(ms_region_id).region_verts = region_verts;
				for (int region_v : region_verts) {
					partitions->at(region_v) = ms_region_id;
				}
			}
			else {
				std::cout << "error: did not find region for verts" << std::endl;
				start_index = unassigned_v + 1;
				continue;
			}
		}
	}

	void parametrize() {
		validateComplex();
		connectSL2Region();
		buildTransFuncsEntries();
		buildTransFuncs();
		if (validateTransFuncs() != 0) {
			std::cout << "trans funcs not valid" << std::endl;
		}
	}

	int findAVertInRegion(ms_region_t &ms_region) {
		// try each steep line
		for (int i = 0; i < 4; i++) {
			auto steepLines = SL->getSteepLines();
			int node1 = ms_region.nodes[i];
			int node2 = ms_region.nodes[(i + 1) % 4];
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
			int prev_index = std::distance(neighbors->begin(), std::find(neighbors->begin(), neighbors->end(), sl_mid_prev));
			int next_index = std::distance(neighbors->begin(), std::find(neighbors->begin(), neighbors->end(), sl_mid_next));
			int nbs_size = neighbors->size();
			// if there are neighbors after next, before prev
			if ((next_index + 1) % nbs_size != prev_index) {
				return neighbors->at((next_index + 1) % neighbors->size());
			}
		}
		return -1;
	}

	void DFS(int start_index, std::vector<int> &region_verts, std::set<int> &sl_indices) {
		int N = he->getNumVerts();
		std::vector<bool> visited(N);
		//std::vector<int> sl_indices;
		DFS_rec(start_index, sl_indices, visited);
		for (int i = 0; i < N; i++) {
			if (visited.at(i)) {
				region_verts.push_back(i);
			}
		}
	}

	bool onNode(int v_index) {
		auto it = std::find(adj_index->begin(), adj_index->end(), v_index);
		if (it != adj_index->end()) {
			return true;
		}
		return false;
	}

	void DFS_rec(int start_index, std::set<int> &sl_indices, std::vector<bool> &visited) {
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
				if (onNode(nb)) {
					visited.at(nb) = true;
				}
				else if (onSteepLine(nb, touched_sl_index)) {
					visited.at(nb) = true;
					// something related to boundary
					sl_indices.insert(touched_sl_index);
					
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