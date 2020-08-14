#pragma once
#include <glad/glad.h>
#include <typedef.h>
#include <image.h>
#include <ys/error.h>
#include <ys/shader.h>

namespace ys
{
	class SkyBox
	{
	public:
		SkyBox(const char* file);

		~SkyBox();

		void Draw(ys::Shader& shader, GLint skyLoc);

	private:
		GLuint texID;
		GLuint VAO;
	};
}