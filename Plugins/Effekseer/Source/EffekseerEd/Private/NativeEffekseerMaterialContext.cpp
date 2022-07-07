#include "NativeEffekseerMaterialContext.h"

#include "MaterialNode/InternalNode.h"

std::shared_ptr<NativeEffekseerMaterialContext> NativeEffekseerMaterialContext::Load(const uint8* data, int size, const char* basePath)
{
	auto ret = std::make_shared<NativeEffekseerMaterialContext>();

	std::vector<uint8_t> dataVec;
	dataVec.resize(size);
	memcpy(dataVec.data(), data, size);

	ret->library = std::make_shared<EffekseerMaterial::Library>();
	ret->material = std::make_shared<EffekseerMaterial::Material>();
	ret->material->Initialize();

	// TODO : specify path
	ret->material->Load(dataVec, ret->library, basePath);

	// Add internal nodes
	auto nodes = ret->material->GetNodes();
	for (auto& node : nodes)
	{
		for (size_t i = 0; i < node->Parameter->OutputPins.size(); i++)
		{
			std::unordered_set<std::shared_ptr<EffekseerMaterial::Pin>> cache;
			auto outputType = ret->material->GetDesiredPinType(node->OutputPins[i], cache);

			auto inputPins = ret->material->GetConnectedPins(node->OutputPins[i]);

			for (size_t j = 0; j < inputPins.size(); j++)
			{
				cache.clear();
				auto inputType = ret->material->GetDesiredPinType(inputPins[j], cache);

				if (outputType == EffekseerMaterial::ValueType::Float2 && (inputType == EffekseerMaterial::ValueType::Float3 || inputType == EffekseerMaterial::ValueType::Float4))
				{
					auto newNode = ret->material->CreateNode(std::make_shared<EffekseerMaterial::NodeCastFloat2ToFloat4>(), true);
					newNode->Pos = node->Pos;
					ret->material->ConnectPin(node->OutputPins[i], newNode->InputPins[0]);
					ret->material->ConnectPin(newNode->OutputPins[i], inputPins[j]);
				}
				else if (outputType == EffekseerMaterial::ValueType::Float3 && (inputType == EffekseerMaterial::ValueType::Float4))
				{
					auto newNode = ret->material->CreateNode(std::make_shared<EffekseerMaterial::NodeCastFloat3ToFloat4>(), true);
					newNode->Pos = node->Pos;
					ret->material->ConnectPin(node->OutputPins[i], newNode->InputPins[0]);
					ret->material->ConnectPin(newNode->OutputPins[i], inputPins[j]);
				}
			}
		}
	}

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

	for (const auto& node : ret->material->GetNodes())
	{
		if (node->Parameter->Type == EffekseerMaterial::NodeType::EffectScale || node->Parameter->Type == EffekseerMaterial::NodeType::DepthFade)
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

	// parse gradient names
	for (auto g : ret->result.Gradients)
	{
		if (g->Name == "" || used.count(g->Name) > 0)
		{
			ret->gradientNames[g->GUID] = g->UniformName;
		}
		else
		{
			ret->gradientNames[g->GUID] = g->Name;
			used.insert(g->UniformName);
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