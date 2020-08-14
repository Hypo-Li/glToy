#define _CRT_SECURE_NO_WARNINGS //避免fopen报错
#include <ys/model.h>

ys::StaticMesh::StaticMesh(BYTE* dataBuffer, BYTE* meshBuffer, GLenum usage)
{
    DWORD* meshData = (DWORD*)meshBuffer;
    //meshData[0] 顶点个数
    //meshData[1] 顶点偏移
    //meshData[2] 面个数
    //meshData[3] 面偏移
    //meshData[4] 纹理个数
    //meshData[5] 纹理偏移
    indexNum = meshData[2] * 3;
    BYTE* vertexBuffer = dataBuffer + meshData[1];
    DWORD vertexSize = meshData[0] * 56;
    BYTE* faceBuffer = dataBuffer + meshData[3];
    DWORD faceSize = meshData[2] * 12;
    CreateVAO(vertexBuffer, vertexSize, faceBuffer, faceSize, usage);
}

ys::StaticMesh::~StaticMesh()
{

}

void ys::StaticMesh::CreateVAO(BYTE* vertexBuffer, DWORD vertexSize, BYTE* faceBuffer, DWORD faceSize, GLenum usage)
{
    GLuint VBO, EBO;
    glGenVertexArrays(1, &VAO); //注册VAO
    glGenBuffers(1, &VBO); //注册VBO
    glGenBuffers(1, &EBO); //注册EBO

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexSize, vertexBuffer, usage);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 56, (void*)0); //Position (56byte是一个顶点结构的大小)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 56, (void*)(3 * sizeof(float))); //Normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 56, (void*)(6 * sizeof(float))); //Tangent
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 56, (void*)(9 * sizeof(float))); //Bitangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, 56, (void*)(12 * sizeof(float))); //Texture Coords
    glEnableVertexAttribArray(4);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, faceSize, faceBuffer, GL_STATIC_DRAW);

    glBindVertexArray(0);

    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void ys::StaticMesh::DistributeTexture(std::vector<GLuint>& id, std::vector<DWORD>& offset, BYTE* textureBuffer, DWORD textureNum)
{
    for (DWORD i = 0; i < textureNum; i++)
    {
        DWORD* textureData = (DWORD*)(textureBuffer + i * 8);
        switch ((TextureType)textureData[0])
        {
        case TEXTURE_TYPE_DIFFUSE:
            for (DWORD j = 0; j < offset.size(); j++)
            {
                if (textureData[1] == offset[j]) {
                    textureID_Diffuse = id[j];
                    break;
                }
            }
            break;
        case TEXTURE_TYPE_SPECULAR:
            for (DWORD j = 0; j < offset.size(); j++)
            {
                if (textureData[1] == offset[j]) {
                    textureID_Specular = id[j];
                    break;
                }
            }
            break;
        case TEXTURE_TYPE_NORMALS:
            for (DWORD j = 0; j < offset.size(); j++)
            {
                if (textureData[1] == offset[j]) {
                    textureID_Normals = id[j];
                    break;
                }
            }
            break;
        default:
            break;
        }
    }
}

void ys::StaticMesh::Draw(ys::Shader& shader)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID_Diffuse);
    glUniform1i(shader.diffuseLoc, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureID_Specular);
    glUniform1i(shader.specularLoc, 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, textureID_Normals);
    glUniform1i(shader.normalsLoc, 2);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indexNum, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void ys::StaticMesh::DeleteVAO()
{
    glDeleteVertexArrays(1, &VAO);
}

ys::StaticModel::StaticModel(const char* file, GLenum usage)
{
    BYTE* buffer;
    FILE* fp = fopen(file, "rb");
    if (fp == nullptr) {
        ys::PutErrorLog(file, "Failed to load model;");
        exit(-1);
    }
    fseek(fp, 0, SEEK_END);
    size_t fileSize = ftell(fp) - 4;
    fseek(fp, 0, SEEK_SET);
    BYTE* fileBuffer = (BYTE*)malloc(fileSize);
    DWORD bufferSize;
    fread(&bufferSize, sizeof(DWORD), 1, fp);
    buffer = (BYTE*)malloc(bufferSize);
    fread(fileBuffer, fileSize, 1, fp);
    fclose(fp);
    FL2_decompress(buffer, bufferSize, fileBuffer, fileSize);
    free(fileBuffer);
    DWORD numMesh = *(DWORD*)buffer;

    //初始化所有网格
    std::vector<DWORD> texOffsetVector;
    for (DWORD i = 0; i < numMesh; i++)
    {
        BYTE* meshBuffer = buffer + sizeof(DWORD) + i * STATIC_MESH_SIZE;
        StaticMesh curtMesh(buffer, meshBuffer, usage);
        mesh.push_back(curtMesh);
        DWORD* meshData = (DWORD*)meshBuffer;
        //meshData[4] 纹理个数
        //meshData[5] 纹理偏移
        for (DWORD j = 0; j < meshData[4]; j++)
        {
            DWORD* textureData = (DWORD*)(buffer + meshData[5] + j * TEXTURE_SIZE);
            DWORD k;
            for (k = 0; k < texOffsetVector.size(); k++)
            {
                if (textureData[1] == texOffsetVector[k])
                    break;
            }
            if (k == texOffsetVector.size())
                texOffsetVector.push_back(textureData[1]);
        }
    }

    //加载所有纹理
    for (DWORD i = 0; i < texOffsetVector.size(); i++)
    {
        char* textureFile = (char*)(buffer + texOffsetVector[i]);
        int width, height, channels;
        BYTE* data = LoadImage(textureFile, &width, &height, &channels, 0);
        if (!data) {
            ys::PutErrorLog(textureFile, "Load error;");
        }
        GLenum format;
        switch (channels)
        {
        case 1:
            format = GL_RED;
            break;
        case 3:
            format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            break;
        default:
            format = GL_RGB;
            break;
        }

        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        FreeImage(data);

        texVector.push_back(textureID);
    }

    //为所有网格分配纹理
    for (DWORD i = 0; i < mesh.size(); i++)
    {
        DWORD* meshData = (DWORD*)(buffer + 4 + i * STATIC_MESH_SIZE);
        mesh[i].DistributeTexture(texVector, texOffsetVector, buffer + meshData[5], meshData[4]);
    }

    free(buffer);
}

ys::StaticModel::~StaticModel()
{
    for (DWORD i = 0; i < mesh.size(); i++)
        mesh[i].DeleteVAO();
    for (DWORD i = 0; i < texVector.size(); i++)
        glDeleteTextures(1, &texVector[i]);
}

void ys::StaticModel::Draw(ys::Shader& shader)
{
    shader.Use();
    for (DWORD i = 0; i < mesh.size(); i++)
        mesh[i].Draw(shader);
}

//动态模型部分暂未完成

ys::AnimMesh::AnimMesh(BYTE* dataBuffer, BYTE* meshBuffer, GLenum usage)
{
    DWORD* meshData = (DWORD*)meshBuffer;
    //meshData[0] 顶点个数
    //meshData[1] 顶点偏移
    //meshData[2] 面个数
    //meshData[3] 面偏移
    //meshData[4] 纹理个数
    //meshData[5] 纹理偏移
    //meshData[6] 骨骼个数
    //meshData[7] 骨骼偏移
    indexNum = meshData[2] * 3;
    BYTE* vertexBuffer = dataBuffer + meshData[1];
    DWORD vertexSize = meshData[0] * 56;
    BYTE* faceBuffer = dataBuffer + meshData[3];
    DWORD faceSize = meshData[2] * 12;
    CreateVAO(vertexBuffer, vertexSize, faceBuffer, faceSize, usage);
}

ys::AnimMesh::~AnimMesh()
{
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &VAO);
}

void ys::AnimMesh::CreateVAO(BYTE* vertexBuffer, DWORD vertexSize, BYTE* faceBuffer, DWORD faceSize, GLenum usage)
{
    glGenVertexArrays(1, &VAO); //注册VAO
    glGenBuffers(1, &VBO); //注册VBO
    glGenBuffers(1, &EBO); //注册EBO

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexSize, vertexBuffer, usage);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 56, (void*)0); //Position (56byte是一个顶点结构的大小)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 56, (void*)(3 * sizeof(float))); //Normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 56, (void*)(6 * sizeof(float))); //Tangent
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 56, (void*)(9 * sizeof(float))); //Bitangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, 56, (void*)(12 * sizeof(float))); //Texture Coords
    glEnableVertexAttribArray(4);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, faceSize, faceBuffer, GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void ys::AnimMesh::DistributeTexture(std::vector<GLuint>& id, std::vector<DWORD>& offset, BYTE* textureBuffer, DWORD textureNum)
{
    for (DWORD i = 0; i < textureNum; i++)
    {
        DWORD* textureData = (DWORD*)(textureBuffer + i * 8);
        switch ((TextureType)textureData[0])
        {
        case TEXTURE_TYPE_DIFFUSE:
            for (DWORD j = 0; j < offset.size(); j++)
            {
                if (textureData[1] == offset[j]) {
                    textureID_Diffuse = id[j];
                    break;
                }
            }
            break;
        case TEXTURE_TYPE_SPECULAR:
            for (DWORD j = 0; j < offset.size(); j++)
            {
                if (textureData[1] == offset[j]) {
                    textureID_Specular = id[j];
                    break;
                }
            }
            break;
        case TEXTURE_TYPE_NORMALS:
            for (DWORD j = 0; j < offset.size(); j++)
            {
                if (textureData[1] == offset[j]) {
                    textureID_Normals = id[j];
                    break;
                }
            }
            break;
        default:
            break;
        }
    }
}

ys::AnimModel::AnimModel(const char* file, GLenum usage)
{
    BYTE* buffer;
    FILE* fp = fopen(file, "rb");
    if (fp == nullptr) {
        ys::PutErrorLog(file, "Failed to load model;");
        exit(-1);
    }
    fseek(fp, 0, SEEK_END);
    size_t fileSize = ftell(fp) - 4;
    fseek(fp, 0, SEEK_SET);
    BYTE* fileBuffer = (BYTE*)malloc(fileSize);
    DWORD bufferSize;
    fread(&bufferSize, sizeof(DWORD), 1, fp);
    buffer = (BYTE*)malloc(bufferSize);
    fread(fileBuffer, fileSize, 1, fp);
    fclose(fp);
    FL2_decompress(buffer, bufferSize, fileBuffer, fileSize);
    free(fileBuffer);
    DWORD numMesh = *(DWORD*)buffer;
    DWORD numAnim = *(((DWORD*)buffer) + 1);
    DWORD numBoneNode = *(((DWORD*)buffer) + 2);

    //初始化所有网格
    std::vector<DWORD> texOffsetVector;
    for (DWORD i = 0; i < numMesh; i++)
    {
        BYTE* meshBuffer = buffer + sizeof(DWORD) * 3 + i * ANIM_MESH_SIZE;
        AnimMesh curtMesh(buffer, meshBuffer, usage);
        mesh.push_back(curtMesh);
        DWORD* meshData = (DWORD*)meshBuffer;
        //meshData[4] 纹理个数
        //meshData[5] 纹理偏移
        for (DWORD j = 0; j < meshData[4]; j++)
        {
            DWORD* textureData = (DWORD*)(buffer + meshData[5] + j * TEXTURE_SIZE);
            DWORD k;
            for (k = 0; k < texOffsetVector.size(); k++)
            {
                if (textureData[1] == texOffsetVector[k])
                    break;
            }
            if (k == texOffsetVector.size())
                texOffsetVector.push_back(textureData[1]);
        }
    }

    //加载所有纹理
    for (DWORD i = 0; i < texOffsetVector.size(); i++)
    {
        char* textureFile = (char*)(buffer + texOffsetVector[i]);
        int width, height, channels;
        BYTE* data = LoadImage(textureFile, &width, &height, &channels, 0);
        if (!data) {
            ys::PutErrorLog(textureFile, "Load error;");
        }
        GLenum format;
        switch (channels)
        {
        case 1:
            format = GL_RED;
            break;
        case 3:
            format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            break;
        default:
            format = GL_RGB;
            break;
        }

        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        FreeImage(data);

        texVector.push_back(textureID);
    }

    //为所有网格分配纹理
    for (DWORD i = 0; i < mesh.size(); i++)
    {
        DWORD* meshData = (DWORD*)(buffer + sizeof(DWORD) * 3 + i * ANIM_MESH_SIZE);
        mesh[i].DistributeTexture(texVector, texOffsetVector, buffer + meshData[5], meshData[4]);
    }

    //读取所有动画信息

}