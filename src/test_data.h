#ifndef TEST_DATA_H
#define TEST_DATA_H

struct TriangleDemoVertex {
	float x, y, z;
	float r, g, b;
};

static const char VERTEX_SHADER_FILENAME[] = "shaders/vertex.spirv";
static const char FRAGMENT_SHADER_FILENAME[] = "shaders/fragment.spirv";

static constexpr int VERTEX_INPUT_BINDING = 0;

static constexpr int NUM_DEMO_VERTICES = 3;
static const TriangleDemoVertex vertices[NUM_DEMO_VERTICES] =
{
	//      position             color
	{  0.5f,  0.5f,  0.0f,  0.1f, 0.8f, 0.1f },
	{ -0.5f,  0.5f,  0.0f,  0.8f, 0.1f, 0.1f },
	{  0.0f, -0.5f,  0.0f,  0.1f, 0.1f, 0.8f },
};

#endif // TEST_DATA_H