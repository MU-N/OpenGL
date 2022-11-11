#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#define ASSERT(x) if(!(x)) __debugbreak();
#define GLCall(x) GLClearErorr();\
		x;\
		ASSERT(GLLogCall(#x,__FILE__,__LINE__))

static void GLClearErorr()
{
	while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line)
{
	while (GLenum erorr = glGetError())
	{
		std::cout << "[OpenGL Erorr]: Error " << erorr << "; Function: " << function
			<< "; File: " << file << "; Line: " << line << std::endl;
		return false;
	}
	return true;
}

struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
};


static ShaderProgramSource ParseShader(const std::string& filePath)
{
	std::ifstream stream(filePath);

	enum class ShderType
	{
		NONE = -1,
		VERTEX = 0,
		FRAGMENT = 1
	};

	std::string line;

	std::stringstream ss[2];

	ShderType type = ShderType::NONE;

	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
			{
				type = ShderType::VERTEX;
			}
			else if (line.find("fragment") != std::string::npos)
			{
				type = ShderType::FRAGMENT;
			}
		}
		else
		{
			ss[(int)type] << line << '\n';
		}

	}

	return { ss[0].str(),ss[1].str() };

}

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	//todo : Handel Erorrs
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);


	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << " failed to compile " <<
			(type == GL_VERTEX_SHADER ? "vertex" : "fragment")
			<< " Shader" << std::endl;

		std::cout << message << std::endl;

		glDeleteShader(id);
		return 0;
	}


	return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);

	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}




int main(void)
{
	GLFWwindow* window;

	if (!glfwInit())
		return -1;

	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK)
		std::cout << " Insta;ized ";

	std::cout << glGetString(GL_VERSION);

	float positions[] = {
		-0.5f, -0.5f,//0
		 0.5f, -0.5f,//1
		 0.5f,  0.5f,//2
		-0.5f,  0.5f,//2
	};


	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	unsigned int buffer;
	glGenBuffers(1, &buffer);

	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), positions, GL_STATIC_DRAW);


	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);


	unsigned int indexBufferObj;
	glGenBuffers(1, &indexBufferObj);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObj);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	ShaderProgramSource shaderSource = ParseShader("res/Shaders/Basic.shader");

	std::cout << "VERTEX" << std::endl;
	std::cout << shaderSource.VertexSource << std::endl;
	std::cout << shaderSource.FragmentSource << std::endl;


	unsigned int sahder = CreateShader(shaderSource.VertexSource, shaderSource.FragmentSource);
	glUseProgram(sahder);

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);

		GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

		glfwSwapBuffers(window);

		glfwPollEvents();
	}
	glDeleteProgram(sahder);
	glfwTerminate();
	return 0;
}