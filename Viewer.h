#pragma once

#include "GenShader.h"
#include <GL/glut.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <memory>
#include <vector>
#include <Eigen/Dense>

class Viewer {
public:
	Viewer(std::shared_ptr<Eigen::MatrixXd> vs, std::shared_ptr<Eigen::MatrixXi> fs);
	Viewer(std::shared_ptr<Eigen::MatrixXd> vs, std::shared_ptr<Eigen::MatrixXi> fs, 
		std::shared_ptr<Eigen::MatrixXd> vns, std::shared_ptr<Eigen::MatrixXi> fns);
	Viewer(std::shared_ptr<Eigen::MatrixXd> vs, std::shared_ptr<Eigen::MatrixXi> fs,
		std::shared_ptr<Eigen::MatrixXd> vns, std::shared_ptr<Eigen::MatrixXi> fns, 
		std::shared_ptr<Eigen::VectorXd> vert_values);
	~Viewer();
	void view();

private:
	GenShader genshader;
	glm::mat4 M;
	glm::mat4 V;
	glm::mat4 P;
	GLuint vao;
	GLuint vbo;
	std::shared_ptr<Eigen::MatrixXd> vertices;
	std::shared_ptr<Eigen::MatrixXd> vns;
	std::shared_ptr<Eigen::MatrixXi> faces;
	std::shared_ptr<Eigen::MatrixXi> fns;
	std::shared_ptr<Eigen::VectorXd> vert_values;
	
	std::shared_ptr<std::vector<float>> createDisplayVerts();
	std::shared_ptr<std::vector<float>> createDisplayNorms();
	std::shared_ptr<std::vector<float>> createDisplayColors(Eigen::MatrixXd &colors);
	void attachFlatShaders();
	void attachLightShaders();
	void attachLightColorShaders();
	void createMVP();
	void createLight();
	void prepareVerts();
	void setup();
};