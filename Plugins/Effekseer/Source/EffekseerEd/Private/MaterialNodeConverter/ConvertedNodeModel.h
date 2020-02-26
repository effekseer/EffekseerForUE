
#pragma once

#include "ConvertedNode.h"
#include "EffekseerEffect.h"
#include "Materials/MaterialExpressionPanner.h"
#include "Materials/MaterialExpressionVertexNormalWS.h"
#include "Materials/MaterialExpressionPixelNormalWS.h"
#include "Materials/MaterialExpressionVertexColor.h"
#include "Materials/MaterialExpressionWorldPosition.h"

#include "../NativeEffekseerMaterialContext.h"

class ConvertedNodeWorldPosition : public ConvertedNode
{
private:
	std::shared_ptr<EffekseerMaterial::Node> effekseerNode_;
	UMaterialExpressionWorldPosition* expression_ = nullptr;

public:
	ConvertedNodeWorldPosition(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
		: effekseerNode_(effekseerNode)
	{
		expression_ = NewObject<UMaterialExpressionWorldPosition>(material);
		material->Expressions.Add(expression_);
	}

	UMaterialExpression* GetExpression() const override { return expression_; }
};

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

	void Connect(int targetInd, std::shared_ptr<ConvertedNode> outputNode, int32_t outputNodePinIndex) override
	{
		if (targetInd == effekseerNode_->GetInputPinIndex("UV"))
		{
			expression_->Coordinate.Expression = outputNode->GetExpressions(outputNodePinIndex);
		}

		if (targetInd == effekseerNode_->GetInputPinIndex("Time"))
		{
			expression_->Time.Expression = outputNode->GetExpressions(outputNodePinIndex);
		}

		if (targetInd == effekseerNode_->GetInputPinIndex("Speed"))
		{
			expression_->Speed.Expression = outputNode->GetExpressions(outputNodePinIndex);
		}
	}
};

using ConvertedNodeVertexNormalWS = ConvertedNodeSimple<UMaterialExpressionVertexNormalWS>;
using ConvertedNodePixelNormalWS = ConvertedNodeSimple<UMaterialExpressionPixelNormalWS>;

class ConvertedNodeVertexColor : public ConvertedNode
{
private:
	UMaterialExpressionComponentMask* expressionR_ = nullptr;
	UMaterialExpressionComponentMask* expressionG_ = nullptr;
	UMaterialExpressionComponentMask* expressionB_ = nullptr;
	UMaterialExpressionComponentMask* expressionA_ = nullptr;
	UMaterialExpressionVertexColor* function_ = nullptr;

public:
	ConvertedNodeVertexColor(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
	{
		expressionR_ = NewObject<UMaterialExpressionComponentMask>(material);
		material->Expressions.Add(expressionR_);

		expressionG_ = NewObject<UMaterialExpressionComponentMask>(material);
		material->Expressions.Add(expressionG_);

		expressionB_ = NewObject<UMaterialExpressionComponentMask>(material);
		material->Expressions.Add(expressionB_);

		expressionA_ = NewObject<UMaterialExpressionComponentMask>(material);
		material->Expressions.Add(expressionA_);

		function_ = NewObject<UMaterialExpressionVertexColor>(material);
		material->Expressions.Add(function_);

		expressionR_->Input.Expression = function_;
		expressionR_->R = 1;
		expressionR_->G = 0;
		expressionR_->B = 0;
		expressionR_->A = 0;

		expressionG_->Input.Expression = function_;
		expressionG_->R = 0;
		expressionG_->G = 1;
		expressionG_->B = 0;
		expressionG_->A = 0;

		expressionB_->Input.Expression = function_;
		expressionB_->R = 0;
		expressionB_->G = 0;
		expressionB_->B = 1;
		expressionB_->A = 0;

		expressionA_->Input.Expression = function_;
		expressionA_->R = 0;
		expressionA_->G = 0;
		expressionA_->B = 0;
		expressionA_->A = 1;

	}

	UMaterialExpression* GetExpression() const override { return function_; }

	UMaterialExpression* GetExpressions(int32_t ind) const override {
		if (ind == 0) return function_;
		if (ind == 1) return expressionR_;
		if (ind == 2) return expressionG_;
		if (ind == 3) return expressionB_;
		if (ind == 4) return expressionA_;
		return nullptr;
	}

	int32_t GetExpressionCount() const { return 5; }
};

class ConvertedNodeObjectScale : public ConvertedNode
{
private:
	std::shared_ptr<EffekseerMaterial::Node> effekseerNode_;
	UMaterialExpressionMaterialFunctionCall* expression_ = nullptr;

public:
	ConvertedNodeObjectScale(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
		: effekseerNode_(effekseerNode)
	{
		expression_ = NewObject<UMaterialExpressionMaterialFunctionCall>(material);
		material->Expressions.Add(expression_);

		FStringAssetReference assetPath("/Engine/Functions/Engine_MaterialFunctions02/WorldPositionOffset/ObjectScale.ObjectScale");
		UMaterialFunction* func = Cast<UMaterialFunction>(assetPath.TryLoad());
		expression_->SetMaterialFunction(func);
	}

	UMaterialExpression* GetExpression() const override { return expression_; }
};
