#include "Engine/Core/DebugRenderSystem.hpp"
#include "Engine/Core/Engine.hpp"

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Texture.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Time.hpp"

#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"

#include <vector>
#include <string>
#include <algorithm>


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Helpers
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
enum class DebugObjectType
{
	WORLD_GEOMETRY,
	WORLD_TEXT,
	WORLD_BILLBOARD_TEXT,
	SCREEN_TEXT,
	MESSAGE
};

struct DebugRenderObject
{
	DebugObjectType m_type = DebugObjectType::WORLD_GEOMETRY;

	std::vector<Vertex> m_localVerts;
	Texture* m_texture = nullptr;

	Mat44 m_transform;

	std::string m_text;
	Vec3 m_billboardOrigin = Vec3(0.f, 0.f, 0.f);
	AABB2 m_screenTextBox = AABB2(0.f, 0.f, 0.f, 0.f);
	float m_textHeight = 1.f;
	float m_cellHeight = 20.f;
	Vec2 m_alignment = Vec2(0.5f, 0.5f);
	bool m_isWireframe = false;

	Rgba8 m_startColor = Rgba8::WHITE;
	Rgba8 m_endColor = Rgba8::WHITE;
	DebugRenderMode m_mode = DebugRenderMode::USE_DEPTH;

	float m_totalDuration = 0.f;
	float m_remainingDuration = 0.f;

	unsigned int m_insertionOrder = 0;
};

static DebugRenderConfig s_config;
static BitmapFont* s_debugFont = nullptr;

static std::vector<DebugRenderObject*> s_worldObjects;
static std::vector<DebugRenderObject*> s_screenObjects;

static bool s_isVisible = true;
static double s_lastBeginFrameTime = 0.0;
static unsigned int s_nextInsertionOrder = 0;

static DebugRenderObject* CreateDebugObject(
	DebugObjectType type,
	float duration,
	Rgba8 const& startColor,
	Rgba8 const& endColor,
	DebugRenderMode mode)
{
	DebugRenderObject* obj = new DebugRenderObject();
	obj->m_type = type;
	obj->m_totalDuration = duration;
	obj->m_remainingDuration = duration;
	obj->m_startColor = startColor;
	obj->m_endColor = endColor;
	obj->m_mode = mode;
	obj->m_insertionOrder = s_nextInsertionOrder++;
	return obj;
}

static void AddWorldObject(DebugRenderObject* obj)
{
	s_worldObjects.push_back(obj);
}

static void AddScreenObject(DebugRenderObject* obj)
{
	s_screenObjects.push_back(obj);
}

static void DeleteObjectList(std::vector<DebugRenderObject*>& objects)
{
	for (DebugRenderObject* obj : objects)
	{
		delete obj;
	}
	objects.clear();
}

static void TransformVertsByModel(std::vector<Vertex>& verts, Mat44 const& model)
{
	for (Vertex& v : verts)
	{
		v.m_position = model.TransformPosition3D(v.m_position);
	}
}

static void TintVerts(std::vector<Vertex>& verts, Rgba8 const& tint)
{
	for (Vertex& v : verts)
	{
		v.m_color = tint;
	}
}

static Rgba8 InterpolateColor(Rgba8 const& startColor, Rgba8 const& endColor, float t)
{
	t = GetClamped(t, 0.f, 1.f);

	unsigned char r = (unsigned char)Interpolate((float)startColor.r, (float)endColor.r, t);
	unsigned char g = (unsigned char)Interpolate((float)startColor.g, (float)endColor.g, t);
	unsigned char b = (unsigned char)Interpolate((float)startColor.b, (float)endColor.b, t);
	unsigned char a = (unsigned char)Interpolate((float)startColor.a, (float)endColor.a, t);

	return Rgba8(r, g, b, a);
}

static Rgba8 GetCurrentColor(DebugRenderObject const& obj)
{
	if (obj.m_totalDuration < 0.f)
	{
		return obj.m_startColor;
	}

	if (obj.m_totalDuration <= 0.f)
	{
		return obj.m_endColor;
	}

	float elapsed = obj.m_totalDuration - obj.m_remainingDuration;
	float t = elapsed / obj.m_totalDuration;
	return InterpolateColor(obj.m_startColor, obj.m_endColor, t);
}

static Rgba8 GetXRayFirstPassColor(Rgba8 const& currentColor)
{
	Rgba8 result = currentColor;

	result.r = (unsigned char)Interpolate((float)currentColor.r, 255.f, 0.35f);
	result.g = (unsigned char)Interpolate((float)currentColor.g, 255.f, 0.35f);
	result.b = (unsigned char)Interpolate((float)currentColor.b, 255.f, 0.35f);
	result.a = (unsigned char)Interpolate((float)currentColor.a, 96.f, 0.5f);

	return result;
}

static void UpdateObjectList(std::vector<DebugRenderObject*>& objects, float deltaSeconds)
{
	for (int i = (int)objects.size() - 1; i >= 0; --i)
	{
		DebugRenderObject* obj = objects[i];

		// Infinite duration
		if (obj->m_totalDuration < 0.f)
		{
			continue;
		}

		obj->m_remainingDuration -= deltaSeconds;

		if (obj->m_remainingDuration < 0.f)
		{
			delete obj;
			objects.erase(objects.begin() + i);
		}
	}
}

static void DrawVertsWithMode(std::vector<Vertex> const& verts, Texture* texture, DebugRenderMode mode, Rgba8 const& currentColor)
{
	if (verts.empty())
	{
		return;
	}

	std::vector<Vertex> tintedVerts = verts;
	TintVerts(tintedVerts, currentColor);
	s_config.m_renderer->SetModelConstants();

	if (mode == DebugRenderMode::ALWAYS)
	{
		s_config.m_renderer->SetBlendMode(currentColor.a < 255 ? BlendMode::ALPHA : BlendMode::OPAQUE);
		s_config.m_renderer->SetDepthMode(DepthMode::DISABLED);
		s_config.m_renderer->BindTexture(texture);
		s_config.m_renderer->DrawVertexArray((int)tintedVerts.size(), tintedVerts.data());
	}
	else if (mode == DebugRenderMode::USE_DEPTH)
	{
		s_config.m_renderer->SetBlendMode(currentColor.a < 255 ? BlendMode::ALPHA : BlendMode::OPAQUE);
		s_config.m_renderer->SetDepthMode(DepthMode::READ_WRITE_LESS_EQUAL);
		s_config.m_renderer->BindTexture(texture);
		s_config.m_renderer->DrawVertexArray((int)tintedVerts.size(), tintedVerts.data());
	}
	else // X_RAY
	{
		// Pass 1: read only depth, alpha blended, lightened
		Rgba8 firstPassColor = GetXRayFirstPassColor(currentColor);
		std::vector<Vertex> firstPassVerts = verts;
		TintVerts(firstPassVerts, firstPassColor);

		s_config.m_renderer->SetBlendMode(BlendMode::ALPHA);
		s_config.m_renderer->SetDepthMode(DepthMode::READ_ONLY_LESS_EQUAL);
		s_config.m_renderer->BindTexture(texture);
		s_config.m_renderer->DrawVertexArray((int)firstPassVerts.size(), firstPassVerts.data());

		// Pass 2: opaque, read/write depth, current color
		s_config.m_renderer->SetBlendMode(BlendMode::OPAQUE);
		s_config.m_renderer->SetDepthMode(DepthMode::READ_WRITE_LESS_EQUAL);
		s_config.m_renderer->BindTexture(texture);
		s_config.m_renderer->DrawVertexArray((int)tintedVerts.size(), tintedVerts.data());
	}
}

static std::vector<Vertex> BuildWorldTextVerts(DebugRenderObject const& obj)
{
	std::vector<Vertex> verts;
	if (s_debugFont == nullptr)
	{
		return verts;
	}

	s_debugFont->AddVertsForText3DAtOriginXForward(
		verts,
		obj.m_textHeight,
		obj.m_text,
		Rgba8::WHITE,
		1.0f,
		obj.m_alignment,
		999999
	);

	Mat44 transform = obj.m_transform;
	TransformVertsByModel(verts, transform);
	return verts;
}

static std::vector<Vertex> BuildBillboardTextVerts(DebugRenderObject const& obj, Camera const& camera)
{
	std::vector<Vertex> verts;
	if (s_debugFont == nullptr)
	{
		return verts;
	}

	s_debugFont->AddVertsForText3DAtOriginXForward(
		verts,
		obj.m_textHeight,
		obj.m_text,
		Rgba8::WHITE,
		1.0f,
		obj.m_alignment,
		999999
	);

	Mat44 billboardTransform = GetBillboardTransform(
		BillboardType::FULL_OPPOSING,
		camera.GetCameraToWorldTransform(),
		obj.m_billboardOrigin,
		Vec2(1.f, 1.f)
	);

	TransformVertsByModel(verts, billboardTransform);
	return verts;
}

static std::vector<Vertex> BuildScreenTextVerts(DebugRenderObject const& obj)
{
	std::vector<Vertex> verts;
	if (s_debugFont == nullptr)
	{
		return verts;
	}

	s_debugFont->AddVertsForTextInBox2D(
		verts,
		obj.m_text,
		obj.m_screenTextBox,
		obj.m_cellHeight,
		Rgba8::WHITE,
		1.0f,
		obj.m_alignment,
		TextBoxMode::SHRINK_TO_FIT,
		999999
	);

	return verts;
}

static void RenderWorldObject(DebugRenderObject const& obj, Camera const& camera)
{
	Rgba8 currentColor = GetCurrentColor(obj);

/*	RasterizerMode oldMode = RasterizerMode::SOLID_CULL_BACK;*/

	if (obj.m_isWireframe)
	{
		s_config.m_renderer->SetRasterizerMode(RasterizerMode::WIREFRAME_CULL_NONE);
	}
	else
	{
		s_config.m_renderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
	}

	if (obj.m_type == DebugObjectType::WORLD_GEOMETRY)
	{
		std::vector<Vertex> verts = obj.m_localVerts;
		TransformVertsByModel(verts, obj.m_transform);
		DrawVertsWithMode(verts, obj.m_texture, obj.m_mode, currentColor);
	}
	else if (obj.m_type == DebugObjectType::WORLD_TEXT)
	{
		std::vector<Vertex> verts = BuildWorldTextVerts(obj);
		DrawVertsWithMode(verts, &s_debugFont->GetTexture(), obj.m_mode, currentColor);
	}
	else if (obj.m_type == DebugObjectType::WORLD_BILLBOARD_TEXT)
	{
		std::vector<Vertex> verts = BuildBillboardTextVerts(obj, camera);
		DrawVertsWithMode(verts, &s_debugFont->GetTexture(), obj.m_mode, currentColor);
	}

	// Restore default
	s_config.m_renderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
}

static bool IsInfinite(DebugRenderObject const* obj)
{
	return obj->m_totalDuration < 0.f;
}

static void RenderScreenTextObject(DebugRenderObject const& obj)
{
	Rgba8 currentColor = GetCurrentColor(obj);
	std::vector<Vertex> verts = BuildScreenTextVerts(obj);
	DrawVertsWithMode(verts, &s_debugFont->GetTexture(), DebugRenderMode::ALWAYS, currentColor);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Debug Render System
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void DebugRenderSystemStartup(const DebugRenderConfig& config)
{
	s_config = config;
	s_isVisible = true;

	DeleteObjectList(s_worldObjects);
	DeleteObjectList(s_screenObjects);

	s_nextInsertionOrder = 0;
	s_lastBeginFrameTime = GetCurrentTimeSeconds();

	if (s_config.m_renderer != nullptr)
	{
		std::string fontPath = s_config.m_fontPath + s_config.m_fontName;
		s_debugFont = s_config.m_renderer->CreateOrGetBitmapFont(fontPath.c_str());
	}

	g_engine->m_event->SubscribeEventCallbackFunction("clear", &Command_DebugRenderClear);
	g_engine->m_event->SubscribeEventCallbackFunction("toggle", &Command_DebugRenderToggle);
}

void DebugRenderSystemShutdown()
{
	DebugRenderClear();
	s_debugFont = nullptr;
	s_config.m_renderer = nullptr;
}

void DebugRenderSetVisible()
{
	s_isVisible = true;
}

void DebugRenderSetHidden()
{
	s_isVisible = false;
}

void DebugRenderClear()
{
	DeleteObjectList(s_worldObjects);
	DeleteObjectList(s_screenObjects);
}

void DebugRenderBeginFrame()
{
	double now = GetCurrentTimeSeconds();
	float deltaSeconds = (float)(now - s_lastBeginFrameTime);
	s_lastBeginFrameTime = now;

	if (deltaSeconds < 0.f)
	{
		deltaSeconds = 0.f;
	}

	UpdateObjectList(s_worldObjects, deltaSeconds);
	UpdateObjectList(s_screenObjects, deltaSeconds);
}

void DebugRenderWorld(const Camera& camera)
{
	if (!s_isVisible || s_config.m_renderer == nullptr)
	{
		return;
	}

	for (DebugRenderObject* obj : s_worldObjects)
	{
		if (obj == nullptr)
		{
			continue;
		}

		RenderWorldObject(*obj, camera);
	}
	// Restore default-ish render state
	s_config.m_renderer->SetBlendMode(BlendMode::OPAQUE);
	s_config.m_renderer->SetDepthMode(DepthMode::READ_WRITE_LESS_EQUAL);
}

void DebugRenderScreen(const Camera& camera)
{
	(void)camera;

	if (!s_isVisible || s_config.m_renderer == nullptr)
	{
		return;
	}

	// Non-message screen objects first
	for (DebugRenderObject* obj : s_screenObjects)
	{
		if (obj == nullptr || obj->m_type == DebugObjectType::MESSAGE)
		{
			continue;
		}

		RenderScreenTextObject(*obj);
	}

	// Messages: infinite first, then finite
	std::vector<DebugRenderObject*> infiniteMessages;
	std::vector<DebugRenderObject*> finiteMessages;

	for (DebugRenderObject* obj : s_screenObjects)
	{
		if (obj == nullptr || obj->m_type != DebugObjectType::MESSAGE)
		{
			continue;
		}

		if (!IsInfinite(obj))
		{
			finiteMessages.push_back(obj);
		}
		else
		{
			infiniteMessages.push_back(obj);
		}
	}

	auto sortByOrder = [](DebugRenderObject const* a, DebugRenderObject const* b)
		{
			return a->m_insertionOrder < b->m_insertionOrder;
		};

	std::sort(infiniteMessages.begin(), infiniteMessages.end(), sortByOrder);
	std::sort(finiteMessages.begin(), finiteMessages.end(), sortByOrder);

	std::vector<DebugRenderObject*> orderedMessages;
	orderedMessages.reserve(infiniteMessages.size() + finiteMessages.size());
	orderedMessages.insert(orderedMessages.end(), infiniteMessages.begin(), infiniteMessages.end());
	orderedMessages.insert(orderedMessages.end(), finiteMessages.begin(), finiteMessages.end());

	IntVec2 screenSize = g_engine->m_window->GetClientDimensions();

	float left = 20.f;
	float right = (float)screenSize.x - 20.f;
	float lineHeight = 22.f;
	float yTop = (float)screenSize.y - 20.f;

	for (int i = (int)orderedMessages.size() - 1; i >= 0; --i)
	{
		DebugRenderObject* msg = orderedMessages[i];
		if (msg == nullptr)
		{
			continue;
		}

		msg->m_screenTextBox = AABB2(
			Vec2(left, yTop - lineHeight),
			Vec2(right, yTop)
		);

		msg->m_cellHeight = lineHeight;
		msg->m_alignment = Vec2(0.f, 0.5f);

		RenderScreenTextObject(*msg);

		yTop -= lineHeight;
	}

	s_config.m_renderer->SetBlendMode(BlendMode::OPAQUE);
	s_config.m_renderer->SetDepthMode(DepthMode::READ_WRITE_LESS_EQUAL);
	s_config.m_renderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
}

//------------------------------------------------------------------------------------------------
void DebugRenderEndFrame()
{

}

void DebugAddWorldSphere(
	const Vec3& center,
	float radius,
	float duration,
	const Rgba8& startColor,
	const Rgba8& endColor,
	DebugRenderMode mode)
{
	DebugRenderObject* obj = CreateDebugObject(
		DebugObjectType::WORLD_GEOMETRY,
		duration,
		startColor,
		endColor,
		mode
	);

	AddVertsForSphere3D(
		obj->m_localVerts,
		Vec3(0.f, 0.f, 0.f),
		radius,
		Rgba8::WHITE,
		AABB2::ZERO_TO_ONE,
		32,
		16
	);

	obj->m_transform = Mat44::MakeTranslation3D(center);
	obj->m_texture = nullptr;

	AddWorldObject(obj);
}

void DebugAddWorldWireSphere(
	const Vec3& center,
	float radius,
	float duration,
	const Rgba8& startColor,
	const Rgba8& endColor,
	DebugRenderMode mode)
{
	DebugRenderObject* obj = CreateDebugObject(
		DebugObjectType::WORLD_GEOMETRY,
		duration,
		startColor,
		endColor,
		mode
	);

	AddVertsForSphere3D(
		obj->m_localVerts,
		Vec3(0.f, 0.f, 0.f),
		radius,
		Rgba8::WHITE,
		AABB2::ZERO_TO_ONE,
		32,
		16
	);

	obj->m_transform = Mat44::MakeTranslation3D(center);
	obj->m_texture = nullptr;
	obj->m_isWireframe = true;

	AddWorldObject(obj);
}

void DebugAddWorldCylinder(
	const Vec3& start,
	const Vec3& end,
	float radius,
	float duration,
	const Rgba8& startColor,
	const Rgba8& endColor,
	DebugRenderMode mode)
{
	DebugRenderObject* obj = CreateDebugObject(
		DebugObjectType::WORLD_GEOMETRY,
		duration,
		startColor,
		endColor,
		mode
	);

	AddVertsForCylinder3D(
		obj->m_localVerts,
		start,
		end,
		radius,
		Rgba8::WHITE,
		AABB2::ZERO_TO_ONE,
		32
	);

	obj->m_transform = Mat44();
	obj->m_texture = nullptr;

	AddWorldObject(obj);
}

void DebugAddWorldWireCylinder(
	const Vec3& start,
	const Vec3& end,
	float radius,
	float duration,
	const Rgba8& startColor,
	const Rgba8& endColor,
	DebugRenderMode mode)
{
	DebugRenderObject* obj = CreateDebugObject(
		DebugObjectType::WORLD_GEOMETRY,
		duration,
		startColor,
		endColor,
		mode
	);

	AddVertsForCylinder3D(
		obj->m_localVerts,
		start,
		end,
		radius,
		Rgba8::WHITE,
		AABB2::ZERO_TO_ONE,
		32
	);

	obj->m_transform = Mat44();
	obj->m_texture = nullptr;
	obj->m_isWireframe = true;

	AddWorldObject(obj);
}

void DebugAddWorldArrow(
	const Vec3& start,
	const Vec3& end,
	float radius,
	float duration,
	const Rgba8& startColor,
	const Rgba8& endColor,
	DebugRenderMode mode)
{
	DebugRenderObject* obj = CreateDebugObject(
		DebugObjectType::WORLD_GEOMETRY,
		duration,
		startColor,
		endColor,
		mode
	);

	AddVertsForArrow3D(
		obj->m_localVerts,
		start,
		end,
		radius,
		Rgba8::WHITE,
		24
	);

	obj->m_transform = Mat44();
	obj->m_texture = nullptr;

	AddWorldObject(obj);
}

void DebugAddWorldWireArrow(
	const Vec3& start,
	const Vec3& end,
	float radius,
	float duration,
	const Rgba8& startColor,
	const Rgba8& endColor,
	DebugRenderMode mode)
{
	DebugAddWorldArrow(start, end, radius * 0.4f, duration, startColor, endColor, mode);
}

void DebugAddBasis(
	const Mat44& transform,
	float duration,
	float length,
	float radius,
	float colorScale,
	float alphaScale,
	DebugRenderMode mode)
{
	Vec3 origin = transform.GetTranslation3D();

	Vec3 i = transform.GetIBasis3D().GetNormalized() * length;
	Vec3 j = transform.GetJBasis3D().GetNormalized() * length;
	Vec3 k = transform.GetKBasis3D().GetNormalized() * length;

	unsigned char alpha = (unsigned char)(255.f * alphaScale);

	Rgba8 red(
		(unsigned char)(255.f * colorScale),
		0,
		0,
		alpha
	);

	Rgba8 green(
		0,
		(unsigned char)(255.f * colorScale),
		0,
		alpha
	);

	Rgba8 blue(
		0,
		0,
		(unsigned char)(255.f * colorScale),
		alpha
	);

	DebugAddWorldArrow(origin, origin + i, radius, duration, red, red, mode);
	DebugAddWorldArrow(origin, origin + j, radius, duration, green, green, mode);
	DebugAddWorldArrow(origin, origin + k, radius, duration, blue, blue, mode);
}

void DebugAddWorldBasis(
	const Mat44& transform,
	float duration,
	DebugRenderMode mode)
{
	DebugAddBasis(transform, duration, 1.f, 0.05f, 1.f, 1.f, mode);
}

void DebugAddWorldText(
	const std::string& text,
	const Mat44& transform,
	float textHeight,
	const Vec2& alignment,
	float duration,
	const Rgba8& startColor,
	const Rgba8& endColor,
	DebugRenderMode mode)
{
	DebugRenderObject* obj = CreateDebugObject(
		DebugObjectType::WORLD_TEXT,
		duration,
		startColor,
		endColor,
		mode
	);

	obj->m_text = text;
	obj->m_transform = transform;
	obj->m_textHeight = textHeight;
	obj->m_alignment = alignment;
	obj->m_texture = (s_debugFont != nullptr) ? &s_debugFont->GetTexture() : nullptr;

	AddWorldObject(obj);
}

void DebugAddWorldBillboardText(
	const std::string& text,
	const Vec3& origin,
	float textHeight,
	const Vec2& alignment,
	float duration,
	const Rgba8& startColor,
	const Rgba8& endColor,
	DebugRenderMode mode)
{
	DebugRenderObject* obj = CreateDebugObject(
		DebugObjectType::WORLD_BILLBOARD_TEXT,
		duration,
		startColor,
		endColor,
		mode
	);

	obj->m_text = text;
	obj->m_billboardOrigin = origin;
	obj->m_textHeight = textHeight;
	obj->m_alignment = alignment;
	obj->m_texture = (s_debugFont != nullptr) ? &s_debugFont->GetTexture() : nullptr;

	AddWorldObject(obj);
}

void DebugAddScreenText(
	const std::string& text,
	const AABB2& box,
	float cellHeight,
	const Vec2& alignment,
	float duration,
	const Rgba8& startColor,
	const Rgba8& endColor)
{
	DebugRenderObject* obj = CreateDebugObject(
		DebugObjectType::SCREEN_TEXT,
		duration,
		startColor,
		endColor,
		DebugRenderMode::ALWAYS
	);

	obj->m_text = text;
	obj->m_screenTextBox = box;
	obj->m_cellHeight = cellHeight;
	obj->m_alignment = alignment;
	obj->m_texture = (s_debugFont != nullptr) ? &s_debugFont->GetTexture() : nullptr;

	AddScreenObject(obj);
}

void DebugAddMessage(
	const std::string& text,
	float duration,
	const Rgba8& startColor,
	const Rgba8& endColor)
{
	DebugRenderObject* obj = CreateDebugObject(
		DebugObjectType::MESSAGE,
		duration,
		startColor,
		endColor,
		DebugRenderMode::ALWAYS
	);

	obj->m_text = text;
	obj->m_texture = (s_debugFont != nullptr) ? &s_debugFont->GetTexture() : nullptr;

	AddScreenObject(obj);
}

bool Command_DebugRenderClear(EventArgs& args)
{
	(void)args;
	DebugRenderClear();
	return true;
}

bool Command_DebugRenderToggle(EventArgs& args)
{
	(void)args;

	if (s_isVisible)
	{
		DebugRenderSetHidden();
	}
	else
	{
		DebugRenderSetVisible();
	}

	return true;
}