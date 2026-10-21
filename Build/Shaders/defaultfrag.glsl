#version 110


// Current position from Vertex Shader
varying vec3 crntPos;

// Normal from Vertex Shader
varying vec3 Normal;

// Color from Vertex Shader
varying vec3 color;

// Texture coordinates from Vertex Shader
varying vec2 texCoord;


// Diffuse texture
uniform sampler2D diffuse0;

// Color of the light
uniform vec4 lightColor;

// Camera position
uniform vec3 camPos;

// Whether lighting is enabled
uniform bool usingLights;


// ============================================================
// Point Light
// ============================================================

struct PointLight
{
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;

    float specular;
};


#define NR_POINT_LIGHTS 16

uniform PointLight pointLights[NR_POINT_LIGHTS];


// ============================================================
// Point Light Function
// ============================================================

vec4 pointLight(PointLight light)
{
    vec3 lightVec =
        light.position - crntPos;

    float dist =
        length(lightVec);

    float inten =
        1.0 /
        (
            light.constant +
            light.linear * dist +
            light.quadratic * dist * dist
        );

    vec3 normal =
        normalize(Normal);

    vec3 lightDirection =
        normalize(light.position - crntPos);

    float diffuse =
        max(
            dot(normal, lightDirection),
            0.0
        );

    vec4 texColor =
        texture2D(
            diffuse0,
            texCoord
        );

    vec3 lighting =
        light.ambient +
        light.diffuse *
        diffuse *
        inten;

    return vec4(
        texColor.rgb * lighting,
        texColor.a
    );
}


// ============================================================
// Directional Light
// ============================================================

vec4 direcLight()
{
    // Ambient lighting
    float ambient = 0.20;

    // Diffuse lighting
    vec3 normal =
        normalize(Normal);

    vec3 lightDirection =
        normalize(
            vec3(
                1.0,
                1.0,
                0.0
            )
        );

    float diffuse =
        max(
            dot(normal, lightDirection),
            0.0
        );

    // Specular lighting
    float specularLight = 0.50;

    vec3 viewDirection =
        normalize(
            camPos - crntPos
        );

    vec3 reflectionDirection =
        reflect(
            -lightDirection,
            normal
        );

    float specAmount =
        pow(
            max(
                dot(
                    viewDirection,
                    reflectionDirection
                ),
                0.0
            ),
            16.0
        );

    float specular =
        specAmount *
        specularLight;

    return
        (
            texture2D(
                diffuse0,
                texCoord
            )
            *
            (diffuse + ambient)
        )
        *
        lightColor;
}


// ============================================================
// Spotlight
// ============================================================

struct SpotLight
{
    vec3 position;
    vec3 direction;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;

    float innerCone;
    float outerCone;
};


#define NR_SPOT_LIGHTS 16

uniform SpotLight spotLights[NR_SPOT_LIGHTS];


// ============================================================
// Spotlight Function
// ============================================================

vec4 spotLight(SpotLight light)
{
    vec3 lightVec =
        light.position - crntPos;

    float dist =
        length(lightVec);

    float inten =
        1.0 /
        (
            light.constant +
            light.linear * dist +
            light.quadratic * dist * dist
        );

    vec3 normal =
        normalize(Normal);

    vec3 lightDirection =
        normalize(
            light.position - crntPos
        );

    float diffuse =
        max(
            dot(
                normal,
                lightDirection
            ),
            0.0
        );


    // --------------------------------------------------------
    // Spotlight cone
    // --------------------------------------------------------

    float angle =
        dot(
            normalize(-light.direction),
            normalize(-lightVec)
        );

    float coneIntensity =
        clamp(
            (
                angle -
                light.outerCone
            )
            /
            (
                light.innerCone -
                light.outerCone
            ),
            0.0,
            1.0
        );


    // --------------------------------------------------------
    // Texture
    // --------------------------------------------------------

    vec4 texColor =
        texture2D(
            diffuse0,
            texCoord
        );


    // --------------------------------------------------------
    // Lighting
    // --------------------------------------------------------

    vec3 lighting =
        light.diffuse *
        diffuse *
        inten *
        coneIntensity;


    return vec4(
        texColor.rgb * lighting,
        texColor.a
    );
}


// ============================================================
// Main
// ============================================================

void main()
{
    vec4 texColor =
        texture2D(
            diffuse0,
            texCoord
        );


    // No lighting
    if (!usingLights)
    {
        gl_FragColor =
            texColor;

        return;
    }


    // Base ambient lighting
    vec3 result =
        texColor.rgb *
        0.20;


    // --------------------------------------------------------
    // Point lights
    // --------------------------------------------------------

    for (int i = 0; i < NR_POINT_LIGHTS; i++)
    {
        result +=
            pointLight(
                pointLights[i]
            ).rgb;
    }


    // --------------------------------------------------------
    // Spot lights
    // --------------------------------------------------------

    for (int i = 0; i < NR_SPOT_LIGHTS; i++)
    {
        result +=
            spotLight(
                spotLights[i]
            ).rgb;
    }


    // Final color
    gl_FragColor =
        vec4(
            result,
            texColor.a
        )
        *
        lightColor;
}