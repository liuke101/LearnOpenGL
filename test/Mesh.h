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


//������������Ҫ�Ļ�������  ��������  ��������
//�������ݽṹ��
struct Vertex {
	// ��������
	glm::vec3 Position;
	// ��������
	glm::vec3 Normal;
	// ����
	glm::vec2 TexCoords;
	// ����
	glm::vec3 Tangent;
	// ������
	glm::vec3 Bitangent;
};
//�������Խṹ��
struct Texture {
	unsigned int id;    //ID
	string type;        //����/����
	string path;        //·�� 
};

//������
class Mesh {
public:
	/*  ��������  */
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;
	unsigned int VAO;

	/*  ����  */
	// ���캯�� ��ʼ��  �������캯��
	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
	{
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;

		// ��������
		setupMesh();
	}

	// ��������
	void Draw(Shader shader)
	{
		// ����ͼID
		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;
		unsigned int normalNr = 1;
		unsigned int heightNr = 1;
		//��������ͼ��������ͼ������
		for (unsigned int i = 0; i < textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);    //�ڰ�����֮ǰ�ȼ�������Ԫ

			string number;  //����������ͼ������   ��Shader���涨����ͼ����ƥ��
			string name = textures[i].type;      //�ж���ͼ�ṹ��������ͼ������ �Ƿ�Ϊ diffuse Speular.. �������Ӧ�ļ���1 Ϊ��ƥ��Shader���涨����ͼ����
			if (name == "texture_diffuse")
				number = std::to_string(diffuseNr++);
			else if (name == "texture_specular")
				number = std::to_string(specularNr++);
			else if (name == "texture_normal")
				number = std::to_string(normalNr++);
			else if (name == "texture_height")
				number = std::to_string(heightNr++);

			// ��ֵ���������ͼuniform
			//Ϊ��������ÿ����ͼ��������Ԫ
			glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);

			// ����ͼ���� 
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}

		// ��������
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// always good practice to set everything back to defaults once configured.
		glActiveTexture(GL_TEXTURE0);
	}

private:
	/*  ��Ⱦ����  */
	unsigned int VBO, EBO;

	/*  ����    */
	// ��ʼ�����ֻ���
	void setupMesh()
	{
		// ���� VBO ���㻺����� VAO����������� EBO�����������
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		//��VAO,VBO��EBO����
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

		// ���ƶ������ݵ������ڴ���
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		// ���ƶ��������������ڴ���
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		//���Ӷ������ԣ����ö�������ָ��
	   //����λ�� 0 vec3
	  //����λ��ֵΪ0�Ķ�������
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		//���㷨������ 1 vec3
		//����λ��ֵΪ1�Ķ�������
		//Ԥ����ָ��offsetof(s, m)�����ĵ�һ��������һ���ṹ�壬�ڶ�������������ṹ���б��������֡������᷵���Ǹ�������ṹ��ͷ�����ֽ�ƫ����(Byte Offset)
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
		//����UV���� 2 vec2
	   //����λ��ֵΪ2�Ķ�������
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
		//������������ 3 vec3
	   //����λ��ֵΪ3�Ķ�������
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
		//���㸱�������� 4 vec3
	   //����λ��ֵΪ4�Ķ�������
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

		glBindVertexArray(0);
	}
};
#endif