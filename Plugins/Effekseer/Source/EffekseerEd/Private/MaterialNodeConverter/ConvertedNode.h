
#pragma once

#include "CoreMinimal.h"
#include "Materials/MaterialExpression.h"
#include <memory>

class ConvertedNode
{
private:
public:
	ConvertedNode() = default;
	virtual ~ConvertedNode() = default;

	virtual UMaterialExpression* GetExpression() const { return nullptr; }

	virtual UMaterialExpression* GetExpressions(int32_t i) const { return GetExpression(); }

	virtual int32_t GetExpressionCount() const { return 1; }

	virtual void Connect(int targetInd, std::shared_ptr<ConvertedNode> outputNode, int32_t outputNodePinIndex) {}
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