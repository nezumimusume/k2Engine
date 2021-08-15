#pragma once
#include "Effect.h"

namespace nsK2EngineLow {
	/// <summary>
	/// �G�t�F�N�g�𔭐��A�Đ�����N���X�B
	/// </summary>
	class EffectEmitter : public IGameObject
	{
	public:
		/// <summary>
		/// �R���X�g���N�^�B
		/// </summary>
		EffectEmitter();
		/// <summary>
		/// �f�X�g���N�^�B
		/// </summary>
		~EffectEmitter();
		/// <summary>
		/// �X�V�����B
		/// </summary>
		void Update();
		/// <summary>
		/// �G�t�F�N�g��ǂݍ��ށB
		/// </summary>
		/// <param name="number">EffectEngine::GetInstance()->ResistEffect�Ŏw�肵���ԍ��B</param>
		void Init(const int number);
		/// <summary>
		/// �G�t�F�N�g���Đ�����B
		/// </summary>
		void Play()
		{
			m_effect.Play();
		}
		/// <summary>
		/// �G�t�F�N�g���~����B
		/// </summary>
		void Stop()
		{
			m_effect.Stop();
		}
		/// <summary>
		/// ���W��ݒ�B
		/// </summary>
		/// <param name="position">���W�B/param>
		void SetPosition(const Vector3& position)
		{
			m_effect.SetPosition(position);
		}
		/// <summary>
		/// ���W���擾�B
		/// </summary>
		/// <returns>���W�B</returns>
		const Vector3& GetPosition() const
		{
			return m_effect.GetPosition();
		}
		/// <summary>
		/// �X�P�[��(�g�嗦��ݒ�)�B
		/// </summary>
		/// <param name="scale">�X�P�[��(�g�嗦)�B</param>
		void SetScale(const Vector3& scale)
		{
			m_effect.SetScale(scale);
		}
		/// <summary>
		/// �X�P�[��(�g�嗦)���擾�B
		/// </summary>
		/// <returns>�X�P�[��(�g�嗦)�B</returns>
		const Vector3& GetScale() const
		{
			return m_effect.GetScale();
		}
		/// <summary>
		/// ��]��ݒ�B
		/// </summary>
		/// <param name="rotation">��]�B</param>
		void SetRotation(const Quaternion& rotation)
		{
			m_effect.SetRotation(rotation);
		}
		/// <summary>
		/// ��]���擾�B
		/// </summary>
		/// <returns>��]�B</returns>
		const Quaternion& GetRotation() const
		{
			m_effect.GetRotation();
		}
		/// <summary>
		/// �Đ������ǂ������擾�B
		/// </summary>
		/// <returns>�Đ����Ȃ�true�B</returns>
		const bool IsPlay() const
		{
			return m_effect.IsPlay();
		}
	private:
		Effect m_effect;					//�G�t�F�N�g�B
	};
}
