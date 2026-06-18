#include "Engine/Core/ObjUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"

#include <cstdlib>
#include <cctype>
#include <string>
#include <vector>
#include <map>

static bool TryParseInt(std::string const& s, int& out)
{
	if (s.empty())
		return false;
	out = atoi(s.c_str());
	return true;
}

static float ParseFloat(std::string const& s)
{
	return (float)atof(s.c_str());
}

static std::string Trim(std::string const& in)
{
	size_t a = 0;
	while (a < in.size() && std::isspace((unsigned char)in[a])) a++;
	size_t b = in.size();
	while (b > a && std::isspace((unsigned char)in[b - 1])) b--;
	return in.substr(a, b - a);
}

static std::string StripQuotes(std::string const& s)
{
	if (s.size() >= 2 && ((s.front() == '"' && s.back() == '"') || (s.front() == '\'' && s.back() == '\'')))
		return s.substr(1, s.size() - 2);
	return s;
}

static std::string GetFileNameOnly(std::string const& path)
{
	std::string p = StripQuotes(Trim(path));
	for (char& c : p)
	{
		if (c == '\\') c = '/';
	}
	size_t slash = p.find_last_of('/');
	if (slash == std::string::npos)
		return p;
	return p.substr(slash + 1);
}

static std::string MakeDataMaterialsPath(std::string const& mtllibToken)
{
	return "Data/Material/" + GetFileNameOnly(mtllibToken);
}

static std::string MakeDataImagesPath(std::string const& mapKdToken)
{
	return "Data/Images/" + GetFileNameOnly(mapKdToken);
}

static std::string JoinTokens(Strings const& tokens, int startIdx)
{
	std::string out;
	for (int i = startIdx; i < (int)tokens.size(); ++i)
	{
		if (!out.empty()) out += " ";
		out += tokens[i];
	}
	return out;
}

static bool TryToZeroBasedIndex(int objIndex, int count, int& outZero)
{
	if (objIndex > 0)
	{
		outZero = objIndex - 1;
		return outZero >= 0 && outZero < count;
	}
	if (objIndex < 0)
	{
		outZero = count + objIndex;
		return outZero >= 0 && outZero < count;
	}
	return false;
}

struct ObjCorner
{
	int v = -1;
	int vt = -1;
	int vn = -1;
};

static ObjCorner ParseCornerToken(std::string const& token, int posCount, int uvCount, int normCount)
{
	ObjCorner c;

	Strings parts = SplitStringOnDelimiter(token, '/');

	// v
	if (parts.size() >= 1 && !parts[0].empty())
	{
		int idxOBJ = 0;
		int idxZero = -1;
		if (TryParseInt(parts[0], idxOBJ) && TryToZeroBasedIndex(idxOBJ, posCount, idxZero))
			c.v = idxZero;
	}

	// vt
	if (parts.size() >= 2 && !parts[1].empty())
	{
		int idxOBJ = 0;
		int idxZero = -1;
		if (TryParseInt(parts[1], idxOBJ) && TryToZeroBasedIndex(idxOBJ, uvCount, idxZero))
			c.vt = idxZero;
	}

	// vn
	if (parts.size() >= 3 && !parts[2].empty())
	{
		int idxOBJ = 0;
		int idxZero = -1;
		if (TryParseInt(parts[2], idxOBJ) && TryToZeroBasedIndex(idxOBJ, normCount, idxZero))
			c.vn = idxZero;
	}

	return c;
}

static ObjObject& GetOrCreateObject(ObjModel& model, std::string const& name)
{
	if (model.objects.empty())
	{
		model.objects.push_back(ObjObject{});
		model.objects.back().name = name.empty() ? "__default_object" : name;
		return model.objects.back();
	}

	ObjObject& last = model.objects.back();
	if (last.name == name)
		return last;

	model.objects.push_back(ObjObject{});
	model.objects.back().name = name.empty() ? "__object" : name;
	return model.objects.back();
}

static ObjGroup& EnsureGroup(ObjObject& obj, std::string const& groupName)
{
	if (obj.groups.empty())
	{
		obj.groups.push_back(ObjGroup{});
		obj.groups.back().name = groupName.empty() ? "__default_group" : groupName;
		return obj.groups.back();
	}

	ObjGroup& g = obj.groups.back();
	if (g.name == groupName)
		return g;

	obj.groups.push_back(ObjGroup{});
	obj.groups.back().name = groupName.empty() ? "__group" : groupName;
	return obj.groups.back();
}

static ObjGroup& EnsureGroupForMaterial(ObjObject& obj, std::string const& groupName, ObjMaterial* mat)
{
	ObjGroup& g = EnsureGroup(obj, groupName);

	if (!g.verts.empty() && g.material != mat)
	{
		obj.groups.push_back(ObjGroup{});
		obj.groups.back().name = groupName.empty() ? "__group" : groupName;
		obj.groups.back().material = mat;
		return obj.groups.back();
	}

	g.material = mat;
	return g;
}

static ObjMaterial& EnsureMaterialExists(ObjModel& model, std::string const& name)
{
	ObjMaterial& mat = model.materials[name];
	if (mat.name.empty())
	{
		mat.name = name;
		mat.kdTint = Rgba8(255, 255, 255, 255);
	}
	return mat;
}

static ObjMaterial* FindOrDefaultMaterial(ObjModel& model, std::string const& name, std::string const& defaultName)
{
	auto it = model.materials.find(name);
	if (it != model.materials.end())
		return &it->second;

	ObjMaterial& def = EnsureMaterialExists(model, defaultName);
	return &def;
}

bool LoadMtlFile(std::map<std::string, ObjMaterial>& outMaterials, std::string const& mtlFilePath)
{
	std::string text;
	int bytesRead = FileReadToString(text, mtlFilePath.c_str());
	if (bytesRead <= 0)
		return false;

	ObjMaterial* current = nullptr;

	Strings lines = SplitStringOnDelimiter(text, '\n');
	for (std::string line : lines)
	{
		if (!line.empty() && line.back() == '\r')
			line.pop_back();

		size_t hashPos = line.find('#');
		if (hashPos != std::string::npos)
			line = line.substr(0, hashPos);

		line = Trim(line);
		if (line.empty())
			continue;

		Strings raw = SplitStringOnDelimiter(line, ' ');
		Strings tokens;
		tokens.reserve(raw.size());
		for (std::string const& t : raw)
			if (!t.empty()) tokens.push_back(t);

		if (tokens.empty())
			continue;

		std::string const& op = tokens[0];

		if (op == "newmtl")
		{
			if (tokens.size() < 2)
				continue;

			std::string name = tokens[1];
			ObjMaterial& mat = outMaterials[name];
			mat.name = name;
			mat.kdTint = Rgba8(255, 255, 255, 255);
			current = &mat;
		}
		else if (op == "Kd")
		{
			if (!current || tokens.size() < 4)
				continue;

			float r = ParseFloat(tokens[1]);
			float g = ParseFloat(tokens[2]);
			float b = ParseFloat(tokens[3]);

			auto ToByte = [](float x) -> unsigned char
				{
					if (x < 0.f) x = 0.f;
					if (x > 1.f) x = 1.f;
					return (unsigned char)(x * 255.f + 0.5f);
				};

			current->kdTint = Rgba8(ToByte(r), ToByte(g), ToByte(b), 255);
		}
		else if (op == "map_Kd")
		{
			if (!current || tokens.size() < 2)
				continue;

			current->uvOffset = Vec2(0.f, 0.f);
			current->uvScale = Vec2(1.f, 1.f);

			for (int i = 1; i < (int)tokens.size() - 1; ++i)
			{
				if (tokens[i] == "-o")
				{
					if (i + 2 < (int)tokens.size())
					{
						float ou = ParseFloat(tokens[i + 1]);
						float ov = ParseFloat(tokens[i + 2]);
						current->uvOffset = Vec2(ou, ov);
						i += 2;
					}
				}
				else if (tokens[i] == "-s")
				{
					if (i + 2 < (int)tokens.size())
					{
						float su = ParseFloat(tokens[i + 1]);
						float sv = ParseFloat(tokens[i + 2]);
						current->uvScale = Vec2(su, sv);
						i += 2;
					}
				}
			}

			std::string fileToken = tokens.back();
			current->diffuseMap = MakeDataImagesPath(fileToken);
		}
	}

	return !outMaterials.empty();
}

bool LoadObjModel(ObjModel& outModel, std::string const& objFilePath, Rgba8 const& defaultColor)
{
	outModel = ObjModel{};
	outModel.sourceObjPath = objFilePath;

	std::string fileText;
	int bytesRead = FileReadToString(fileText, objFilePath.c_str());
	if (bytesRead <= 0)
		return false;

	std::vector<Vec3> positions;
	std::vector<Vec2> uvs;
	std::vector<Vec3> normals;

	std::string currentObjectName = "__default_object";
	std::string currentGroupName = "__default_group";
	std::string currentMaterialName = "__default_material";

	ObjMaterial& defaultMat = EnsureMaterialExists(outModel, "__default_material");
	defaultMat.kdTint = Rgba8(255, 255, 255, 255);

	ObjObject& firstObj = GetOrCreateObject(outModel, currentObjectName);
	ObjGroup& firstGrp = EnsureGroupForMaterial(firstObj, currentGroupName, &defaultMat);

	ObjObject* curObjPtr = &firstObj;
	ObjGroup* curGrpPtr = &firstGrp;

	Strings lines = SplitStringOnDelimiter(fileText, '\n');
	for (std::string line : lines)
	{
		if (!line.empty() && line.back() == '\r')
			line.pop_back();

		size_t hashPos = line.find('#');
		if (hashPos != std::string::npos)
			line = line.substr(0, hashPos);

		line = Trim(line);
		if (line.empty())
			continue;

		Strings raw = SplitStringOnDelimiter(line, ' ');
		Strings tokens;
		tokens.reserve(raw.size());
		for (std::string const& t : raw)
			if (!t.empty()) tokens.push_back(t);

		if (tokens.empty())
			continue;

		std::string const& op = tokens[0];

		if (op == "mtllib")
		{
			for (int i = 1; i < (int)tokens.size(); ++i)
			{
				std::string mtlPath = MakeDataMaterialsPath(tokens[i]);
				outModel.mtllibFiles.push_back(mtlPath);
				LoadMtlFile(outModel.materials, mtlPath);
			}

			EnsureMaterialExists(outModel, "__default_material");
		}
		else if (op == "o")
		{
			currentObjectName = (tokens.size() >= 2) ? tokens[1] : "__object";
			curObjPtr = &GetOrCreateObject(outModel, currentObjectName);

			ObjMaterial* mat = FindOrDefaultMaterial(outModel, currentMaterialName, "__default_material");
			curGrpPtr = &EnsureGroupForMaterial(*curObjPtr, currentGroupName, mat);
		}
		else if (op == "g")
		{
			currentGroupName = (tokens.size() >= 2) ? JoinTokens(tokens, 1) : "__group";

			ObjMaterial* mat = FindOrDefaultMaterial(outModel, currentMaterialName, "__default_material");
			curGrpPtr = &EnsureGroupForMaterial(*curObjPtr, currentGroupName, mat);
		}
		else if (op == "usemtl")
		{
			currentMaterialName = (tokens.size() >= 2 && !tokens[1].empty()) ? tokens[1] : "__default_material";
			ObjMaterial& mat = EnsureMaterialExists(outModel, currentMaterialName);
			curGrpPtr = &EnsureGroupForMaterial(*curObjPtr, currentGroupName, &mat);
		}
		else if (op == "v")
		{
			if (tokens.size() < 4) continue;
			positions.emplace_back(ParseFloat(tokens[1]), ParseFloat(tokens[2]), ParseFloat(tokens[3]));
		}
		else if (op == "vt")
		{
			if (tokens.size() < 2) continue;
			float u = ParseFloat(tokens[1]);
			float v = (tokens.size() >= 3) ? ParseFloat(tokens[2]) : 0.f;

			uvs.emplace_back(u, v);
		}
		else if (op == "vn")
		{
			if (tokens.size() < 4) continue;
			normals.emplace_back(ParseFloat(tokens[1]), ParseFloat(tokens[2]), ParseFloat(tokens[3]));
		}
		else if (op == "f")
		{
			if (tokens.size() < 4) continue;
			if (!curGrpPtr) continue;

			int posCount = (int)positions.size();
			int uvCount = (int)uvs.size();
			int normCount = (int)normals.size();

			std::vector<ObjCorner> corners;
			corners.reserve(tokens.size() - 1);
			for (size_t i = 1; i < tokens.size(); ++i)
				corners.push_back(ParseCornerToken(tokens[i], posCount, uvCount, normCount));

			ObjGroup& group = *curGrpPtr;

			auto EmitCorner = [&](ObjCorner const& c)
				{
					if (c.v < 0 || c.v >= (int)positions.size())
						return;

					Vec3 position = positions[c.v];

					Vec2 uv(0.f, 0.f);
					if (c.vt >= 0 && c.vt < (int)uvs.size())
						uv = uvs[c.vt];

					if (group.material)
					{
						uv = uv * group.material->uvScale + group.material->uvOffset;
					}

					Rgba8 final = defaultColor;
					if (group.material)
					{
						final.r = (unsigned char)((final.r * (unsigned int)group.material->kdTint.r) / 255u);
						final.g = (unsigned char)((final.g * (unsigned int)group.material->kdTint.g) / 255u);
						final.b = (unsigned char)((final.b * (unsigned int)group.material->kdTint.b) / 255u);
						final.a = (unsigned char)((final.a * (unsigned int)group.material->kdTint.a) / 255u);
					}

					group.verts.emplace_back(position, final, uv);
				};

			for (size_t i = 1; i + 1 < corners.size(); ++i)
			{
				EmitCorner(corners[0]);
				EmitCorner(corners[i + 1]);
				EmitCorner(corners[i]);
			}
		}
	}

	size_t totalVerts = 0;
	for (ObjObject const& obj : outModel.objects)
		for (ObjGroup const& grp : obj.groups)
			totalVerts += grp.verts.size();

	return totalVerts > 0;
}

bool LoadObjToMaterialVertexArrays(
	std::map<std::string, std::vector<Vertex>>& outVertsByMaterial,
	std::string const& objFilePath,
	Rgba8 const& color)
{
	outVertsByMaterial.clear();

	ObjModel model;
	if (!LoadObjModel(model, objFilePath, color))
		return false;

	for (ObjObject const& obj : model.objects)
	{
		for (ObjGroup const& grp : obj.groups)
		{
			std::string name = "__default";
			if (grp.material && !grp.material->name.empty())
				name = grp.material->name;

			auto& dst = outVertsByMaterial[name];
			dst.insert(dst.end(), grp.verts.begin(), grp.verts.end());
		}
	}

	size_t total = 0;
	for (auto const& kv : outVertsByMaterial)
		total += kv.second.size();

	return total > 0;
}