#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>

//点光源
class PointLight
{
public:
	PointLight(glm::vec3 _positon, glm::vec3 _color = glm::vec3 (1.0f, 1.0f, 1.0f)):
		position(_positon), 
		color(_color)
	{
		//设置衰减参数
		constant = 1.0f;
		linear = 0.14f;
		quadratic = 0.07f;
	}

	glm::vec3 position;
	glm::vec3 color;
	float constant;
	float linear;
	float quadratic;
};