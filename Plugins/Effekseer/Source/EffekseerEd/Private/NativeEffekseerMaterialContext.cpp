#include "NativeEffekseerMaterialContext.h"

std::shared_ptr<NativeEffekseerMaterialContext> NativeEffekseerMaterialContext::Load(const uint8* data, int size, const char* basePath)
{
	auto ret = std::make_shared< NativeEffekseerMaterialContext>();

	std::vector<uint8_t> dataVec;
	dataVec.resize(size);
	memcpy(dataVec.data(), data, size);

	ret->library = std::make_shared<EffekseerMaterial::Library>();
	ret->material = std::make_shared<EffekseerMaterial::Material>();
	ret->material->Initialize();

	// TODO : specify path
	ret->material->Load(dataVec, ret->library, basePath);

	for (auto node : ret->material->GetNodes())
	{
		if (node->Parameter->Type == EffekseerMaterial::NodeType::Output)
		{
			ret->outputNode = node;
		}
	}

	assert(ret->outputNode != nullptr);

	std::shared_ptr<EffekseerMaterial::TextExporter> textExporter = std::make_shared<EffekseerMaterial::TextExporter>();
	ret->result = textExporter->Export(ret->material, ret->outputNode, "$SUFFIX");

	for(const auto& node : ret->material->GetNodes())
	{
		if(node->Parameter->Type == EffekseerMaterial::NodeType::EffectScale || node->Parameter->Type == EffekseerMaterial::NodeType::DepthFade)
		{
			ret->isEffectScaleRequired = true;
		}
	}

	std::unordered_set<std::string> used;

	// parse uniform names
	for (auto u : ret->result.Uniforms)
	{
		if (u->Name == "" || used.count(u->Name) > 0)
		{
			ret->uniformNames[u->GUID] = u->UniformName;
		}
		else
		{
			ret->uniformNames[u->GUID] = u->Name;
			used.insert(u->UniformName);
		}
	}

	// parse texture names
	for (auto t : ret->result.Textures)
	{
		if (t->Name == "" || used.count(t->Name) > 0)
		{
			ret->textureNames[t->GUID] = t->UniformName;
		}
		else
		{
			ret->textureNames[t->GUID] = t->Name;
			used.insert(t->UniformName);
		}
	}

	return ret;
}