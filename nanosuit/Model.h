#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h> 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define STB_IMAGE_IMPLEMENTATION    //ͨ������STB_IMAGE_IMPLEMENTATION��Ԥ���������޸�ͷ�ļ�������ֻ������صĺ�������Դ�룬�����ǽ����ͷ�ļ���Ϊһ�� .cpp �ļ���
#include "stb/stb_image.h"             //ͼƬ����
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"            //����������
#include "Shader.h"          //��������Ⱦ��ɫ��

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;

unsigned int TextureFromFile(const char* path, const string& directory, bool gamma = false);

class Model
{
public:
	/*  ģ������  */
	vector<Texture> textures_loaded;      //��ͼ��������  �����м��ع�����������һ��vector��
	vector<Mesh> meshes;                 //�����������
	string directory;                    //Ŀ¼  
	bool gammaCorrection;

	/*  ����   */
	// ������ ����ģ�͵�·��
	Model(string const& path, bool gamma = false) : gammaCorrection(gamma)
	{
		//����ģ�͵�·��
		loadModel(path);
	}

	// ����ģ����Ķ������ 
	void Draw(Shader shader)
	{
		for (unsigned int i = 0; i < meshes.size(); i++)
			meshes[i].Draw(shader);
	}

private:
	/*  ����  */
	// �ӹ�������ֱ�ӵ���loadModel��������ģ��·������Assimp������ģ����Assimp��һ������scene�����ݽṹ��
	//����Assimp���ݽӿڵĸ�����һ������������������������Ǿ��ܷ��ʵ����غ��ģ������������������ˡ�
	void loadModel(string const& path)
	{
		// ��ȡģ��·��  ��ȡ������Assimp�����ռ���Importer
		Assimp::Importer importer;
		//��һ������һ���ļ�·��
		//�ڶ���������һЩ���ڴ���(Post-processing)��ѡ��
		//���˼����ļ�֮�⣬Assimp���������趨һЩѡ����ǿ�����Ե����������һЩ����ļ���������
		//ͨ���趨aiProcess_Triangulate������Assimp�����ģ�Ͳ��ǣ�ȫ��������������ɣ�����Ҫ��ģ�����е�ͼԪ��״�任Ϊ�����Ρ�
		//aiProcess_FlipUVs���ڴ����ʱ��תy����������꣨��OpenGL�д󲿷ֵ�ͼ���y�ᶼ�Ƿ��ģ�����������ڴ���ѡ����޸������
		//aiProcess_GenNormals�����ģ�Ͳ������������Ļ�����Ϊÿ�����㴴�����ߡ�
		//aiProcess_SplitLargeMeshes�����Ƚϴ������ָ�ɸ�С����������������Ⱦ����󶥵������ƣ�ֻ����Ⱦ��С��������ô����ǳ����á�
		//aiProcess_OptimizeMeshes�����ϸ�ѡ���෴�����Ὣ���С����ƴ��Ϊһ��������񣬼��ٻ��Ƶ��ôӶ������Ż���
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
		// ��鳡��������ڵ㲻Ϊnull�����Ҽ��������һ�����(Flag)�����鿴���ص������ǲ��ǲ�������
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			//ͨ����������GetErrorString������������󲢷���
			cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
			return;
		}
		// ��ȡ�ļ�·����Ŀ¼·��
		directory = path.substr(0, path.find_last_of('/'));

		// ����һ���ڵ㣨���ڵ㣩�����˵ݹ��processNode����
		//��Ϊÿ���ڵ㣨���ܣ������ж���ӽڵ㣬���ȴ�������еĽڵ㣬�ټ�������ýڵ����е��ӽڵ㣬�Դ����ơ�
		processNode(scene->mRootNode, scene);
	}

	// �ݹ麯��
	//�����������нڵ�����������
	//��Assimp�Ľṹ�У�ÿ���ڵ������һϵ�е�����������ÿ������ָ�򳡾������е��Ǹ��ض�����
	//���ǽ���������ȥ��ȡ��Щ������������ȡÿ�����񣬴���ÿ�����񣬽��Ŷ�ÿ���ڵ���ӽڵ��ظ���һ����
	//��һ���ڵ㲻�����κ��ӽڵ�֮�������������ִֹͣ�С�
	void processNode(aiNode* node, const aiScene* scene)
	{
		// ����ڵ����е���������еĻ���
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			//���ÿ���ڵ������������������������mMeshes��������ȡ��Ӧ������
			//���ص����񽫻ᴫ�ݵ�processMesh�����У����᷵��һ��Mesh�������ǿ��Խ����洢��meshes�б� / vector
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(mesh, scene));
		}
		// �������������ӽڵ��ظ���һ����
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}

	}
	//��Assimp�����ݽ�����Mesh��
	Mesh processMesh(aiMesh* mesh, const aiScene* scene)
	{
		// ����Ҫ��������
		vector<Vertex> vertices;
		vector<unsigned int> indices;
		vector<Texture> textures;

		// ��ȡ���еĶ�������
		//���������е����ж��㣨ʹ��mesh->mNumVertices����ȡ��
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			//���涥������
			glm::vec3 vector;
			// λ��
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.Position = vector;
			// ����
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;
			// UV
			//Assimp����һ��ģ����һ�������������8����ͬ���������꣬���ﲻ���õ���ô�ֻ࣬���ĵ�һ����������
			if (mesh->mTextureCoords[0]) // �������е�Mesh ����UV���� �����ж�
			{
				glm::vec2 vec;
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.TexCoords = vec;
			}
			else
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);
			// ����
			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.Tangent = vector;
			// ������
			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.Bitangent = vector;
			vertices.push_back(vertex);
		}
		//��������
		//Assimp�Ľӿڶ�����ÿ��������һ����(Face)���飬ÿ���������һ��ͼԪ
		//�����ǵ������У�����ʹ����aiProcess_Triangulateѡ������������Ρ�
		//һ��������˶�����������Ƕ�������ÿ��ͼԪ�У�����Ӧ�û����ĸ����㣬����ʲô˳����ơ�
		//����������Ǳ��������е��棬�����������������indices���vector�оͿ����ˡ�
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];

			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}
		// �������
		//һ������ֻ������һ��ָ����ʶ���������������Ҫ��ȡ���������Ĳ��ʣ����ǻ���Ҫ����������mMaterials����
		//�����������λ������mMaterialIndex�����У�
		//���һ�������Ƿ�����в���
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

			//һ�����ʶ�����ڲ���ÿ���������Ͷ��洢��һ������λ������
			//��ͬ���������Ͷ���aiTextureType_Ϊǰ׺��
			//ʹ��loadMaterialTextures�Ĺ��ߺ������Ӳ����л�ȡ��������������᷵��һ��Texture�ṹ���vector
			//���ǽ���ģ�͵�textures vector��β��֮��洢����

			// 1. diffuse maps
			vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			// 2. specular maps
			vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
			// 3. normal maps
			std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
			textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
			// 4. height maps
			std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
			textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
		}
		// ����Mesh����������
		return Mesh(vertices, indices, textures);
	}

	//loadMaterialTextures���������˸����������͵���������λ�ã���ȡ��������ļ�λ�ã������ز�������������Ϣ��������һ��textures�ṹ����
	vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
	{
		vector<Texture> textures;
		//GetTextureCount������鴢���ڲ��������������
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			//GetTexture��ȡÿ��������ļ�λ�ã����Ὣ���������һ��aiString��
			mat->GetTexture(type, i, &str);

			// �������û�б����ع�
			bool skip = false;
			for (unsigned int j = 0; j < textures_loaded.size(); j++)
			{
				//�������·���봢����textures_loaded���vector�е�����������бȽϣ�������ǰ�����·���Ƿ������е�һ����ͬ��
				//����ǵĻ����������������/���ɵĲ��֣�ֱ��ʹ�ö�λ��������ṹ��Ϊ���������
				if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
				{
					textures.push_back(textures_loaded[j]);
					skip = true;
					break;
				}
			}
			if (!skip)
			{   // �������û�б����أ��������
				Texture texture;
				//TextureFromFile�Ĺ��ߺ����������ᣨ��stb_image.h������һ���������ظ������ID
				texture.id = TextureFromFile(str.C_Str(), this->directory);
				texture.type = typeName;
				texture.path = str.C_Str();
				textures.push_back(texture);
				// ��ӵ��Ѽ��ص�������
				textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
			}
		}
		return textures;
	}
};

//������ͼ����
unsigned int TextureFromFile(const char* path, const string& directory, bool gamma)
{
	string filename = string(path);
	//������ģ����ͬ·���µ���ͼ
	filename = directory + '/' + filename;
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}
	return textureID;
}
#endif