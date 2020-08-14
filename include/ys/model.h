#pragma once
#include <vector>
#include <fastlzma2/fast-lzma2.h>
#include <typedef.h>
#include <md.h>
#include <image.h>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <ys/shader.h>
#include <ys/error.h>

namespace ys
{
	class StaticMesh
	{
	public:
		StaticMesh(BYTE* dataBuffer, BYTE* meshBuffer, GLenum usage);

		~StaticMesh();

		void DistributeTexture(std::vector<GLuint>& id, std::vector<DWORD>& offset, BYTE* textureBuffer, DWORD textureNum);

		void Draw(ys::Shader& shader);

		void DeleteVAO();

	private:
		GLuint VAO;
		DWORD indexNum;
		GLuint textureID_Diffuse;
		GLuint textureID_Specular;
		GLuint textureID_Normals;

		void CreateVAO(BYTE* vertexBuffer, DWORD vertexSize, BYTE* faceBuffer, DWORD faceSize, GLenum usage);
	};

	class StaticModel
	{
	public:
		StaticModel(const char* file, GLenum usage);

		~StaticModel();

		void Draw(ys::Shader& shader);

	private:
		std::vector<StaticMesh> mesh;
		std::vector<GLuint> texVector;
	};

	class AnimMesh
	{
	public:
		AnimMesh(BYTE* dataBuffer, BYTE* meshBuffer, GLenum usage);

		~AnimMesh();

		void DistributeTexture(std::vector<GLuint>& id, std::vector<DWORD>& offset, BYTE* textureBuffer, DWORD textureNum);

	private:
		GLuint VAO, VBO, EBO;
		DWORD indexNum;
		GLuint textureID_Diffuse;
		GLuint textureID_Specular;
		GLuint textureID_Normals;

		void CreateVAO(BYTE* vertexBuffer, DWORD vertexSize, BYTE* faceBuffer, DWORD faceSize, GLenum usage);
	};

	typedef struct Anim
	{
		DWORD fps;
		DWORD duration;
	}Anim;

	typedef struct BoneNode
	{
		DWORD id;
		BoneNode* parent;
		std::vector<BoneNode> children;
	}BoneNode;

	class AnimModel
	{
	public:
		AnimModel(const char* file, GLenum usage);

		~AnimModel();

		void Draw(ys::Shader& shader, DWORD anim, float time);
	private:
		std::vector<AnimMesh> mesh;
		std::vector<GLuint> texVector;
		std::vector<Anim> anim;
		std::vector<BoneNode*> boneStruct;
	};
}