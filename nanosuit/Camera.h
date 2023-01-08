#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <iostream>
//定义摄像机移动的几个可能选项。用作抽象，以远离特定于窗口系统的输入方法
enum Camera_Movement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};
//默认参数
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

//处理输入并计算相应的欧拉角、向量和矩阵的抽象相机类，供OpenGL使用
class Camera
{
public:
	//摄相机参数
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;

	//欧拉角
	float Yaw;
	float Pitch;

	//功能
	float MovementSpeed;	//键盘控制的移动速度
	float MouseSensitivity;	//鼠标灵敏度
	float Zoom;	//滚轮缩放

	//构造函数与向量
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) 
		: Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}

	//构造函数与标量值
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) 
		: Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = glm::vec3(posX, posY, posZ);
		WorldUp = glm::vec3(upX, upY, upZ);
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}

	// 返回使用欧拉角和LookAt矩阵计算的观察矩阵
	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(Position, Position + Front, Up);
	}

	//处理从任何类似键盘的输入系统接收的输入。接受摄像机定义ENUM形式的输入参数(从窗口系统中抽象它)
	void ProcessKeyboard(Camera_Movement direction, float deltaTime)
	{
		//deltaTime:当前帧和上一帧之间的时间间隔
		//velocity:即速度v
		float velocity = MovementSpeed * deltaTime;
		if (direction == FORWARD)
			Position += Front * velocity;
		if (direction == BACKWARD)
			Position -= Front * velocity;
		if (direction == LEFT)
			Position -= Right * velocity;
		if (direction == RIGHT)
			Position += Right * velocity;
	}

	// 处理从鼠标输入系统接收的输入。期望在x和y方向上的偏移值。
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
	{
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		Yaw += xoffset;
		Pitch += yoffset;

		// 确保当俯仰角超出界限时，屏幕不会翻转
		if (constrainPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}

		//使用更新后的欧拉角更新前、右和上向量
		updateCameraVectors();
	}

	//处理从鼠标滚轮事件接收的输入。只需要在垂直轮轴上输入
	void ProcessMouseScroll(float yoffset)
	{
		Zoom -= (float)yoffset;
		if (Zoom < 1.0f)
			Zoom = 1.0f;
		if (Zoom > 45.0f)
			Zoom = 45.0f;
	}

private:
	// 从相机的(更新的)欧拉角计算前方矢量
	void updateCameraVectors()
	{
		//计算新的Front向量
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);
		// 也重新计算右和向上向量
		Right = glm::normalize(glm::cross(Front, WorldUp));  //将这些向量归一化，因为你越往上或往下看，它们的长度就越接近于0，这就会导致运动变慢。
		Up = glm::normalize(glm::cross(Right, Front));
	}
};

