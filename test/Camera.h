#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <iostream>
//����������ƶ��ļ�������ѡ�����������Զ���ض��ڴ���ϵͳ�����뷽��
enum Camera_Movement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};
//Ĭ�ϲ���
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

//�������벢������Ӧ��ŷ���ǡ������;���ĳ�������࣬��OpenGLʹ��
class Camera
{
public:
	//���������
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;

	//ŷ����
	float Yaw;
	float Pitch;

	//����
	float MovementSpeed;	//���̿��Ƶ��ƶ��ٶ�
	float MouseSensitivity;	//���������
	float Zoom;	//��������

	//���캯��������
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) 
		: Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}

	//���캯�������ֵ
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) 
		: Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = glm::vec3(posX, posY, posZ);
		WorldUp = glm::vec3(upX, upY, upZ);
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}

	// ����ʹ��ŷ���Ǻ�LookAt�������Ĺ۲����
	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(Position, Position + Front, Up);
	}

	//������κ����Ƽ��̵�����ϵͳ���յ����롣�������������ENUM��ʽ���������(�Ӵ���ϵͳ�г�����)
	void ProcessKeyboard(Camera_Movement direction, float deltaTime)
	{
		//deltaTime:��ǰ֡����һ֮֡���ʱ����
		//velocity:���ٶ�v
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

	// ������������ϵͳ���յ����롣������x��y�����ϵ�ƫ��ֵ��
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
	{
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		Yaw += xoffset;
		Pitch += yoffset;

		// ȷ���������ǳ�������ʱ����Ļ���ᷭת
		if (constrainPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}

		//ʹ�ø��º��ŷ���Ǹ���ǰ���Һ�������
		updateCameraVectors();
	}

	//������������¼����յ����롣ֻ��Ҫ�ڴ�ֱ����������
	void ProcessMouseScroll(float yoffset)
	{
		Zoom -= (float)yoffset;
		if (Zoom < 1.0f)
			Zoom = 1.0f;
		if (Zoom > 45.0f)
			Zoom = 45.0f;
	}

private:
	// �������(���µ�)ŷ���Ǽ���ǰ��ʸ��
	void updateCameraVectors()
	{
		//�����µ�Front����
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);
		// Ҳ���¼����Һ���������
		Right = glm::normalize(glm::cross(Front, WorldUp));  //����Щ������һ������Ϊ��Խ���ϻ����¿������ǵĳ��Ⱦ�Խ�ӽ���0����ͻᵼ���˶�������
		Up = glm::normalize(glm::cross(Right, Front));
	}
};

