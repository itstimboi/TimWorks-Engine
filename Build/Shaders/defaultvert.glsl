#version 110

// Positions / Coordinates
attribute vec3 aPos;

// Normals
attribute vec3 aNormal;

// Colors
attribute vec3 aColor;

// Texture Coordinates
attribute vec2 aTex;


// Outputs to the Fragment Shader
varying vec3 crntPos;
varying vec3 Normal;
varying vec3 color;
varying vec2 texCoord;


// Camera matrix
uniform mat4 camMatrix;

// Model matrix
uniform mat4 model;

uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;


void main()
{
    // Calculate current position
    crntPos =
        vec3(
            model *
            translation *
            rotation *
            scale *
            vec4(aPos, 1.0)
        );

    // Normal
    Normal = aNormal;

    // Color
    color = aColor;

    // Texture coordinates
    texCoord = vec2(
        aTex.x,
        1.0 - aTex.y
    );

    // Final vertex position
    gl_Position =
        camMatrix *
        model *
        vec4(aPos, 1.0);
}