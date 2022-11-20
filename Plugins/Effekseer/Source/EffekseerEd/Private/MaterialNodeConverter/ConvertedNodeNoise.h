#pragma once

#include "../MaterialNode/InternalNode.h"
#include "../NativeEffekseerMaterialContext.h"
#include "ConvertedNode.h"
#include "EffekseerEffect.h"

class ConvertedNodeSimpleNoise : public ConvertedNode
{
private:
	std::shared_ptr<EffekseerMaterial::Node> effekseerNode_;
	UMaterialExpressionMaterialFunctionCall* expression_ = nullptr;

public:
	ConvertedNodeSimpleNoise(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
		: effekseerNode_(effekseerNode)
	{
		expression_ = NewObject<UMaterialExpressionMaterialFunctionCall>(material);
		ConvertedNodeHelper::AddExpression(material, expression_);

		EffekseerUE::UEFSoftObjectPath assetPath("/Effekseer/MaterialFunctions/EfkSimpleNoise.EfkSimpleNoise");
		UMaterialFunction* func = Cast<UMaterialFunction>(assetPath.TryLoad());
		expression_->SetMaterialFunction(func);
	}

	UMaterialExpression* GetExpression() const override
	{
		return expression_;
	}

	void Connect(int targetInd, std::shared_ptr<ConvertedNode> outputNode, int32_t outputNodePinIndex) override
	{
		if (targetInd == effekseerNode_->GetInputPinIndex("UV"))
		{
			outputNode->GetNodeOutputConnector(outputNodePinIndex).Apply(*expression_->GetInput(0));
		}

		if (targetInd == effekseerNode_->GetInputPinIndex("Scale"))
		{
			outputNode->GetNodeOutputConnector(outputNodePinIndex).Apply(*expression_->GetInput(1));
		}
	}
};