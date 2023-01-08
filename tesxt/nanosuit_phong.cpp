//GLAD的头文件包含了正确的OpenGL头文件（例如GL/gl.h），所以需要在其它依赖于OpenGL的头文件之前包含GLAD。
#include <glad/glad.h>
#include <GLFW/glfw3.h>
//GLM数学库
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "PointLight.h"
#include "Mesh.h"

#include <iostream>

// 窗口回调函数,每次更改窗口大小，调用该函数。
void framebuffer_size_callback(GLFWwindow*, int, int);
// 输入控制函数
void processInput(GLFWwindow*);
// 鼠标回调函数
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
// 滚轮回调函数
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
// 加载天空盒
unsigned int loadCubemap(vector<std::string> faces);

// 窗口宽高
const unsigned int SCR_WIDTH = 2560;
const unsigned int SCR_HEIGHT = 1440;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// 当前帧和上一帧之间的时间间隔
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

	//创建窗口
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Crysis", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	//启用回调函数
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//隐藏光标并捕捉
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//监听鼠标移动时间，来计算俯仰角和偏航角
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	//初始化GLAD用于管理OpenGL函数指针
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	//翻转y轴
	//stbi_set_flip_vertically_on_load(true);

	//启用深度测试
	glEnable(GL_DEPTH_TEST);

	//创建和编译Shader
	Shader ourShader("vertexshader.vert", "fragmentshader.frag");
	
	Shader skyboxShader("skyboxvertex.vert", "skyboxfragment.frag");

	//加载模型
	Model ourModel("nanosuit/nanosuit.obj");

	//天空盒数据
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
		//注意顺序
		"texture/right.jpg",
			"texture/left.jpg",
			"texture/top.jpg",
			"texture/bottom.jpg",
			"texture/front.jpg",
			"texture/back.jpg"
	};
	unsigned int cubemapTexture = loadCubemap(faces);

	//渲染循环
	while (!glfwWindowShouldClose(window))
	{
		//计算上下两帧的渲染时间差
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// 按ESC关闭窗口
		processInput(window);

		// 渲染
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);	//设置屏幕颜色
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	//清除屏幕颜色缓冲和深度缓冲

		//设置uniforms前不要忘了开启shader
		ourShader.use();

		//计算漫反射光照：点光源位置
		lightPos.x = 1.0f * sin(glfwGetTime() * 2.0f);
		lightPos.y = 1.0f * cos(glfwGetTime() * 2.0f);
		ourShader.use();

		PointLight pointlight(lightPos, lightColor);
		ourShader.setVec3("pointlight.position", pointlight.position);

		//计算高光反照：观察者坐标
		ourShader.setVec3("viewPos", camera.Position);

		//灯参数
		//控制光照强度
		ourShader.setVec3("pointlight.ambient", 0.5f, 0.5f, 0.5f);
		ourShader.setVec3("pointlight.diffuse", 0.5f, 0.5f, 0.5f);
		ourShader.setVec3("pointlight.specular", 1.0f, 1.0f, 1.0f);
		//光的衰减
		ourShader.setFloat("pointlight.constant", pointlight.constant);
		ourShader.setFloat("pointlight.linear", pointlight.linear);
		ourShader.setFloat("pointlight.quadratic", pointlight.quadratic);
		
		//设置材质
		ourShader.setFloat("material.shininess", 32.0f);

		//观察矩阵
		glm::mat4 view = camera.GetViewMatrix();
		ourShader.setMat4("view", view);

		//投影矩阵
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		ourShader.setMat4("projection", projection);

		//模型矩阵
		glm::mat4 model;
		model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		ourShader.setMat4("model", model);

		//绘制模型
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

		// 交换颜色缓冲，使图像显示在屏幕上
		glfwSwapBuffers(window);
		// 检测触发事件，更新窗口状态，并调用相应的回调函数
		glfwPollEvents();
	}
	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &skyboxVBO);
	//释放资源
	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int witdh, int height)
{
	//设置视口大小
	//前两个参数控制窗口左下角的位置
	//第三个和第四个参数控制渲染窗口的宽度和高度（像素）
	glViewport(0, 0, witdh, height);
}

void processInput(GLFWwindow* window)
{
	//检查用户是否按了Esc，按了就关闭GLFW窗口
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	//WASD控制移动
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}


// glfw: 每当鼠标移动，这个回调函数就被调用
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
	float yoffset = lastY - ypos; // 注意这里是相反的，因为y坐标是从底部往顶部依次增大的

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: 每当鼠标滚轮滚动，这个回调函数就会被调用
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

//加载天空盒
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