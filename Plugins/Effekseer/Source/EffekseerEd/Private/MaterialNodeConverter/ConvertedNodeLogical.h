
#pragma once

#include "../NativeEffekseerMaterialContext.h"
#include "ConvertedNode.h"
#include "EffekseerEffect.h"
#include "Materials/MaterialExpressionConstant.h"
#include "Materials/MaterialExpressionIf.h"
#include "Materials/MaterialExpressionMaterialFunctionCall.h"

class ConvertedNodeBranch : public ConvertedNode
{
private:
	std::shared_ptr<EffekseerMaterial::Node> effekseerNode_;
	UMaterialExpressionIf* expression_ = nullptr;
	UMaterialExpressionConstant* expression1_ = nullptr;
	UMaterialExpressionConstant* expression2_ = nullptr;

public:
	ConvertedNodeBranch(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
		: effekseerNode_(effekseerNode)
	{
		expression_ = NewObject<UMaterialExpressionIf>(material);
		ConvertedNodeHelper::AddExpression(material, expression_);
		expression_->ConstB = 0.5f;
		expression_->EqualsThreshold = 0.0f;

		if (effekseerMaterial->material->GetConnectedPins(effekseerNode->InputPins[1]).size() == 0)
		{
			expression1_ = NewObject<UMaterialExpressionConstant>(material);
			ConvertedNodeHelper::AddExpression(material, expression1_);
			expression1_->R = effekseerNode->Properties[0]->Floats[0];
			expression_->A.Expression = expression1_;
		}

		if (effekseerMaterial->material->GetConnectedPins(effekseerNode->InputPins[2]).size() == 0)
		{
			expression2_ = NewObject<UMaterialExpressionConstant>(material);
			ConvertedNodeHelper::AddExpression(material, expression2_);
			expression2_->R = effekseerNode->Properties[1]->Floats[0];
			expression_->A.Expression = expression2_;
		}
	}

	UMaterialExpression* GetExpression() const override
	{
		return expression_;
	}

	UMaterialExpression* GetExpressions(int32_t ind) const override
	{
		if (ind == 0)
			return expression_;
		if (ind == 1)
			return expression1_;
		if (ind == 2)
			return expression2_;
		return nullptr;
	}

	int32_t GetExpressionCount() const
	{
		return 3;
	}

	void Connect(int targetInd, std::shared_ptr<ConvertedNode> outputNode, int32_t outputNodePinIndex) override
	{
		if (targetInd == effekseerNode_->GetInputPinIndex("True"))
		{
			outputNode->GetNodeOutputConnector(outputNodePinIndex).Apply(*expression_->GetInput(2));
		}

		else if (targetInd == effekseerNode_->GetInputPinIndex("False"))
		{
			outputNode->GetNodeOutputConnector(outputNodePinIndex).Apply(*expression_->GetInput(4));
		}
	}
};

// TODO
class ConvertedNodeCompare : public ConvertedNode
{
private:
	std::shared_ptr<EffekseerMaterial::Node> effekseerNode_;
	UMaterialExpressionMaterialFunctionCall* expression_ = nullptr;
	UMaterialExpressionConstant* expression1_ = nullptr;
	UMaterialExpressionConstant* expression2_ = nullptr;

public:
	ConvertedNodeCompare(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
		: effekseerNode_(effekseerNode)
	{
		expression_ = NewObject<UMaterialExpressionMaterialFunctionCall>(material);
		ConvertedNodeHelper::AddExpression(material, expression_);

		// const char* op[6] = {"<", "<=", ">", ">=", "==", "!="};
		if (effekseerNode->Properties[1]->Floats[0] == 0)
		{
			EffekseerUE::UEFSoftObjectPath assetPath("/Effekseer/MaterialFunctions/EfkCondition_Less.EfkCondition_Less");
			UMaterialFunction* func = Cast<UMaterialFunction>(assetPath.TryLoad());
			expression_->SetMaterialFunction(func);
		}
		else if (effekseerNode->Properties[1]->Floats[0] == 1)
		{
			EffekseerUE::UEFSoftObjectPath assetPath("/Effekseer/MaterialFunctions/EfkCondition_Less.EfkCondition_LessEqual");
			UMaterialFunction* func = Cast<UMaterialFunction>(assetPath.TryLoad());
			expression_->SetMaterialFunction(func);
		}
		else if (effekseerNode->Properties[1]->Floats[0] == 2)
		{
			EffekseerUE::UEFSoftObjectPath assetPath("/Effekseer/MaterialFunctions/EfkCondition_Less.EfkCondition_Greater");
			UMaterialFunction* func = Cast<UMaterialFunction>(assetPath.TryLoad());
			expression_->SetMaterialFunction(func);
		}
		else if (effekseerNode->Properties[1]->Floats[0] == 3)
		{
			EffekseerUE::UEFSoftObjectPath assetPath("/Effekseer/MaterialFunctions/EfkCondition_Less.EfkCondition_GreaterEqual");
			UMaterialFunction* func = Cast<UMaterialFunction>(assetPath.TryLoad());
			expression_->SetMaterialFunction(func);
		}
		else if (effekseerNode->Properties[1]->Floats[0] == 4)
		{
			EffekseerUE::UEFSoftObjectPath assetPath("/Effekseer/MaterialFunctions/EfkCondition_Less.EfkCondition_Equal");
			UMaterialFunction* func = Cast<UMaterialFunction>(assetPath.TryLoad());
			expression_->SetMaterialFunction(func);
		}
		else if (effekseerNode->Properties[1]->Floats[0] == 5)
		{
			EffekseerUE::UEFSoftObjectPath assetPath("/Effekseer/MaterialFunctions/EfkCondition_Less.EfkCondition_NotEqual");
			UMaterialFunction* func = Cast<UMaterialFunction>(assetPath.TryLoad());
			expression_->SetMaterialFunction(func);
		}

		if (effekseerMaterial->material->GetConnectedPins(effekseerNode->InputPins[1]).size() == 0)
		{
			expression1_ = NewObject<UMaterialExpressionConstant>(material);
			ConvertedNodeHelper::AddExpression(material, expression1_);
			expression1_->R = effekseerNode->Properties[0]->Floats[0];

			ConvertedNodeOutputConnector connecter(expression1_, 0);
			connecter.Apply(*expression_->GetInput(0));
		}

		if (effekseerMaterial->material->GetConnectedPins(effekseerNode->InputPins[2]).size() == 0)
		{
			expression2_ = NewObject<UMaterialExpressionConstant>(material);
			ConvertedNodeHelper::AddExpression(material, expression2_);
			expression2_->R = effekseerNode->Properties[1]->Floats[0];

			ConvertedNodeOutputConnector connecter(expression2_, 0);
			connecter.Apply(*expression_->GetInput(1));
		}
	}

	UMaterialExpression* GetExpression() const override
	{
		return expression_;
	}

	UMaterialExpression* GetExpressions(int32_t ind) const override
	{
		if (ind == 0)
			return expression_;
		if (ind == 1)
			return expression1_;
		if (ind == 2)
			return expression2_;
		return nullptr;
	}

	int32_t GetExpressionCount() const
	{
		return 3;
	}

	void Connect(int targetInd, std::shared_ptr<ConvertedNode> outputNode, int32_t outputNodePinIndex) override
	{
		if (targetInd == effekseerNode_->GetInputPinIndex("A"))
		{
			outputNode->GetNodeOutputConnector(outputNodePinIndex).Apply(*expression_->GetInput(0));
		}

		if (targetInd == effekseerNode_->GetInputPinIndex("B"))
		{
			outputNode->GetNodeOutputConnector(outputNodePinIndex).Apply(*expression_->GetInput(1));
		}
	}
	/*
	* const char* op[6] = {"<", "<=", ">", ">=", "==", "!="};
			Type = NodeType::Compare;
		TypeName = "Compare";
		Group = std::vector<std::string>{"Logical"};

		auto aPin = std::make_shared<PinParameter>();
		aPin->Name = "A";
		aPin->Type = ValueType::Float1;
		InputPins.push_back(aPin);

		auto bPin = std::make_shared<PinParameter>();
		bPin->Name = "B";
		bPin->Type = ValueType::Float1;
		InputPins.push_back(bPin);

		auto output = std::make_shared<PinParameter>();
		output->Name = "Output";
		output->Type = ValueType::Bool;
		OutputPins.push_back(output);

		auto aProp = std::make_shared<NodePropertyParameter>();
		aProp->Name = "A";
		aProp->Type = ValueType::Float1;
		aProp->DefaultValues[0] = 0.0f;
		Properties.push_back(aProp);

		auto bProp = std::make_shared<NodePropertyParameter>();
		bProp->Name = "B";
		bProp->Type = ValueType::Float1;
		bProp->DefaultValues[0] = 0.0f;
		Properties.push_back(bProp);

		auto condProp = std::make_shared<NodePropertyParameter>();
		condProp->Name = "Condition";
		condProp->Type = ValueType::Enum;
		condProp->DefaultValues[0] = 0;
		Properties.push_back(condProp);

		BehaviorComponents = {std::make_shared<NodeParameterBehaviorComponentTwoInputMath>()};
	*/
};

// BoolAnd
// BoolOr
// BoolNot
// IsFrontFace

/*

class NodeBoolAnd : public NodeParameter
{
public:
	NodeBoolAnd()
	{
		Type = NodeType::BoolAnd;
		TypeName = "BoolAnd";
		Group = std::vector<std::string>{"Logical"};

		auto input1 = std::make_shared<PinParameter>();
		input1->Name = "V1";
		input1->Type = ValueType::Bool;
		InputPins.push_back(input1);

		auto input2 = std::make_shared<PinParameter>();
		input2->Name = "V2";
		input2->Type = ValueType::Bool;
		InputPins.push_back(input2);

		auto output = std::make_shared<PinParameter>();
		output->Name = "Output";
		output->Type = ValueType::Bool;
		OutputPins.push_back(output);
	}
};

class NodeBoolOr : public NodeParameter
{
public:
	NodeBoolOr()
	{
		Type = NodeType::BoolOr;
		TypeName = "BoolOr";
		Group = std::vector<std::string>{"Logical"};

		auto input1 = std::make_shared<PinParameter>();
		input1->Name = "V1";
		input1->Type = ValueType::Bool;
		InputPins.push_back(input1);

		auto input2 = std::make_shared<PinParameter>();
		input2->Name = "V2";
		input2->Type = ValueType::Bool;
		InputPins.push_back(input2);

		auto output = std::make_shared<PinParameter>();
		output->Name = "Output";
		output->Type = ValueType::Bool;
		OutputPins.push_back(output);
	}
};

class NodeBoolNot : public NodeParameter
{
public:
	NodeBoolNot()
	{
		Type = NodeType::BoolNot;
		TypeName = "BoolNot";
		Group = std::vector<std::string>{"Logical"};

		auto input = std::make_shared<PinParameter>();
		input->Name = "V";
		input->Type = ValueType::Bool;
		InputPins.push_back(input);

		auto output = std::make_shared<PinParameter>();
		output->Name = "Output";
		output->Type = ValueType::Bool;
		OutputPins.push_back(output);
	}
};

class NodeIsFrontFace : public NodeParameter
{
public:
	NodeIsFrontFace()
	{
		Type = NodeType::IsFrontFace;
		TypeName = "IsFrontFace";
		Group = std::vector<std::string>{"Logical"};

		auto output = std::make_shared<PinParameter>();
		output->Name = "Output";
		output->Type = ValueType::Bool;
		OutputPins.push_back(output);
	}
};

*/