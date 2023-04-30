#include "BravoMesh.h"


#include "BravoTexture.h"

#include "BravoEngine.h"

void BravoMesh::Use()
{
}
void BravoMesh::StopUsage()
{
}

bool BravoMesh::Load_Internal()
{
	// read file via ASSIMP
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(Path, aiProcess_Triangulate | aiProcess_FlipUVs  | aiProcess_CalcTangentSpace);
	// check for errors
	if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		Log::LogMessage( "Failed to load mesh: " + std::string(importer.GetErrorString()));
		return false;
	}

	// process ASSIMP's root node recursively
	ProcessNode(scene->mRootNode, scene);
	return true;
}

void BravoMesh::ProcessNode(aiNode *node, const aiScene *scene)
{
	Log::LogMessage("LoadingNode: " + std::string(node->mName.C_Str()));
	// process each mesh located at the current node
	for(unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		// the node object only contains indices to index the actual objects in the scene. 
		// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		MeshParts.push_back(ProcessMesh(mesh, scene));
	}
	// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for(unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene);
	}
}

std::shared_ptr<BravoMeshPart> BravoMesh::ProcessMesh(aiMesh *mesh, const aiScene *scene)
{
	Log::LogMessage("  -LoadingMesh: " + std::string(mesh->mName.C_Str()));
	std::shared_ptr<BravoMeshPart> NewMeshPart = std::shared_ptr<BravoMeshPart>(new BravoMeshPart());
	
		
	// Walk through each of the mesh's vertices
	for(unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		BravoMeshPart::Vertex vertex;
		glm::vec3 vector;
		// positions
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;
		
		if ( mesh->mNormals )
		{
			// normals
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;
		}
		// texture coordinates
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
			// tangent
			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.Tangent = vector;
		}
		if ( mesh->mBitangents )
		{
			// bitangent
			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.Bitangent = vector;
		}
		NewMeshPart->Vertices.push_back(vertex);
	}
	// Indices
	for(unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for(unsigned int j = 0; j < face.mNumIndices; j++)
		{
			NewMeshPart->Indices.push_back(face.mIndices[j]);	
		}
	}
	
	NewMeshPart->Init();
	return NewMeshPart;
}

void BravoMesh::UnLoad_Internal()
{
	for (std::shared_ptr<BravoMeshPart> mpart : MeshParts )
	{
		glDeleteVertexArrays(1, &(mpart->VAO));
		glDeleteBuffers(1, &(mpart->VBO));
		glDeleteBuffers(1, &(mpart->EBO));
	}
}

void BravoMesh::SetMaterial(unsigned int MeshIndex, BravoMaterialPtr Material)
{
	if ( MeshIndex >= 0 && MeshIndex < MeshParts.size() )
		MeshParts[MeshIndex]->Material = Material;
}



void BravoMeshPart::Init()
{
	// create buffers/arrays
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	// load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(Vertex), &Vertices[0], GL_STATIC_DRAW);  

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(unsigned int), &Indices[0], GL_STATIC_DRAW);

	// vertex Positions
	glEnableVertexAttribArray(0);	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);	
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::Normal));
	// vertex texture coords
	glEnableVertexAttribArray(2);	
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::TexCoords));
	// vertex tangent
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::Tangent));
	// vertex bitangent
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::Bitangent));

	glBindVertexArray(0);
}

