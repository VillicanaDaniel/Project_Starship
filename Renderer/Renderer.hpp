#pragma once
#include "Game/EngineBuildPreferences.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/AABB2.hpp"
#include <string>
#include <vector>
#include <map>

#define DX_SAFE_RELEASE(dxObject)	\
{									\
	if ((dxObject) != nullptr)		\
	{								\
		(dxObject)->Release();		\
		(dxObject) = nullptr;		\
	}								\
}									\

struct Rgba8;
struct Vertex;
struct Vertex_TBN;
struct IntVec2;
class Camera;
class Texture;
class BitmapFont;
class Shader;
class ConstantBuffer;
class Image;

struct ID3D11RasterizerState;
struct ID3D11RenderTargetView;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11BlendState;
struct ID3D11SamplerState;

struct ID3D11Texture2D;
struct ID3D11DepthStencilView;
struct ID3D11DepthStencilState;

#ifdef OPAQUE
#undef OPAQUE
#endif

enum class BlendMode
{
	OPAQUE,
	ALPHA,
	ADDITIVE,
	COUNT
};

enum class SamplerMode
{
	POINT_CLAMP,
	BILINEAR_WRAP,
	MIRROR,
	COUNT
};

enum class RasterizerMode
{
	SOLID_CULL_NONE,
	SOLID_CULL_BACK,
	WIREFRAME_CULL_NONE,
	WIREFRAME_CULL_BACK,
	COUNT
};

enum class DepthMode
{
	DISABLED,
	READ_ONLY_ALWAYS,
	READ_ONLY_LESS_EQUAL,
	READ_WRITE_LESS_EQUAL,
	COUNT
};

struct RendererConfig
{
	bool	m_isEnabled = true;
};


class Renderer
{
public:
	Renderer(RendererConfig const& config);
	~Renderer() = default;

	void Startup();
	void Shutdown();

	void BeginFrame();
	void EndFrame();

	void ClearScreen(Rgba8 const& clearColor);

	void BeginCamera(Camera const& camera);
	void BeginCamera(Camera const& camera, AABB2 const& normalizedViewport);
	void EndCamera(Camera const& camera);

	void DrawVertexArray(int numVertexes, Vertex const* vertexes);
	void DrawVertexArray(std::vector<Vertex> const& verts);
	void DrawIndexedVertexArray(std::vector<Vertex> const& verts, std::vector<unsigned int> const& indexes);

	void DrawVertexArray(int numVertexes, Vertex_TBN const* vertexes);
	void DrawVertexArray(std::vector<Vertex_TBN> const& verts);
	void DrawIndexedVertexArray(std::vector<Vertex_TBN> const& verts, std::vector<unsigned int> const& indexes);

	void DrawVertexBuffer(VertexBuffer* vbo, unsigned int vertexCount);
	void DrawIndexedVertexBuffer(VertexBuffer* vbo, IndexBuffer* ibo, unsigned int indexCount);

	void BindTexture(Texture* texture);
	void BindShader(Shader* shader);
	void BindVertexBuffer(VertexBuffer* vbo);
	void BindIndexBuffer(IndexBuffer* ibo);
	void BindConstantBuffer(int slot, ConstantBuffer* cbo);

	Texture* CreateOrGetTextureFromFile(char const* imageFilePath);
	Texture* GetTextureForFileName(char const* imageFilePath);
	Texture* CreateTextureFromFile(char const* imageFilePath);
	Texture* CreateTextureFromImage(Image const& image);

	BitmapFont* CreateOrGetBitmapFont(char const* bitmapFontFilePathWithNoExtension);
	BitmapFont* CreateBitmapFont(char const* bitmapFontFilePathWithNoExtension);

	Shader* CreateShader(char const* shaderName, char const* shaderSource);
	Shader* CreateShader(char const* shaderName);

	bool CompileShaderToByteCode(std::vector<unsigned char>& outByteCode, char const* name, char const* source, char const* entryPoint, char const* target);

	VertexBuffer* CreateVertexBuffer(unsigned int size, unsigned int stride);
	IndexBuffer* CreateIndexBuffer(unsigned int size, unsigned int stride);
	ConstantBuffer* CreateConstantBuffer(unsigned int size);

	void CopyCPUToGPU(void const* data, unsigned int size, VertexBuffer* vbo);
	void CopyCPUToGPU(void const* data, unsigned int size, IndexBuffer* ibo);
	void CopyCPUToGPU(void const* data, unsigned int size, ConstantBuffer* cbo);

	void SetBlendMode(BlendMode blendMode);
	void SetSamplerMode(SamplerMode samplerMode);
	void SetRasterizerMode(RasterizerMode rasterizerMode);
	void SetDepthMode(DepthMode depthMode);

	void SetModelConstants(Mat44 const& modelToWorldTransform = Mat44(), Rgba8 const& modelColor = Rgba8(255, 255, 255, 255));
	void SetLightConstants(Vec3 const& sunDirection, float sunIntensity, float ambientIntensity);

	void SetStatesIfChanged();
	void CreateRasterizerStates();
	void CreateDepthStencilStates();

	unsigned int AlignTo16(unsigned int byteCount);

public:
	RendererConfig m_config;

	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_deviceContext = nullptr;
	IDXGISwapChain* m_swapChain = nullptr;
	ID3D11RenderTargetView* m_renderTargetView = nullptr;

	ID3D11Texture2D* m_depthStencilTexture = nullptr;
	ID3D11DepthStencilView* m_depthStencilDSV = nullptr;

	ID3D11BlendState* m_blendStates[(int)BlendMode::COUNT] = {};
	ID3D11SamplerState* m_samplerStates[(int)SamplerMode::COUNT] = {};
	ID3D11RasterizerState* m_rasterizerStates[(int)RasterizerMode::COUNT] = {};
	ID3D11DepthStencilState* m_depthStencilStates[(int)DepthMode::COUNT] = {};

	ID3D11BlendState* m_blendState = nullptr;
	ID3D11SamplerState* m_samplerState = nullptr;
	ID3D11RasterizerState* m_rasterizerState = nullptr;
	ID3D11DepthStencilState* m_depthStencilState = nullptr;

	BlendMode m_desiredBlendMode = BlendMode::ALPHA;
	SamplerMode m_desiredSamplerMode = SamplerMode::POINT_CLAMP;
	RasterizerMode m_desiredRasterizerMode = RasterizerMode::SOLID_CULL_BACK;
	DepthMode m_desiredDepthMode = DepthMode::READ_WRITE_LESS_EQUAL;

	Texture* m_defaultTexture = nullptr;
	std::vector<Texture*> m_loadedTextures;

	Shader* m_defaultShader = nullptr;
	Shader* m_currentShader = nullptr;
	std::vector<Shader*> m_loadedShaders;

	std::map<std::string, BitmapFont*> m_loadedFontsByName;

	VertexBuffer* m_immediateVBO = nullptr;
	VertexBuffer* m_immediateTBNVBO = nullptr;
	IndexBuffer* m_immediateIBO = nullptr;

	ConstantBuffer* m_cameraCBO = nullptr;
	ConstantBuffer* m_lightCBO = nullptr;
	ConstantBuffer* m_modelCBO = nullptr;
};