#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <ys/error.h>
#include <ys/io.h>
namespace ys
{
	class Shader
	{
	public:
		GLint diffuseLoc;
		GLint specularLoc;
		GLint normalsLoc;

		Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);

		~Shader();

		GLint GetUniformLoc(const char* uniform);

		void GetDefaultLoc(const char* diffuseUniform, const char* specularUniform, const char* normalsUniform);

		void Use();

		void SetBool(GLint loc, bool value);

		void SetInt(GLint loc, int value);

		void SetFloat(GLint loc, float value);

		void SetVec3(GLint loc, glm::vec3 value);

		void SetVec3(GLint loc, float x, float y, float z);

		void SetMat4(GLint loc, glm::mat4 value);

		void SetTexture2D(GLint loc, GLuint num, GLuint texID);

	private:
		GLuint id;

		void CheckError(GLuint checkID, bool type);
	};
}

inline GLint ys::Shader::GetUniformLoc(const char* uniform)
{
	return glGetUniformLocation(id, uniform);
}

inline void ys::Shader::GetDefaultLoc(const char* diffuseUniform, const char* specularUniform, const char* normalsUniform)
{
	diffuseLoc = glGetUniformLocation(id, diffuseUniform);
	specularLoc = glGetUniformLocation(id, specularUniform);
	normalsLoc = glGetUniformLocation(id, normalsUniform);
}

inline void ys::Shader::Use()
{
	glUseProgram(id);
}

inline void ys::Shader::SetBool(GLint loc, bool value)
{
	glUniform1i(loc, value);
}

inline void ys::Shader::SetInt(GLint loc, int value)
{
	glUniform1i(loc, value);
}

inline void ys::Shader::SetFloat(GLint loc, float value)
{
	glUniform1f(loc, value);
}

inline void ys::Shader::SetVec3(GLint loc, glm::vec3 value)
{
	glUniform3fv(loc, 1, &value[0]);
}

inline void ys::Shader::SetVec3(GLint loc, float x, float y, float z)
{
	glUniform3f(loc, x, y, z);
}

inline void ys::Shader::SetMat4(GLint loc, glm::mat4 value)
{
	glUniformMatrix4fv(loc, 1, GL_FALSE, &value[0][0]);
}

inline void ys::Shader::SetTexture2D(GLint loc, GLuint num, GLuint texID)
{
	glActiveTexture(GL_TEXTURE0 + num);
	glBindTexture(GL_TEXTURE_2D, texID);
	glUniform1i(loc, num);
}