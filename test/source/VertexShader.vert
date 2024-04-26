#version 450


layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec3 color;

layout(location = 0) out vec3 fragColor;

layout(binding = 0) buffer InBuf{
    mat4x4 rot;
};

void main(){
    gl_Position = rot * vec4(vertexPos.x, -vertexPos.y, vertexPos.z, 1.0);
    fragColor = color;
}