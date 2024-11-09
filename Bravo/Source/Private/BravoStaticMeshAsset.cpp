#include "BravoStaticMeshAsset.h"


#include "BravoTextureAsset.h"

#include "BravoEngine.h"

EAssetLoadingState BravoStaticMeshAsset::Load(const std::string& ResourcesPath, const BravoStaticMeshLoadingParams& params)
{
	std::thread asyncLoadThread(&BravoStaticMeshAsset::AsyncLoad, this, ResourcesPath, params);

	LoadingState = EAssetLoadingState::AsyncLoading;
	asyncLoadThread.detach();
	return LoadingState;
}

void BravoStaticMeshAsset::AsyncLoad(const std::string& ResourcesPath, const BravoStaticMeshLoadingParams& params)
{
	// read file via ASSIMP
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(ResourcesPath+params.MeshPath, aiProcess_Triangulate | aiProcess_FlipUVs  | aiProcess_CalcTangentSpace);
	// check for errors
	if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		Log::LogMessage(ELog::Error, "Failed to load mesh: {}", importer.GetErrorString());
		LoadingState = EAssetLoadingState::Failed;
		return;
	}

	// process ASSIMP's root node recursively
	ProcessNode(scene->mRootNode, scene);

	LoadingState = EAssetLoadingState::InRAM;
}

void BravoStaticMeshAsset::ProcessNode(aiNode *node, const aiScene *scene)
{
	//Log::LogMessage("LoadingNode: {}", node->mName.C_Str() );

	// process each mesh located at the current node
	for(uint32 i = 0; i < node->mNumMeshes; i++)
	{
		// the node object only contains indices to index the actual objects in the scene. 
		// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		if ( !mesh )
		{
			Log::LogMessage(ELog::Warning, "  -Failed to load mesh #: {}", i );
			continue;
		}

		//Log::LogMessage(ELog::Log"  -LoadingMesh: {}", mesh->mName.C_Str());
		// Walk through each of the mesh's vertices
		Vertices.reserve(mesh->mNumVertices + Vertices.size());
		for(uint32 i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			glm::vec3 vector;
			if ( mesh->mVertices )
			{
				vector.x = mesh->mVertices[i].x;
				vector.y = mesh->mVertices[i].y;
				vector.z = mesh->mVertices[i].z;
				vertex.Position = vector;
			}
		
			if ( mesh->mNormals )
			{
				vector.x = mesh->mNormals[i].x;
				vector.y = mesh->mNormals[i].y;
				vector.z = mesh->mNormals[i].z;
				vertex.Normal = vector;
			}
			if(mesh->mTextureCoords[0])
			{
				glm::vec2 vec;
				vec.x = mesh->mTextureCoords[0][i].x; 
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.TexCoords = vec;
			}
			else
			{
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);
			}
			if ( mesh->mTangents )
			{
				vector.x = mesh->mTangents[i].x;
				vector.y = mesh->mTangents[i].y;
				vector.z = mesh->mTangents[i].z;
				vertex.Tangent = vector;
			}
			if ( mesh->mBitangents )
			{
				vector.x = mesh->mBitangents[i].x;
				vector.y = mesh->mBitangents[i].y;
				vector.z = mesh->mBitangents[i].z;
				vertex.Bitangent = vector;
			}
			if ( mesh->HasVertexColors(0) )
			{
				vertex.Color = glm::vec4(mesh->mColors[0][i].r, mesh->mColors[0][i].g, mesh->mColors[0][i].b, mesh->mColors[0][i].a);
			}
			Vertices.push_back(vertex);
		}
		

		for(uint32 i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			Indices.reserve(face.mNumIndices + Indices.size());
			for(uint32 j = 0; j < face.mNumIndices; j++)
			{
				Indices.push_back(face.mIndices[j]);	
			}
		}
	}

	for(uint32 i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene);
	}
}


bool BravoStaticMeshAsset::LoadToGPU_Internal()
{
	if ( !(Vertices.size() && Indices.size() && Indices.size() % 3 == 0) )
	{
		Log::LogMessage(ELog::Warning, "Vertex data is corrupt for mesh: {}",  GetName() );
		return false;
	}

	// create buffers/arrays
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(Vertex), &Vertices[0], GL_STATIC_DRAW);  

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(uint32), &Indices[0], GL_STATIC_DRAW);

	// vertex Positions
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);
	// vertex normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::Normal));
	glEnableVertexAttribArray(1);
	// vertex texture coords
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::TexCoords));
	glEnableVertexAttribArray(2);
	// vertex tangent
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::Tangent));
	glEnableVertexAttribArray(3);
	// vertex bitangent
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::Bitangent));
	glEnableVertexAttribArray(4);
	// vertex colors
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::Color));
	glEnableVertexAttribArray(5);

	glBindVertexArray(0);

	return true;
}

void BravoStaticMeshAsset::Render(size_t InstanceCount)
{
	if ( InstanceCount == 0 )
		return;
	
	glBindVertexArray(VAO);
	if ( InstanceCount == 1 )
	{
		glDrawElements(GL_TRIANGLES, (int32)Indices.size(), GL_UNSIGNED_INT, 0);
	}
	else
	{
		glDrawElementsInstanced(GL_TRIANGLES, (int32)Indices.size(), GL_UNSIGNED_INT, 0, (int32)InstanceCount);
	}
	glBindVertexArray(0);
}


void BravoStaticMeshAsset::ReleaseFromGPU_Internal()
{
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(6, &VAO);

	VAO = 0;
	VBO = 0;
	EBO = 0;
}
