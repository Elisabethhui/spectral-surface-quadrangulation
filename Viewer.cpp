//#include "Viewer.h"
//#include "stdio.h"
//#include <iostream>
//
//float PI = 3.14159f;
//int N = 0;
//int line_num;
//float delta_angle = 0.1f;
//
//namespace viewer {
//	//namespace silly{
//		GenShader meshShader;
//		GenShader lineShader;
//		glm::mat4 M;
//		glm::mat4 V;
//		glm::mat4 P;
//		GLuint vao_mesh;
//		GLuint vbo_mesh;
//		GLuint vao_line;
//		GLuint vbo_line;
//
//		//pseudo colors
//		std::vector<glm::vec3> pseudo_cols = {
//			glm::vec3(1.f, 0.f, 0.f),
//			glm::vec3(0.f, 1.f, 0.f),
//			glm::vec3(0.f, 0.f, 1.f),
//			glm::vec3(1.f, 1.f, 0.f),
//			glm::vec3(0.f, 1.f, 1.f),
//			glm::vec3(1.f, 0.f, 1.f),
//			glm::vec3(0.55294117647f, 0.50588235294f, 0.46666666666f),
//			glm::vec3(0.90980392156f, 1.00000000000f, 0.89019607843f),
//			glm::vec3(0.56078431372f, 0.69803921568f, 0.89019607843f),
//			glm::vec3(0.81960784313f, 0.91764705882f, 0.87058823529f),
//			glm::vec3(0.84705882352f, 0.91764705882f, 0.87058823529f) };
//		
//		DrawMode drawMode = DrawMode::VALUE;
//		std::shared_ptr<Eigen::MatrixXd> vertices;
//		std::shared_ptr<Eigen::MatrixXd> vns;
//		std::shared_ptr<Eigen::MatrixXi> faces;
//		std::shared_ptr<Eigen::MatrixXi> fns;
//		std::shared_ptr<Eigen::VectorXd> vert_values;
//		std::shared_ptr<std::vector<int>> partitions;
//		std::shared_ptr<std::vector<std::vector<float>>> display_lines;
//		std::shared_ptr<std::vector<std::vector<int>>> lines_indices;
//
//		std::shared_ptr<std::vector<float>> createDisplayVerts();
//		std::shared_ptr<std::vector<float>> createDisplayNorms();
//		std::shared_ptr<std::vector<float>> createDisplayColors(Eigen::MatrixXd &colors);
//		std::shared_ptr<std::vector<std::vector<float>>> createDisplayLineVerts();
//		std::shared_ptr<Eigen::MatrixXd> vals2Colors(std::shared_ptr<Eigen::VectorXd> values);
//
//		void prepareLineVerts();
//		bool drawLines = false;
//		void attachFlatShaders(GenShader &shader);
//		void attachLightShaders(GenShader &shader);
//		void attachLightColorShaders(GenShader &shader);
//		void createMVP(GenShader &shader);
//		void createLight();
//		void prepareVerts();
//		void setup();
//
//
//		std::shared_ptr<Eigen::MatrixXd> vals2Colors(std::shared_ptr<Eigen::VectorXd> values) {
//			Eigen::MatrixXd colors(values->size(), 3);
//			Eigen::Vector3d red, blue;
//			red << 1.0f, 0.0f, 0.0f;
//			blue << 0.0f, 0.0f, 1.0f;
//			for (int i = 0; i < values->size(); i++) {
//				colors.row(i) = (*values)(i) * red + (1.0f - (*values)(i)) * blue;
//			}
//			return std::make_shared<Eigen::MatrixXd>(colors);
//		}
//
//		std::shared_ptr<Eigen::MatrixXd> partition2Colors() {
//			std::shared_ptr<Eigen::MatrixXd> colors = std::make_shared<Eigen::MatrixXd>(partitions->size(), 3);
//			for (int i = 0; i < partitions->size(); i++) {
//				int col_index = partitions->at(i) % pseudo_cols.size();
//				colors->row(i) << pseudo_cols[col_index][0],
//					pseudo_cols[col_index][1], pseudo_cols[col_index][2];
//			}
//			return colors;
//		}
//
//		void draw_mesh(void) {
//			glBindVertexArray(vao_mesh);
//			glBindBuffer(GL_ARRAY_BUFFER, vbo_mesh);
//			GLenum err;
//			if ((err = glGetError()) != GL_NO_ERROR) {
//				std::cout << "err: " << err << std::endl;
//			}
//			meshShader.useProgram();
//			glDrawArrays(GL_TRIANGLES, 0, N * 3);
//		}
//
//		void draw_lines(void) {
//			lineShader.useProgram();
//			int current_elem = 0;
//			for (int i = 0; i < line_num; i++) {
//				glBindVertexArray(vao_line);
//				glBindBuffer(GL_ARRAY_BUFFER, vbo_line);
//				glDrawArrays(GL_LINE_STRIP, current_elem, (*lines_indices)[i].size());
//				current_elem += (*lines_indices)[i].size();
//			}
//		}
//
//		void draw(void) {
//			glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
//			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//			draw_mesh();
//			if (drawLines) {
//				draw_lines();
//			}
//			glutSwapBuffers();
//		}
//
//		void updateRotation(int param) {
//			int timeSinceStart = glutGet(GLUT_ELAPSED_TIME);
//			std::cout << "timeSinceStart: " << timeSinceStart << std::endl;
//			M = glm::rotate(M, delta_angle, glm::vec3(0.f, 1.f, 0.f));
//			glm::mat4 MVP = P * V * M;
//			GLint MVP_loc = meshShader.getUniformLocation("MVP");
//			meshShader.useProgram();
//			glUniformMatrix4fv(MVP_loc, 1, GL_FALSE, glm::value_ptr(MVP));
//			if (drawLines) {
//				MVP_loc = lineShader.getUniformLocation("MVP");
//				lineShader.useProgram();
//				glUniformMatrix4fv(MVP_loc, 1, GL_FALSE, glm::value_ptr(MVP));
//			}
//			glutPostRedisplay();
//			glutTimerFunc(40, updateRotation, 0);
//		}
//
//		void setup() {
//			glutInitWindowSize(500, 500);
//			glutCreateWindow("Matrix Fractal");
//			glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
//
//			glewExperimental = GL_TRUE;
//			GLint GlewInitResult = glewInit();
//			if (GLEW_OK != GlewInitResult)
//			{
//			}
//			meshShader.generateProgramObject();
//			if (vns == nullptr) {
//				attachFlatShaders(meshShader);
//			}
//			else {
//				if (vert_values == nullptr) {
//					attachLightShaders(meshShader);
//				}
//				else {
//					attachLightColorShaders(meshShader);
//				}
//			}
//			meshShader.useProgram();
//
//			glEnable(GL_DEPTH_TEST);
//		}
//
//		void attachFlatShaders(GenShader &shader) {
//			shader.attachVertexShader("../shaders/vertex.vs");
//			shader.attachFragmentShader("../shaders/fragment.fs");
//		}
//
//		void attachLightShaders(GenShader &shader) {
//			shader.attachVertexShader("../shaders/vertex_w_normal.vs");
//			shader.attachFragmentShader("../shaders/fragment.fs");
//		}
//
//		void attachLightColorShaders(GenShader &shader) {
//			shader.attachVertexShader("../shaders/vertex_w_normal_w_color.vs");
//			shader.attachFragmentShader("../shaders/fragment.fs");
//		}
//
//		void createMVP(GenShader &shader) {
//			M = glm::mat4(1.0f);
//			glm::vec3 eye = glm::vec3(10.0f, 8.0f, 6.0f);
//			glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
//			V = glm::lookAt(eye, glm::vec3(0.0f, 0.f, 0.f), up);
//			P = glm::perspective(PI / 4.f, 1.f, 0.1f, 1000.f);
//			glm::mat4 MVP = P * V * M;
//			GLint MVP_loc = shader.getUniformLocation("MVP");
//			glUniformMatrix4fv(MVP_loc, 1, GL_FALSE, glm::value_ptr(MVP));
//		}
//
//		void createLight() {
//			glm::vec3 lightDir = glm::vec3(4, 3, 3);
//			GLint light_loc = meshShader.getUniformLocation("lightPos");
//			glUniform3fv(light_loc, 1, glm::value_ptr(lightDir));
//		}
//
//		void prepareLineVerts() {
//			glGenVertexArrays(1, &vao_line);
//			glGenBuffers(1, &vbo_line);
//			glBindVertexArray(vao_line);
//			glBindBuffer(GL_ARRAY_BUFFER, vbo_line);
//			// get total number of elements in display_lines
//			int element_num = 0;
//			for (int i = 0; i < line_num; i++) {
//				element_num += (*display_lines)[i].size();
//			}
//			glBufferData(GL_ARRAY_BUFFER, element_num * sizeof(float), 0, GL_STATIC_DRAW);
//
//			int current_elem = 0;
//			for (int i = 0; i < line_num; i++) {
//				float *display_verts_arr = &((*display_lines)[i])[0];
//				glBufferSubData(GL_ARRAY_BUFFER, current_elem * sizeof(float), 
//					(*display_lines)[i].size() * sizeof(float), display_verts_arr);
//				current_elem += (*display_lines)[i].size();
//			}
//			lineShader.useProgram();
//			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
//			glEnableVertexAttribArray(0);
//		}
//
//		void prepareVerts() {
//			GLenum err;
//			glGenVertexArrays(1, &vao_mesh);
//			glGenBuffers(1, &vbo_mesh);
//			glBindVertexArray(vao_mesh);
//			glBindBuffer(GL_ARRAY_BUFFER, vbo_mesh);
//			std::shared_ptr<std::vector<float>> display_verts = createDisplayVerts();
//			float *display_verts_arr = &(*display_verts)[0];
//			if (vns == nullptr) {
//				glBufferData(GL_ARRAY_BUFFER, N * 9 * sizeof(float), display_verts_arr, GL_STATIC_DRAW);
//			}
//			else {
//				std::shared_ptr<std::vector<float>> display_vns = createDisplayNorms();
//				float *display_vns_arr = &(*display_vns)[0];
//				// buffer size differs
//				if (vert_values == nullptr) {
//					glBufferData(GL_ARRAY_BUFFER, N * 18 * sizeof(float), 0, GL_STATIC_DRAW);
//				}
//				else {
//					glBufferData(GL_ARRAY_BUFFER, N * 27 * sizeof(float), 0, GL_STATIC_DRAW);
//					//store vertices colors into buffer
//					std::shared_ptr<Eigen::MatrixXd> colors;
//					if (drawMode == DrawMode::VALUE) {
//						colors = vals2Colors(vert_values);
//					}
//					else {
//						colors = partition2Colors();
//					}
//					std::shared_ptr<std::vector<float>> display_colors = createDisplayColors(*colors);
//				
//					float *display_colors_arr = &(*display_colors)[0];
//					glBufferSubData(GL_ARRAY_BUFFER, N * 18 * sizeof(float), N * 9 * sizeof(float), display_colors_arr);
//					if ((err = glGetError()) != GL_NO_ERROR) {
//						std::cout << "error: " << err << std::endl;
//					}
//					// link shader colors
//					glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(float), (void *)(N * 18 * sizeof(float)));
//					glEnableVertexAttribArray(2);
//				}
//				// store vertices positions into buffer
//				glBufferSubData(GL_ARRAY_BUFFER, 0, N * 9 * sizeof(float), display_verts_arr);
//				GLenum err;
//				if ((err = glGetError()) != GL_NO_ERROR) {
//					std::cout << "error: " << err << std::endl;
//				}
//				// store vertices normals into buffer
//				glBufferSubData(GL_ARRAY_BUFFER, N * 9 * sizeof(float), N * 9 * sizeof(float), display_vns_arr);
//				if ((err = glGetError()) != GL_NO_ERROR) {
//					std::cout << "error: " << err << std::endl;
//				}
//				// link shader normals
//				glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(float), (void *)(N * 9 * sizeof(float)));
//				glEnableVertexAttribArray(1);
//			}
//			// link shader positions
//			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
//			glEnableVertexAttribArray(0);
//		}
//
//		std::shared_ptr<std::vector<float>> createDisplayInfo(Eigen::MatrixXi &indices, Eigen::MatrixXd &data) {
//			int r = indices.rows();
//			N = r;
//			std::vector<float> vnData(r * 9);
//			for (int i = 0; i < r; i++) {
//				for (int j = 0; j < 3; j++) {
//					int vn_index = indices(i, j);
//					for (int k = 0; k < 3; k++) {
//						vnData[i * 9 + j * 3 + k] = data(vn_index, k);
//					}
//				}
//			}
//			return std::make_shared<std::vector<float>>(vnData);
//		}
//
//		std::shared_ptr<std::vector<float>> createDisplayVerts() {
//			return createDisplayInfo(*faces, *vertices);
//		}
//
//		std::shared_ptr<std::vector<float>> createDisplayColors(Eigen::MatrixXd &colors) {
//			return createDisplayInfo(*faces, colors);
//		}
//
//		std::shared_ptr<std::vector<float>> createDisplayNorms() {
//			return createDisplayInfo(*fns, *vns);
//		}
//
//		std::shared_ptr<std::vector<std::vector<float>>> createDisplayLineVerts() {
//			auto display_lines_verts = std::make_shared<std::vector<std::vector<float>>>(line_num);
//			for (int i = 0; i < line_num; i++) {
//				int vert_num = (*lines_indices)[i].size();
//				std::vector<float> line_verts(vert_num * 3);
//				for (int j = 0; j < vert_num; j++) {
//					line_verts[j * 3] = (*vertices)((*lines_indices)[i][j], 0);
//					line_verts[j * 3 + 1] = (*vertices)((*lines_indices)[i][j], 1);
//					line_verts[j * 3 + 2] = (*vertices)((*lines_indices)[i][j], 2);
//				}
//				(*display_lines_verts)[i] = line_verts;
//			}
//			return display_lines_verts;
//		}
//	//}
//
//	// visible methods
//
//	void setVertices(std::shared_ptr<Eigen::MatrixXd> vs) {
//		vertices = vs;
//	}
//
//	void setFaces(std::shared_ptr<Eigen::MatrixXi> fs) {
//		faces = fs;
//	}
//
//	void setVNs(std::shared_ptr<Eigen::MatrixXd> vnormals) {
//		vns = vnormals;
//	}
//
//	void setFNs(std::shared_ptr<Eigen::MatrixXi> fnormals) {
//		fns = fnormals;
//	}
//
//	void setColors(std::shared_ptr<Eigen::VectorXd> vValues) {
//		vert_values = vValues;
//	}
//
//	void setMeshInfo(std::shared_ptr<Eigen::MatrixXd> vs, std::shared_ptr<Eigen::MatrixXi> fs,
//		std::shared_ptr<Eigen::MatrixXd> vnormals, std::shared_ptr<Eigen::MatrixXi> fnormals,
//		std::shared_ptr<Eigen::VectorXd> vValues) {
//		setVertices(vs);
//		setFaces(fs);
//		setVNs(vnormals);
//		setFNs(fnormals);
//		setColors(vValues);
//	}
//
//	void passLines(std::shared_ptr<std::vector<std::vector<int>>> lines) {
//		drawLines = true;
//		lines_indices = lines;
//		line_num = lines->size();
//		lineShader.generateProgramObject();
//		attachFlatShaders(lineShader);
//		createMVP(lineShader);
//		display_lines = createDisplayLineVerts();
//		prepareLineVerts();
//	}
//
//	void prepareMesh() {
//		setup();
//		createMVP(meshShader);
//		prepareVerts();
//		if (vns != nullptr) {
//			createLight();
//		}
//	}
//
//	void setDrawMode(DrawMode mode) {
//		drawMode = mode;
//	}
//
//	void setPartition(std::shared_ptr<std::vector<int>> p) {
//		partitions = p;
//	}
//
//	void view() {
//		glutDisplayFunc(draw);
//		glutTimerFunc(40, updateRotation, 0);
//		glutMainLoop();
//	}
//
//}
