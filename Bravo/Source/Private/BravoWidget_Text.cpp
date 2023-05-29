#include "BravoWidget_Text.h"
#include "stb_truetype.h"
#include "openGL.h"
#include "BravoFont.h"
#include "BravoHUD.h"
#include "BravoAssetManager.h"
#include "BravoShader.h"

bool BravoWidget_Text::Initialize_Internal()
{
	if ( !BravoWidget::Initialize_Internal() )
		return false;

	Shader = GetAssetManager()->LoadAsset<BravoShader>("Shaders\\wText");
	return true;
}

void BravoWidget_Text::SetText(const std::string& _Text)
{
	Text = _Text;
}

void BravoWidget_Text::SetTextSize(uint32 _TextSize)
{
	TextSize = _TextSize;
}

void BravoWidget_Text::SetFont(std::shared_ptr<BravoFont> _Font)
{
	Font = _Font;
}

void BravoWidget_Text::SetTextColor(const glm::vec4& _TextColor)
{
	TextColor = _TextColor;
}

void BravoWidget_Text::SetMargin(const glm::vec2& _Margin)
{
	Margin = _Margin;
}

void BravoWidget_Text::SetShader(std::shared_ptr<class BravoShader> _Shader)
{
	Shader = _Shader;
}

// TODO: optimize?
void BravoWidget_Text::Render_Internal()
{
	if ( Text.empty() )
		return;

	if ( !GetFont() || !GetShader() || !GetHUD() )
		return;

	std::vector<HUDVertex> TextVertecies;

	GLuint VAO = 0;	// vertex array buffer (holds both vertex attributes and indices)
	GLuint VBO = 0;	// vertex attribute buffer
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(HUDVertex) * 6 * Text.size(), 0, GL_STATIC_DRAW);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(HUDVertex), 0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(HUDVertex), (const void*)offsetof(HUDVertex, TexCoords));

			GetFont()->Use();
			GetShader()->Use();

			GetShader()->SetMatrix4d("model", GetHUD()->GetModelMatrix());
			GetShader()->SetVector4d("uTextColor", TextColor);

			glm::vec2 actualSize = GetActualSize();

			glm::vec2 pos = GetPosition() * GetHUD()->GetSize();
			pos -= GetOrigin() * actualSize;
			pos.y += actualSize.y;
			// TODO: god knows why, but whatever...
			if ( const BravoFontInfo* fontInfo = GetFont()->GetFontInfo(TextSize, 1.0f) )
				pos.y -= abs(fontInfo->Descent) * GetHUD()->GetTargetScale().y;

			TextVertecies.reserve(Text.size());
			for(char& symbol : Text)
			{
				stbtt_aligned_quad RenderQuad;
				pos.x += GetFont()->GetCharacterQuad(symbol, TextSize, GetHUD()->GetTargetScale().y, pos.x, pos.y, true, &RenderQuad);
				TextVertecies.push_back(HUDVertex(glm::vec2(RenderQuad.x1, RenderQuad.y1), glm::vec2(RenderQuad.s1, RenderQuad.t1)));
				TextVertecies.push_back(HUDVertex(glm::vec2(RenderQuad.x1, RenderQuad.y0), glm::vec2(RenderQuad.s1, RenderQuad.t0)));
				TextVertecies.push_back(HUDVertex(glm::vec2(RenderQuad.x0, RenderQuad.y0), glm::vec2(RenderQuad.s0, RenderQuad.t0)));
				TextVertecies.push_back(HUDVertex(glm::vec2(RenderQuad.x1, RenderQuad.y1), glm::vec2(RenderQuad.s1, RenderQuad.t1)));
				TextVertecies.push_back(HUDVertex(glm::vec2(RenderQuad.x0, RenderQuad.y0), glm::vec2(RenderQuad.s0, RenderQuad.t0)));
				TextVertecies.push_back(HUDVertex(glm::vec2(RenderQuad.x0, RenderQuad.y1), glm::vec2(RenderQuad.s0, RenderQuad.t1)));
			}

			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(HUDVertex) * TextVertecies.size(), TextVertecies.data());
			glDrawArrays(GL_TRIANGLES, 0, (int32)TextVertecies.size());

			GetShader()->StopUsage();
			GetFont()->StopUsage();

			glDeleteVertexArrays(1, &VAO);
			glDeleteBuffers(1, &VBO);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

glm::vec2 BravoWidget_Text::GetActualSize() const
{
	if ( Text.empty() )
		return  glm::vec2(0.0f, 0.0f);

	glm::vec2 size = glm::vec2(0.0f, 0.0f);
	for(const char& symbol : Text)
	{
		stbtt_aligned_quad RenderQuad;
		size.x += GetFont()->GetCharacterQuad(symbol, TextSize, GetHUD()->GetTargetScale().y, size.x, size.y, true, &RenderQuad);
	}
	if ( const BravoFontInfo* fontInfo = GetFont()->GetFontInfo(TextSize, 1.0f) )
	{
		size.y = (abs(fontInfo->Ascent) + abs(fontInfo->Descent) + abs(fontInfo->Linegap)) * GetHUD()->GetTargetScale().y;
	}

	// add margin
	size += Margin * GetHUD()->GetTargetScale().y * 2.0f;

	return size;
}
