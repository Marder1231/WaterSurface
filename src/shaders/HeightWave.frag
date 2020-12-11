#version 450 core
out vec4 FragColor;

struct Material {
    vec3 diffuse;
    vec3 specular;
    float shininess;
    samplerCube u_skybox;
}; 


struct DirLight {
    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};

#define NR_POINT_LIGHTS 3
uniform int u_PointLightAmount;
#define NR_DIR_LIGHTS 1
uniform int u_DirLightAmount;
#define NR_SPOT_LIGHTS 1
uniform int u_SpotLightAmount;

in vec3 f_position;
in vec2 f_textCoords;

uniform vec3 u_cameraPos;

uniform DirLight dirLights[NR_DIR_LIGHTS];
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLights[NR_SPOT_LIGHTS];
uniform Material material;

// function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 f_position, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 f_position, vec3 viewDir);

float uniformQuantization(float f, int step)
{
    f*=256;
    float q = 256.0 / step;
    int interval = int(f / q);

    f = interval * q;

    return f / 256.0;
}
vec3 ToonShader(vec3 originColor)
{
    float newPixel[3] = {
        uniformQuantization(originColor.x, 8),
        uniformQuantization(originColor.y, 8),
        uniformQuantization(originColor.z, 4),
    };

    return vec3(newPixel[0], newPixel[1], newPixel[2]);
}

void main()
{    
    // properties
    vec3 dx = dFdx(f_position);
    vec3 dy = dFdy(f_position);
    vec3 norm = normalize(cross(dx, dy));
    vec3 viewDir = normalize(u_cameraPos - f_position);
    
    vec3 dxTexture = dFdx(vec3(f_textCoords, 1));
    vec3 dyTexture = dFdy(vec3(f_textCoords, 1));
    vec3 normTexture = normalize(cross(dxTexture, dyTexture));

    float air = 1.00f;
    float water = 1.33f;
    float ice = 1.309f;
    float glass = 1.52f;
    float diamond = 2.42f;
    //          from / into;
    float ratio = air / air;

    vec3 result = vec3(0);
    vec3 R;
   
    R = refract(-viewDir, norm, ratio);
//    R.x = -R.x;
//    R.z = -R.z;
    result += vec3(1.0f * texture(material.u_skybox, R).rgb );

    R = reflect(-viewDir, norm);
    result += vec3(.5f * texture(material.u_skybox, R).rgb);
//    result = norm;
    // == =====================================================
    // Our lighting is set up in 3 phases: directional, point lights and an optional flashlight
    // For each phase, a calculate function is defined that calculates the corresponding color
    // per lamp. In the main() function we take all the calculated colors and sum them up for
    // this fragment's final color.
    // == =====================================================
    // phase 1: directional lighting
    
//    for(int i = 0; i < u_DirLightAmount; i++)
//        result += CalcDirLight(dirLights[i], norm, viewDir);  
//    for(int i = 0; i < u_PointLightAmount; i++)
//        result += CalcPointLight(pointLights[i], norm, f_position, viewDir);
//    for(int i = 0; i < u_SpotLightAmount; i++)
//        result += CalcSpotLight(spotLights[i], norm, f_position, viewDir);

    FragColor = vec4((result) , 1.0) ;
}

// calculates the color when using a directional light.

// calculates the color when using a point light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient = light.ambient * material.diffuse;
    vec3 diffuse = light.diffuse * diff * material.diffuse;
    vec3 specular = light.specular * spec * material.specular;
    return (ambient + diffuse + specular);
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 f_position, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - f_position);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - f_position);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient = light.ambient * material.diffuse;
    vec3 diffuse = light.diffuse * diff * material.diffuse;
    vec3 specular = light.specular * spec * material.specular;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 f_position, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - f_position);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - f_position);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient * vec3((material.diffuse));
    vec3 diffuse = light.diffuse * diff * vec3((material.diffuse));
    vec3 specular = light.specular * spec * vec3((material.specular));
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}