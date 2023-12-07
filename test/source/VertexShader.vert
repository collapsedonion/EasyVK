#version 450



vec2 position[]={
    {0.0, -0.5},
    {0.5, 0.5},
    {-0.5, 0.5}
};

vec3 color[]={
    {1.0,0.0,0.0},
    {0.0,1.0,0.0},
    {0.0,0.0,1.0}
};

layout(location = 0) out vec3 fragColor;


void main(){
    gl_Position = vec4(position[gl_VertexIndex], 0.0, 1.0);        
    fragColor = color[gl_VertexIndex];
}