#include "BravoMesh.h"


#include "BravoTexture.h"

#include "BravoEngine.h"

BravoMeshPart::BravoMeshPart(aiMesh* mesh, const aiScene* scene)
{
	if ( mesh && scene )
	{
		// Walk through each of the mesh's vertices
		Vertices.reserve(mesh->mNumVertices);
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
			Vertices.push_back(vertex);
		}
		// Indices
		for(unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			Indices.reserve(face.mNumIndices);
			for(unsigned int j = 0; j < face.mNumIndices; j++)
			{
				Indices.push_back(face.mIndices[j]);	
			}
		}
	}
}
BravoMeshPart::~BravoMeshPart()
{
	ReleaseFromGPU();
}

void BravoMeshPart::LoadToGPU()
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
}

void BravoMeshPart::Render()
{
	if ( !VAO )
		LoadToGPU();
	
	if ( VAO )
	{
		// draw mesh
		glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glActiveTexture(0);
	}
}

void BravoMeshPart::ReleaseFromGPU()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	VAO = 0;
	VBO = 0;
	EBO = 0;
}

bool BravoMesh::Initialize_Internal(const std::vector<std::string>& _Params)
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

BravoMesh::~BravoMesh()
{
	for (std::shared_ptr<BravoMeshPart> mpart : MeshParts )
	{
		mpart->ReleaseFromGPU();
	}
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
		Log::LogMessage("  -LoadingMesh: " + std::string(mesh->mName.C_Str()));

		MeshParts.push_back(std::shared_ptr<BravoMeshPart>(new BravoMeshPart(mesh, scene)));
	}
	// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for(unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene);
	}
}


bool BravoMesh::LoadToGPU_Internal()
{
	for (std::shared_ptr<BravoMeshPart> mpart : MeshParts )
	{
		mpart->LoadToGPU();
	}
	return true;
}

void BravoMesh::Render()
{
	for (std::shared_ptr<BravoMeshPart> mpart : MeshParts )
	{
		mpart->Render();
	}
	if ( Material )
		Material->StopUsage();
}

void BravoMesh::ReleaseFromGPU_Internal()
{
	for (std::shared_ptr<BravoMeshPart> mpart : MeshParts )
	{
		mpart->ReleaseFromGPU();
	}
}

void BravoMesh::SetMaterial(BravoMaterialPtr _Material)
{
	Material = _Material;
}

BravoMaterialPtr BravoMesh::GetMaterial() const
{
	return Material;
}