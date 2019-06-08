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

	void prepareMesh();
	void view();
	void passLines(std::shared_ptr<std::vector<std::vector<int>>> lines);
	void setMeshInfo(std::shared_ptr<Eigen::MatrixXd> vs, std::shared_ptr<Eigen::MatrixXi> fs,
		std::shared_ptr<Eigen::MatrixXd> vnormals, std::shared_ptr<Eigen::MatrixXi> fnormals,
		std::shared_ptr<Eigen::VectorXd> vValues);
	void setVertices(std::shared_ptr<Eigen::MatrixXd> vs);
	void setFaces(std::shared_ptr<Eigen::MatrixXi> fs);
	void setVNs(std::shared_ptr<Eigen::MatrixXd> vnormals);
	void setFNs(std::shared_ptr<Eigen::MatrixXi> fnormals);
	void setColors(std::shared_ptr<Eigen::VectorXd> vValues);

	//GenShader meshShader;
	//GenShader lineShader;
	//glm::mat4 M;
	//glm::mat4 V;
	//glm::mat4 P;
	//GLuint vao;
	//GLuint vbo;

	//std::shared_ptr<Eigen::MatrixXd> vertices;
	//std::shared_ptr<Eigen::MatrixXd> vns;
	//std::shared_ptr<Eigen::MatrixXi> faces;
	//std::shared_ptr<Eigen::MatrixXi> fns;
	//std::shared_ptr<Eigen::VectorXd> vert_values;
	//
	//std::shared_ptr<std::vector<float>> createDisplayVerts();
	//std::shared_ptr<std::vector<float>> createDisplayNorms();
	//std::shared_ptr<std::vector<float>> createDisplayColors(Eigen::MatrixXd &colors);
	//std::shared_ptr<std::vector<std::vector<float>>> createDisplayLineVerts(const std::vector<std::vector<int>> &lines);
	//
	////void prepareLineVerts(std::vector<std::vector<float>> &display_verts);
	//bool drawLines = false;
	//void attachFlatShaders(GenShader &shader);
	//void attachLightShaders(GenShader &shader);
	//void attachLightColorShaders(GenShader &shader);
	//void createMVP();
	//void createLight();
	//void prepareVerts();
	//void setup();
};