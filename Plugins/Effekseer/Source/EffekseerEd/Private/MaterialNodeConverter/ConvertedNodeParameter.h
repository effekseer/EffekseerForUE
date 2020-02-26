
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
};

class ConvertedNodeParameter2 : public ConvertedNode
{
private:
	std::shared_ptr<EffekseerMaterial::Node> effekseerNode_;
	UMaterialExpressionComponentMask* expression_ = nullptr;
	UMaterialExpressionVectorParameter* function_ = nullptr;

public:
	ConvertedNodeParameter2(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
		: effekseerNode_(effekseerNode)
	{
		expression_ = NewObject<UMaterialExpressionComponentMask>(material);
		material->Expressions.Add(expression_);

		function_ = NewObject<UMaterialExpressionVectorParameter>(material);
		material->Expressions.Add(function_);
		function_->ParameterName = FName(effekseerMaterial->uniformNames[effekseerNode_->GUID].c_str());
		function_->DefaultValue.R = effekseerNode_->GetProperty("Value")->Floats[0];
		function_->DefaultValue.G = effekseerNode_->GetProperty("Value")->Floats[1];

		expression_->Input.Expression = function_;
		expression_->R = 1;
		expression_->G = 1;
		expression_->B = 0;
		expression_->A = 0;
	}

	UMaterialExpression* GetExpression() const override { return expression_; }

	UMaterialExpression* GetExpressions(int32_t ind) const override {
		if (ind == 0) return expression_;
		if (ind == 1) return function_;
		return nullptr;
	}

	int32_t GetExpressionCount() const { return 2; }
};

class ConvertedNodeParameter3 : public ConvertedNode
{
private:
	std::shared_ptr<EffekseerMaterial::Node> effekseerNode_;
	UMaterialExpressionComponentMask* expression_ = nullptr;
	UMaterialExpressionVectorParameter* function_ = nullptr;

public:
	ConvertedNodeParameter3(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
		: effekseerNode_(effekseerNode)
	{
		expression_ = NewObject<UMaterialExpressionComponentMask>(material);
		material->Expressions.Add(expression_);

		function_ = NewObject<UMaterialExpressionVectorParameter>(material);
		material->Expressions.Add(function_);
		function_->ParameterName = FName(effekseerMaterial->uniformNames[effekseerNode_->GUID].c_str());
		function_->DefaultValue.R = effekseerNode_->GetProperty("Value")->Floats[0];
		function_->DefaultValue.G = effekseerNode_->GetProperty("Value")->Floats[1];
		function_->DefaultValue.B = effekseerNode_->GetProperty("Value")->Floats[2];

		expression_->Input.Expression = function_;
		expression_->R = 1;
		expression_->G = 1;
		expression_->B = 1;
		expression_->A = 0;
	}

	UMaterialExpression* GetExpression() const override { return expression_; }

	UMaterialExpression* GetExpressions(int32_t ind) const override {
		if (ind == 0) return expression_;
		if (ind == 1) return function_;
		return nullptr;
	}

	int32_t GetExpressionCount() const { return 2; }
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
};

class ConvertedNodeCustomData1 : public ConvertedNode
{
private:
	UMaterialExpressionComponentMask* expression_ = nullptr;
	UMaterialExpressionMaterialFunctionCall* function_ = nullptr;

public:
	ConvertedNodeCustomData1(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
	{
		expression_ = NewObject<UMaterialExpressionComponentMask>(material);
		material->Expressions.Add(expression_);

		function_ = NewObject<UMaterialExpressionMaterialFunctionCall>(material);
		material->Expressions.Add(function_);

		FStringAssetReference assetPath("/Effekseer/MaterialFunctions/EfkCustomData1.EfkCustomData1");
		UMaterialFunction* func = Cast<UMaterialFunction>(assetPath.TryLoad());
		function_->SetMaterialFunction(func);

		expression_->Input.Expression = function_;
		expression_->R = effekseerNode->GetProperty("R")->Floats[0] > 0 ? 1 : 0;
		expression_->G = effekseerNode->GetProperty("G")->Floats[0] > 0 ? 1 : 0;
		expression_->B = effekseerNode->GetProperty("B")->Floats[0] > 0 ? 1 : 0;
		expression_->A = effekseerNode->GetProperty("A")->Floats[0] > 0 ? 1 : 0;
	}

	UMaterialExpression* GetExpression() const override { return expression_; }

	UMaterialExpression* GetExpressions(int32_t ind) const override {
		if (ind == 0) return expression_;
		if (ind == 1) return function_;
		return nullptr;
	}

	int32_t GetExpressionCount() const { return 2; }
};

class ConvertedNodeCustomData2 : public ConvertedNode
{
private:
	UMaterialExpressionComponentMask* expression_ = nullptr;
	UMaterialExpressionMaterialFunctionCall* function_ = nullptr;

public:
	ConvertedNodeCustomData2(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
	{
		expression_ = NewObject<UMaterialExpressionComponentMask>(material);
		material->Expressions.Add(expression_);

		function_ = NewObject<UMaterialExpressionMaterialFunctionCall>(material);
		material->Expressions.Add(function_);

		FStringAssetReference assetPath("/Effekseer/MaterialFunctions/EfkCustomData2.EfkCustomData2");
		UMaterialFunction* func = Cast<UMaterialFunction>(assetPath.TryLoad());
		function_->SetMaterialFunction(func);

		expression_->Input.Expression = function_;
		expression_->R = effekseerNode->GetProperty("R")->Floats[0] > 0 ? 1 : 0;
		expression_->G = effekseerNode->GetProperty("G")->Floats[0] > 0 ? 1 : 0;
		expression_->B = effekseerNode->GetProperty("B")->Floats[0] > 0 ? 1 : 0;
		expression_->A = effekseerNode->GetProperty("A")->Floats[0] > 0 ? 1 : 0;
	}

	UMaterialExpression* GetExpression() const override { return expression_; }

	UMaterialExpression* GetExpressions(int32_t ind) const override {
		if (ind == 0) return expression_;
		if (ind == 1) return function_;
		return nullptr;
	}

	int32_t GetExpressionCount() const { return 2; }
};