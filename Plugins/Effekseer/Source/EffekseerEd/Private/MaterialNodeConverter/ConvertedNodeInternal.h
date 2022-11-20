
#pragma once

#include "../MaterialNode/InternalNode.h"
#include "../NativeEffekseerMaterialContext.h"
#include "ConvertedNode.h"
#include "EffekseerEffect.h"
#include "Materials/MaterialExpressionScalarParameter.h"
#include "Materials/MaterialExpressionVectorParameter.h"

class ConvertedNodeCastFloat2ToFloat4 : public ConvertedNode
{
private:
	std::shared_ptr<EffekseerMaterial::Node> effekseerNode_;
	UMaterialExpressionMaterialFunctionCall* expression_ = nullptr;

public:
	ConvertedNodeCastFloat2ToFloat4(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
		: effekseerNode_(effekseerNode)
	{
		expression_ = NewObject<UMaterialExpressionMaterialFunctionCall>(material);
		ConvertedNodeHelper::AddExpression(material, expression_);

		EffekseerUE::UEFSoftObjectPath assetPath("/Effekseer/MaterialFunctions/EfkCastFloat2ToFloat4.EfkCastFloat2ToFloat4");
		UMaterialFunction* func = Cast<UMaterialFunction>(assetPath.TryLoad());
		expression_->SetMaterialFunction(func);
	}

	UMaterialExpression* GetExpression() const override
	{
		return expression_;
	}

	void Connect(int targetInd, std::shared_ptr<ConvertedNode> outputNode, int32_t outputNodePinIndex) override
	{
		outputNode->GetNodeOutputConnector(outputNodePinIndex).Apply(*expression_->GetInput(targetInd));
	}
};

class ConvertedNodeCastFloat3ToFloat4 : public ConvertedNode
{
private:
	std::shared_ptr<EffekseerMaterial::Node> effekseerNode_;
	UMaterialExpressionMaterialFunctionCall* expression_ = nullptr;

public:
	ConvertedNodeCastFloat3ToFloat4(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
		: effekseerNode_(effekseerNode)
	{
		expression_ = NewObject<UMaterialExpressionMaterialFunctionCall>(material);
		ConvertedNodeHelper::AddExpression(material, expression_);

		EffekseerUE::UEFSoftObjectPath assetPath("/Effekseer/MaterialFunctions/EfkCastFloat3ToFloat4.EfkCastFloat3ToFloat4");
		UMaterialFunction* func = Cast<UMaterialFunction>(assetPath.TryLoad());
		expression_->SetMaterialFunction(func);
	}

	UMaterialExpression* GetExpression() const override
	{
		return expression_;
	}

	void Connect(int targetInd, std::shared_ptr<ConvertedNode> outputNode, int32_t outputNodePinIndex) override
	{
		outputNode->GetNodeOutputConnector(outputNodePinIndex).Apply(*expression_->GetInput(targetInd));
	}
};
