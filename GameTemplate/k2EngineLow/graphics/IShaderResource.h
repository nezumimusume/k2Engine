
namespace nsK2EngineLow {
	class IShaderResource : public Noncopyable {
	public:
		virtual ~IShaderResource()
		{
		}
		/// <summary>
		/// SRV�ɓo�^�B
		/// </summary>
		/// <param name="descriptorHandle"></param>
		virtual void RegistShaderResourceView(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle, int bufferNo) = 0;
	};
}