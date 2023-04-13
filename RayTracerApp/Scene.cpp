#include <glm\glm.hpp>
#include <iostream>

#include "Scene.h"


// TODO: Implement physically based spherical lights and the Oren-Nayar material model.

/*
* OPTIONAL
*/

void Scene::CreateScene()
{
	// Materials
	
	// Pink diffuse
	Lambert* lambert = new Lambert();
	lambert->Albedo = { 1.0f, 0.0f, 1.0f };
	Materials.push_back(lambert);

	// Blue diffuse
	OrenNayar* orenNayar = new OrenNayar();
	orenNayar->Albedo = { 0.0f, 0.0f, 1.0f };
	orenNayar->Roughness = 0.5f;
	Materials.push_back(orenNayar);

	//Lambert* lambert2 = new Lambert();
	//lambert2->Albedo = { 0.0f, 0.0f, 1.0f };
	//Materials.push_back(lambert2);

	// Green diffuse
	Lambert* lambert3 = new Lambert();
	lambert3->Albedo = { 0.0f, 1.0f, 0.0f };
	Materials.push_back(lambert3);

	// Shape 1
	Sphere* sphere1 = new Sphere();
	sphere1->Position = { 0.0f, 0.4f, 0.0f };
	sphere1->Radius = 0.4f;
	sphere1->MaterialIndex = 0;
	Shapes.push_back(sphere1);

	// Shape 2
	Sphere* sphere2 = new Sphere();
	sphere2->Position = { 2.0f, 0.6f, 0.0f };
	sphere2->Radius = 0.6f;
	sphere2->MaterialIndex = 1;
	Shapes.push_back(sphere2);

	// Shape 3 - Floor
	Plane* plane = new Plane();
	plane->Position = { 0.0f, 0.0f, 0.0f };
	plane->Normal = { 0.0f, 1.0f, 0.0f };
	plane->MaterialIndex = 2;
	Shapes.push_back(plane);



	// Basic diffuse Light
	DiffuseAreaLight* diffuseLight = new DiffuseAreaLight();
	diffuseLight->Color = { 1.0f, 1.0f, 1.0f };
	diffuseLight->Intensity = 1.0f;

	Sphere* lightShape = new Sphere();
	lightShape->Position = { -1.0f, 1.0f, 0.0f };
	lightShape->Radius = 1.0f;
	diffuseLight->Shape = lightShape;

	Lights.push_back(diffuseLight);
}
