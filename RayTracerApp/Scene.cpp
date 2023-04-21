#include <glm/glm.hpp>
#include <iostream>

#include "Scene.h"
#include "Utils.h"


void Scene::CreateScene()
{
	// Materials
	
	// Pink diffuse 0
	PerfectSpecular* specular = new PerfectSpecular();
	specular->fresnel1 = 1.5f;
	specular->fresnel2 = 1.0f;
	Materials.push_back(specular);

	// Green Diffuse 1
	Lambert* lambert2 = new Lambert();
	lambert2->Albedo = { 0.0f, 1.0f, 0.0f };
	lambert2->IsSpecular = false;
	Materials.push_back(lambert2);

	// Red Diffuse 2
	Lambert* lambert3 = new Lambert();
	lambert3->Albedo = { 1.0f, 0.0f, 0.0f };
	lambert3->IsSpecular = false;
	Materials.push_back(lambert3);

	// White Diffuse 3
	Lambert* lambert4 = new Lambert();
	lambert4->Albedo = { 1.0f, 1.0f, 1.0f };
	lambert4->IsSpecular = false;
	Materials.push_back(lambert4);


	// Green Oren Nayar diffuse 4
	OrenNayar* orenNayar = new OrenNayar();
	orenNayar->Albedo = { 0.0f, 1.0f, 0.0f };
	orenNayar->Roughness = 0.9f;
	orenNayar->IsSpecular = false;
	Materials.push_back(orenNayar);

	// Black Diffuse 5
	Lambert* lambert6 = new Lambert();
	lambert6->Albedo = { 0.0f, 0.0f, 0.0f };
	lambert6->IsSpecular = false;
	Materials.push_back(lambert6);


	// Shape 1
	Sphere* sphere1 = new Sphere(glm::vec3{ -1.7f, 0.8f, -1.0f }, glm::vec3(0.0f));
	sphere1->Radius = 0.8f;
	sphere1->MaterialIndex = 4;
	Shapes.push_back(sphere1);

	// Shape 2
	Sphere* sphere2 = new Sphere(glm::vec3{ 0.0f, 0.8f, 0.0f }, glm::vec3(0.0f));
	sphere2->Radius = 0.8f;
	sphere2->MaterialIndex = 1;
	Shapes.push_back(sphere2);

	//// Shape 3
	Sphere* sphere3 = new Sphere(glm::vec3{ 1.7f, 0.8f, 1.0f }, glm::vec3(0.0f));
	sphere3->Radius = 0.8f;
	sphere3->MaterialIndex = 0;
	Shapes.push_back(sphere3);

	// Shape 3 - Floor
	Quad* quad = new Quad(glm::vec3(0.0f), glm::vec3(0.0f));
	quad->width = 5.0f;
	quad->height = 5.0f;
	quad->MaterialIndex = 3;
	Shapes.push_back(quad);

	////// Shape 3 - Back
	Quad* back = new Quad(glm::vec3{ 0.0f, 2.5f, -2.5f }, glm::vec3{ -90.0f, 0.0f, 0.0f });
	back->width = 5.0f;
	back->height = 5.0f;
	back->MaterialIndex = 3;
	Shapes.push_back(back);

	//// Shape 4 - Left
	Quad* left = new Quad(glm::vec3{ 2.5f, 2.5f, 0.0f }, glm::vec3{ 0.0f, 0.0f, -90.0f });
	left->width = 5.0f;
	left->height = 5.0f;
	left->MaterialIndex = 1;
	Shapes.push_back(left);

	//// Shape 5 - Right
	Quad* right = new Quad(glm::vec3{ -2.5f, 2.5f, 0.0f }, glm::vec3{ 0.0f, 0.0f, 90.0f });
	right->width = 5.0f;
	right->height = 5.0f;
	right->MaterialIndex = 2;
	Shapes.push_back(right);

	//// Shape 6 - Ceiling
	Quad* ceiling = new Quad(glm::vec3{ 0.0f, 5.0f, 0.0f }, glm::vec3{ 180.0f, 0.0f, 0.0f });
	ceiling->width = 5.0f;
	ceiling->height = 5.0f;
	ceiling->MaterialIndex = 3;
	Shapes.push_back(ceiling);

	// Basic diffuse Light
	DiffuseAreaLight* diffuseLight = new DiffuseAreaLight();
	diffuseLight->Color = { 1.0f, 1.0f, 1.0f };
	diffuseLight->Intensity = 5.0f;

	// Light Shape sphere
	//Sphere* lightShape = new Sphere(glm::vec3{ 0.0f, 4.5f, 0.0f }, glm::vec3{0.0f,0.0f,0.0f});
	//lightShape->Radius = 0.1f;
	//diffuseLight->Shape = lightShape;
	Quad* lightShape = new Quad(glm::vec3{ 0.0f, 4.99f, 0.0f }, glm::vec3{ 180.0f, 0.0f, 0.0f });
	lightShape->width = 1.0f;
	lightShape->height = 1.0f;
	diffuseLight->Shape = lightShape;

	Lights.push_back(diffuseLight);
}