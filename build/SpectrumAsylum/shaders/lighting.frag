#version 330 core

// Point light components
struct PointLight
{
    vec4 diffuse;
    vec4 ambient;
    vec4 specular;

    vec3 attenuation;
    vec3 position;
};

// Directional light components
struct DirectionalLight
{
    vec4 diffuse;
    vec4 ambient;
    vec4 specular;

    vec3 attenuation;
    vec3 direction;
};

// Spot light components
struct SpotLight
{
    vec4 diffuse;
    vec4 ambient;
    vec4 specular;

    vec3 attenuation;
    vec3 position;
    vec3 direction;

    float innerCutoff;
    float outerCutoff;

};


// Material for light reflection
struct Material
{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec4 emissive;

    float shininess;
};

// One material per mesh
uniform Material objMat;

// Allow for up to 10 ligths of each type
uniform PointLight              omnilights[20];
uniform DirectionalLight        dirlights[20];
uniform SpotLight               spotlights[20];

// One texture per mesh
uniform sampler2D               meshTexture;

// Camera position
uniform vec3 viewPos;

// Number of active lights of each type to limit loop iterations
uniform int activeOmnis;
uniform int activeDirectionals;
uniform int activeSpots;

// Adapt base color if mesh is not textured
uniform int isTextured;

// Vertex shader output
in vec4 vertColor;
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

// Output fragment color
out vec4 FragColor;

vec4 AddOmnis(vec3 normalized, vec3 viewDirection);
vec4 AddDirectional(vec3 normalized, vec3 viewDirection);
vec4 AddSpot(vec3 normalized, vec3 viewDirection);

void main()
{
    // Fragment color
    vec4 finalColor;

    // Fragment normal
    vec3 normalized = normalize(Normal);

    // Eye to fragment direction vector
    vec3 viewDirection = normalize(viewPos - FragPos);

    finalColor += AddOmnis(normalized, viewDirection);

    finalColor += AddDirectional(normalized, viewDirection);

    finalColor += AddSpot(normalized, viewDirection);


    if (isTextured != 0.f)
        finalColor *= texture(meshTexture, TexCoords);

    // Output final fragment color after illumination (white if no lights in scene)
    FragColor = objMat.emissive + finalColor;

    FragColor = vec4(FragColor.rgb, objMat.ambient.a);
}

vec4 AddOmnis(vec3 normalized, vec3 viewDirection)
{
    vec4 color = vec4(0.f);

    // Loop through all active omnidirectional lights
    for (int i = 0; i < activeOmnis; ++i)
    {
        // Find ambient light
        vec4 ambient = omnilights[i].ambient * objMat.ambient;

        // Get point light direction
        vec3 lightDirection = normalize(omnilights[i].position - FragPos);

        // Get incidence angle cosine
        float cosTheta = max(dot(normalized, lightDirection), 0.f);

        // Get diffuse color from angle, material color and light color
        vec4 diffuse = omnilights[i].diffuse * (cosTheta * objMat.diffuse);

        // Find halfway direction between light and eye direction for blinn-phong shading
        vec3 halfwayDir = normalize(lightDirection + viewDirection);

        // Find reflection strength and multiply by material color
        float specularFactor = pow(max(dot(normalized, halfwayDir), 0.f), objMat.shininess);
        vec4 specular = omnilights[i].specular * (specularFactor * objMat.specular);

        // Apply attenuation if applicable
        if (omnilights[i].attenuation != vec3(0.f, 0.f, 0.f))
        {
             // Find fragment distance to light
            float dist = length(omnilights[i].position - FragPos);

            // Calculate attenuation from light constant, linear and quadratic components
            float atten = omnilights[i].attenuation.x;

            atten += omnilights[i].attenuation.y * dist;
            atten += omnilights[i].attenuation.z * (dist * dist);
            atten = 1.f / atten;

            // Update color components
            ambient *= atten;
            diffuse *= atten;
            specular *= atten;

        }

        // Update fragment color (ambient + diffuse + specular)
        color += vec4(ambient.xyz, objMat.ambient.w) + vec4(diffuse.xyz, objMat.diffuse.w) + vec4(specular.xyz, objMat.specular.w);
    }

    return color;
}

vec4 AddDirectional(vec3 normalized, vec3 viewDirection)
{
    vec4 color = vec4(0.f);

     for (int i = 0; i < activeDirectionals; ++i)
    {
        // Get direction from fragment to light
        vec3 lightDirection = normalize(-dirlights[i].direction);

        // Get incidence angle cosine
        float cosTheta = max(dot(normalized, lightDirection), 0.f);

        // Get diffuse color from angle, material color and light color
        vec4 diffuse = dirlights[i].diffuse * (cosTheta * objMat.diffuse);

        // Find halfway direction between light and eye direction for blinn-phong shading
        vec3 halfwayDir = normalize(lightDirection + viewDirection);

        // Find reflection strength and multiply by material color
        float specularFactor = pow(max(dot(normalized, halfwayDir), 0.f), 100);
        vec4 specular = dirlights[i].specular * (specularFactor * objMat.specular);


        // Update fragment color (ambient + diffuse + specular)
        color += (dirlights[i].ambient * objMat.ambient) + vec4(diffuse.xyz, objMat.diffuse.w) + vec4(specular.xyz, objMat.specular.w);

    }

    return color;
}

vec4 AddSpot(vec3 normalized, vec3 viewDirection)
{
    vec4 color = vec4(0.f);

    for (int i = 0; i < activeSpots; ++i)
    {
        // Find ambient light
        vec4 ambient = spotlights[i].ambient * objMat.ambient;

        // Find direction from fragment to ligth source
        vec3 fragTolight = normalize(spotlights[i].position - FragPos);

        // Get direction to light
        vec3 lightDirection = normalize(-spotlights[i].direction);

        // Get incidence angle cosine
        float cosTheta = max(dot(normalized, lightDirection), 0.f);

        // Get diffuse color from angle, material color and light color
        vec4 diffuse = spotlights[i].diffuse * (cosTheta * objMat.diffuse);

        // Get angle betweenn light direction and fragment-to-light direction
        float theta = dot(fragTolight, lightDirection);

        // Find difference beween cutoff cosines
        float epsilon   =  spotlights[i].innerCutoff - spotlights[i].outerCutoff;

        // Calculate intensity for soft edges
        float intensity = clamp((theta - spotlights[i].outerCutoff) / epsilon, 0.0, 1.0);

         // Find halfway direction between light and eye direction for blinn-phong shading
        vec3 halfwayDir = normalize(lightDirection + viewDirection);

        // Find reflection strength and multiply by material color
        float specularFactor = pow(max(dot(normalized, halfwayDir), 0.f), 100);
        vec4 specular = spotlights[i].specular * (specularFactor * objMat.specular);

        // Decrease intensity if fragment is far fron the cone's center
        diffuse  *= intensity;
        specular *= intensity;

        // Apply attenuation if applicable
        if (spotlights[i].attenuation != vec3(0.f, 0.f, 0.f))
        {
             // Find fragment distance to light
            float dist = length(spotlights[i].position - FragPos);

            // Calculate attenuation from light constant, linear and quadratic components
            float atten = spotlights[i].attenuation.x;

            atten += spotlights[i].attenuation.y * dist;
            atten += spotlights[i].attenuation.z * (dist * dist);
            atten = 1.f / atten;

            // Update color components
            ambient *= atten;
            diffuse *= atten;
            specular *= atten;

        }

        // Update fragment color
       color += vec4(ambient.xyz, objMat.ambient.w) + vec4(diffuse.xyz, objMat.diffuse.w) + vec4(specular.xyz, objMat.specular.w);
    }

    return color;
}
