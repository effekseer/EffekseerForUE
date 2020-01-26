
#pragma once

#include "ConvertedNode.h"
#include "EffekseerEffect.h"
#include "Materials/MaterialExpressionPanner.h"
#include "Materials/MaterialExpressionVertexNormalWS.h"
#include "Materials/MaterialExpressionPixelNormalWS.h"
#include "Materials/MaterialExpressionVertexColor.h"
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

class ConvertedNodePanner : public ConvertedNode
{
private:
	std::shared_ptr<EffekseerMaterial::Node> effekseerNode_;
	UMaterialExpressionPanner* expression_ = nullptr;

public:
	ConvertedNodePanner(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
		: effekseerNode_(effekseerNode)
	{
		expression_ = NewObject<UMaterialExpressionPanner>(material);
		material->Expressions.Add(expression_);

		auto index = static_cast<int32_t>(effekseerNode_->GetProperty("UVIndex")->Floats[0]);

		expression_->SpeedX = effekseerNode_->GetProperty("Speed")->Floats[0];
		expression_->SpeedY = effekseerNode_->GetProperty("Speed")->Floats[1];
		expression_->ConstCoordinate = index;
	}

	UMaterialExpression* GetExpression() const override { return expression_; }

	void Connect(int targetInd, std::shared_ptr<ConvertedNode> outputNode) override
	{
		if (targetInd == effekseerNode_->GetInputPinIndex("UV"))
		{
			expression_->Coordinate.Expression = outputNode->GetExpression();
		}

		if (targetInd == effekseerNode_->GetInputPinIndex("Time"))
		{
			expression_->Time.Expression = outputNode->GetExpression();
		}

		if (targetInd == effekseerNode_->GetInputPinIndex("Speed"))
		{
			expression_->Speed.Expression = outputNode->GetExpression();
		}
	}
};

using ConvertedNodeVertexNormalWS = ConvertedNodeSimple<UMaterialExpressionVertexNormalWS>;
using ConvertedNodePixelNormalWS = ConvertedNodeSimple<UMaterialExpressionPixelNormalWS>;
using ConvertedNodeVertexColor = ConvertedNodeSimple<UMaterialExpressionVertexColor>;
