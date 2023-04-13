#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>

#include "Geometry/Primitives.h"
#include "PBR/Lights.h"
#include "PBR/Materials.h"

// Scene
class Scene
{
public:
	std::vector<Light*> Lights;
	std::vector<Shape*> Shapes;
	std::vector<Material*> Materials;

public:
	void CreateScene();
};
