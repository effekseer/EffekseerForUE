
#pragma once

#include "ConvertedNode.h"
#include "EffekseerEffect.h"

#include "../NativeEffekseerMaterialContext.h"

class ConvertedNodeTextureCoordinate : public ConvertedNode
{
private:
	std::shared_ptr<EffekseerMaterial::Node> effekseerNode_;
	UMaterialExpressionMaterialFunctionCall* expression_ = nullptr;

public:
	ConvertedNodeTextureCoordinate(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
		: effekseerNode_(effekseerNode)
	{
		expression_ = NewObject<UMaterialExpressionMaterialFunctionCall>(material);
		material->Expressions.Add(expression_);

		auto index = static_cast<int32_t>(effekseerNode_->GetProperty("UVIndex")->Floats[0]);

		if (index == 0)
		{
			FStringAssetReference assetPath("/Effekseer/MaterialFunctions/EfkUV1.EfkUV1");
			UMaterialFunction* func = Cast<UMaterialFunction>(assetPath.TryLoad());
			expression_->SetMaterialFunction(func);
		}
		else
		{
			FStringAssetReference assetPath("/Effekseer/MaterialFunctions/EfkUV2.EfkUV2");
			UMaterialFunction* func = Cast<UMaterialFunction>(assetPath.TryLoad());
			expression_->SetMaterialFunction(func);
		}
	}

	UMaterialExpression* GetExpression() const override { return expression_; }

	void Connect(int targetInd, std::shared_ptr<ConvertedNode> outputNode) override
	{
	}
};