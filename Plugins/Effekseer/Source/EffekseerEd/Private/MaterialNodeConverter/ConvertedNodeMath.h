
#pragma once

#include "ConvertedNode.h"
#include "EffekseerEffect.h"
#include "Materials/MaterialExpressionAbs.h"
#include "Materials/MaterialExpressionAdd.h"
#include "Materials/MaterialExpressionSubtract.h"
#include "Materials/MaterialExpressionMultiply.h"
#include "Materials/MaterialExpressionDivide.h"
#include "Materials/MaterialExpressionFmod.h"
#include "Materials/MaterialExpressionCeil.h"
#include "Materials/MaterialExpressionFloor.h"
#include "Materials/MaterialExpressionFrac.h"
#include "Materials/MaterialExpressionMax.h"
#include "Materials/MaterialExpressionMin.h"
#include "Materials/MaterialExpressionPower.h"
#include "Materials/MaterialExpressionSquareRoot.h"
#include "Materials/MaterialExpressionClamp.h"
#include "Materials/MaterialExpressionDotProduct.h"
#include "Materials/MaterialExpressionCrossProduct.h"
#include "Materials/MaterialExpressionNormalize.h"
#include "Materials/MaterialExpressionLinearInterpolate.h"
#include "Materials/MaterialExpressionOneMinus.h"

#include "Materials/MaterialExpressionComponentMask.h"
#include "Materials/MaterialExpressionAppendVector.h"

#include "Materials/MaterialExpressionSine.h"
#include "Materials/MaterialExpressionArctangent2.h"

#include "Materials/MaterialExpressionConstant.h"
#include "../NativeEffekseerMaterialContext.h"

template<class T>
class ConvertedNodeOneInput : public ConvertedNode
{
private:
	T* expression_ = nullptr;

public:
	ConvertedNodeOneInput(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
	{
		expression_ = NewObject<T>(material);
		material->Expressions.Add(expression_);
	}

	UMaterialExpression* GetExpression() const override { return expression_; }

	void Connect(int targetInd, std::shared_ptr<ConvertedNode> outputNode, int32_t outputNodePinIndex) override
	{
		outputNode->GetNodeOutputConnector(outputNodePinIndex).Apply(expression_->Input);
	}
};

template<class T>
class ConvertedNodeOneVectorInput : public ConvertedNode
{
private:
	T* expression_ = nullptr;

public:
	ConvertedNodeOneVectorInput(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
	{
		expression_ = NewObject<T>(material);
		material->Expressions.Add(expression_);
	}

	UMaterialExpression* GetExpression() const override { return expression_; }

	void Connect(int targetInd, std::shared_ptr<ConvertedNode> outputNode, int32_t outputNodePinIndex) override
	{
		outputNode->GetNodeOutputConnector(outputNodePinIndex).Apply(expression_->VectorInput);
	}
};

template<class T>
class ConvertedNodeConstABInputProp : public ConvertedNode
{
private:
	std::shared_ptr<EffekseerMaterial::Node> effekseerNode_;
	T* expression_ = nullptr;

public:
	ConvertedNodeConstABInputProp(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
		: effekseerNode_(effekseerNode)
	{
		expression_ = NewObject<T>(material);
		material->Expressions.Add(expression_);

		expression_->ConstA = effekseerNode_->GetProperty("ConstValue1")->Floats[0];
		expression_->ConstB = effekseerNode_->GetProperty("ConstValue2")->Floats[0];
	}

	UMaterialExpression* GetExpression() const override { return expression_; }

	void Connect(int targetInd, std::shared_ptr<ConvertedNode> outputNode, int32_t outputNodePinIndex) override
	{
		if (targetInd == effekseerNode_->GetInputPinIndex("Value1"))
		{
			outputNode->GetNodeOutputConnector(outputNodePinIndex).Apply(expression_->A);
		}

		if (targetInd == effekseerNode_->GetInputPinIndex("Value2"))
		{
			outputNode->GetNodeOutputConnector(outputNodePinIndex).Apply(expression_->B);
		}
	}
};

template<class T>
class ConvertedNodeABInput : public ConvertedNode
{
private:
	std::shared_ptr<EffekseerMaterial::Node> effekseerNode_;
	T* expression_ = nullptr;

public:
	ConvertedNodeABInput(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
		: effekseerNode_(effekseerNode)
	{
		expression_ = NewObject<T>(material);
		material->Expressions.Add(expression_);
	}

	UMaterialExpression* GetExpression() const override { return expression_; }

	void Connect(int targetInd, std::shared_ptr<ConvertedNode> outputNode, int32_t outputNodePinIndex) override
	{
		if (targetInd == effekseerNode_->GetInputPinIndex("Value1"))
		{
			outputNode->GetNodeOutputConnector(outputNodePinIndex).Apply(expression_->A);
		}

		if (targetInd == effekseerNode_->GetInputPinIndex("Value2"))
		{
			outputNode->GetNodeOutputConnector(outputNodePinIndex).Apply(expression_->B);
		}
	}
};



using ConvertedNodeAbs = ConvertedNodeOneInput<UMaterialExpressionAbs>;

class ConvertedNodeSine : public ConvertedNode
{
private:
	std::shared_ptr<EffekseerMaterial::Node> effekseerNode_;
	UMaterialExpressionSine* expression_ = nullptr;

public:
	ConvertedNodeSine(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
		: effekseerNode_(effekseerNode)
	{
		expression_ = NewObject<UMaterialExpressionSine>(material);
		material->Expressions.Add(expression_);

		expression_->Period = 3.141592f * 2.0f;
	}

	UMaterialExpression* GetExpression() const override { return expression_; }

	void Connect(int targetInd, std::shared_ptr<ConvertedNode> outputNode, int32_t outputNodePinIndex) override
	{
		outputNode->GetNodeOutputConnector(outputNodePinIndex).Apply(expression_->Input);
	}
};

class ConvertedNodeArctangent2 : public ConvertedNode
{
private:
	std::shared_ptr<EffekseerMaterial::Node> effekseerNode_;
	UMaterialExpressionArctangent2* expression_ = nullptr;
	UMaterialExpressionConstant* expression1_ = nullptr;
	UMaterialExpressionConstant* expression2_ = nullptr;
public:
	ConvertedNodeArctangent2(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
		: effekseerNode_(effekseerNode)
	{
#if ENGINE_MINOR_VERSION >= 19 // TODO Check correct version
		expression_ = NewObject<UMaterialExpressionArctangent2>(material);
#else
		UE_LOG(LogTemp, Warning, TEXT("atan2 is unsupported"));
#endif
		material->Expressions.Add(expression_);

		if (effekseerMaterial->material->GetConnectedPins(effekseerNode->InputPins[effekseerNode_->GetInputPinIndex("Y")]).size() == 0)
		{
			expression1_ = NewObject<UMaterialExpressionConstant>(material);
			material->Expressions.Add(expression1_);
			expression1_->R = effekseerNode->Properties[effekseerNode_->GetInputPinIndex("Y")]->Floats[0];
			expression_->Y.Expression = expression1_;
		}

		if (effekseerMaterial->material->GetConnectedPins(effekseerNode->InputPins[effekseerNode_->GetInputPinIndex("X")]).size() == 0)
		{
			expression2_ = NewObject<UMaterialExpressionConstant>(material);
			material->Expressions.Add(expression2_);
			expression2_->R = effekseerNode->Properties[effekseerNode_->GetInputPinIndex("X")]->Floats[0];
			expression_->X.Expression = expression2_;
		}
	}

	UMaterialExpression* GetExpression() const override { return expression_; }

	UMaterialExpression* GetExpressions(int32_t ind) const override {
		if (ind == 0) return expression_;
		if (ind == 1) return expression1_;
		if (ind == 2) return expression2_;
		return nullptr;
	}

	int32_t GetExpressionCount() const { return 3; }

	void Connect(int targetInd, std::shared_ptr<ConvertedNode> outputNode, int32_t outputNodePinIndex) override
	{
		if (expression_ == nullptr)
		{
			return;
		}

		if (targetInd == effekseerNode_->GetInputPinIndex("Y"))
		{
			outputNode->GetNodeOutputConnector(outputNodePinIndex).Apply(expression_->Y);
		}

		if (targetInd == effekseerNode_->GetInputPinIndex("X"))
		{
			outputNode->GetNodeOutputConnector(outputNodePinIndex).Apply(expression_->X);
		}
	}
};

using ConvertedNodeAdd = ConvertedNodeConstABInputProp<UMaterialExpressionAdd>;
using ConvertedNodeSubtract = ConvertedNodeConstABInputProp<UMaterialExpressionSubtract>;
using ConvertedNodeMultiply = ConvertedNodeConstABInputProp<UMaterialExpressionMultiply>;
using ConvertedNodeDivide = ConvertedNodeConstABInputProp<UMaterialExpressionDivide>;

class ConvertedNodeFmod : public ConvertedNode
{
private:
	std::shared_ptr<EffekseerMaterial::Node> effekseerNode_;
	UMaterialExpressionConstant* expression1_ = nullptr;
	UMaterialExpressionConstant* expression2_ = nullptr;
	UMaterialExpressionFmod* expression_ = nullptr;

public:
	ConvertedNodeFmod(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
		: effekseerNode_(effekseerNode)
	{
		expression_ = NewObject<UMaterialExpressionFmod>(material);
		material->Expressions.Add(expression_);

		if (effekseerMaterial->material->GetConnectedPins(effekseerNode->InputPins[0]).size() == 0)
		{
			expression1_ = NewObject<UMaterialExpressionConstant>(material);
			material->Expressions.Add(expression1_);
			expression1_->R = effekseerNode->Properties[0]->Floats[0];
			expression_->A.Expression = expression1_;
		}

		if (effekseerMaterial->material->GetConnectedPins(effekseerNode->InputPins[1]).size() == 0)
		{
			expression2_ = NewObject<UMaterialExpressionConstant>(material);
			material->Expressions.Add(expression2_);
			expression2_->R = effekseerNode->Properties[1]->Floats[0];
			expression_->B.Expression = expression2_;
		}
	}

	UMaterialExpression* GetExpression() const override { return expression_; }

	UMaterialExpression* GetExpressions(int32_t ind) const override {
		if (ind == 0) return expression_;
		if (ind == 1) return expression1_;
		if (ind == 2) return expression2_;
		return nullptr;
	}

	int32_t GetExpressionCount() const { return 3; }

	void Connect(int targetInd, std::shared_ptr<ConvertedNode> outputNode, int32_t outputNodePinIndex) override
	{
		if (targetInd == effekseerNode_->GetInputPinIndex("Value1"))
		{
			outputNode->GetNodeOutputConnector(outputNodePinIndex).Apply(expression_->A);
		}

		if (targetInd == effekseerNode_->GetInputPinIndex("Value2"))
		{
			outputNode->GetNodeOutputConnector(outputNodePinIndex).Apply(expression_->B);
		}
	}
};

class ConvertedNodeStep : public ConvertedNode
{
private:
	std::shared_ptr<EffekseerMaterial::Node> effekseerNode_;
	UMaterialExpressionMaterialFunctionCall* expression_ = nullptr;

public:
	ConvertedNodeStep(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
		: effekseerNode_(effekseerNode)
	{
		expression_ = NewObject<UMaterialExpressionMaterialFunctionCall>(material);
		material->Expressions.Add(expression_);

		FStringAssetReference assetPath("/Effekseer/MaterialFunctions/EfkStep.EfkStep");
		UMaterialFunction* func = Cast<UMaterialFunction>(assetPath.TryLoad());
		expression_->SetMaterialFunction(func);
	}

	UMaterialExpression* GetExpression() const override { return expression_; }

	void Connect(int targetInd, std::shared_ptr<ConvertedNode> outputNode, int32_t outputNodePinIndex) override
	{
		if (targetInd == effekseerNode_->GetInputPinIndex("Edge"))
		{
			outputNode->GetNodeOutputConnector(outputNodePinIndex).Apply(*expression_->GetInput(0));
		}

		if (targetInd == effekseerNode_->GetInputPinIndex("Value"))
		{
			outputNode->GetNodeOutputConnector(outputNodePinIndex).Apply(*expression_->GetInput(1));
		}
	}
};

#if ENGINE_MINOR_VERSION >= 19 // TODO Check correct version
using ConvertedNodeCeil = ConvertedNodeOneInput<UMaterialExpressionCeil>;
using ConvertedNodeFloor = ConvertedNodeOneInput<UMaterialExpressionFloor>;
using ConvertedNodeFrac = ConvertedNodeOneInput<UMaterialExpressionFrac>;
#endif

using ConvertedNodeMax = ConvertedNodeConstABInputProp<UMaterialExpressionMax>;
using ConvertedNodeMin = ConvertedNodeConstABInputProp<UMaterialExpressionMin>;


class ConvertedNodePower : public ConvertedNode
{
private:
	std::shared_ptr<EffekseerMaterial::Node> effekseerNode_;
	UMaterialExpressionPower* expression_ = nullptr;

public:
	ConvertedNodePower(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
		: effekseerNode_(effekseerNode)
	{
		expression_ = NewObject<UMaterialExpressionPower>(material);
		material->Expressions.Add(expression_);

		expression_->ConstExponent = effekseerNode_->GetProperty("Exp")->Floats[0];
	}

	UMaterialExpression* GetExpression() const override { return expression_; }

	void Connect(int targetInd, std::shared_ptr<ConvertedNode> outputNode, int32_t outputNodePinIndex) override
	{
		if (targetInd == effekseerNode_->GetInputPinIndex("Base"))
		{
			outputNode->GetNodeOutputConnector(outputNodePinIndex).Apply(expression_->Base);
		}

		if (targetInd == effekseerNode_->GetInputPinIndex("Exp"))
		{
			outputNode->GetNodeOutputConnector(outputNodePinIndex).Apply(expression_->Exponent);
		}
	}
};

using ConvertedNodeSquareRoot = ConvertedNodeOneInput<UMaterialExpressionSquareRoot>;

class ConvertedNodeClamp : public ConvertedNode
{
private:
	std::shared_ptr<EffekseerMaterial::Node> effekseerNode_;
	UMaterialExpressionClamp* expression_ = nullptr;

public:
	ConvertedNodeClamp(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
		: effekseerNode_(effekseerNode)
	{
		expression_ = NewObject<UMaterialExpressionClamp>(material);
		material->Expressions.Add(expression_);

		expression_->MinDefault = effekseerNode_->GetProperty("Min")->Floats[0];
		expression_->MaxDefault = effekseerNode_->GetProperty("Max")->Floats[0];
	}

	UMaterialExpression* GetExpression() const override { return expression_; }

	void Connect(int targetInd, std::shared_ptr<ConvertedNode> outputNode, int32_t outputNodePinIndex) override
	{
		if (targetInd == effekseerNode_->GetInputPinIndex("Input"))
		{
			outputNode->GetNodeOutputConnector(outputNodePinIndex).Apply(expression_->Input);
		}

		if (targetInd == effekseerNode_->GetInputPinIndex("Min"))
		{
			outputNode->GetNodeOutputConnector(outputNodePinIndex).Apply(expression_->Min);
		}

		if (targetInd == effekseerNode_->GetInputPinIndex("Max"))
		{
			outputNode->GetNodeOutputConnector(outputNodePinIndex).Apply(expression_->Max);
		}
	}
};

using ConvertedNodeDotProduct = ConvertedNodeABInput<UMaterialExpressionDotProduct>;
using ConvertedNodeCrossProduct = ConvertedNodeABInput<UMaterialExpressionCrossProduct>;
using ConvertedNodeNormalize = ConvertedNodeOneVectorInput<UMaterialExpressionNormalize>;

class ConvertedNodeLinearInterpolate : public ConvertedNode
{
private:
	std::shared_ptr<EffekseerMaterial::Node> effekseerNode_;
	UMaterialExpressionLinearInterpolate* expression_ = nullptr;

public:
	ConvertedNodeLinearInterpolate(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
		: effekseerNode_(effekseerNode)
	{
		expression_ = NewObject<UMaterialExpressionLinearInterpolate>(material);
		material->Expressions.Add(expression_);

		expression_->ConstA = effekseerNode_->GetProperty("Value1")->Floats[0];
		expression_->ConstB = effekseerNode_->GetProperty("Value2")->Floats[0];
		expression_->ConstAlpha = effekseerNode_->GetProperty("Alpha")->Floats[0];
	}

	UMaterialExpression* GetExpression() const override { return expression_; }

	void Connect(int targetInd, std::shared_ptr<ConvertedNode> outputNode, int32_t outputNodePinIndex) override
	{
		if (targetInd == effekseerNode_->GetInputPinIndex("Value1"))
		{
			outputNode->GetNodeOutputConnector(outputNodePinIndex).Apply(expression_->A);
		}

		if (targetInd == effekseerNode_->GetInputPinIndex("Value2"))
		{
			outputNode->GetNodeOutputConnector(outputNodePinIndex).Apply(expression_->B);
		}

		if (targetInd == effekseerNode_->GetInputPinIndex("Alpha"))
		{
			outputNode->GetNodeOutputConnector(outputNodePinIndex).Apply(expression_->Alpha);
		}
	}
};

using ConvertedNodeOneMinus = ConvertedNodeOneInput<UMaterialExpressionOneMinus>;

class ConvertedNodeComponentMask : public ConvertedNode
{
private:
	std::shared_ptr<EffekseerMaterial::Node> effekseerNode_;
	UMaterialExpressionComponentMask* expression_ = nullptr;

public:
	ConvertedNodeComponentMask(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
		: effekseerNode_(effekseerNode)
	{
		expression_ = NewObject<UMaterialExpressionComponentMask>(material);
		material->Expressions.Add(expression_);
		expression_->R = effekseerNode->GetProperty("R")->Floats[0] > 0 ? 1 : 0;
		expression_->G = effekseerNode->GetProperty("G")->Floats[0] > 0 ? 1 : 0;
		expression_->B = effekseerNode->GetProperty("B")->Floats[0] > 0 ? 1 : 0;
		expression_->A = effekseerNode->GetProperty("A")->Floats[0] > 0 ? 1 : 0;
	}

	UMaterialExpression* GetExpression() const override { return expression_; }

	void Connect(int targetInd, std::shared_ptr<ConvertedNode> outputNode, int32_t outputNodePinIndex) override
	{
		outputNode->GetNodeOutputConnector(outputNodePinIndex).Apply(expression_->Input);
	}
};

using ConvertedNodeAppendVector = ConvertedNodeABInput<UMaterialExpressionAppendVector>;
