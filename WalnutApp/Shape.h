#pragma once

class Primitive {
public:
	virtual glm::mat4 ObjectToWorld() = default;
	virtual glm::mat4 WorldToObject() = default;
	virtual double Intersect(glm::vec3 rayOrigin, glm::vec3 rayDirection) = default;
	virtual double Sample(glm::vec3 rayOrigin, glm::vec3 rayDirection) = default;
};

class Shape : Primitive {
public:

};