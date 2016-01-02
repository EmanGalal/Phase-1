#include "shader.h"
#include <iostream>
// for files handling (read , write ,open ,....)
#include <fstream>
#include<sstream>

std::string vertexCode;
std::string fragmentCode;

void readShader(const GLchar* vertexPath, const GLchar* fragmentPath)
{
	// 1. Retrieve the vertex/fragment source code from filePath
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	// ensures ifstream objects can throw exceptions:
	vShaderFile.exceptions(std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::badbit);
	try
	{
		// Open files
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		// Read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// close file handlers
		vShaderFile.close();
		fShaderFile.close();
		// Convert stream into GLchar array
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

}

shader::shader(char*path1, char*path2)
{
	// creating the handel for the program ( memory space in GPU )
	m_program = glCreateProgram();
	//readShader(path1, path2);
	// storing the handels for each shader in the array 
	m_shaders[0] = createShader(LoadShader(path1),GL_VERTEX_SHADER);
	m_shaders[1] = createShader(LoadShader(path2), GL_FRAGMENT_SHADER);

	for (int i = 0; i < num_shaders; i++)   { 
		// open gl function to attach shaders to the program ===> must be done before linking
		//https://www.khronos.org/opengles/sdk/docs/man/xhtml/glAttachShader.xml
		glAttachShader(m_program,m_shaders[i]) ; 
	}


	glBindAttribLocation(m_program, 0, "position");


	// now the program has the 2 shaders attached to it  (all form the program now ) ,then we link the program
	//https://www.khronos.org/opengles/sdk/docs/man/xhtml/glLinkProgram.xml
	glLinkProgram(m_program);
	// linking would fail if not attached the vertex and the fragment shaders 
	// must check errors here
	// checking errors parameters 1-handel of the checked object (shader , program,..) 2- flag   3- bool is program 4- message
	CheckShaderError(m_program, GL_LINK_STATUS, true, "ERROR: linking program failed ");
	glValidateProgram(m_program);
	CheckShaderError(m_program, GL_VALIDATE_STATUS, true , "ERROR: validating program failed ");

}


shader::~shader()
{
	// de attach then delete shaders then delete program 

	for (unsigned int i = 0; i < num_shaders; i++)
	{
		glDetachShader(m_program, m_shaders[i]);
		glDeleteShader(m_shaders[i]);
	}

	glDeleteProgram(m_program);
}

// Now we prepare to use the program in the GPU with bind function 
void shader::bind() {
	glUseProgram(m_program);

}
//when to use Static functions ??

GLuint shader::createShader(const std::string &text, unsigned int type) {

	// in this function  we will 1- create shader handel 2- prepare source strings and their  sizes (shader code)3- compile the shader  

	// return 0 if failed to create
	GLuint shader = glCreateShader(type);

	if (shader == 0) std::cerr << "Error : creating shader of type" << type << std::endl;

	
	// create array for the text (shader source strings ) to read and another for this text lengths (size of the shader) 

	const GLchar* shaderSourceStrings[1];
	// opengl understand only c_str ===> so convert string text to it 
	shaderSourceStrings[0] = text.c_str();
	GLint shaderSourcelengths[1];
	shaderSourcelengths[0] = text.length();

	// load gl shader source code in the shader created  then compile it 
	glShaderSource(shader,1,shaderSourceStrings,shaderSourcelengths);
	glCompileShader(shader);

	CheckShaderError(shader, GL_COMPILE_STATUS, false, " Error : compiling shader failed "+type );
	return shader;
}
// to read the shader file into the program as a   <<< string >>>

char *shader::LoadShader(char* path)
{     
	FILE *file = fopen(path, "rt");
	if (!file){

		std::cout << "faiiiiiled\n";
		system("pause");

	}
	//Set the cursor to the end of the file
	fseek(file, 0, SEEK_END);

	//Get the position of the cursor
	unsigned long length = ftell(file);

	//The data we from the file
	char *data = new char[length + 1];

	//fill the array with 0
	memset(data, 0, length + 1);

	//Set the cursor to the begining of the file
	fseek(file, 0, SEEK_SET);

	//Store the data in the array
	fread(data, 1, length + 1, file);

	//Close the file
	fclose(file);

	return data;

	
	
}


//check errors while linking the shader file and compiling 
void shader::CheckShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string& errorMessage)
{
	GLint success = 0;
	GLchar error[1024] = { 0 };

	if (isProgram)
		glGetProgramiv(shader, flag, &success);
	else
		glGetShaderiv(shader, flag, &success);

	if (success == GL_FALSE)
	{
		if (isProgram)
			glGetProgramInfoLog(shader, sizeof(error), NULL, error);
		else
			glGetShaderInfoLog(shader, sizeof(error), NULL, error);

		std::cerr << errorMessage << ": '" << error << "'" << std::endl;
	}
}