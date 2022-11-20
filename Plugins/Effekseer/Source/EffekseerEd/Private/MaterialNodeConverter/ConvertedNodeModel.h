
#pragma once

#include "../NativeEffekseerMaterialContext.h"
#include "ConvertedNode.h"
#include "EffekseerEffect.h"
#include "Materials/MaterialExpressionPanner.h"
#include "Materials/MaterialExpressionPixelNormalWS.h"
#include "Materials/MaterialExpressionVertexColor.h"
#include "Materials/MaterialExpressionVertexNormalWS.h"
#include "Materials/MaterialExpressionWorldPosition.h"

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
		ConvertedNodeHelper::AddExpression(material, expression_);
	}

	UMaterialExpression* GetExpression() const override
	{
		return expression_;
	}
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
		ConvertedNodeHelper::AddExpression(material, expression_);

		auto index = static_cast<int32_t>(effekseerNode_->GetProperty("UVIndex")->Floats[0]);

		if (index == 0)
		{
			EffekseerUE::UEFSoftObjectPath assetPath("/Effekseer/MaterialFunctions/EfkUV1.EfkUV1");
			UMaterialFunction* func = Cast<UMaterialFunction>(assetPath.TryLoad());
			expression_->SetMaterialFunction(func);
		}
		else
		{
			EffekseerUE::UEFSoftObjectPath assetPath("/Effekseer/MaterialFunctions/EfkUV2.EfkUV2");
			UMaterialFunction* func = Cast<UMaterialFunction>(assetPath.TryLoad());
			expression_->SetMaterialFunction(func);
		}
	}

	UMaterialExpression* GetExpression() const override
	{
		return expression_;
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
		ConvertedNodeHelper::AddExpression(material, expression_);

		auto index = static_cast<int32_t>(effekseerNode_->GetProperty("UVIndex")->Floats[0]);

		expression_->SpeedX = effekseerNode_->GetProperty("Speed")->Floats[0];
		expression_->SpeedY = effekseerNode_->GetProperty("Speed")->Floats[1];
		expression_->ConstCoordinate = index;
	}

	UMaterialExpression* GetExpression() const override
	{
		return expression_;
	}

	void Connect(int targetInd, std::shared_ptr<ConvertedNode> outputNode, int32_t outputNodePinIndex) override
	{
		if (targetInd == effekseerNode_->GetInputPinIndex("UV"))
		{
			outputNode->GetNodeOutputConnector(outputNodePinIndex).Apply(expression_->Coordinate);
		}

		if (targetInd == effekseerNode_->GetInputPinIndex("Time"))
		{
			outputNode->GetNodeOutputConnector(outputNodePinIndex).Apply(expression_->Time);
		}

		if (targetInd == effekseerNode_->GetInputPinIndex("Speed"))
		{
			outputNode->GetNodeOutputConnector(outputNodePinIndex).Apply(expression_->Speed);
		}
	}
};

using ConvertedNodeVertexNormalWS = ConvertedNodeSimple<UMaterialExpressionVertexNormalWS>;
using ConvertedNodePixelNormalWS = ConvertedNodeSimple<UMaterialExpressionPixelNormalWS>;

class ConvertedNodeVertexColor : public ConvertedNode
{
private:
	UMaterialExpressionMaterialFunctionCall* function_ = nullptr;

public:
	ConvertedNodeVertexColor(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
	{
		function_ = NewObject<UMaterialExpressionMaterialFunctionCall>(material);
		ConvertedNodeHelper::AddExpression(material, function_);

		EffekseerUE::UEFSoftObjectPath assetPath("/Effekseer/MaterialFunctions/EfkVertexColor.EfkVertexColor");
		UMaterialFunction* func = Cast<UMaterialFunction>(assetPath.TryLoad());
		function_->SetMaterialFunction(func);
	}

	UMaterialExpression* GetExpression() const override
	{
		return function_;
	}

	UMaterialExpression* GetExpressions(int32_t ind) const override
	{
		if (ind == 0)
			return function_;
		return nullptr;
	}

	int32_t GetExpressionCount() const
	{
		return 1;
	}

	ConvertedNodeOutputConnector GetNodeOutputConnector(int32_t index) const override
	{
		return ConvertedNodeOutputConnector(function_, index);
	}
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
		ConvertedNodeHelper::AddExpression(material, expression_);

		EffekseerUE::UEFSoftObjectPath assetPath("/Engine/Functions/Engine_MaterialFunctions02/WorldPositionOffset/ObjectScale.ObjectScale");
		UMaterialFunction* func = Cast<UMaterialFunction>(assetPath.TryLoad());
		expression_->SetMaterialFunction(func);
	}

	UMaterialExpression* GetExpression() const override
	{
		return expression_;
	}
};
