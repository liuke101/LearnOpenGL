////phong模型
#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
}; 
uniform Material material;

//点光源
struct PointLight
{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};
uniform PointLight pointlight;

uniform vec3 viewPos;

void main()
{
    //环境光
    vec3 ambient  = pointlight.ambient  * texture(material.diffuse, TexCoords).rgb;
    //漫反射
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(pointlight.position - FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse  = pointlight.diffuse  * diff * texture(material.diffuse, TexCoords).rgb;
    //高光反射
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = pointlight.specular * spec * texture(material.specular, TexCoords).rgb;
    //放射光贴图
    vec3 emission = texture(material.emission, TexCoords).rgb;
    // 衰减
    float distance    = length(pointlight.position - FragPos);
    float attenuation = 1.0 / (pointlight.constant + pointlight.linear * distance +
    pointlight.quadratic * (distance * distance));

    vec3 result = (ambient + diffuse + specular + emission) * attenuation;
    FragColor = vec4(result , 1.0);
}
        
// 反射
//#version 330 core
//out vec4 FragColor;
//
//in vec3 Normal;
//in vec3 Position;
//
//uniform vec3 cameraPos;
//uniform samplerCube skybox;
//
//void main()
//{
//    vec3 I = normalize(Position - cameraPos);
//    vec3 R = reflect(I, normalize(Normal));
//    FragColor = vec4(texture(skybox, R).rgb, 1.0);
//}

//// 折射
//#version 330 core
//out vec4 FragColor;
//
//in vec3 Normal;
//in vec3 Position;
//
//uniform vec3 cameraPos;
//uniform samplerCube skybox;
//
//void main()
//{
//    float ratio = 1.00 / 2.01;
//    vec3 I = normalize(Position - cameraPos);
//    vec3 R = refract(I, normalize(Normal), ratio);
//    FragColor = vec4(texture(skybox, R).rgb, 1.0);
//}