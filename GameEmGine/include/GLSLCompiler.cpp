#include "GLSLCompiler.h"

GLuint *GLSLCompiler::m_programs = new GLuint[0], *GLSLCompiler::m_attribs = new GLuint[0], GLSLCompiler::m_num;

GLSLCompiler::GLSLCompiler()
{
	glewInit();
}

GLSLCompiler::~GLSLCompiler()
{
	if (m_programID)
		glDeleteProgram(m_programID);
}

void GLSLCompiler::refresh()
{
	create(m_vtPath, m_fmPath);
}

void GLSLCompiler::create(const std::string & vertFilePath, const std::string & fragFilePath)
{
	compileShaders(vertFilePath, fragFilePath);
	linkShaders();
}

void GLSLCompiler::createDefault()
{
	//create("Shaders/error.vert", "Shaders/error.frag");
	
	
	if (m_programID)
		glDeleteProgram(m_programID);
	
	if (m_fragID)
		glDeleteShader(m_fragID);
	if (m_vertID)
		glDeleteShader(m_vertID);
	
	
	m_programID = glCreateProgram();
	m_vertID = glCreateShader(GL_VERTEX_SHADER);
	m_fragID = glCreateShader(GL_FRAGMENT_SHADER);
	

	std::string tmpFileContent =
		"#version 330\n"
	
		"uniform mat4 uModel;\n"
		"uniform mat4 uView;\n"
		"uniform mat4 uProj;\n"
	
		"layout(location = 0) in vec3 in_vert;\n"
		"layout(location = 1) in vec2 in_uv;\n"
		"layout(location = 2) in vec3 in_normal;\n"
	
		"out vec2 texcoord;\n"
		"out vec3 norm;\n"
		"out vec3 pos;\n"
	
		"void main()\n"
		"{\n"
		"texcoord = in_uv;\n"
		"norm = mat3(uView) * mat3(uModel) * in_normal;\n"
	
		"pos = (uView * uModel * vec4(in_vert, 1.0f)).xyz;\n"
	
		"gl_Position = uProj * vec4(pos, 1.0f);\n"
		"}\n"
		;
	
	m_vtsh = tmpFileContent;
	const char * tmp = tmpFileContent.c_str();
	m_vertID = glCreateShader(GL_VERTEX_SHADER);
	
	glShaderSource(m_vertID, 1, &tmp, nullptr);
	glCompileShader(m_vertID);
	
	
	tmpFileContent =
		"#version 330\n"
	
		"in vec2 texcoord;\n"
		"in vec3 norm;\n"
		"in vec3 pos;\n"
	
		"out vec4 outColor;\n"
	
		"void main()\n"
		"{\n"
		"outColor = vec4(1, 0, 1, 1);\n"
		"}\n"
		;
	tmp = tmpFileContent.c_str();
	
	m_fragID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(m_fragID, 1, &tmp, nullptr);
	glCompileShader(m_fragID);
	
	linkShaders();
	//delete tmpFileContent;
}

void GLSLCompiler::compileShaders(const std::string & vertFilePath, const std::string & fragFilePath)
{
	m_vtPath = vertFilePath;
	m_fmPath = fragFilePath;


	if (m_programID)
		glDeleteProgram(m_programID);

	m_programID = glCreateProgram();
	m_vertID = glCreateShader(GL_VERTEX_SHADER);
	compileShader(VT_SHADER, vertFilePath, m_vertID);

	m_fragID = glCreateShader(GL_FRAGMENT_SHADER);
	compileShader(FG_SHADER, fragFilePath, m_fragID);
}

void GLSLCompiler::linkShaders()
{
	glAttachShader(m_programID, m_vertID);
	glAttachShader(m_programID, m_fragID);

	glLinkProgram(m_programID);

	GLint isLinked = 0;
	glGetProgramiv(m_programID, GL_LINK_STATUS, (int *)&isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		char* infoLog = new char[maxLength];
		glGetProgramInfoLog(m_programID, maxLength, &maxLength, infoLog);

		// We don't need the program anymore.
		glDeleteProgram(m_programID);
		// Don't leak shaders either.
		glDeleteShader(m_vertID);
		glDeleteShader(m_fragID);

		// Use the infoLog as you see fit.
		m_log->writeLog(infoLog + '\n');
		// In this simple program, we'll just leave
		return;
	}
	glDetachShader(m_programID, m_vertID);
	glDetachShader(m_programID, m_fragID);
	glDeleteShader(m_vertID);
	glDeleteShader(m_fragID);
}

void GLSLCompiler::addAtribute(const std::string attributeName, short attribSize)
{
	glBindAttribLocation(m_programID, m_attribNum, attributeName.c_str());
	m_attribNum += attribSize;
}

GLint GLSLCompiler::getUniformLocation(const char * uniform)
{
	return glGetUniformLocation(m_programID, uniform);
}

void GLSLCompiler::enable()
{
	glUseProgram(m_programID);
		
	if (!m_enabled)
	{
		m_attribs = (GLuint*)realloc(m_attribs, ++m_num * sizeof(GLuint));
		m_programs = (GLuint*)realloc(m_programs, m_num * sizeof(GLuint));
	
		m_programs[m_num - 1] = m_programID;
		findAtributes();
		m_attribs[m_num - 1] = m_attribNum;
	
		glUseProgram(m_programID);
		for (int a = 0; a < m_attribNum; a++)
			glEnableVertexAttribArray(a);
		m_enabled = true;
	}
}

void GLSLCompiler::disable()
{
	if (m_enabled)
	{
		for (int a = 0; a < m_attribNum; a++)
			glDisableVertexAttribArray(a);
	
		if (m_num - 1 > 0)
		{
			glUseProgram(m_programs[--m_num - 1]);
			for (int a = 0; a < m_attribs[m_num - 1]; a++)
				glEnableVertexAttribArray(a);
			m_enabled = false;
		} else
			glUseProgram(0);
	
		m_enabled = !m_enabled;
	}

	//glUseProgram(0);

}

void GLSLCompiler::compileShader(Shaders shadType, const std::string filePath, GLuint id)
{
	std::ifstream
		shader(filePath);

	std::string fileContent = "", line;

	while (std::getline(shader, line))
		fileContent += line + '\n';
	shader.close();

	if (shadType == VT_SHADER) //stores vtsh
		m_vtsh = fileContent;

	const char* tmpFileContent = fileContent.c_str();
	glShaderSource(id, 1, &tmpFileContent, nullptr);
	glCompileShader(id);
	//delete tmpFileContent;

	GLint success = 0;
	glGetShaderiv(id, GL_COMPILE_STATUS, &success);

	if (success == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &maxLength);

		char* errorLog = new char[maxLength];
		glGetShaderInfoLog(id, maxLength, &maxLength, errorLog);

		m_log->writeLog(errorLog + '\n');

		createDefault();
	}
}

void GLSLCompiler::findAtributes()
{
	unsigned short count = 0;
	for (int a = 0; a < m_vtsh.size(); a++)
	{
		if (m_vtsh.substr(a, 3) == "in ")
		{
			count++;
			a += 3;
		}
	}
	m_attribNum = count;
}

