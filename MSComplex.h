#pragma once
#include "SteepLine.h"
#include "HE.h"
#include <glm.hpp>
#include <memory>
#include <algorithm>
#include <tuple>
#include <iostream>

namespace MSComplex {

	struct ms_region_t {
		std::vector<int> nodes;
		std::vector<int> region_verts;
	};

	void setSL(std::shared_ptr<SteepLine> steepLine);

	void setHE(std::shared_ptr<HE> halfEdge);

	int v2AdjIndex(int v);

	void buildAdjMatrix();

	std::tuple<int, int, bool> getNextSL(int start, int end);

	std::tuple<int, int, bool> getSL(int start, int end);

	int getSLIndex(int start_v, int end_v);

	void connectSL2Region();

	std::vector<int> getPatchSlIndices(ms_region_t patch);

	std::vector<int> getNbPatches(int patch_index);

	void buildTransFuncsEntries();

	void buildTransFuncs();

	int getOppositeTransFunc(int func_index);

	void buildTransFuncsRec(int from_patch, int patch, std::shared_ptr<std::vector<bool>> built);

	std::shared_ptr<std::vector<ms_region_t>> buildMSComplex();

	void fillMsComplex();

	void parametrize();

	int findAVertInRegion(ms_region_t &ms_region);

	void DFS(int start_index, std::vector<int> &region_verts);

	void DFS_rec(int start_index, std::vector<int> &sl_indices, std::vector<bool> &visited);

	bool onSteepLine(int v_index, int &sl_index);
}