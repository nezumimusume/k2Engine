
#pragma once

#include "graphics/MyRenderer.h"

namespace nsK2Engine {
	namespace nsDbg {
		/// <summary>
		/// �x�N�g�������_���[
		/// </summary>
		class VectorRenderer : public IGameObject{
		public:
			/// <summary>
			/// �`��f�[�^�B
			/// </summary>
			struct SRenderData {
				std::string name;						//���O
				Vector3 vector;		//�x�N�g���B
				Vector3 origin;		//��_�B
			};
			/// <summary>
			/// �`�悷��x�N�g����ǉ��B
			/// </summary>
			/// <param name="renderData">�`�悷��x�N�g���̏��B</param>
			void AddVector(SRenderData& renderData)
			{
				if (m_renderData.size() < MAX_VECTOR) {
					m_renderData.emplace_back(renderData);
				}
			}
			/// <summary>
			/// �`���L���ɂ���B
			/// </summary>
			void EnableRender()
			{
				m_isRender = true;
			}
			/// <summary>
			/// �`��𖳌��ɂ���B
			/// </summary>
			void DisableRender()
			{
				m_isRender = false;
			}
			/// <summary>
			/// �`�悷��x�N�g���̖��O���X�g��ݒ�B
			/// </summary>
			void SetRenderVectorNameList(const std::vector<std::string>& list)
			{
				m_renderVectorNameList = list;
			}
			/// <summary>
			/// Update()�����s����钼�O�Ɉ�x�����Ă΂�鏈���B
			/// </summary>
			/// <returns></returns>
			bool Start() override;
			/// <summary>
			/// �X�V
			/// </summary>
			void Update() override;
			/// <summary>
			/// �`�揈��
			/// </summary>
			/// <param name="rc"></param>
			void Render(RenderContext& rc) override;
		private:
			/// <summary>
			/// �`�悷��x�N�g�������肷��B
			/// </summary>
			/// <param name="renderData">�`��f�[�^</param>
			/// <returns>true���Ԃ��Ă�����`�悷��B</returns>
			bool IsRenderVector(const SRenderData& renderData) const;
		private:
			static const int MAX_VECTOR = 1000;	// �`��\�ȃx�N�g���̍ő吔�B
			std::vector<SRenderData> m_renderData;			// �`��f�[�^�B
			bool m_isRender = true;								// �`��t���O
			std::vector<std::string> m_renderVectorNameList;// �`�悷��x�N�g���̖��O���X�g
			ModelRender m_modelRender;						// ���f�������_���[�B
		};
	}
}