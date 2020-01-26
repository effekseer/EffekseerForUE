
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
