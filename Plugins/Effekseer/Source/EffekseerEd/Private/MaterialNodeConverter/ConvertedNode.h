
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

	virtual void Connect(int targetInd, std::shared_ptr<ConvertedNode> outputNode) {}
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

	void Connect(int targetInd, std::shared_ptr<ConvertedNode> outputNode) override
	{
	}
};