#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS
#pragma comment( lib, "xinput.lib")
#pragma comment(lib, "dxcompiler.lib")

#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory>
#include <math.h>
#include <vector>
#include <list>
#include <D3Dcompiler.h>
#include <Windows.h>
#include <d3d12.h>
#include <map>
#include <functional>
#include <algorithm>
#include <DirectXMath.h>
#include <Xinput.h>
#include <array>
#include <dxcapi.h>
#include <atlbase.h>
#include <dxgi.h>
#include <dxgitype.h>
#include <dxgiformat.h>


#include "DirectXTK/Inc/CommonStates.h"
#include "DirectXTK/Inc/SpriteBatch.h"
#include "DirectXTK/Inc/SpriteFont.h"

#include "Effekseer.h"
#include "EffekseerRendererDX12.h"

#include "graphics/d3dx12.h"

const int MAX_RENDERING_TARGET = D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT;	//レンダリングターゲットの最大数。

using namespace Microsoft::WRL;

#define USE_FPS_LIMITTER		// 有効でFPSに上限を設ける。
static const int MAX_FPS = 60;	// 最大FPS。USE_FPS_LIMITTERが有効なときに使用される。
#define ENABLE_DXR_ON_RTX_ONLY	// 有効でDXRはRTXシリーズのみ有効。

#include "util/Noncopyable.h"
#include "dbg/MyAssert.h"
#include "dbg/MyLog.h"
#include "time/GameTime.h"
#include "math/Math.h"
#include "math/Vector.h"
#include "math/Matrix.h"

#include "k2EngineLow.h"
#include "graphics/VertexBuffer.h"
#include "graphics/IndexBuffer.h"
#include "graphics/IShaderResource.h"
#include "graphics/IUnorderAccessResource.h"
#include "graphics/GPUBuffer.h"
#include "graphics/Texture.h"
#include "graphics/RWStructuredBuffer.h"
#include "graphics/StructuredBuffer.h"
#include "graphics/Shader.h"
#include "graphics/PipelineState.h"
#include "graphics/ConstantBuffer.h"
#include "graphics/RootSignature.h"
#include "graphics/raytracing/Raytracing.h"
#include "graphics/GraphicsEngine.h"
#include "graphics/DescriptorHeap.h"
#include "graphics/RenderTarget.h"
#include "graphics/Animation.h"
#include "graphics/font/Font.h"

#include "graphics/RenderContext_inline.h"
#include "graphics/DescriptorHeap_inline.h"

#include "graphics/Sprite.h"
#include "graphics/Model.h"
#include "HID/GamePad.h"

#include "graphics/GaussianBlur.h"

const UINT FRAME_BUFFER_W = 1920;	// フレームバッファの幅。
const UINT FRAME_BUFFER_H = 1080;	// フレームバッファの高さ。
const UINT UI_SPACE_WIDTH = 1920;	// UI空間の幅。
const UINT UI_SPACE_HEIGHT = 1080;	// UI空間の高さ。
static const int MAX_BONE = 512;	// ボーンの最大数。



#include "btBulletDynamicsCommon.h"
#include "BulletCollision\CollisionDispatch\btGhostObject.h"

#include "physics/RigidBody.h"
#include "physics/Physics.h"
#include "physics/MeshCollider.h"
#include "physics/SphereCollider.h"
#include "physics/BoxCollider.h"
#include "physics/CollisionAttr.h"
#include "physics/CharacterController.h"
#include "physics/PhysicsStaticObject.h"

#include "gameObject/GameObjectManager.h"
#include "gameObject/IGameObject.h"

#include <windows.h>
#include <xaudio2.h>
#include <x3daudio.h>
#include <xaudio2fx.h>


#include "graphics/effect/EffectEngine.h"
#include "sound/SoundSource.h"
#include "graphics/effect/EffectEmitter.h"