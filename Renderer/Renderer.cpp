#include "ThirdParty/stb/stb_image.h"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/DefaultShader.hpp"
#include "Engine/Renderer/ConstantBuffer.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/Vertex_TBN.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/Image.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
// #include <gl/gl.h>
// #pragma comment(lib, "opengl32")

#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#ifdef OPAQUE
#undef OPAQUE
#endif

#if defined(_DEBUG)
#define ENGINE_DEBUG_RENDER
#endif

#if defined(ENGINE_DEBUG_RENDER)
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
#endif

#if defined (ENGINE_DEBUG_RENDER)
void* m_dxgiDebug = nullptr;
void* m_dxgiDebugModule = nullptr;
#endif

struct CameraConstants
{
	Mat44 WorldToCameraTransform;
	Mat44 CameraToRenderTransform;
	Mat44 RenderToClipTransform;
};
static const int k_cameraConstantsSlot = 2;

struct LightConstants
{
	Vec3 SunDirection = Vec3(0.f, 0.f, -1.f);
	float SunIntensity = 0.8f;
	float AmbientIntensity = 0.2f;
	float Padding[3] = {};
};
static const int k_lightConstantsSlot = 1;

struct ModelConstants
{
	Mat44 ModelToWorldTransform;
	float ModelColor[4];
};
static const int k_modelConstantsSlot = 3;

Renderer::Renderer(RendererConfig const& config)
	:m_config(config)
{
}

void Renderer::Startup( )
{
	unsigned int deviceFlags = 0;
#if defined (ENGINE_DEBUG_RENDER)
	deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// Create debug module
#if defined(ENGINE_DEBUG_RENDER)
	m_dxgiDebugModule = (void*)::LoadLibraryA("dxgidebug.dll");
	if (m_dxgiDebugModule == nullptr)
	{
		ERROR_AND_DIE("Could not load dxgidebug.dll.");
	}

	typedef HRESULT(WINAPI* GetDebugModuleCB)(REFIID, void**);
	((GetDebugModuleCB)::GetProcAddress(
		(HMODULE)m_dxgiDebugModule,
		"DXGIGetDebugInterface"))
		(__uuidof(IDXGIDebug), &m_dxgiDebug);

	if (m_dxgiDebug == nullptr)
	{
		ERROR_AND_DIE("Could not load debug module.");
	}
#endif

	// Create device and swap chain
	DXGI_SWAP_CHAIN_DESC swapChainDesc = { };
	swapChainDesc.BufferDesc.Width = g_engine->m_window->GetClientDimensions().x;
	swapChainDesc.BufferDesc.Height = g_engine->m_window->GetClientDimensions().y;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; swapChainDesc.BufferCount = 2;
	swapChainDesc.OutputWindow = (HWND)g_engine->m_window->GetHwnd();
	swapChainDesc.Windowed = true;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	HRESULT hr;
	hr = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE, NULL, deviceFlags, nullptr, 0, D3D11_SDK_VERSION, &swapChainDesc,
		&m_swapChain, &m_device, nullptr, &m_deviceContext);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could not create D3D 11 device and swap chain.");
	}

	m_immediateVBO = CreateVertexBuffer(sizeof(Vertex), sizeof(Vertex));
	m_immediateTBNVBO = CreateVertexBuffer(sizeof(Vertex_TBN), sizeof(Vertex_TBN)); //To support Vertex_TBN
	m_immediateIBO = CreateIndexBuffer(sizeof(unsigned int), sizeof(unsigned int));

	m_cameraCBO = CreateConstantBuffer((unsigned int)sizeof(CameraConstants));
	m_lightCBO = CreateConstantBuffer((unsigned int)sizeof(LightConstants));
	m_modelCBO = CreateConstantBuffer((unsigned int)sizeof(ModelConstants));

	// OPAQUE
	{
		D3D11_BLEND_DESC blendDesc = {};
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = blendDesc.RenderTarget[0].SrcBlend;
		blendDesc.RenderTarget[0].DestBlendAlpha = blendDesc.RenderTarget[0].DestBlend;
		blendDesc.RenderTarget[0].BlendOpAlpha = blendDesc.RenderTarget[0].BlendOp;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		hr = m_device->CreateBlendState(&blendDesc, &m_blendStates[(int)BlendMode::OPAQUE]);
		if (!SUCCEEDED(hr))
		{
			ERROR_AND_DIE("CreateBlendState for BlendMode::OPAQUE failed.");
		}
	}

	// ALPHA
	{
		D3D11_BLEND_DESC blendDesc = {};
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = blendDesc.RenderTarget[0].SrcBlend;
		blendDesc.RenderTarget[0].DestBlendAlpha = blendDesc.RenderTarget[0].DestBlend;
		blendDesc.RenderTarget[0].BlendOpAlpha = blendDesc.RenderTarget[0].BlendOp;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		hr = m_device->CreateBlendState(&blendDesc, &m_blendStates[(int)BlendMode::ALPHA]);
		if (!SUCCEEDED(hr))
		{
			ERROR_AND_DIE("CreateBlendState for BlendMode::ALPHA failed.");
		}
	}

	// ADDITIVE
	{
		D3D11_BLEND_DESC blendDesc = {};
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = blendDesc.RenderTarget[0].SrcBlend;
		blendDesc.RenderTarget[0].DestBlendAlpha = blendDesc.RenderTarget[0].DestBlend;
		blendDesc.RenderTarget[0].BlendOpAlpha = blendDesc.RenderTarget[0].BlendOp;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		hr = m_device->CreateBlendState(&blendDesc, &m_blendStates[(int)BlendMode::ADDITIVE]);
		if (!SUCCEEDED(hr))
		{
			ERROR_AND_DIE("CreateBlendState for BlendMode::ADDITIVE failed.");
		}
	}

	// Initialize current state
	m_blendState = m_blendStates[(int)m_desiredBlendMode];
	SetStatesIfChanged();

	// POINT_CLAMP
	{
		D3D11_SAMPLER_DESC samplerDesc = {};
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		hr = m_device->CreateSamplerState(&samplerDesc,
			&m_samplerStates[(int)SamplerMode::POINT_CLAMP]);

		if (!SUCCEEDED(hr))
		{
			ERROR_AND_DIE("CreateSamplerState for SamplerMode::POINT_CLAMP failed.");
		}
	}

	// BILINEAR_WRAP
	{
		D3D11_SAMPLER_DESC samplerDesc = {};
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		hr = m_device->CreateSamplerState(&samplerDesc,
			&m_samplerStates[(int)SamplerMode::BILINEAR_WRAP]);

		if (!SUCCEEDED(hr))
		{
			ERROR_AND_DIE("CreateSamplerState for SamplerMode::BILINEAR_WRAP failed.");
		}
	}

	// MIRROR
	{
		D3D11_SAMPLER_DESC samplerDesc = {};
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		hr = m_device->CreateSamplerState(&samplerDesc,
			&m_samplerStates[(int)SamplerMode::MIRROR]);

		if (!SUCCEEDED(hr))
		{
			ERROR_AND_DIE("CreateSamplerState for SamplerMode::POINT_CLAMP failed.");
		}
	}

	// Initialize current sampler state to desired
	m_samplerState = m_samplerStates[(int)m_desiredSamplerMode];
	SetStatesIfChanged();

	//Initialize Default Texture
	Image defaultImage(IntVec2(2, 2), Rgba8(255, 255, 255, 255));
	defaultImage.m_imageFilePath = "Default";

	m_defaultTexture = CreateTextureFromImage(defaultImage);
	m_defaultTexture->m_name = "Default";
	m_loadedTextures.push_back(m_defaultTexture); 

	// Get back buffer texture
	ID3D11Texture2D* backBuffer;
	hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer); 
	if (!SUCCEEDED(hr))
	{
			ERROR_AND_DIE("Could not get swap chain buffer.");
	}
	
	hr = m_device->CreateRenderTargetView(backBuffer, NULL, &m_renderTargetView); 
	if (!SUCCEEDED(hr))
	{
	ERROR_AND_DIE("Could create render target view for swap chain buffer.");
	}
	
	backBuffer->Release();

	CreateRasterizerStates();

	// Create depth stencil texture and view
	D3D11_TEXTURE2D_DESC depthDesc = {};
	depthDesc.Width = (UINT)g_engine->m_window->GetClientDimensions().x;
	depthDesc.Height = (UINT)g_engine->m_window->GetClientDimensions().y;
	depthDesc.MipLevels = 1;
	depthDesc.ArraySize = 1;
	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthDesc.SampleDesc.Count = 1;

	hr = m_device->CreateTexture2D(&depthDesc, nullptr, &m_depthStencilTexture);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could not create texture for depth stencil.");
	}

	hr = m_device->CreateDepthStencilView(m_depthStencilTexture, nullptr, &m_depthStencilDSV);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could not create depth stencil view.");
	}

	CreateDepthStencilStates();

	m_defaultShader = CreateShader("Default", g_defaultShaderSource);
	m_currentShader = m_defaultShader;
	m_loadedShaders.push_back(m_currentShader);
	BindShader(m_currentShader);

	BindTexture(m_defaultTexture);
}

void Renderer::Shutdown()
{ 
	for (Shader* shader : m_loadedShaders)
	{
		delete shader;
	}

	m_loadedShaders.clear();
	m_currentShader = nullptr;


	//Release Blend Modes
	for (int i = 0; i < (int)BlendMode::COUNT; ++i)
	{
		DX_SAFE_RELEASE(m_blendStates[i]);
	}
	m_blendState = nullptr;

	//Release Sampler States
	for (int i = 0; i < (int)SamplerMode::COUNT; ++i)
	{
		DX_SAFE_RELEASE(m_samplerStates[i]);
	}
	m_samplerState = nullptr;

	//Release Rasterizer States
	for (int i = 0; i < (int)RasterizerMode::COUNT; ++i)
	{
		DX_SAFE_RELEASE(m_rasterizerStates[i]);
	}
	m_rasterizerState = nullptr;

	//Release Depth Stencil States & misc
	for (int i = 0; i < (int)DepthMode::COUNT; ++i)
	{
		DX_SAFE_RELEASE(m_depthStencilStates[i]);
	}
	m_depthStencilState = nullptr;

	DX_SAFE_RELEASE(m_depthStencilDSV);
	DX_SAFE_RELEASE(m_depthStencilTexture);

	//Texture Stuff
	for (Texture* tex : m_loadedTextures)
	{
		delete tex;
	}
	m_loadedTextures.clear();

	//Delete immediateVBO
	delete m_immediateVBO;
	m_immediateVBO = nullptr;

	//Delete immediateTBNVBO
	delete m_immediateTBNVBO;
	m_immediateTBNVBO = nullptr;

	//Delete immediateIBO
	delete m_immediateIBO;
	m_immediateIBO = nullptr;

	// Delete cameraCBO
	delete m_cameraCBO;
	m_cameraCBO = nullptr;

	// Delete lightCBO
	delete m_lightCBO;
	m_lightCBO = nullptr;

	// Delete modelCBO
	delete m_modelCBO;
	m_modelCBO = nullptr;

	if (m_deviceContext)
	{
		m_deviceContext->ClearState();
		m_deviceContext->Flush();
	}

	// Release Member Variables
	DX_SAFE_RELEASE(m_renderTargetView);
	DX_SAFE_RELEASE(m_swapChain);
	DX_SAFE_RELEASE(m_deviceContext);
	DX_SAFE_RELEASE(m_device);

	//IDRK how this works, look into it 
	for (auto& pair : m_loadedFontsByName)
	{
		delete pair.second;
	}
	m_loadedFontsByName.clear();

	// Report error leaks and release debug module
#if defined(ENGINE_DEBUG_RENDER)
	((IDXGIDebug*)m_dxgiDebug)->ReportLiveObjects(
		DXGI_DEBUG_ALL,
		(DXGI_DEBUG_RLO_FLAGS)(
			DXGI_DEBUG_RLO_DETAIL |
			DXGI_DEBUG_RLO_IGNORE_INTERNAL
			)
	);

	((IDXGIDebug*)m_dxgiDebug)->Release();
	m_dxgiDebug = nullptr;

	::FreeLibrary((HMODULE)m_dxgiDebugModule);
	m_dxgiDebugModule = nullptr;
#endif
}

void Renderer::BeginFrame()
{
	// Set render target
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilDSV);
}

void Renderer::EndFrame()
{
	// Present 
	HRESULT hr;
	hr = m_swapChain->Present(0, 0);
	if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
	{
		ERROR_AND_DIE("Device has been lost, application will now terminate.");
	}
}

void Renderer::ClearScreen(Rgba8 const& clearColor)
{
	float colorAsFloats[4];
	clearColor.GetAsFloats(colorAsFloats);
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, colorAsFloats);
	m_deviceContext->ClearDepthStencilView(m_depthStencilDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void Renderer::BeginCamera(Camera const& camera)
{
	BeginCamera(camera, AABB2::ZERO_TO_ONE);
}

void Renderer::BeginCamera(Camera const& camera, AABB2 const& normalizedViewport)
{
	IntVec2 clientDims = g_engine->m_window->GetClientDimensions();

	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = normalizedViewport.m_mins.x * (float)clientDims.x;
	viewport.TopLeftY = (1.f - normalizedViewport.m_maxs.y) * (float)clientDims.y;
	viewport.Width = normalizedViewport.GetDimensions().x * (float)clientDims.x;
	viewport.Height = normalizedViewport.GetDimensions().y * (float)clientDims.y;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	m_deviceContext->RSSetViewports(1, &viewport);

	CameraConstants camConsts = {};
	camConsts.WorldToCameraTransform = camera.GetWorldToCameraTransform();
	camConsts.CameraToRenderTransform = camera.GetCameraToRenderTransform();
	camConsts.RenderToClipTransform = camera.GetRenderToClipTransform();

	CopyCPUToGPU(&camConsts, (unsigned int)sizeof(CameraConstants), m_cameraCBO);
	BindConstantBuffer(k_cameraConstantsSlot, m_cameraCBO);

	SetModelConstants();
}

void Renderer::EndCamera([[maybe_unused]]Camera const& camera)
{ 
}

void Renderer::DrawVertexArray(int numVertexes, Vertex const* vertexes)
{
	CopyCPUToGPU(vertexes, (unsigned int)(numVertexes * sizeof(Vertex)), m_immediateVBO);
	DrawVertexBuffer(m_immediateVBO, numVertexes);
}

void Renderer::DrawVertexArray(std::vector<Vertex> const& verts)
{
	DrawVertexArray(static_cast<int>(verts.size()), verts.data());
}

void Renderer::DrawVertexArray(int numVertexes, Vertex_TBN const* vertexes)
{
	CopyCPUToGPU(
		vertexes,
		(unsigned int)(numVertexes * sizeof(Vertex_TBN)),
		m_immediateTBNVBO
	);

	DrawVertexBuffer(m_immediateTBNVBO, numVertexes);
}

void Renderer::DrawVertexArray(std::vector<Vertex_TBN> const& verts)
{
	DrawVertexArray((int)verts.size(), verts.data());
}

void Renderer::DrawIndexedVertexArray(std::vector<Vertex> const& verts, std::vector<unsigned int> const& indexes)
{
	int numVertexes = (int)verts.size();
	int numIndexes = (int)indexes.size();

	CopyCPUToGPU(verts.data(), (unsigned int)(numVertexes * sizeof(Vertex)), m_immediateVBO);
	CopyCPUToGPU(indexes.data(), (unsigned int)(numIndexes * sizeof(unsigned int)), m_immediateIBO);

	DrawIndexedVertexBuffer(m_immediateVBO, m_immediateIBO, numIndexes);
}

void Renderer::DrawIndexedVertexArray(std::vector<Vertex_TBN> const& verts, std::vector<unsigned int> const& indexes)
{
	int numVertexes = (int)verts.size();
	int numIndexes = (int)indexes.size();

	CopyCPUToGPU(
		verts.data(),
		(unsigned int)(numVertexes * sizeof(Vertex_TBN)),
		m_immediateTBNVBO
	);

	CopyCPUToGPU(
		indexes.data(),
		(unsigned int)(numIndexes * sizeof(unsigned int)),
		m_immediateIBO
	);

	DrawIndexedVertexBuffer(m_immediateTBNVBO, m_immediateIBO, numIndexes);
}

void Renderer::DrawVertexBuffer(VertexBuffer* vbo, unsigned int vertexCount)
{
	BindVertexBuffer(vbo);
	SetStatesIfChanged();
	m_deviceContext->Draw(vertexCount, 0);
}

void Renderer::DrawIndexedVertexBuffer(VertexBuffer* vbo, IndexBuffer* ibo, unsigned int indexCount)
{
	BindVertexBuffer(vbo);
	BindIndexBuffer(ibo);
	SetStatesIfChanged();
	m_deviceContext->DrawIndexed(indexCount, 0, 0);
}

void Renderer::BindTexture(Texture* texture)
{
	Texture* textureToBind = texture ? texture : m_defaultTexture;
	GUARANTEE_OR_DIE(textureToBind != nullptr, "Renderer::BindTexture - default texture is null!");

	ID3D11ShaderResourceView* srv = textureToBind->m_shaderResourceView;
	GUARANTEE_OR_DIE(srv != nullptr, "Renderer::BindTexture - texture has null SRV!");

	m_deviceContext->PSSetShaderResources(0, 1, &srv);
}

void Renderer::CreateDepthStencilStates()
{
	HRESULT hr;

	D3D11_DEPTH_STENCIL_DESC dsDesc = {};

	// DISABLED
	hr = m_device->CreateDepthStencilState(&dsDesc, &m_depthStencilStates[(int)DepthMode::DISABLED]);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("CreateDepthStencilState for DepthMode::DISABLED failed.");
	}

	// Enable depth for the rest
	dsDesc.DepthEnable = TRUE;

	// READ_ONLY_ALWAYS
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	hr = m_device->CreateDepthStencilState(&dsDesc, &m_depthStencilStates[(int)DepthMode::READ_ONLY_ALWAYS]);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("CreateDepthStencilState for DepthMode::READ_ONLY_ALWAYS failed.");
	}

	// READ_ONLY_LESS_EQUAL
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	hr = m_device->CreateDepthStencilState(&dsDesc, &m_depthStencilStates[(int)DepthMode::READ_ONLY_LESS_EQUAL]);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("CreateDepthStencilState for DepthMode::READ_ONLY_LESS_EQUAL failed.");
	}

	// READ_WRITE_LESS_EQUAL
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	hr = m_device->CreateDepthStencilState(&dsDesc, &m_depthStencilStates[(int)DepthMode::READ_WRITE_LESS_EQUAL]);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("CreateDepthStencilState for DepthMode::READ_WRITE_LESS_EQUAL failed.");
	}
}

Texture* Renderer::CreateOrGetTextureFromFile(char const* imageFilePath)
{
	for (Texture* tex : m_loadedTextures)
	{
		if (tex && tex->m_name == imageFilePath)
			return tex;
	}

	Texture* created = CreateTextureFromFile(imageFilePath);

	m_loadedTextures.push_back(created);
	return created;
}

// 	// See if we already have this texture previously loaded
// 	Texture* existingTexture = GetTextureForFileName(imageFilePath); // You need to write this
// 	if (existingTexture)
// 	{
// 		return existingTexture;
// 	}
// 
// 	Texture* newTexture = CreateTextureFromFile(imageFilePath);
// 	return newTexture;
// }
// 
Texture* Renderer::GetTextureForFileName(char const* imageFilePath)
{
	for (Texture* tex : m_loadedTextures)
	{
		if (tex && tex->m_name == imageFilePath)
			return tex;
	}
	return nullptr;
}
// 
// Texture* Renderer::CreateTextureFromData(char const* name, IntVec2 dimensions, int bytesPerTexel, uint8_t* texelData)
// {
// 	// Check if the load was successful
// 	GUARANTEE_OR_DIE(texelData, Stringf("CreateTextureFromData failed for \"%s\" - texelData was null!", name));
// 	GUARANTEE_OR_DIE(bytesPerTexel >= 3 && bytesPerTexel <= 4, Stringf("CreateTextureFromData failed for \"%s\" - unsupported BPP=%i (must be 3 or 4)", name, bytesPerTexel));
// 	GUARANTEE_OR_DIE(dimensions.x > 0 && dimensions.y > 0, Stringf("CreateTextureFromData failed for \"%s\" - illegal texture dimensions (%i x %i)", name, dimensions.x, dimensions.y));
// 
// 	Texture* newTexture = new Texture();
// 	newTexture->m_name = name; // NOTE: m_name must be a std::string, otherwise it may point to temporary data!
// 	newTexture->m_dimensions = dimensions;
// 
// 	// Enable OpenGL texturing
// 	glEnable(GL_TEXTURE_2D);
// 
// 	// Tell OpenGL that our pixel data is single-byte aligned
// 	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
// 
// 	// Ask OpenGL for an unused texName (ID number) to use for this texture
// 	glGenTextures(1, (GLuint*)&newTexture->m_textureID);
// 
// 	// Tell OpenGL to bind (set) this as the currently active texture
// 	glBindTexture(GL_TEXTURE_2D, newTexture->m_textureID);
// 
// 	// Set texture clamp vs. wrap (repeat) default settings
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // GL_CLAMP or GL_REPEAT
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // GL_CLAMP or GL_REPEAT
// 
// 	// Set magnification (texel > pixel) and minification (texel < pixel) filters
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // one of: GL_NEAREST, GL_LINEAR
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // one of: GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_LINEAR
// 
// 	// Pick the appropriate OpenGL format (RGB or RGBA) for this texel data
// 	GLenum bufferFormat = GL_RGBA; // the format our source pixel data is in; any of: GL_RGB, GL_RGBA, GL_LUMINANCE, GL_LUMINANCE_ALPHA, ...
// 	if (bytesPerTexel == 3)
// 	{
// 		bufferFormat = GL_RGB;
// 	}
// 	GLenum internalFormat = bufferFormat; // the format we want the texture to be on the card; technically allows us to translate into a different texture format as we upload to OpenGL
// 
// 	// Upload the image texel data (raw pixels bytes) to OpenGL under the currently-bound OpenGL texture ID
// 	glTexImage2D(			// Upload this pixel data to our new OpenGL texture
// 		GL_TEXTURE_2D,		// Creating this as a 2d texture
// 		0,					// Which mipmap level to use as the "root" (0 = the highest-quality, full-res image), if mipmaps are enabled
// 		internalFormat,		// Type of texel format we want OpenGL to use for this texture internally on the video card
// 		dimensions.x,		// Texel-width of image; for maximum compatibility, use 2^N + 2^B, where N is some integer in the range [3,11], and B is the border thickness [0,1]
// 		dimensions.y,		// Texel-height of image; for maximum compatibility, use 2^M + 2^B, where M is some integer in the range [3,11], and B is the border thickness [0,1]
// 		0,					// Border size, in texels (must be 0 or 1, recommend 0)
// 		bufferFormat,		// Pixel format describing the composition of the pixel data in buffer
// 		GL_UNSIGNED_BYTE,	// Pixel color components are unsigned bytes (one byte per color channel/component)
// 		texelData);		// Address of the actual pixel data bytes/buffer in system memory
// 
// 	m_loadedTextures.push_back(newTexture);
// 	return newTexture;
// }

BitmapFont* Renderer::CreateOrGetBitmapFont(char const* bitmapFontFilePathWithNoExtension)
{
	std::string name(bitmapFontFilePathWithNoExtension);

	auto found = m_loadedFontsByName.find(name);
	if (found != m_loadedFontsByName.end())
		return found->second;

	BitmapFont* font = CreateBitmapFont(bitmapFontFilePathWithNoExtension);
	m_loadedFontsByName[name] = font;
	return font;
}

BitmapFont* Renderer::CreateBitmapFont(char const* bitmapFontFilePathWithNoExtension)
{
	std::string pngPath = std::string(bitmapFontFilePathWithNoExtension) + ".png";

	Texture* texture = CreateOrGetTextureFromFile(pngPath.c_str());

	BitmapFont* font = new BitmapFont(bitmapFontFilePathWithNoExtension, *texture);
	return font;
}

Texture* Renderer::CreateTextureFromFile(char const* imageFilePath)
{
	Image image(imageFilePath);
	Texture* tex = CreateTextureFromImage(image);

	return tex;
}

Texture* Renderer::CreateTextureFromImage(Image const& image)
{
	Texture* newTexture = new Texture();
	newTexture->m_name = image.GetImageFilePath();
	newTexture->m_dimensions = image.GetDimensions();

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = (UINT)image.GetDimensions().x;
	textureDesc.Height = (UINT)image.GetDimensions().y;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA textureData = {};
	textureData.pSysMem = image.GetRawData();
	textureData.SysMemPitch = 4 * image.GetDimensions().x; // 4 bytes per texel (RGBA8)

	HRESULT hr = m_device->CreateTexture2D(&textureDesc, &textureData, &newTexture->m_texture);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE(Stringf("CreateTextureFromImage failed for image file \"%s\".",
			image.GetImageFilePath().c_str()));
	}

	hr = m_device->CreateShaderResourceView(newTexture->m_texture, nullptr, &newTexture->m_shaderResourceView);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE(Stringf("CreateShaderResourceView failed for image file \"%s\".",
			image.GetImageFilePath().c_str()));
	}

	/*m_loadedTextures.push_back(newTexture);*/

	return newTexture;
}

void Renderer::SetBlendMode([[maybe_unused]]BlendMode blendMode)
{
	m_desiredBlendMode = blendMode;
}

void Renderer::SetStatesIfChanged()
{
	//Blend State
	ID3D11BlendState* desiredBlend = m_blendStates[(int)m_desiredBlendMode];
	if (desiredBlend != m_blendState)
	{
		m_blendState = desiredBlend;

		float blendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
		UINT sampleMask = 0xffffffff;
		m_deviceContext->OMSetBlendState(m_blendState, blendFactor, sampleMask);
	}

	//Sampler State
	ID3D11SamplerState* desiredSampler = m_samplerStates[(int)m_desiredSamplerMode];
	if (desiredSampler != m_samplerState)
	{
		m_samplerState = desiredSampler;
		m_deviceContext->PSSetSamplers(0, 1, &m_samplerState);
	}

	//Rasterizer State
	ID3D11RasterizerState* desiredRasterizer = m_rasterizerStates[(int)m_desiredRasterizerMode];
	if (desiredRasterizer != m_rasterizerState)
	{
		m_rasterizerState = desiredRasterizer;
		m_deviceContext->RSSetState(m_rasterizerState);
	}

	// Depth stencil state
	ID3D11DepthStencilState* desiredDS = m_depthStencilStates[(int)m_desiredDepthMode];
	if (desiredDS != m_depthStencilState)
	{
		m_depthStencilState = desiredDS;
		m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 0);
	}
}

void Renderer::SetModelConstants(Mat44 const& modelToWorldTransform /*= Mat44()*/, Rgba8 const& modelColor /*= Rgba8(255,255,255)*/)
{
	ModelConstants modelConsts = {};
	modelConsts.ModelToWorldTransform = modelToWorldTransform;

	float colorAsFloats[4];
	modelColor.GetAsFloats(colorAsFloats);
	modelConsts.ModelColor[0] = colorAsFloats[0];
	modelConsts.ModelColor[1] = colorAsFloats[1];
	modelConsts.ModelColor[2] = colorAsFloats[2];
	modelConsts.ModelColor[3] = colorAsFloats[3];

	CopyCPUToGPU(&modelConsts, (unsigned int)sizeof(ModelConstants), m_modelCBO);
	BindConstantBuffer(k_modelConstantsSlot, m_modelCBO);
}

void Renderer::SetLightConstants(Vec3 const& sunDirection, float sunIntensity, float ambientIntensity)
{
	LightConstants lightConsts = {};
	lightConsts.SunDirection = -sunDirection;
	lightConsts.SunIntensity = sunIntensity;
	lightConsts.AmbientIntensity = ambientIntensity;

	CopyCPUToGPU(&lightConsts, (unsigned int)sizeof(LightConstants), m_lightCBO);
	BindConstantBuffer(k_lightConstantsSlot, m_lightCBO);
}

void Renderer::SetSamplerMode(SamplerMode samplerMode)
{
	m_desiredSamplerMode = samplerMode;
}

void Renderer::SetRasterizerMode(RasterizerMode rasterizerMode)
{
	m_desiredRasterizerMode = rasterizerMode;
}

void Renderer::SetDepthMode(DepthMode depthMode)
{
	m_desiredDepthMode = depthMode;
}

void Renderer::CreateRasterizerStates()
{
	// SOLID_CULL_NONE
	{
		D3D11_RASTERIZER_DESC desc = {};
		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_NONE;
		desc.FrontCounterClockwise = true;
		desc.DepthBias = 0;
		desc.DepthBiasClamp = 0.0f;
		desc.SlopeScaledDepthBias = 0.0f;
		desc.DepthClipEnable = true;
		desc.ScissorEnable = false;
		desc.MultisampleEnable = false;
		desc.AntialiasedLineEnable = true;

		HRESULT hr = m_device->CreateRasterizerState(&desc, &m_rasterizerStates[(int)RasterizerMode::SOLID_CULL_NONE]);
		if (!SUCCEEDED(hr))
		{
			ERROR_AND_DIE("CreateRasterizerState failed for SOLID_CULL_NONE.");
		}
	}

	// SOLID_CULL_BACK
	{
		D3D11_RASTERIZER_DESC desc = {};
		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_BACK;
		desc.FrontCounterClockwise = false;
		desc.DepthBias = 0;
		desc.DepthBiasClamp = 0.0f;
		desc.SlopeScaledDepthBias = 0.0f;
		desc.DepthClipEnable = true;
		desc.ScissorEnable = false;
		desc.MultisampleEnable = false;
		desc.AntialiasedLineEnable = true;

		HRESULT hr = m_device->CreateRasterizerState(&desc, &m_rasterizerStates[(int)RasterizerMode::SOLID_CULL_BACK]);
		if (!SUCCEEDED(hr))
		{
			ERROR_AND_DIE("CreateRasterizerState failed for SOLID_CULL_BACK.");
		}
	}

	// WIREFRAME_CULL_NONE
	{
		D3D11_RASTERIZER_DESC desc = {};
		desc.FillMode = D3D11_FILL_WIREFRAME;
		desc.CullMode = D3D11_CULL_NONE;
		desc.FrontCounterClockwise = false;
		desc.DepthBias = 0;
		desc.DepthBiasClamp = 0.0f;
		desc.SlopeScaledDepthBias = 0.0f;
		desc.DepthClipEnable = true;
		desc.ScissorEnable = false;
		desc.MultisampleEnable = false;
		desc.AntialiasedLineEnable = true;

		HRESULT hr = m_device->CreateRasterizerState(&desc, &m_rasterizerStates[(int)RasterizerMode::WIREFRAME_CULL_NONE]);
		if (!SUCCEEDED(hr))
		{
			ERROR_AND_DIE("CreateRasterizerState failed for WIREFRAME_CULL_NONE.");
		}
	}

	// WIREFRAME_CULL_BACK
	{
		D3D11_RASTERIZER_DESC desc = {};
		desc.FillMode = D3D11_FILL_WIREFRAME;
		desc.CullMode = D3D11_CULL_BACK;
		desc.FrontCounterClockwise = false;
		desc.DepthBias = 0;
		desc.DepthBiasClamp = 0.0f;
		desc.SlopeScaledDepthBias = 0.0f;
		desc.DepthClipEnable = true;
		desc.ScissorEnable = false;
		desc.MultisampleEnable = false;
		desc.AntialiasedLineEnable = true;

		HRESULT hr = m_device->CreateRasterizerState(&desc, &m_rasterizerStates[(int)RasterizerMode::WIREFRAME_CULL_BACK]);
		if (!SUCCEEDED(hr))
		{
			ERROR_AND_DIE("CreateRasterizerState failed for WIREFRAME_CULL_BACK.");
		}
	}

	// Initialize current rasterizer state
	m_rasterizerState = m_rasterizerStates[(int)m_desiredRasterizerMode];
	m_deviceContext->RSSetState(m_rasterizerState);
}

Shader* Renderer::CreateShader(char const* shaderName, char const* shaderSource)
{
	ShaderConfig config;
	config.m_name = shaderName;
	config.m_vertexEntryPoint = "VertexMain";
	config.m_pixelEntryPoint = "PixelMain";

	Shader* shader = new Shader(config);

	// Compile Vertex Shader
	std::vector<uint8_t> vertexShaderBytes;
	CompileShaderToByteCode(
		vertexShaderBytes,
		shaderName,
		shaderSource,
		config.m_vertexEntryPoint.c_str(),
		"vs_5_0"
	);

	// Create vertex shader
	HRESULT hr = m_device->CreateVertexShader(
		vertexShaderBytes.data(),
		vertexShaderBytes.size(),
		nullptr,
		&shader->m_vertexShader
	);

	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE(Stringf("Could not create vertex shader for '%s'.", shaderName));
	}

	// Compile Pixel Shader
	std::vector<uint8_t> pixelShaderBytes;
	CompileShaderToByteCode(
		pixelShaderBytes,
		shaderName,
		shaderSource,
		config.m_pixelEntryPoint.c_str(),
		"ps_5_0"
	);

	// Create pixel shader
	hr = m_device->CreatePixelShader(
		pixelShaderBytes.data(),
		pixelShaderBytes.size(),
		nullptr,
		&shader->m_pixelShader
	);

	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE(Stringf("Could not create pixel shader for '%s'.", shaderName));
	}

	// Create input layout for Vertex_TBN
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{ "POSITION",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,                            D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",      0, DXGI_FORMAT_R8G8B8A8_UNORM,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",   0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BITANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",     0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT numElements = ARRAYSIZE(inputElementDesc);

	hr = m_device->CreateInputLayout(
		inputElementDesc,
		numElements,
		vertexShaderBytes.data(),
		vertexShaderBytes.size(),
		&shader->m_inputLayout
	);

	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE(Stringf("Could not create vertex layout for '%s'.", shaderName));
	}

	return shader;
}

Shader* Renderer::CreateShader(char const* shaderName)
{
	std::string filename = std::string(shaderName) + ".hlsl";

	std::string shaderSource;
	int bytesRead = FileReadToString(shaderSource, filename);
	if (bytesRead < 0)
	{
		ERROR_AND_DIE(Stringf("Failed to read shader file '%s'", filename.c_str()));
	}

	return CreateShader(shaderName, shaderSource.c_str());
}

bool Renderer::CompileShaderToByteCode(std::vector<unsigned char>& outByteCode, char const* name, char const* source, char const* entryPoint, char const* target)
{
	// Compile vertex shader
	DWORD shaderFlags = D3DCOMPILE_OPTIMIZATION_LEVEL3;

#if defined(ENGINE_DEBUG_RENDER)
	shaderFlags = D3DCOMPILE_DEBUG;
	shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
	shaderFlags |= D3DCOMPILE_WARNINGS_ARE_ERRORS;
#endif

	ID3DBlob* shaderBlob = NULL;
	ID3DBlob* errorBlob = NULL;

	HRESULT hr = D3DCompile(
		source, strlen(source),
		name, nullptr, nullptr,
		entryPoint, target, shaderFlags,
		0, &shaderBlob, &errorBlob);

	if (SUCCEEDED(hr))
	{
		outByteCode.resize(shaderBlob->GetBufferSize());
		memcpy(
			outByteCode.data(),
			shaderBlob->GetBufferPointer(),
			shaderBlob->GetBufferSize());
	}
	else
	{
		if (errorBlob != NULL)
		{
			DebuggerPrintf((char*)errorBlob->GetBufferPointer());
		}
		ERROR_AND_DIE(Stringf("Could not compile vertex shader."));
	}

	shaderBlob->Release();

	if (errorBlob != NULL)
	{
		errorBlob->Release();
	}

	return true;
}

void Renderer::BindShader(Shader* shader)
{
	if (shader == nullptr)
	{
		shader = m_defaultShader;
	}

	GUARANTEE_OR_DIE(shader != nullptr, "Renderer::BindShader failed: default shader is null!");

	m_currentShader = shader;

	m_deviceContext->IASetInputLayout(shader->m_inputLayout);
	m_deviceContext->VSSetShader(shader->m_vertexShader, nullptr, 0);
	m_deviceContext->PSSetShader(shader->m_pixelShader, nullptr, 0);
}

VertexBuffer* Renderer::CreateVertexBuffer(const unsigned int size, unsigned int stride)
{
	return new VertexBuffer(m_device, size, stride);
}

IndexBuffer* Renderer::CreateIndexBuffer(const unsigned int size, unsigned int stride)
{
	return new IndexBuffer(m_device, size, stride);
}

void Renderer::CopyCPUToGPU(const void* data, unsigned int size, VertexBuffer* vbo)
{
	if (vbo->m_size < size)
	{
		vbo->Resize(size);
	}

	D3D11_MAPPED_SUBRESOURCE mapped = {};
	HRESULT hr = m_deviceContext->Map(vbo->m_buffer, 
	0, 
	D3D11_MAP_WRITE_DISCARD, 
	0, 
	&mapped);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Renderer::CopyCPUToGPU failed to Map vertex buffer.");
	}

	memcpy(mapped.pData, data, size);

	m_deviceContext->Unmap(vbo->m_buffer, 0);
}

void Renderer::CopyCPUToGPU(const void* data, unsigned int size, IndexBuffer* ibo)
{
	if (ibo->m_size < size)
	{
		ibo->Resize(size);
	}

	D3D11_MAPPED_SUBRESOURCE mapped = {};
	HRESULT hr = m_deviceContext->Map(
		ibo->m_buffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mapped);

	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Renderer::CopyCPUToGPU failed to Map index buffer.");
	}

	memcpy(mapped.pData, data, size);

	m_deviceContext->Unmap(ibo->m_buffer, 0);
}

void Renderer::BindVertexBuffer(VertexBuffer* vbo)
{
	UINT stride = vbo->m_stride;
	UINT offset = 0;

	ID3D11Buffer* buffer = vbo->m_buffer;
	m_deviceContext->IASetVertexBuffers(0, 1, &buffer, &stride, &offset);
	m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Renderer::BindIndexBuffer(IndexBuffer* ibo)
{
	ID3D11Buffer* buffer = ibo->m_buffer;
	m_deviceContext->IASetIndexBuffer(buffer, DXGI_FORMAT_R32_UINT, 0);
}

ConstantBuffer* Renderer::CreateConstantBuffer(const unsigned int size)
{
	GUARANTEE_OR_DIE(size > 0, "CreateConstantBuffer: size must be > 0");

	ConstantBuffer* cbo = new ConstantBuffer(size);

	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = AlignTo16(size);
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	HRESULT hr = m_device->CreateBuffer(&bufferDesc, nullptr, &cbo->m_buffer);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Renderer::CreateConstantBuffer failed to CreateBuffer.");
	}

	return cbo;
}

void Renderer::CopyCPUToGPU(const void* data, unsigned int size, ConstantBuffer* cbo)
{
	GUARANTEE_OR_DIE(cbo != nullptr, "Renderer::CopyCPUToGPU(ConstantBuffer): cbo was null");
	GUARANTEE_OR_DIE(data != nullptr, "Renderer::CopyCPUToGPU(ConstantBuffer): data was null");
	GUARANTEE_OR_DIE(size <= cbo->m_size, "Renderer::CopyCPUToGPU(ConstantBuffer): size exceeds ConstantBuffer capacity");

	D3D11_MAPPED_SUBRESOURCE mapped = {};
	HRESULT hr = m_deviceContext->Map(
		cbo->m_buffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mapped
	);

	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Renderer::CopyCPUToGPU(ConstantBuffer) failed to Map.");
	}

	memcpy(mapped.pData, data, size);

	m_deviceContext->Unmap(cbo->m_buffer, 0);
}

void Renderer::BindConstantBuffer(int slot, ConstantBuffer* cbo)
{
	GUARANTEE_OR_DIE(slot >= 0, "Renderer::BindConstantBuffer: slot must be >= 0");

	ID3D11Buffer* buffer = (cbo != nullptr) ? cbo->m_buffer : nullptr;

	m_deviceContext->VSSetConstantBuffers(slot, 1, &buffer);
	m_deviceContext->PSSetConstantBuffers(slot, 1, &buffer);
}

unsigned int Renderer::AlignTo16(unsigned int byteCount)
{
	return (byteCount + 15u) & ~15u;
}