#include "Viewer.h"
#include "stdio.h"
#include <iostream>

float PI = 3.14159f;
int N = 0;

Viewer::Viewer(std::shared_ptr<Eigen::MatrixXd> vs, std::shared_ptr<Eigen::MatrixXi> fs) :
	genshader (GenShader()),
	vertices (vs),
	faces (fs),
	vns (nullptr),
	fns (nullptr) {
}

Viewer::Viewer(std::shared_ptr<Eigen::MatrixXd> vs, std::shared_ptr<Eigen::MatrixXi> fs,
	std::shared_ptr<Eigen::MatrixXd> vns, std::shared_ptr<Eigen::MatrixXi> fns) :
	genshader(GenShader()),
	vertices(vs),
	faces(fs),
	vns(vns),
	fns(fns) {
}

Viewer::Viewer(std::shared_ptr<Eigen::MatrixXd> vs, std::shared_ptr<Eigen::MatrixXi> fs,
	std::shared_ptr<Eigen::MatrixXd> vns, std::shared_ptr<Eigen::MatrixXi> fns,
	std::shared_ptr<Eigen::VectorXd> vert_values) :
	genshader(GenShader()),
	vertices(vs),
	faces(fs),
	vns(vns),
	fns(fns),
	vert_values(vert_values) {
}

std::shared_ptr<Eigen::MatrixXd> vals2Colors(std::shared_ptr<Eigen::VectorXd> values) {
	Eigen::MatrixXd colors(values->size(), 3);
	Eigen::Vector3d red, blue;
	red << 1.0f, 0.0f, 0.0f;
	blue << 0.0f, 0.0f, 1.0f;
	for (int i = 0; i < values->size(); i++) {
		colors.row(i) = (*values)(i) * red + (1.0f - (*values)(i)) * blue;
	}
	return std::make_shared<Eigen::MatrixXd>(colors);
}

void draw(void) {
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	GLenum err;
	if ((err = glGetError()) != GL_NO_ERROR) {
		std::cout << "err: " << err << std::endl;
	}
	glDrawArrays(GL_TRIANGLES, 0, N * 3);
	glutSwapBuffers();
}

void Viewer::view() {
	setup();
	createMVP();
	prepareVerts();
	if (vns != nullptr) {
		createLight();
	}
	glutDisplayFunc(draw);
	glutMainLoop();
}

Viewer::~Viewer() {
}

void Viewer::setup() {
	glutInitWindowSize(500, 500);
	glutCreateWindow("Matrix Fractal");
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glewExperimental = GL_TRUE;
	GLint GlewInitResult = glewInit();
	if (GLEW_OK != GlewInitResult)
	{
	}
	genshader.generateProgramObject();
	if (vns == nullptr) {
		attachFlatShaders();
	}
	else {
		if (vert_values == nullptr) {
			attachLightShaders();
		}
		else {
			attachLightColorShaders();
		}
	}
	genshader.useProgram();

	glEnable(GL_DEPTH_TEST);
}

void Viewer::attachFlatShaders() {
	genshader.attachVertexShader("../shaders/vertex.vs");
	genshader.attachFragmentShader("../shaders/fragment.fs");
}

void Viewer::attachLightShaders() {
	genshader.attachVertexShader("../shaders/vertex_w_normal.vs");
	genshader.attachFragmentShader("../shaders/fragment.fs");
}

void Viewer::attachLightColorShaders() {
	genshader.attachVertexShader("../shaders/vertex_w_normal_w_color.vs");
	genshader.attachFragmentShader("../shaders/fragment.fs");
}



void Viewer::createMVP() {
	M = glm::mat4(1.0f);
	glm::vec3 eye = glm::vec3(10.0f, 8.0f, 6.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	V = glm::lookAt(eye, glm::vec3(0.0f, 0.f, 0.f), up);
	P = glm::perspective(PI / 4.f, 1.f, 0.1f, 1000.f);
	glm::mat4 MVP = P * V * M;
	GLint MVP_loc = genshader.getUniformLocation("MVP");
	glUniformMatrix4fv(MVP_loc, 1, GL_FALSE, glm::value_ptr(MVP));
}

void Viewer::createLight() {
	glm::vec3 lightDir = glm::vec3(4, 3, 3);
	GLint light_loc = genshader.getUniformLocation("lightPos");
	glUniform3fv(light_loc, 1, glm::value_ptr(lightDir));
}

static const GLfloat g_vertex_buffer_data[] = {
	-1.0f, -1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,
	0.0f,  1.0f, 0.0f,
};

void Viewer::prepareVerts() {
	GLenum err;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	std::shared_ptr<std::vector<float>> display_verts = createDisplayVerts();
	float *display_verts_arr = &(*display_verts)[0];
	if (vns == nullptr) {
		glBufferData(GL_ARRAY_BUFFER, N * 9 * sizeof(float), display_verts_arr, GL_STATIC_DRAW);
	}
	else {
		std::shared_ptr<std::vector<float>> display_vns = createDisplayNorms();
		float *display_vns_arr = &(*display_vns)[0];
		// buffer size differs
		if (vert_values == nullptr) {
			glBufferData(GL_ARRAY_BUFFER, N * 18 * sizeof(float), 0, GL_STATIC_DRAW);
		}
		else {
			glBufferData(GL_ARRAY_BUFFER, N * 27 * sizeof(float), 0, GL_STATIC_DRAW);
			 //store vertices colors into buffer
			auto colors = vals2Colors(vert_values);
			auto display_colors = createDisplayColors(*colors);
			float *display_colors_arr = &(*display_colors)[0];
			glBufferSubData(GL_ARRAY_BUFFER, N * 18 * sizeof(float), N * 9 * sizeof(float), display_colors_arr);
			if ((err = glGetError()) != GL_NO_ERROR) {
				std::cout << "error: " << err << std::endl;
			}
			// link shader colors
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(float), (void *)(N * 18 * sizeof(float)));
			glEnableVertexAttribArray(2);
		}
		// store vertices positions into buffer
		glBufferSubData(GL_ARRAY_BUFFER, 0, N * 9 * sizeof(float), display_verts_arr);
		GLenum err;
		if ((err = glGetError()) != GL_NO_ERROR) {
			std::cout << "error: " << err << std::endl;
		}
		// store vertices normals into buffer
		glBufferSubData(GL_ARRAY_BUFFER, N * 9 * sizeof(float), N * 9 * sizeof(float), display_vns_arr);
		if ((err = glGetError()) != GL_NO_ERROR) {
			std::cout << "error: " << err << std::endl;
		}
		// link shader normals
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(float), (void *)(N * 9 * sizeof(float)));
		glEnableVertexAttribArray(1);
	}
	// link shader positions
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);
}

std::shared_ptr<std::vector<float>> createDisplayInfo(Eigen::MatrixXi &indices, Eigen::MatrixXd &data) {
	int r = indices.rows();
	N = r;
	std::vector<float> vnData(r * 9);
	for (int i = 0; i < r; i++) {
		for (int j = 0; j < 3; j++) {
			int vn_index = indices(i, j);
			for (int k = 0; k < 3; k++) {
				vnData[i * 9 + j * 3 + k] = data(vn_index, k);
			}
		}
	}
	return std::make_shared<std::vector<float>>(vnData);
}

std::shared_ptr<std::vector<float>> Viewer::createDisplayVerts() {
	return createDisplayInfo(*faces, *vertices);
}

std::shared_ptr<std::vector<float>> Viewer::createDisplayColors(Eigen::MatrixXd &colors) {
	return createDisplayInfo(*faces, colors);
}

std::shared_ptr<std::vector<float>> Viewer::createDisplayNorms() {
	return createDisplayInfo(*fns, *vns);
}
