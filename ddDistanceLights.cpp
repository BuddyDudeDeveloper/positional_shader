/*
Based on the shader code given in the Arnold doc "Creating a Shader".
	https://docs.arnoldrenderer.com/display/AFMUG/Creating+a+shader#CreatingaShader-1.1.1InstallingaC++Compiler 
Malcolm Kesson
Aug 17 2019
*/
#include <ai.h>
#include <cstring>

AI_SHADER_NODE_EXPORT_METHODS(SampleMethods);

AtRGB mix(AtRGB first_color, AtRGB second_color, float alpha) {
	return first_color * (1.0f - alpha) + (second_color * alpha);
}

namespace {
	enum paramIndex {k_light_intensity, k_primary_red_channel, k_primary_green_channel, k_primary_blue_channel, k_secondary_red_channel, k_secondary_green_channel, k_secondary_blue_channel, k_swap_colors, k_height, k_blur};
	};

static const char* mode_labels[] = {"x", "y", "z", NULL};
node_parameters {
    AiParameterFlt("lightIntensity", 20.0f);
	AiParameterEnum("primaryRedChannel", 0, mode_labels);
	AiParameterEnum("primaryGreenChannel", 1, mode_labels);
	AiParameterEnum("primaryBlueChannel", 2, mode_labels);
	AiParameterEnum("secondaryRedChannel", 0, mode_labels);
	AiParameterEnum("secondaryGreenChannel", 1, mode_labels);
	AiParameterEnum("secondaryBlueChannel", 2, mode_labels);
	AiParameterBool("swapColors", 0);
	AiParameterFlt("height", 0.5f);
	AiParameterFlt("blur", 0.5f);
}
 
shader_evaluate {

	AtRGB primary_color;
	AtRGB secondary_color;
	
	int primary_red_channel = AiShaderEvalParamEnum(k_primary_red_channel);
	int primary_green_channel = AiShaderEvalParamEnum(k_primary_green_channel);
	int primary_blue_channel = AiShaderEvalParamEnum(k_primary_blue_channel);
	
	int secondary_red_channel = AiShaderEvalParamEnum(k_secondary_red_channel);
	int secondary_green_channel = AiShaderEvalParamEnum(k_secondary_green_channel);
	int secondary_blue_channel = AiShaderEvalParamEnum(k_secondary_blue_channel);
	
	bool swap_colors = AiShaderEvalParamBool(k_swap_colors);
	
	float blur = AiShaderEvalParamFlt(k_blur);
	float height = AiShaderEvalParamFlt(k_height);

	const int X = 0;
	const int Y = 1;
	const int Z = 2;
	
	float world_x = fmod(sg->P.x, 1);
	float world_y = fmod(sg->P.y, 1);
	float world_z = fmod(sg->P.z, 1);
	
	switch(primary_red_channel) {
		case X:
			primary_color.r = world_x;
			break;
		case Y:
			primary_color.r = world_y;
			break;
		case Z:
			primary_color.r = world_z;
			break;
	}
	
	switch(primary_green_channel) {
		case X:
			primary_color.g = world_x;
			break;
		case Y:
			primary_color.g = world_y;
			break;
		case Z:
			primary_color.g = world_z;
			break;
	}
	
	switch(primary_blue_channel) {
		case X:
			primary_color.b = world_x;
			break;
		case Y:
			primary_color.b = world_y;
			break;
		case Z:
			primary_color.b = world_z;
			break;
	}
	
	if(primary_color.r < 0) primary_color.r *= -1;
	if(primary_color.g < 0) primary_color.g *= -1;
	if(primary_color.b < 0) primary_color.b *= -1;
	
	switch(secondary_red_channel) {
		case X:
			secondary_color.r = world_x;
			break;
		case Y:
			secondary_color.r = world_y;
			break;
		case Z:
			secondary_color.r = world_z;
			break;
	}
	
	switch(secondary_green_channel) {
		case X:
			secondary_color.g = world_x;
			break;
		case Y:
			secondary_color.g = world_y;
			break;
		case Z:
			secondary_color.g = world_z;
			break;
	}
	
	switch(secondary_blue_channel) {
		case X:
			secondary_color.b = world_x;
			break;
		case Y:
			secondary_color.b = world_y;
			break;
		case Z:
			secondary_color.b = world_z;
			break;
	}
	
	if(secondary_color.r < 0) secondary_color.r *= -1;
	if(secondary_color.g < 0) secondary_color.g *= -1;
	if(secondary_color.b < 0) secondary_color.b *= -1;
	
	float half_blur = blur / 2;
	float minimum_height = height - half_blur;
	float maximum_height = height + half_blur;
	float blend = AiSmoothStep(minimum_height, maximum_height, sg->Po.y);
	
	if(sg->N == sg->Nf) {
		if(swap_colors) {
			sg->out.RGB() = mix(secondary_color, primary_color, blend);
		} else {
			sg->out.RGB() = mix(primary_color, secondary_color, blend);
		}
	} else {
		if(swap_colors) {
			sg->out.RGB() = mix(primary_color, secondary_color, blend);
		} else {
			sg->out.RGB() = mix(secondary_color, primary_color, blend);
		}
	}
	
	
}
 
node_loader {
    if (i > 0)
        return false; 
    node->methods        = SampleMethods;
    node->output_type    = AI_TYPE_RGB;
	node->name           = "ddDistanceLights";
    node->node_type      = AI_NODE_SHADER;
    strcpy(node->version, AI_VERSION);
    return true;
	}
	
// The remaining macros can be left "empty"
node_initialize { }
node_update { }
node_finish { }
