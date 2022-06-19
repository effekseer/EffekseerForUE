#pragma once

#include <array>

#include "../MaterialNode/InternalNode.h"
#include "../NativeEffekseerMaterialContext.h"
#include "ConvertedNode.h"
#include "EffekseerEffect.h"

class ConvertedNodeSampleGradient : public ConvertedNode
{
private:
	std::shared_ptr<EffekseerMaterial::Node> effekseerNode_;
	UMaterialExpressionMaterialFunctionCall* expression_ = nullptr;
	std::array<UMaterialExpression*, 13> parameters_;

public:
	ConvertedNodeSampleGradient(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode);

	UMaterialExpression* GetExpression() const override;

	int32_t GetExpressionCount() const override;

	UMaterialExpression* GetExpressions(int32_t ind) const override;

	void Connect(int targetInd, std::shared_ptr<ConvertedNode> outputNode, int32_t outputNodePinIndex) override;
};