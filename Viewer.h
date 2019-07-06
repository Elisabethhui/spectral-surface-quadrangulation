#pragma once

#include "GenShader.h"
#include <GL/glut.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <memory>
#include <vector>
#include <Eigen/Dense>

namespace viewer {

	enum class DrawMode {
		VALUE,
		PSEUDO_COLOR

	};
	void prepareMesh();
	void view();
	void setDrawMode(DrawMode mode);
	void passLines(std::shared_ptr<std::vector<std::vector<int>>> lines);
	void setMeshInfo(std::shared_ptr<Eigen::MatrixXd> vs, std::shared_ptr<Eigen::MatrixXi> fs,
		std::shared_ptr<Eigen::MatrixXd> vnormals, std::shared_ptr<Eigen::MatrixXi> fnormals,
		std::shared_ptr<Eigen::VectorXd> vValues);
	void setVertices(std::shared_ptr<Eigen::MatrixXd> vs);
	void setFaces(std::shared_ptr<Eigen::MatrixXi> fs);
	void setVNs(std::shared_ptr<Eigen::MatrixXd> vnormals);
	void setFNs(std::shared_ptr<Eigen::MatrixXi> fnormals);
	void setColors(std::shared_ptr<Eigen::VectorXd> vValues);
	void setPartition(std::shared_ptr<std::vector<int>> p);
};