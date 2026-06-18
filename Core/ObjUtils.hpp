#pragma once
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Vec2.hpp"
#include <string>
#include <vector>
#include <map>

struct Vertex;
class Texture;

struct ObjMaterial
{
	std::string name;
	std::string diffuseMap;
	Rgba8       kdTint = Rgba8(255, 255, 255, 255);
	Texture* diffuseTex = nullptr;
	Vec2 uvOffset = Vec2(0.f, 0.f);
	Vec2 uvScale = Vec2(1.f, 1.f);
};

struct ObjGroup
{
	std::string         name;
	ObjMaterial* material = nullptr;
	std::vector<Vertex> verts;
};

struct ObjObject
{
	std::string           name;
	std::vector<ObjGroup> groups;
};

struct ObjModel
{
	std::string                        sourceObjPath;
	std::vector<ObjObject>             objects;
	std::map<std::string, ObjMaterial> materials;
	std::vector<std::string>           mtllibFiles;
};

bool LoadMtlFile(
	std::map<std::string, ObjMaterial>& outMaterials,
	std::string const& mtlFilePath);

bool LoadObjModel(
	ObjModel& outModel,
	std::string const& objFilePath,
	Rgba8 const& defaultColor);

bool LoadObjToMaterialVertexArrays(
	std::map<std::string, std::vector<Vertex>>& outVertsByMaterial,
	std::string const& objFilePath,
	Rgba8 const& color);