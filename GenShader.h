//#pragma once
//#include <GL/glew.h>
//#include <string>
//
//
//class GenShader {
//public:
//	GenShader();
//	~GenShader();
//	bool isGenerated();
//	void generateProgramObject();
//	void attachVertexShader(const char *filePath);
//	void attachFragmentShader(const char *filePath);
//	void useProgram();
//	GLint getUniformLocation(const char *name);
//
//private:
//	GLuint progHandle = 100;
//	GLuint vertexShader = 100;
//	GLuint fragShader = 100;
//
//	void attachShader(GLuint shader, const char *filePath);
//	std::string extractSourceCode(const char *filePath);
//	std::string toErrorName(GLenum  error);
//};