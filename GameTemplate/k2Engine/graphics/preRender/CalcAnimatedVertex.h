#pragma once

namespace nsK2EngineLow {
	/// <summary>
	/// �A�j���[�V�����ςݒ��_�v�Z�����B
	/// </summary>
	/// <remark>
	/// �R���s���[�g�V�F�[�_�[�ŃA�j���[�V�����ςݒ��_���v�Z���܂��B
	/// </remark>
	class CalcAnimatedVertex : public Noncopyable {
	public:
		/// <summary>
		/// �������B
		/// </summary>
		void Init();
		/// <summary>
		/// ���s�B
		/// </summary>
		void Execute(RenderContext& rc);
	};
}