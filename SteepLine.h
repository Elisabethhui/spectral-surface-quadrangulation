#pragma once
#include <Eigen/Dense>
#include <vector>
#include <memory>
#include "HE.h"

class SteepLine {
public:
	SteepLine(Eigen::VectorXd &values, HE &he);
	void getVerticesType();
	std::shared_ptr<std::vector<std::vector<int>>> getSteepLines();
	std::shared_ptr<std::vector<int>> getSaddles();
	std::shared_ptr<std::vector<int>> getMins();
	std::shared_ptr<std::vector<int>> getMaxs();

private:
	enum class CrtlType { NA = 0, Max = 1, Min = 2, Sdl = 3, Reg = 4};
	HE &he;
	Eigen::VectorXd &values;
	std::vector<CrtlType> types;
	std::shared_ptr<std::vector<int>> saddles;
	std::shared_ptr<std::vector<int>> maxs;
	std::shared_ptr<std::vector<int>> mins;
	std::vector<std::vector<int>> steep_lines;

	std::shared_ptr<std::vector<double>> getNeighborValues(std::vector<int> &neighbors);
	std::shared_ptr<std::vector<int>> getLineToExtrm(int v_index, CrtlType extrm_type);
	void getLineToExtrmRec(int v_index, CrtlType extrm_type, std::vector<int> &line);
};