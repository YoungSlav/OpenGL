#include "BravoWidget_Plane.h"
#include "BravoAssetManager.h"
#include "BravoHUD.h"
#include "BravoShader.h"
#include "BravoTexture.h"

#include "openGL.h"

bool BravoWidget_Plane::Initialize_Internal()
{
	if ( !BravoWidget::Initialize_Internal() )
		return false;

	Shader = GetAssetManager()->LoadAsset<BravoShader>("Shaders\\wPlane");
	return true;
}

void BravoWidget_Plane::SetColor(const glm::vec4& _Color)
{
	Color = _Color;
}

void BravoWidget_Plane::SetTexture(std::shared_ptr<class BravoTexture> _Texture)
{
	Texture = _Texture;
}

void BravoWidget_Plane::SetShader(std::shared_ptr<class BravoShader> _Shader)
{
	Shader = _Shader;
}

void BravoWidget_Plane::Render_Internal()
{
	if ( Color.a == 0.0f )
		return;
	
	if ( !GetShader() || !GetHUD() )
		return;

	std::vector<HUDVertex> PlaneVertecies;

	GLuint VAO = 0;	// vertex array buffer (holds both vertex attributes and indices)
	GLuint VBO = 0;	// vertex attribute buffer
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(HUDVertex) * 6, 0, GL_STATIC_DRAW);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(HUDVertex), 0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(HUDVertex), (const void*)offsetof(HUDVertex, TexCoords));

			GetShader()->Use();

			GetShader()->SetMatrix4d("model", GetHUD()->GetModelMatrix());
			GetShader()->SetVector4d("uColor", Color);
			if ( GetTexture() )
			{
				GetTexture()->Use();
				GetShader()->SetBool("uTextureBound", true);
				GetShader()->SetTexture("uTexture", GetTexture());
			}
			else
			{
				GetShader()->SetBool("uTextureBound", false);
			}

			glm::vec2 actualSize = GetActualSize();
			glm::vec2 pos0 = GetPosition() * GetHUD()->GetSize();
			pos0 -= GetOrigin() * actualSize;
			glm::vec2 pos1 = pos0 + actualSize;
			
			PlaneVertecies.reserve(6);
			
			PlaneVertecies.push_back(HUDVertex(glm::vec2(pos0.x, pos0.y), glm::vec2(0.0f, 0.0f)));
			PlaneVertecies.push_back(HUDVertex(glm::vec2(pos1.x, pos1.y), glm::vec2(1.0f, 1.0f)));
			PlaneVertecies.push_back(HUDVertex(glm::vec2(pos1.x, pos0.y), glm::vec2(1.0f, 0.0f)));
			PlaneVertecies.push_back(HUDVertex(glm::vec2(pos0.x, pos0.y), glm::vec2(0.0f, 0.0f)));
			PlaneVertecies.push_back(HUDVertex(glm::vec2(pos0.x, pos1.y), glm::vec2(0.0f, 1.0f)));
			PlaneVertecies.push_back(HUDVertex(glm::vec2(pos1.x, pos1.y), glm::vec2(1.0f, 1.0f)));

			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(HUDVertex) * PlaneVertecies.size(), PlaneVertecies.data());
			glDrawArrays(GL_TRIANGLES, 0, (int32)PlaneVertecies.size());

			GetShader()->StopUsage();
			if ( GetTexture() )
			{
				GetTexture()->StopUsage();
			}

			glDeleteVertexArrays(1, &VAO);
			glDeleteBuffers(1, &VBO);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}