#ifndef MESH_H
#define MESH_H

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
using namespace std;


//定义网格所需要的基本属性  顶点数据  纹理数据
//顶点数据结构体
struct Vertex {
	// 顶点数据
	glm::vec3 Position;
	// 法线数据
	glm::vec3 Normal;
	// 坐标
	glm::vec2 TexCoords;
	// 切线
	glm::vec3 Tangent;
	// 副切线
	glm::vec3 Bitangent;
};
//纹理属性结构体
struct Texture {
	unsigned int id;    //ID
	string type;        //类型/名字
	string path;        //路径 
};

//网格类
class Mesh {
public:
	/*  网格数据  */
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;
	unsigned int VAO;

	/*  函数  */
	// 构造函数 初始化  拷贝构造函数
	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
	{
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;

		// 设置网格
		setupMesh();
	}

	// 绘制网格
	void Draw(Shader shader)
	{
		// 绑定贴图ID
		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;
		unsigned int normalNr = 1;
		unsigned int heightNr = 1;
		//遍历结贴图构体中贴图的数量
		for (unsigned int i = 0; i < textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);    //在绑定纹理之前先激活纹理单元

			string number;  //各种类型贴图的数量   与Shader里面定义贴图名字匹配
			string name = textures[i].type;      //判断贴图结构体数据贴图的名字 是否为 diffuse Speular.. 如果是相应的加上1 为了匹配Shader里面定义贴图名字
			if (name == "texture_diffuse")
				number = std::to_string(diffuseNr++);
			else if (name == "texture_specular")
				number = std::to_string(specularNr++);
			else if (name == "texture_normal")
				number = std::to_string(normalNr++);
			else if (name == "texture_height")
				number = std::to_string(heightNr++);

			// 赋值材质里的贴图uniform
			//为遍历出的每个贴图分配纹理单元
			glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);

			// 绑定贴图采样 
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}

		// 绘制网格
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// always good practice to set everything back to defaults once configured.
		glActiveTexture(GL_TEXTURE0);
	}

private:
	/*  渲染数据  */
	unsigned int VBO, EBO;

	/*  函数    */
	// 初始化各种缓冲
	void setupMesh()
	{
		// 创建 VBO 顶点缓冲对象 VAO顶点数组对象 EBO索引缓冲对象
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		//绑定VAO,VBO与EBO对象
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

		// 复制顶点数据到缓冲内存中
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		// 复制顶点索引到缓冲内存中
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		//链接顶点属性，设置顶点属性指针
	   //顶点位置 0 vec3
	  //属性位置值为0的顶点属性
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		//顶点法线坐标 1 vec3
		//属性位置值为1的顶点属性
		//预处理指令offsetof(s, m)，它的第一个参数是一个结构体，第二个参数是这个结构体中变量的名字。这个宏会返回那个变量距结构体头部的字节偏移量(Byte Offset)
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
		//顶点UV坐标 2 vec2
	   //属性位置值为2的顶点属性
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
		//顶点切线坐标 3 vec3
	   //属性位置值为3的顶点属性
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
		//顶点副切线坐标 4 vec3
	   //属性位置值为4的顶点属性
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

		glBindVertexArray(0);
	}
};
#endif