#include "SteepLine.h"
#include <algorithm>

SteepLine::SteepLine(Eigen::VectorXd &values, HE &he) :
	values (values),
	types (std::vector<CrtlType>(values.size())),
	he (he) {
}

std::shared_ptr<std::vector<double>> SteepLine::getNeighborValues(std::vector<int> &neighbors) {
	std::shared_ptr<std::vector<double>> neighbors_vals = std::make_shared<std::vector<double>>(neighbors.size());
	for (int j = 0; j < neighbors.size(); j++) {
		(*neighbors_vals)[j] = values(neighbors[j]);
	}
	return neighbors_vals;
}
void SteepLine::getVerticesType() {
	for (int i = 0; i < values.size(); i++) {
		std::shared_ptr<std::vector<int>> neighbors = he.getNeighbors(i);
		double v_val = values(i);
		int neighbors_len = neighbors->size();
		std::shared_ptr<std::vector<double>> neighbors_vals = getNeighborValues(*neighbors);

		int sign_change = 0;
		for (int j = 0; j < neighbors_len; j++) {
			// different sign
			if ((v_val - (*neighbors_vals)[j]) * (v_val - (*neighbors_vals)[(j + 1) % neighbors_len]) < 0) {
				sign_change += 1;
			}
		}
		if (sign_change == 0 && v_val - (*neighbors_vals)[0] < 0) {
			types[i] = CrtlType::Min;
		}
		else if (sign_change == 0 && v_val - (*neighbors_vals)[0] >= 0) {
			types[i] = CrtlType::Max;
		}
		else if (sign_change == 2) {
			types[i] = CrtlType::Reg;
		}
		else {
			types[i] = CrtlType::Sdl;
			saddles.push_back(i);
		}
	}
}

std::shared_ptr<std::vector<int>> SteepLine::getLineToExtrm(int v_index, CrtlType extrm_type) {
	std::shared_ptr<std::vector<int>> line = std::make_shared<std::vector<int>>();
	getLineToExtrmRec(v_index, extrm_type, *line);
	return line;
}

void SteepLine::getLineToExtrmRec(int v_index, CrtlType extrm_type, std::vector<int> &line) {
	// compare function for getting max and min neighbors
	auto cmp = [this](const int &lhs, const int &rhs) { return values[lhs] < values[rhs]; };
	// append this vertex to line
	line.push_back(v_index);
	if (extrm_type == CrtlType::Min) {
		// reach min
		if (types[v_index] == CrtlType::Min) {
			return;
		}
		// keep looking for min
		else {
			std::shared_ptr<std::vector<int>> neighbors = he.getNeighbors(v_index);
			auto min_nb = std::min_element(neighbors->begin(), neighbors->end(), cmp);
			getLineToExtrmRec(*min_nb, CrtlType::Min, line);
		}
	}
	else {
		// reach max
		if (types[v_index] == CrtlType::Max) {
			return;
		}
		// keep looking for max
		else {
			std::shared_ptr<std::vector<int>> neighbors = he.getNeighbors(v_index);
			auto max_nb = std::max_element(neighbors->begin(), neighbors->end(), cmp);
			getLineToExtrmRec(*max_nb, CrtlType::Max, line);
		}
	}
}

std::shared_ptr<std::vector<std::vector<int>>> SteepLine::getSteepLines() {
	if (steep_lines.size() > 0) {
		return std::make_shared<std::vector<std::vector<int>>>(steep_lines);
	}
	for (int i = 0; i < saddles.size(); i++) {
		int sdl_index = saddles[i];
		std::shared_ptr<std::vector<int>> neighbors = he.getNeighbors(sdl_index);
		std::shared_ptr<std::vector<double>> neighbor_values = getNeighborValues(*neighbors);
		auto getLeftNb = [neighbors](const int nb_indx) {
			return (*neighbors)[(nb_indx - 1 + neighbors->size()) % neighbors->size()]; };
		auto getRightNb = [neighbors](const int nb_indx) {
			return (*neighbors)[(nb_indx + 1 + neighbors->size()) % neighbors->size()]; };
		for (int j = 0; j < neighbors->size(); j++) {
			int nb_indx = (*neighbors)[j];
			std::shared_ptr<std::vector<int>> line_to_extrm;
			if (values(nb_indx) <= values(sdl_index) && values(nb_indx) <= values(getLeftNb(j)) &&
				values(nb_indx) <= values(getRightNb(j))) {
				line_to_extrm = getLineToExtrm(nb_indx, CrtlType::Min);
			}
			else if (values(nb_indx) >= values(sdl_index) && values(nb_indx) >= values(getLeftNb(j)) &&
				values(nb_indx) >= values(getRightNb(j))) {
				line_to_extrm = getLineToExtrm(nb_indx, CrtlType::Max);
			}
			if (line_to_extrm) {
				line_to_extrm->insert(line_to_extrm->begin(), sdl_index);
				steep_lines.push_back(*line_to_extrm);
			}
		}
	}
	return std::make_shared<std::vector<std::vector<int>>>(steep_lines);
}


