
#pragma once

#include "../NativeEffekseerMaterialContext.h"
#include "ConvertedNode.h"
#include "EffekseerEffect.h"
#include "Materials/MaterialExpressionCameraPositionWS.h"
#include "Materials/MaterialExpressionConstant.h"
#include "Materials/MaterialExpressionConstant2Vector.h"
#include "Materials/MaterialExpressionConstant3Vector.h"
#include "Materials/MaterialExpressionConstant4Vector.h"

class ConvertedNodeConstant1 : public ConvertedNode
{
private:
	std::shared_ptr<EffekseerMaterial::Node> effekseerNode_;
	UMaterialExpressionConstant* expression_ = nullptr;

public:
	ConvertedNodeConstant1(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
		: effekseerNode_(effekseerNode)
	{
		expression_ = NewObject<UMaterialExpressionConstant>(material);
		ConvertedNodeHelper::AddExpression(material, expression_);

		expression_->R = effekseerNode_->GetProperty("Value")->Floats[0];
	}

	UMaterialExpression* GetExpression() const override
	{
		return expression_;
	}
};

class ConvertedNodeConstant2 : public ConvertedNode
{
private:
	std::shared_ptr<EffekseerMaterial::Node> effekseerNode_;
	UMaterialExpressionConstant2Vector* expression_ = nullptr;

public:
	ConvertedNodeConstant2(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
		: effekseerNode_(effekseerNode)
	{
		expression_ = NewObject<UMaterialExpressionConstant2Vector>(material);
		ConvertedNodeHelper::AddExpression(material, expression_);

		expression_->R = effekseerNode_->GetProperty("Value")->Floats[0];
		expression_->G = effekseerNode_->GetProperty("Value")->Floats[1];
	}

	UMaterialExpression* GetExpression() const override
	{
		return expression_;
	}
};

class ConvertedNodeConstant3 : public ConvertedNode
{
private:
	std::shared_ptr<EffekseerMaterial::Node> effekseerNode_;
	UMaterialExpressionConstant3Vector* expression_ = nullptr;

public:
	ConvertedNodeConstant3(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
		: effekseerNode_(effekseerNode)
	{
		expression_ = NewObject<UMaterialExpressionConstant3Vector>(material);
		ConvertedNodeHelper::AddExpression(material, expression_);

		expression_->Constant.R = effekseerNode_->GetProperty("Value")->Floats[0];
		expression_->Constant.G = effekseerNode_->GetProperty("Value")->Floats[1];
		expression_->Constant.B = effekseerNode_->GetProperty("Value")->Floats[2];
	}

	UMaterialExpression* GetExpression() const override
	{
		return expression_;
	}
};

class ConvertedNodeConstant4 : public ConvertedNode
{
private:
	std::shared_ptr<EffekseerMaterial::Node> effekseerNode_;
	UMaterialExpressionConstant4Vector* expression_ = nullptr;

public:
	ConvertedNodeConstant4(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
		: effekseerNode_(effekseerNode)
	{
		expression_ = NewObject<UMaterialExpressionConstant4Vector>(material);
		ConvertedNodeHelper::AddExpression(material, expression_);

		expression_->Constant.R = effekseerNode_->GetProperty("Value")->Floats[0];
		expression_->Constant.G = effekseerNode_->GetProperty("Value")->Floats[1];
		expression_->Constant.B = effekseerNode_->GetProperty("Value")->Floats[2];
		expression_->Constant.A = effekseerNode_->GetProperty("Value")->Floats[3];
	}

	UMaterialExpression* GetExpression() const override
	{
		return expression_;
	}
};

class ConvertedNodeTime : public ConvertedNode
{
private:
	std::shared_ptr<EffekseerMaterial::Node> effekseerNode_;
	UMaterialExpressionMaterialFunctionCall* expression_ = nullptr;

public:
	ConvertedNodeTime(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
		: effekseerNode_(effekseerNode)
	{
		expression_ = NewObject<UMaterialExpressionMaterialFunctionCall>(material);
		ConvertedNodeHelper::AddExpression(material, expression_);

		EffekseerUE::UEFSoftObjectPath assetPath("/Effekseer/MaterialFunctions/EfkTime.EfkTime");
		UMaterialFunction* func = Cast<UMaterialFunction>(assetPath.TryLoad());
		expression_->SetMaterialFunction(func);
	}

	UMaterialExpression* GetExpression() const override
	{
		return expression_;
	}
};

class ConvertedNodeCameraPositionWS : public ConvertedNode
{
private:
	std::shared_ptr<EffekseerMaterial::Node> effekseerNode_;
	UMaterialExpressionMaterialFunctionCall* expression_ = nullptr;

public:
	ConvertedNodeCameraPositionWS(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
		: effekseerNode_(effekseerNode)
	{
		expression_ = NewObject<UMaterialExpressionMaterialFunctionCall>(material);
		ConvertedNodeHelper::AddExpression(material, expression_);

		EffekseerUE::UEFSoftObjectPath assetPath("/Effekseer/MaterialFunctions/EfkCameraPosition.EfkCameraPosition");
		UMaterialFunction* func = Cast<UMaterialFunction>(assetPath.TryLoad());
		expression_->SetMaterialFunction(func);
	}

	UMaterialExpression* GetExpression() const override
	{
		return expression_;
	}
};

class ConvertedNodeEffectScale : public ConvertedNode
{
private:
	std::shared_ptr<EffekseerMaterial::Node> effekseerNode_;
	UMaterialExpressionMaterialFunctionCall* expression_ = nullptr;

public:
	ConvertedNodeEffectScale(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
		: effekseerNode_(effekseerNode)
	{
		expression_ = NewObject<UMaterialExpressionMaterialFunctionCall>(material);
		ConvertedNodeHelper::AddExpression(material, expression_);

		EffekseerUE::UEFSoftObjectPath assetPath("/Effekseer/MaterialFunctions/EfkEffectScale.EfkEffectScale");
		UMaterialFunction* func = Cast<UMaterialFunction>(assetPath.TryLoad());
		expression_->SetMaterialFunction(func);
	}

	UMaterialExpression* GetExpression() const override
	{
		return expression_;
	}
};
