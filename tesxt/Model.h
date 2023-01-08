#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h> 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define STB_IMAGE_IMPLEMENTATION    //通过定义STB_IMAGE_IMPLEMENTATION，预处理器会修改头文件，让其只包含相关的函数定义源码，等于是将这个头文件变为一个 .cpp 文件了
#include "stb/stb_image.h"             //图片处理
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"            //基础的网格
#include "Shader.h"          //基础的渲染着色器

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
	/*  模型数据  */
	vector<Texture> textures_loaded;      //贴图对象数组  将所有加载过的纹理储存在一个vector中
	vector<Mesh> meshes;                 //网格对象数组
	string directory;                    //目录  
	bool gammaCorrection;

	/*  函数   */
	// 构造器 参数模型的路径
	Model(string const& path, bool gamma = false) : gammaCorrection(gamma)
	{
		//导入模型的路径
		loadModel(path);
	}

	// 绘制模型里的多个网格 
	void Draw(Shader shader)
	{
		for (unsigned int i = 0; i < meshes.size(); i++)
			meshes[i].Draw(shader);
	}

private:
	/*  函数  */
	// 从构造器中直接调用loadModel函数加载模型路径，用Assimp来加载模型至Assimp的一个叫做scene的数据结构中
	//这是Assimp数据接口的根对象。一旦我们有了这个场景对象，我们就能访问到加载后的模型中所有所需的数据了。
	void loadModel(string const& path)
	{
		// 读取模型路径  读取函数在Assimp命名空间中Importer
		Assimp::Importer importer;
		//第一个参数一个文件路径
		//第二个参数是一些后期处理(Post-processing)的选项
		//除了加载文件之外，Assimp允许我们设定一些选项来强制它对导入的数据做一些额外的计算或操作。
		//通过设定aiProcess_Triangulate，告诉Assimp，如果模型不是（全部）由三角形组成，它需要将模型所有的图元形状变换为三角形。
		//aiProcess_FlipUVs将在处理的时候翻转y轴的纹理坐标（在OpenGL中大部分的图像的y轴都是反的，所以这个后期处理选项将会修复这个）
		//aiProcess_GenNormals：如果模型不包含法向量的话，就为每个顶点创建法线。
		//aiProcess_SplitLargeMeshes：将比较大的网格分割成更小的子网格，如果你的渲染有最大顶点数限制，只能渲染较小的网格，那么它会非常有用。
		//aiProcess_OptimizeMeshes：和上个选项相反，它会将多个小网格拼接为一个大的网格，减少绘制调用从而进行优化。
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
		// 检查场景和其根节点不为null，并且检查了它的一个标记(Flag)，来查看返回的数据是不是不完整的
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			//通过导入器的GetErrorString函数来报告错误并返回
			cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
			return;
		}
		// 获取文件路径的目录路径
		directory = path.substr(0, path.find_last_of('/'));

		// 将第一个节点（根节点）传入了递归的processNode函数
		//因为每个节点（可能）包含有多个子节点，首先处理参数中的节点，再继续处理该节点所有的子节点，以此类推。
		processNode(scene->mRootNode, scene);
	}

	// 递归函数
	//处理自身所有节点下所有网格
	//在Assimp的结构中，每个节点包含了一系列的网格索引，每个索引指向场景对象中的那个特定网格。
	//我们接下来就想去获取这些网格索引，获取每个网格，处理每个网格，接着对每个节点的子节点重复这一过程
	//当一个节点不再有任何子节点之后，这个函数将会停止执行。
	void processNode(aiNode* node, const aiScene* scene)
	{
		// 处理节点所有的网格（如果有的话）
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			//检查每个节点的网格索引，并索引场景的mMeshes数组来获取对应的网格。
			//返回的网格将会传递到processMesh函数中，它会返回一个Mesh对象，我们可以将它存储在meshes列表 / vector
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(mesh, scene));
		}
		// 接下来对它的子节点重复这一过程
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}

	}
	//将Assimp的数据解析到Mesh类
	Mesh processMesh(aiMesh* mesh, const aiScene* scene)
	{
		// 声明要填充的数据
		vector<Vertex> vertices;
		vector<unsigned int> indices;
		vector<Texture> textures;

		// 获取所有的顶点数据
		//遍历网格中的所有顶点（使用mesh->mNumVertices来获取）
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			//储存顶点数据
			glm::vec3 vector;
			// 位置
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.Position = vector;
			// 法线
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;
			// UV
			//Assimp允许一个模型在一个顶点上有最多8个不同的纹理坐标，这里不会用到那么多，只关心第一组纹理坐标
			if (mesh->mTextureCoords[0]) // 不是所有的Mesh 都有UV坐标 进行判断
			{
				glm::vec2 vec;
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.TexCoords = vec;
			}
			else
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);
			// 切线
			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.Tangent = vector;
			// 副切线
			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.Bitangent = vector;
			vertices.push_back(vertex);
		}
		//设置索引
		//Assimp的接口定义了每个网格都有一个面(Face)数组，每个面代表了一个图元
		//在我们的例子中（由于使用了aiProcess_Triangulate选项）它总是三角形。
		//一个面包含了多个索引，它们定义了在每个图元中，我们应该绘制哪个顶点，并以什么顺序绘制。
		//所以如果我们遍历了所有的面，并储存了面的索引到indices这个vector中就可以了。
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];

			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}
		// 处理材质
		//一个网格只包含了一个指向材质对象的索引。如果想要获取网格真正的材质，我们还需要索引场景的mMaterials数组
		//网格材质索引位于它的mMaterialIndex属性中；
		//检测一个网格是否包含有材质
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

			//一个材质对象的内部对每种纹理类型都存储了一个纹理位置数组
			//不同的纹理类型都以aiTextureType_为前缀。
			//使用loadMaterialTextures的工具函数来从材质中获取纹理。这个函数将会返回一个Texture结构体的vector
			//我们将在模型的textures vector的尾部之后存储它。

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
		// 返回Mesh的所需数据
		return Mesh(vertices, indices, textures);
	}

	//loadMaterialTextures函数遍历了给定纹理类型的所有纹理位置，获取了纹理的文件位置，并加载并生成纹理，将信息储存在了一个textures结构体中
	vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
	{
		vector<Texture> textures;
		//GetTextureCount函数检查储存在材质中纹理的数量
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			//GetTexture获取每个纹理的文件位置，它会将结果储存在一个aiString中
			mat->GetTexture(type, i, &str);

			// 检查它有没有被加载过
			bool skip = false;
			for (unsigned int j = 0; j < textures_loaded.size(); j++)
			{
				//将纹理的路径与储存在textures_loaded这个vector中的所有纹理进行比较，看看当前纹理的路径是否与其中的一个相同。
				//如果是的话，则跳过纹理加载/生成的部分，直接使用定位到的纹理结构体为网格的纹理。
				if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
				{
					textures.push_back(textures_loaded[j]);
					skip = true;
					break;
				}
			}
			if (!skip)
			{   // 如果纹理还没有被加载，则加载它
				Texture texture;
				//TextureFromFile的工具函数，它将会（用stb_image.h）加载一个纹理并返回该纹理的ID
				texture.id = TextureFromFile(str.C_Str(), this->directory);
				texture.type = typeName;
				texture.path = str.C_Str();
				textures.push_back(texture);
				// 添加到已加载的纹理中
				textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
			}
		}
		return textures;
	}
};

//导入贴图函数
unsigned int TextureFromFile(const char* path, const string& directory, bool gamma)
{
	string filename = string(path);
	//导入与模型相同路径下的贴图
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