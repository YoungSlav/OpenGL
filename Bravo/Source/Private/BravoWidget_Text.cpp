#include "BravoWidget_Text.h"
#include "stb_truetype.h"
#include "openGL.h"
#include "BravoFont.h"
#include "BravoHUD.h"

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


void BravoWidget_Text::Render_Internal()
{
	if ( Text.empty() )
		return;

	if ( !GetFont() )
		return;

	std::vector<HUDVertex> TextVertecies;
	TextVertecies.reserve(Text.size());

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
			GetFont()->SetModelMatrix(GetHUD()->GetModelMatrix());
			GetFont()->SetColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
			glm::vec2 pos = ScreenPosition;
			
			for(char& symbol : Text)
			{
				stbtt_aligned_quad q;
				GetFont()->GetCharacterQuad(symbol, 48, &pos.x, &pos.y, &q, true);
				TextVertecies.push_back(HUDVertex(glm::vec2(q.x0, q.y0), glm::vec2(q.s0, q.t0)));
				TextVertecies.push_back(HUDVertex(glm::vec2(q.x1, q.y0), glm::vec2(q.s1, q.t0)));
				TextVertecies.push_back(HUDVertex(glm::vec2(q.x1, q.y1), glm::vec2(q.s1, q.t1)));
				TextVertecies.push_back(HUDVertex(glm::vec2(q.x0, q.y1), glm::vec2(q.s0, q.t1)));
				TextVertecies.push_back(HUDVertex(glm::vec2(q.x0, q.y0), glm::vec2(q.s0, q.t0)));
				TextVertecies.push_back(HUDVertex(glm::vec2(q.x1, q.y1), glm::vec2(q.s1, q.t1)));
			}

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(HUDVertex) * TextVertecies.size(), TextVertecies.data());
			glDrawArrays(GL_TRIANGLES, 0, TextVertecies.size());

			GetFont()->StopUsage();

			glDeleteVertexArrays(1, &VAO);
			glDeleteBuffers(1, &VBO);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}