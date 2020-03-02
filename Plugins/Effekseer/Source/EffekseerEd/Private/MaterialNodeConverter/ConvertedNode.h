
#pragma once

#include "CoreMinimal.h"
#include "Materials/MaterialExpression.h"
#include <memory>

struct ConvertedNodeOutputConnector
{
	UMaterialExpression* Expression = nullptr;
	int32_t OutputIndex = 0;

	ConvertedNodeOutputConnector(UMaterialExpression* e, int32_t ind)
		: Expression(e)
		, OutputIndex(ind)
	{
	}

	void Apply(FExpressionInput& input)
	{
		input.Expression = Expression;
		input.OutputIndex = OutputIndex;
	}
};

class ConvertedNode
{
private:
public:
	ConvertedNode() = default;
	virtual ~ConvertedNode() = default;

	virtual UMaterialExpression* GetExpression() const { return nullptr; }

	virtual UMaterialExpression* GetExpressions(int32_t i) const { return GetExpression(); }

	virtual int32_t GetExpressionCount() const { return 1; }

	virtual ConvertedNodeOutputConnector GetNodeOutputConnector(int32_t index) const
	{
		return ConvertedNodeOutputConnector(GetExpressions(index), 0);
	}

	virtual void Connect(int targetInd, std::shared_ptr<ConvertedNode> outputNode, int32_t outputNodePinIndex) {}

	virtual void SetEditorPosition(int32_t x, int32_t y)
	{
		for (int32_t i = 0; i < GetExpressionCount(); i++)
		{
			if (GetExpressions(i) != nullptr)
			{
				GetExpressions(i)->MaterialExpressionEditorX = x;
				GetExpressions(i)->MaterialExpressionEditorY = y;
			}
		}
	}
};

template<class T>
class ConvertedNodeSimple : public ConvertedNode
{
private:
	T* expression_ = nullptr;

public:
	ConvertedNodeSimple(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
	{
		expression_ = NewObject<T>(material);
		material->Expressions.Add(expression_);
	}

	UMaterialExpression* GetExpression() const override { return expression_; }
};