#pragma once

#include "../NativeEffekseerMaterialContext.h"

namespace EffekseerMaterial
{

class NodeCastFloat2ToFloat4 : public NodeParameter
{
public:
	NodeCastFloat2ToFloat4()
	{
		// TODO replace it
		Type = NodeType::Function;
		TypeName = "CastFloat2ToFloat4";
		Group = std::vector<std::string>{"Internal"};

		auto input = std::make_shared<PinParameter>();
		input->Name = "Input";
		input->Type = ValueType::Float2;
		InputPins.push_back(input);

		auto output = std::make_shared<PinParameter>();
		output->Name = "Output";
		output->Type = ValueType::Float4;
		OutputPins.push_back(output);
	}
};

class NodeCastFloat3ToFloat4 : public NodeParameter
{
public:
	NodeCastFloat3ToFloat4()
	{
		// TODO replace it
		Type = NodeType::Function;
		TypeName = "CastFloat3ToFloat4";
		Group = std::vector<std::string>{"Internal"};

		auto input = std::make_shared<PinParameter>();
		input->Name = "Input";
		input->Type = ValueType::Float3;
		InputPins.push_back(input);

		auto output = std::make_shared<PinParameter>();
		output->Name = "Output";
		output->Type = ValueType::Float4;
		OutputPins.push_back(output);
	}
};

} // namespace EffekseerMaterial