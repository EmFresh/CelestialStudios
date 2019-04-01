#version 420
#define MAX_LIGHTS_SIZE 50

//
const int DEFAULT=0;
const int POINT=1;
const int DIRECTIONAL=2;
const int SPOTLIGHT=3;


uniform vec4 LightPosition[MAX_LIGHTS_SIZE];

//color
uniform vec3 LightAmbient;
uniform vec3 LightDiffuse[MAX_LIGHTS_SIZE];
uniform vec3 LightSpecular[MAX_LIGHTS_SIZE];

//scalar
uniform float LightSpecularExponent[MAX_LIGHTS_SIZE];
uniform float Attenuation_Constant[MAX_LIGHTS_SIZE];
uniform float Attenuation_Linear[MAX_LIGHTS_SIZE];
uniform float Attenuation_Quadratic[MAX_LIGHTS_SIZE];

//amount of lights
uniform int LightAmount;

uniform int LightType[MAX_LIGHTS_SIZE];
uniform vec3 LightDirection[MAX_LIGHTS_SIZE] ;
uniform float LightAngleConstraint[MAX_LIGHTS_SIZE];

uniform sampler2D uPos;
uniform sampler2D uNorm;
uniform sampler2D uAlbedo;
//uniform vec4 colourMod;
//uniform bool textured;

in vec2 texcoord;
//in vec3 norm;
//in vec3 pos;

layout(location = 0) out vec4 outColor;

void pointLight(int a);

void defaultLight(int a)
{
    pointLight(a);
}

void spotLight(int a)
{

}

void directionalLight(int a)
{
    vec3 normal = normalize(texture(uNorm,texcoord).xyz);
    vec3 lightVec = LightPosition[a].xyz - texture(uPos,texcoord).xyz;
    float dist = length(lightVec);
    float NdotL = max(dot(normal, LightDirection[a]),0.0);

    //float angle = acos(dot(lightDir , LightDirection) / length(lightDir * LightDirection));
    if(NdotL > 0.0)
    {
        //The light contributes to this surface
        //Calculate attenuation (falloff)
        float attenuation = 1.0 / (Attenuation_Constant[a] + (Attenuation_Linear[a] * dist) + (Attenuation_Quadratic[a] * dist * dist));
       
        //Calculate diffuse contribution
        outColor.rgb += LightDiffuse[a] * NdotL * attenuation;
        
        //Blinn-Phong half vector
        float NdotHV =  max(dot(normal, normalize(LightDirection[a] + lightVec)), 0.0); 
        
        //Calculate specular contribution
        outColor.rgb += LightSpecular[a] * pow(NdotHV, LightSpecularExponent[a]) * attenuation;
      //  outColor = vec4(LightDiffuse[a]* NdotL * attenuation,1.0) ;
    }
}

void pointLight(int a)
{
    vec3 normal = normalize(texture(uNorm,texcoord).xyz);
    vec3 lightVec = LightPosition[a].xyz - texture(uPos,texcoord).xyz;
    float dist = length(lightVec);
   
   
    //The light contributes to this surface
    //Calculate attenuation (falloff)
    float attenuation = 1.0 / (Attenuation_Constant[a] + (Attenuation_Linear[a] * dist) + (Attenuation_Quadratic[a] * dist * dist));
      
    //Calculate diffuse contribution
    outColor.rgb += LightDiffuse[a] * attenuation ;
    
    //Blinn-Phong half vector
    float NdotHV =  max(dot(normal, normalize(lightVec + texture(uPos,texcoord).xyz)), 0.0); 
    
    //Calculate specular contribution
    outColor.rgb += LightSpecular[a] * pow(NdotHV, LightSpecularExponent[a]) * attenuation;

    //outColor = vec4(LightDiffuse[a] * attenuation,1.0) ;
}

void main()
{
    
    //outColor = vec4(normal, 1.0f);
    //outColor = vec4(0.5f, 1.0f, 0.5f, 1.0f);
    
    //if(textured)
    //{       
    //   vec4 textureColor = texture(uTex, texcoord);
    //   outColor = textureColor;
    //   outColor.rgb *= textureColor.rgb;
    //   outColor *= colourMod;
    //}
    //else     
    //{
    //    outColor = colourMod; 
    //}

    vec4 textureColor = texture(uAlbedo,texcoord);
    outColor = textureColor;
    outColor.rgb *= LightAmbient;
    
    //account for rasterizer interpolating
    vec3 normal = normalize(texture(uNorm,texcoord).xyz);
    
    for(int a = 0; a < LightAmount; a++)
    { 
        switch(LightType[a])
        {        
        case POINT:
            pointLight(a);
            break;
        case DIRECTIONAL:
            directionalLight(a);
            break;
        case SPOTLIGHT:
            spotLight(a);
            break;
        default:
            defaultLight(a);
        }
     // outColor.rgb = vec3(NdotL,NdotL,NdotL); 
    }    
}