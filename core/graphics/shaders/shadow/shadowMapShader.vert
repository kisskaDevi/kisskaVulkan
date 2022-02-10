#version 450
#define MAX_NUM_JOINTS 130
#define MAX_LIGHT_SOURCES 10

struct LightBufferObject
{
    mat4 projView;
    vec4 position;
    vec4 lightColor;
    int type;
    int enableShadow;
};

layout(set = 0, binding = 1) uniform LightUniformBufferObject
{
    LightBufferObject ubo[MAX_LIGHT_SOURCES];
} light;

layout (push_constant) uniform Number
{
    int value;
} number;

layout (set = 1, binding = 0) uniform LocalUniformBuffer
{
    mat4 matrix;
} local;

layout (set = 2, binding = 0) uniform UBONode
{
    mat4 matrix;
    mat4 jointMatrix[MAX_NUM_JOINTS];
    float jointCount;
} node;

layout(location = 0)	in  vec3 inPosition;
layout(location = 1)	in  vec4 inJoint0;
layout(location = 2)	in  vec4 inWeight0;

void main()
{
    vec3 outPosition;
    if (node.jointCount > 0.0)
    {
	    // Mesh is skinned
	    mat4 skinMat =
	            inWeight0.x * node.jointMatrix[int(inJoint0.x)] +
	            inWeight0.y * node.jointMatrix[int(inJoint0.y)] +
	            inWeight0.z * node.jointMatrix[int(inJoint0.z)] +
	            inWeight0.w * node.jointMatrix[int(inJoint0.w)];

	    outPosition = vec3(local.matrix*node.matrix * skinMat* vec4(inPosition,1.0));
    } else
    {
	    outPosition = vec3(local.matrix*node.matrix * vec4(inPosition,1.0));
    }

    gl_Position = light.ubo[number.value].projView * vec4(outPosition,1.0f);
}
