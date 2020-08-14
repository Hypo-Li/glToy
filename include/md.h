#pragma once
#include <glm/glm.hpp>
#include <typedef.h>

enum TextureType
{
	TEXTURE_TYPE_AMBIENT = 0x0,

	TEXTURE_TYPE_DIFFUSE = 0x1,

	TEXTURE_TYPE_SPECULAR = 0x2,

	TEXTURE_TYPE_NORMALS = 0x3,

	TEXTURE_TYPE_HEIGHT = 0x4,
};

const DWORD BONE_NUM_PER_VERTEX = 4;

const DWORD STATIC_MESH_SIZE = 24;
const DWORD VERTEX_SIZE = 56;
const DWORD WEIGHT_VERTEX_SIZE = VERTEX_SIZE + 8 * BONE_NUM_PER_VERTEX;
const DWORD FACE_SIZE = 12;
const DWORD TEXTURE_SIZE = 8;
const DWORD ANIM_MESH_SIZE = 32;
const DWORD BONE_SIZE = 76;
const DWORD WEIGHT_SIZE = 8;
const DWORD BONE_NODE_SIZE = 16;
const DWORD ANIM_SIZE = 16;
const DWORD BONE_ANIM_SIZE = 28;
const DWORD POS_FRAME_SIZE = 16;
const DWORD ROT_FRAME_SIZE = 20;
const DWORD SCA_FRAME_SIZE = 16;

/*
typedef struct Vertex
{
	glm::vec3 position;

	glm::vec3 normals;

	glm::vec3 tangent;

	glm::vec3 bitangent;

	glm::vec2 texCoords;
}Vertex; //56byte

typedef struct WeightVertex
{
	glm::vec3 position;

	glm::vec3 normals;

	glm::vec3 tangent;

	glm::vec3 bitangent;

	glm::vec2 texCoords;

	DWORD bone[];

	float weight[];
}WeightVertex;

typedef struct Face
{
	DWORD indexes[3];
}Face; //12byte

enum TextureType
{
	TEXTURE_TYPE_AMBIENT = 0x0,

	TEXTURE_TYPE_DIFFUSE = 0x1,

	TEXTURE_TYPE_SPECULAR = 0x2,

	TEXTURE_TYPE_NORMALS = 0x3,

	TEXTURE_TYPE_HEIGHT = 0x4,
};

typedef struct Texture
{
	TextureType type;

	DWORD offset;
}Texture; //8byte

typedef struct StaticMesh
{
	DWORD numVertex;

	DWORD vertexOffset;

	DWORD numFace;

	DWORD faceOffset;

	DWORD numTexture;

	DWORD textureOffset;
}StaticMesh; //24byte

typedef struct Bone
{
	DWORD id;

	DWORD numWeight;

	DWORD weightOffset;

	glm::mat4 offsetMat;
}Bone; //76byte

typedef struct Weight
{
	DWORD indexes;

	float weight;
}Weight; //8byte

typedef struct AnimMesh
{
	DWORD numVertex;

	DWORD vertexOffset;

	DWORD numFace;

	DWORD faceOffset;

	DWORD numTexture;

	DWORD textureOffset;

	DWORD numBone;

	DWORD boneOffset;
}AnimMesh; //32byte

typedef struct BoneNode
{
	DWORD id;

	DWORD parentNodeID;

	DWORD numChildren;

	DWORD childrenNodeID;
}BoneNode; //16byte

typedef struct Anim
{
	DWORD fps;

	DWORD numFrame;

	DWORD numBoneAnim;

	DWORD boneAnimOffset;
}Anim; //16byte

typedef struct BoneAnim
{
	DWORD id;

	DWORD numPositionKeyFrame;

	DWORD PositionKeyFrameOffset;

	DWORD numRotationKeyFrame;

	DWORD RotationKeyFrameOffset;

	DWORD numScalingKeyFrame;

	DWORD ScalingKeyFrameOffset;
}BoneAnim; //28byte

typedef struct PositionKeyFrame
{
	DWORD timeStamp;

	glm::vec3 position;
}PositionKeyFrame; //16byte

typedef struct RotationKeyFrame
{
	DWORD timeStamp;

	glm::vec4 rotation; //ËÄÔªÊý
}RotationKeyFrame; //20byte

typedef struct ScalingKeyFrame
{
	DWORD timeStamp;

	glm::vec3 scaling;
}ScalingKeyFrame; //16byte
*/
