//#include <stdio.h>
//#include <fstream>
//#include <sstream>
//#include <iostream>
//#include "GenShader.h"
//
//using namespace std;
//
//string GenShader::toErrorName(GLenum  error) {
//	switch (error) {
//	case GL_INVALID_ENUM:
//		return "GL_INVALID_ENUM";
//	case GL_INVALID_VALUE:
//		return "GL_INVALID_VALUE";
//	case GL_INVALID_OPERATION:
//		return "GL_INVALID_OPERATION";
//	case GL_INVALID_FRAMEBUFFER_OPERATION:
//		return "GL_INVALID_FRAMEBUFFER_OPERATION";
//	case GL_OUT_OF_MEMORY:
//		return "GL_OUT_OF_MEMORY";
//	case GL_STACK_UNDERFLOW:
//		return "GL_STACK_UNDERFLOW";
//	case GL_STACK_OVERFLOW:
//		return "GL_STACK_OVERFLOW";
//	default:
//		return "other";
//	}
//}
//
//GenShader::GenShader() {
//}
//GenShader::~GenShader() {
//}
//
//void GenShader::generateProgramObject() {
//	if (isGenerated()) {
//		return;
//	}
//	else {
//		GLenum err;
//		progHandle = glCreateProgram();
//		if ((err = glGetError()) != GL_NO_ERROR) {
//			cout << "glCreateProgram error: " << err << toErrorName(err) << endl;
//		}
//	}
//}
//
//bool GenShader::isGenerated() {
//	return progHandle != 100;
//}
//
//void GenShader::attachVertexShader(const char *filePath) {
//	vertexShader = glCreateShader(GL_VERTEX_SHADER);
//	GLenum err = glGetError(); 
//	if (err != GL_NO_ERROR) {
//		cout << "glCreateShader error: " << err << toErrorName(err) << endl;
//	}
//	attachShader(vertexShader, filePath);
//}
//
//void GenShader::attachFragmentShader(const char *filePath) {
//	fragShader = glCreateShader(GL_FRAGMENT_SHADER);
//	GLenum err;
//	if ((err = glGetError()) != GL_NO_ERROR) {
//		cout << "glCreateShader error: " << err << toErrorName(err) << endl;
//	}
//	attachShader(fragShader, filePath);
//}
//
//void GenShader::attachShader(GLuint shader, const char *filePath) {
//	GLenum err;
//
//	if ((err = glGetError()) != GL_NO_ERROR) {
//		cout << "glCreateShader error: " << err << toErrorName(err) << endl;
//	}
//	string sourceCodeStr = extractSourceCode(filePath);
//	const char *sourceCode = sourceCodeStr.c_str();
//	cout << "extractSourceCode: " << sourceCode << endl;
//	glShaderSource(shader, 1, (const GLchar **)&sourceCode, NULL);
//	glCompileShader(shader);
//	int rvalue;
//	glGetShaderiv(shader, GL_COMPILE_STATUS, &rvalue);
//	if (!rvalue) {
//		fprintf(stderr, "Error in compiling the compute shader\n");
//		GLchar log[10240];
//		GLsizei length;
//		glGetShaderInfoLog(shader, 10239, &length, log);
//		fprintf(stderr, "Compiler log:\n%s\n", log);
//		exit(40);
//	}
//	glAttachShader(progHandle, shader);
//	if ((err = glGetError()) != GL_NO_ERROR) {
//		cout << "glAttachShader error: " << err << toErrorName(err) << endl;
//	}
//	glLinkProgram(progHandle);
//	if ((err = glGetError()) != GL_NO_ERROR) {
//		cout << "glLinkProgram error: " << err << toErrorName(err) << endl;
//	}
//	glGetProgramiv(progHandle, GL_LINK_STATUS, &rvalue);
//	if (!rvalue) {
//		fprintf(stderr, "Error in linking compute shader program\n");
//		GLchar log[10240];
//		GLsizei length;
//		glGetProgramInfoLog(progHandle, 10239, &length, log);
//		fprintf(stderr, "Linker log:\n%s\n", log);
//		exit(41);
//	}
//	glUseProgram(progHandle);
//	//if ((err = glGetError()) != GL_NO_ERROR) {
//	//	cout << "glLinkProgram error: " << err << toErrorName(err) << endl;
//	//}
//}
//
//string GenShader::extractSourceCode(const char *filePath) {
//	ifstream file;
//	file.open(filePath);
//	stringstream strBuffer;
//	string str;
//
//	while (file.good()) {
//		getline(file, str, '\r');
//		strBuffer << str;
//	}
//	file.close();
//	strBuffer << '\0';  // Append null terminator.
//	return strBuffer.str();
//
//}
//
//GLint GenShader::getUniformLocation(const char *name) {
//	return glGetUniformLocation(progHandle, (const GLchar *)name);
//}
//
//void GenShader::useProgram() {
//	glUseProgram(progHandle);
//}