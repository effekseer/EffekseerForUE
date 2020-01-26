
#pragma once

#include "ConvertedNode.h"
#include "EffekseerEffect.h"
#include "Materials/MaterialExpressionScalarParameter.h"
#include "Materials/MaterialExpressionVectorParameter.h"
#include "../NativeEffekseerMaterialContext.h"


class ConvertedNodeParameter1 : public ConvertedNode
{
private:
	std::shared_ptr<EffekseerMaterial::Node> effekseerNode_;
	UMaterialExpressionScalarParameter* expression_ = nullptr;

public:
	ConvertedNodeParameter1(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
		: effekseerNode_(effekseerNode)
	{
		expression_ = NewObject<UMaterialExpressionScalarParameter>(material);
		material->Expressions.Add(expression_);
		expression_->ParameterName = FName(effekseerMaterial->uniformNames[effekseerNode_->GUID].c_str());
		expression_->DefaultValue = effekseerNode_->GetProperty("Value")->Floats[0];
	}

	UMaterialExpression* GetExpression() const override { return expression_; }

	void Connect(int targetInd, std::shared_ptr<ConvertedNode> outputNode) override
	{
	}
};

class ConvertedNodeParameter4 : public ConvertedNode
{
private:
	std::shared_ptr<EffekseerMaterial::Node> effekseerNode_;
	UMaterialExpressionVectorParameter* expression_ = nullptr;

public:
	ConvertedNodeParameter4(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
		: effekseerNode_(effekseerNode)
	{
		expression_ = NewObject<UMaterialExpressionVectorParameter>(material);
		material->Expressions.Add(expression_);
		expression_->ParameterName = FName(effekseerMaterial->uniformNames[effekseerNode_->GUID].c_str());
		expression_->DefaultValue.R = effekseerNode_->GetProperty("Value")->Floats[0];
		expression_->DefaultValue.G = effekseerNode_->GetProperty("Value")->Floats[1];
		expression_->DefaultValue.B = effekseerNode_->GetProperty("Value")->Floats[2];
		expression_->DefaultValue.A = effekseerNode_->GetProperty("Value")->Floats[3];
	}

	UMaterialExpression* GetExpression() const override { return expression_; }

	void Connect(int targetInd, std::shared_ptr<ConvertedNode> outputNode) override
	{
	}
};

class ConvertedNodeCustomData1 : public ConvertedNode
{
private:
	UMaterialExpressionComponentMask* expression_ = nullptr;
	UMaterialExpressionMaterialFunctionCall* functions_ = nullptr;

public:
	ConvertedNodeCustomData1(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
	{
		expression_ = NewObject<UMaterialExpressionComponentMask>(material);
		material->Expressions.Add(expression_);

		functions_ = NewObject<UMaterialExpressionMaterialFunctionCall>(material);
		material->Expressions.Add(functions_);

		FStringAssetReference assetPath("/Effekseer/MaterialFunctions/EfkCustomData1.EfkCustomData1");
		UMaterialFunction* func = Cast<UMaterialFunction>(assetPath.TryLoad());
		functions_->SetMaterialFunction(func);

		expression_->Input.Expression = functions_;
		expression_->R = effekseerNode->GetProperty("R")->Floats[0] > 0 ? 1 : 0;
		expression_->G = effekseerNode->GetProperty("G")->Floats[0] > 0 ? 1 : 0;
		expression_->B = effekseerNode->GetProperty("B")->Floats[0] > 0 ? 1 : 0;
		expression_->A = effekseerNode->GetProperty("A")->Floats[0] > 0 ? 1 : 0;
	}

	UMaterialExpression* GetExpression() const override { return expression_; }

	void Connect(int targetInd, std::shared_ptr<ConvertedNode> outputNode) override
	{
	}
};

class ConvertedNodeCustomData2 : public ConvertedNode
{
private:
	UMaterialExpressionComponentMask* expression_ = nullptr;
	UMaterialExpressionMaterialFunctionCall* functions_ = nullptr;

public:
	ConvertedNodeCustomData2(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
	{
		expression_ = NewObject<UMaterialExpressionComponentMask>(material);
		material->Expressions.Add(expression_);

		functions_ = NewObject<UMaterialExpressionMaterialFunctionCall>(material);
		material->Expressions.Add(functions_);

		FStringAssetReference assetPath("/Effekseer/MaterialFunctions/EfkCustomData2.EfkCustomData2");
		UMaterialFunction* func = Cast<UMaterialFunction>(assetPath.TryLoad());
		functions_->SetMaterialFunction(func);

		expression_->Input.Expression = functions_;
		expression_->R = effekseerNode->GetProperty("R")->Floats[0] > 0 ? 1 : 0;
		expression_->G = effekseerNode->GetProperty("G")->Floats[0] > 0 ? 1 : 0;
		expression_->B = effekseerNode->GetProperty("B")->Floats[0] > 0 ? 1 : 0;
		expression_->A = effekseerNode->GetProperty("A")->Floats[0] > 0 ? 1 : 0;
	}

	UMaterialExpression* GetExpression() const override { return expression_; }

	void Connect(int targetInd, std::shared_ptr<ConvertedNode> outputNode) override
	{
	}
};