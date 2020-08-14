#include <ys/shader.h>

ys::Shader::Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
{
	GLuint vertexShader, fragmentShader, geometryShader;
	size_t fileSize;

	//��ȡԴ���ļ�
	ys::FileBufferA* vertexCode = new ys::FileBufferA(vertexPath);
	ys::FileBufferA* fragmentCode = new ys::FileBufferA(fragmentPath);
	extern ys::FileBufferA* geometryCode;

	//������ɫ��
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(vertexShader, 1, &vertexCode->buffer, NULL);
	glShaderSource(fragmentShader, 1, &fragmentCode->buffer, NULL);

	//����
	glCompileShader(vertexShader);
	CheckError(vertexShader, true);
	delete vertexCode;
	glCompileShader(fragmentShader);
	CheckError(fragmentShader, true);
	delete fragmentCode;

	//��������ɫ��
	if (geometryPath != nullptr) {
		ys::FileBufferA* geometryCode = new ys::FileBufferA(geometryPath);
		geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometryShader, 1, &geometryCode->buffer, NULL);
		glCompileShader(geometryShader);
		CheckError(geometryShader, true);
		delete geometryCode;
	}

	//����
	this->id = glCreateProgram();
	glAttachShader(id, vertexShader);
	glAttachShader(id, fragmentShader);
	if (geometryPath != nullptr)
		glAttachShader(id, geometryShader);
	glLinkProgram(id);
	CheckError(id, false);

	//ɾ��shader
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	if (geometryPath != nullptr)
		glDeleteShader(geometryShader);
}

ys::Shader::~Shader()
{
	glDeleteProgram(id);
}

void ys::Shader::CheckError(GLuint checkID, bool type)
{
	int success;
	char infolog[512];
	if (type) {
		glGetShaderiv(checkID, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(checkID, 512, NULL, infolog);
			ys::PutErrorLog("Shader_Compile", infolog);
		}
	}
	else {
		glGetProgramiv(checkID, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(checkID, 512, NULL, infolog);
			ys::PutErrorLog("Program_Link", infolog);
		}
	}
}