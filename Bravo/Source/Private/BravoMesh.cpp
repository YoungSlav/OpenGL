#include "BravoMesh.h"


#include "BravoTexture.h"

#include "BravoEngine.h"

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
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	
	VBO = 0;
	EBO = 0;
}


void BravoMesh::ProcessNode(aiNode *node, const aiScene *scene)
{
	//Log::LogMessage("LoadingNode: " + std::string(node->mName.C_Str()));
	// process each mesh located at the current node
	for(uint32 i = 0; i < node->mNumMeshes; i++)
	{
		// the node object only contains indices to index the actual objects in the scene. 
		// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		if ( !mesh )
		{
			Log::LogMessage("  -Failed to load mesh #: " +  std::to_string(i), ELog::Warning);
			continue;
		}

		//Log::LogMessage("  -LoadingMesh: " + std::string(mesh->mName.C_Str()));
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
			if ( mesh->mColors[0] )
			{
				vertex.Color = glm::vec4(mesh->mColors[0]->r, mesh->mColors[0]->g, mesh->mColors[0]->b, mesh->mColors[0]->a);
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


bool BravoMesh::LoadToGPU_Internal()
{
	if ( !(Vertices.size() && Indices.size() && Indices.size() % 3 == 0) )
	{
		Log::LogMessage("Vertex data is corrupt for mesh: " +  Path, ELog::Warning);
		return false;
	}

	// create buffers/arrays
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(Vertex), &Vertices[0], GL_STATIC_DRAW);  

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(uint32), &Indices[0], GL_STATIC_DRAW);

	


	return true;
}


void BravoMesh::ReleaseFromGPU_Internal()
{
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	VBO = 0;
	EBO = 0;
}
