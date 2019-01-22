#version 420

uniform sampler2D uTex;

in vec2 texcoord;

out vec4 outColor;

void main()
{
    
    vec4 source = texture(uTex, texcoord); 

	float lumiance = 0.2989 * source.r + 0.587 * source.g + 0.114 * source.b;

	outColor.rgb = vec3(lumiance, lumiance, lumiance);
	outColor.a = 1.0f
}