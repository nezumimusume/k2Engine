/*!
* @brief	�t�H���g�B
*/
#include "k2EngineLowPreCompile.h"
#include "FontEngine.h"
#include "font.h"

namespace nsK2EngineLow {
	void Font::Begin(RenderContext& rc)
	{
		auto& fontEngine = g_graphicsEngine->GetFontEngine();
		fontEngine.BeginDraw(rc);
	}
	void Font::End(RenderContext& rc)
	{
		auto& fontEngine = g_graphicsEngine->GetFontEngine();
		fontEngine.EndDraw(rc);
	}
	void Font::Draw(
		wchar_t const* text,
		const Vector2& position,
		const Vector4& color,
		float rotation,
		float scale,
		Vector2 pivot)
	{
		if (text == nullptr) {
			return;
		}
		pivot.y = 1.0f - pivot.y;
		DirectX::XMFLOAT2 tkFloat2Zero(0, 0);
		//���W�n���X�v���C�g�ƍ��킹��B
		Vector2 pos = position;
		float frameBufferHalfWidth = UI_SPACE_WIDTH * 0.5f;
		float frameBufferHalfHeight = UI_SPACE_HEIGHT * 0.5f;
		pos.x += frameBufferHalfWidth;
		pos.y = -pos.y + frameBufferHalfHeight;

		auto& fontEngine = g_graphicsEngine->GetFontEngine();

		if (m_isDrawShadow) {
			//�e�������B
			Vector2 offsetTbl[] = {
				{ m_shadowOffset , 0.0f},
				{ -m_shadowOffset , 0.0f },
				{ 0.0f , m_shadowOffset },
				{ 0.0f , -m_shadowOffset },

				{ m_shadowOffset ,  m_shadowOffset },
				{ m_shadowOffset ,  -m_shadowOffset },
				{ -m_shadowOffset , m_shadowOffset },
				{ -m_shadowOffset , -m_shadowOffset },
			};
			for (auto offset : offsetTbl) {

				Vector2 sPos = pos;
				sPos.x += offset.x;
				sPos.y += offset.y;
				fontEngine.Draw(
					text,
					sPos,
					m_shadowColor,
					rotation,
					scale,
					pivot

				);
			}

		}
		fontEngine.Draw(
			text,
			pos,
			color,
			rotation,
			scale,
			pivot
		);
	}
}