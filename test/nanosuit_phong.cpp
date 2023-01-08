//GLAD��ͷ�ļ���������ȷ��OpenGLͷ�ļ�������GL/gl.h����������Ҫ������������OpenGL��ͷ�ļ�֮ǰ����GLAD��
#include <glad/glad.h>
#include <GLFW/glfw3.h>
//GLM��ѧ��
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "PointLight.h"
#include "Mesh.h"

#include <iostream>

// ���ڻص�����,ÿ�θ��Ĵ��ڴ�С�����øú�����
void framebuffer_size_callback(GLFWwindow*, int, int);
// ������ƺ���
void processInput(GLFWwindow*);
// ���ص�����
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
// ���ֻص�����
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
// ������պ�
unsigned int loadCubemap(vector<std::string> faces);

// ���ڿ��
const unsigned int SCR_WIDTH = 2560;
const unsigned int SCR_HEIGHT = 1440;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// ��ǰ֡����һ֮֡���ʱ����
float lastFrame = 0.0f;

//lighting
glm::vec3 lightPos(0.0f, 0.0f, 2.0f);
glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//��������
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Crysis", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	//���ûص�����
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//���ع�겢��׽
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//��������ƶ�ʱ�䣬�����㸩���Ǻ�ƫ����
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	//��ʼ��GLAD���ڹ���OpenGL����ָ��
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	//��תy��
	//stbi_set_flip_vertically_on_load(true);

	//������Ȳ���
	glEnable(GL_DEPTH_TEST);

	//�����ͱ���Shader
	Shader ourShader("vertexshader.vert", "fragmentshader.frag");
	
	Shader skyboxShader("skyboxvertex.vert", "skyboxfragment.frag");

	//����ģ��
	Model ourModel("nanosuit/nanosuit.obj");

	//��պ�����
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};
	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	vector<std::string> faces
	{
		//ע��˳��
		"texture/right.jpg",
			"texture/left.jpg",
			"texture/top.jpg",
			"texture/bottom.jpg",
			"texture/front.jpg",
			"texture/back.jpg"
	};
	unsigned int cubemapTexture = loadCubemap(faces);

	//��Ⱦѭ��
	while (!glfwWindowShouldClose(window))
	{
		//����������֡����Ⱦʱ���
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// ��ESC�رմ���
		processInput(window);

		// ��Ⱦ
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);	//������Ļ��ɫ
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	//�����Ļ��ɫ�������Ȼ���

		//����uniformsǰ��Ҫ���˿���shader
		ourShader.use();

		//������������գ����Դλ��
		lightPos.x = 1.0f * sin(glfwGetTime() * 2.0f);
		lightPos.y = 1.0f * cos(glfwGetTime() * 2.0f);
		ourShader.use();

		PointLight pointlight(lightPos, lightColor);
		ourShader.setVec3("pointlight.position", pointlight.position);

		//����߹ⷴ�գ��۲�������
		ourShader.setVec3("viewPos", camera.Position);

		//�Ʋ���
		//���ƹ���ǿ��
		ourShader.setVec3("pointlight.ambient", 0.5f, 0.5f, 0.5f);
		ourShader.setVec3("pointlight.diffuse", 0.5f, 0.5f, 0.5f);
		ourShader.setVec3("pointlight.specular", 1.0f, 1.0f, 1.0f);
		//���˥��
		ourShader.setFloat("pointlight.constant", pointlight.constant);
		ourShader.setFloat("pointlight.linear", pointlight.linear);
		ourShader.setFloat("pointlight.quadratic", pointlight.quadratic);
		
		//���ò���
		ourShader.setFloat("material.shininess", 32.0f);

		//�۲����
		glm::mat4 view = camera.GetViewMatrix();
		ourShader.setMat4("view", view);

		//ͶӰ����
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		ourShader.setMat4("projection", projection);

		//ģ�;���
		glm::mat4 model;
		model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		ourShader.setMat4("model", model);

		//����ģ��
		ourModel.Draw(ourShader);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);


		// draw skybox as last
		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		skyboxShader.use();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
		skyboxShader.setMat4("view", view);
		skyboxShader.setMat4("projection", projection);
		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // set depth function back to default

		// ������ɫ���壬ʹͼ����ʾ����Ļ��
		glfwSwapBuffers(window);
		// ��ⴥ���¼������´���״̬����������Ӧ�Ļص�����
		glfwPollEvents();
	}
	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &skyboxVBO);
	//�ͷ���Դ
	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int witdh, int height)
{
	//�����ӿڴ�С
	//ǰ�����������ƴ������½ǵ�λ��
	//�������͵��ĸ�����������Ⱦ���ڵĿ�Ⱥ͸߶ȣ����أ�
	glViewport(0, 0, witdh, height);
}

void processInput(GLFWwindow* window)
{
	//����û��Ƿ���Esc�����˾͹ر�GLFW����
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	//WASD�����ƶ�
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}


// glfw: ÿ������ƶ�������ص������ͱ�����
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // ע���������෴�ģ���Ϊy�����Ǵӵײ����������������

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: ÿ�������ֹ���������ص������ͻᱻ����
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

//������պ�
unsigned int loadCubemap(vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}