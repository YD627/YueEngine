#include "Model.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "Core/Log.h"
#include "Renderer/Material.h"
#include "Renderer/Renderer.h"

namespace Yue {
	static Ref<Mesh> CreateMesh(aiMesh* mesh)
	{
		std::vector<Vertex> vertices;
		vertices.reserve(mesh->mNumVertices);

		for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
		{
			Vertex vertex{};
			vertex.Position = {
				mesh->mVertices[i].x,
				mesh->mVertices[i].y,
				mesh->mVertices[i].z
			};

			if (mesh->HasNormals())
			{
				vertex.Normal = {
					mesh->mNormals[i].x,
					mesh->mNormals[i].y,
					mesh->mNormals[i].z
				};
			}
			else
			{
				vertex.Normal = { 0.0f, 1.0f, 0.0f };
			}

			if (mesh->HasTextureCoords(0))
			{
				vertex.TexCoord = {
					mesh->mTextureCoords[0][i].x,
					mesh->mTextureCoords[0][i].y
				};
			}
			else
			{
				vertex.TexCoord = { 0.0f, 0.0f };
			}

			// Model files usually have no vertex colors; use white so the
			// texture is not multiplied to black.
			vertex.Color = { 1.0f, 1.0f, 1.0f };

			vertices.push_back(vertex);
		}

		std::vector<uint32_t> indices;
		for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
		{
			const aiFace& face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; ++j)
			{
				indices.push_back(face.mIndices[j]);
			}
		}

		return Mesh::Create(vertices, indices);
	}

	// Current version only handles mesh data and ignores node hierarchy transforms.
	// A single-node OBJ works; glTF/FBX hierarchy support can accumulate transforms later.
	static void ProcessNode(aiNode* node, const aiScene* scene,
		std::vector<Ref<Mesh>>& meshes)
	{
		for (unsigned int i = 0; i < node->mNumMeshes; ++i)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(CreateMesh(mesh));
		}

		for (unsigned int i = 0; i < node->mNumChildren; ++i)
		{
			ProcessNode(node->mChildren[i], scene, meshes);
		}
	}

	Model::Model(const std::string& path)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(
			path,
			aiProcess_Triangulate | aiProcess_FlipUVs);

		if (!scene || !scene->mRootNode || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE))
		{
			YUE_CORE_ERROR("Failed to load model: " + path + " ("
				+ importer.GetErrorString() + ")");
			return;
		}

		ProcessNode(scene->mRootNode, scene, m_Meshes);
		m_IsLoaded = !m_Meshes.empty();

		if (m_IsLoaded)
		{
			YUE_CORE_INFO("Model loaded: " + path + " ("
				+ std::to_string(m_Meshes.size()) + " meshes)");
		}
		else
		{
			YUE_CORE_ERROR("Model contains no meshes: " + path);
		}
	}

	void Model::Draw(const Ref<Material>& material, const glm::mat4& transform) const
	{
		for (const auto& mesh : m_Meshes)
		{
			Renderer::Submit(material, mesh->GetVertexArray(), transform);
		}
	}
}
