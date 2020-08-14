#version 460 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec3 tangent;
layout (location = 3) in vec3 bitangent;
layout (location = 4) in vec2 texCoords;

out VS_OUT
{
    vec2 texCoords;
    vec3 viewPos;
    vec3 fragPos;
    vec3 lightPos;
    vec4 fragPos_lightSpace;
}vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMat;
uniform vec3 viewPos;
uniform vec3 lightPos;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
    vs_out.texCoords = texCoords;
    vec3 fragPos = vec3(model * vec4(position, 1.0f));

    mat3 normalMat = transpose(inverse(mat3(model)));
    vec3 T = normalize(normalMat * tangent);
    vec3 B = normalize(normalMat * bitangent);
    vec3 N = normalize(normalMat * normals);
    mat3 TBN = transpose(mat3(T, B, N));
    vs_out.viewPos = TBN * viewPos;
    vs_out.fragPos = TBN * fragPos;
    vs_out.lightPos = TBN * lightPos;

    vs_out.fragPos_lightSpace = lightSpaceMat * vec4(fragPos, 1.0);
}