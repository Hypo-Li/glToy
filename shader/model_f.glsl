#version 460 core
in VS_OUT
{
    vec2 texCoords;
    vec3 viewPos;
    vec3 fragPos;
    vec3 lightPos;
    vec4 fragPos_lightSpace;
}fs_in;

out vec4 fragColor;

uniform sampler2D textureDiffuse;
uniform sampler2D textureSpecular;
uniform sampler2D textureNormals;
uniform sampler2D depthMap;

void main()
{
    vec3 normals = texture(textureNormals, fs_in.texCoords).rgb;
    normals = normalize(normals * 2.0 - 1.0);
    vec3 diffuseColor = texture(textureDiffuse, fs_in.texCoords).rgb;
    vec3 slightColor = vec3(1.0f, 1.0f, 1.0f);

    vec3 ambient = 0.5 * diffuseColor;

    vec3 lightDir = normalize(fs_in.lightPos - fs_in.fragPos); //从片段指向光源
    vec3 diffuse = max(dot(normals, lightDir), 0.0) * diffuseColor;

    vec3 viewDir = normalize(fs_in.viewPos - fs_in.fragPos); //从片段指向观察者
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normals, halfwayDir), 0.0f), 64);
    vec3 specular = texture(textureSpecular, fs_in.texCoords).r * spec * slightColor;

    vec3 projCoords = fs_in.fragPos_lightSpace.xyz / fs_in.fragPos_lightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(depthMap, projCoords.xy).r;
    float curtDepth = projCoords.z;
    float shadow = curtDepth > closestDepth ? 1.0 : 0.0;
    if(projCoords.z > 1.0)
        shadow = 0.0;

    vec3 result = ambient + (1.0 - shadow) * (diffuse + specular);
    fragColor = vec4(result, 1.0);
}

/* 
颜色：
嫣红: vec3(0.898039, 0.301961, 0.258824);
桔橙: vec3(0.952941, 0.482353, 0.113725);
明黄: vec3(0.984314, 0.741176, 0.0313726);
橄榄: vec3(0.552941, 0.776471, 0.247059);
森绿: vec3(0.223529, 0.709804, 0.290196);
天青: vec3(0.109804, 0.733333, 0.705882);
海蓝: vec3(0, 0.505882, 1);
姹紫: vec3(0.403922, 0.223529, 0.713726);
木槿: vec3(0.611765, 0.14902, 0.690196);
桃粉: vec3(0.878431, 0.223529, 0.592157);
棕褐: vec3(0.647059, 0.403922, 0.247059);
玄灰: vec3(0.529412, 0.6, 0.639216);
 */