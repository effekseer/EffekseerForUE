#include "ConvertedNodeGradient.h"

#include "Materials/MaterialExpressionConstant4Vector.h"
#include "Materials/MaterialExpressionVectorParameter.h"

ConvertedNodeSampleGradient::ConvertedNodeSampleGradient(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
	: effekseerNode_(effekseerNode)
{
	expression_ = NewObject<UMaterialExpressionMaterialFunctionCall>(material);
	material->Expressions.Add(expression_);

	FStringAssetReference assetPath("/Effekseer/MaterialFunctions/EfkSampleGradient.EfkSampleGradient");
	UMaterialFunction* func = Cast<UMaterialFunction>(assetPath.TryLoad());
	expression_->SetMaterialFunction(func);

	auto connectedPins = effekseerMaterial->material->GetConnectedPins(effekseerNode->InputPins[0]);
	if (effekseerMaterial->material->GetConnectedPins(effekseerNode->InputPins[0]).size() == 0)
	{
		const auto& gradientData = effekseerNode->Properties[0]->GradientData;

		for (size_t i = 0; i < 13; i++)
		{
			auto paramExpression = NewObject<UMaterialExpressionConstant4Vector>(material);
			material->Expressions.Add(paramExpression);
			parameters_[i] = paramExpression;
			expression_->GetInput(i)->Connect(0, paramExpression);
		}
	}
	else
	{
		const auto node = connectedPins[0]->Parent.lock();

		if (node->Parameter->Type == EffekseerMaterial::NodeType::Gradient)
		{
		}
		else if (node->Parameter->Type == EffekseerMaterial::NodeType::GradientParameter)
		{
		}
	}
}

UMaterialExpression* ConvertedNodeSampleGradient::GetExpression() const
{
	return expression_;
}

UMaterialExpression* ConvertedNodeSampleGradient::GetExpressions(int32_t ind) const
{
	if (ind == 0)
	{
		return expression_;
	}

	return parameters_[ind - 1];
}

int32_t ConvertedNodeSampleGradient::GetExpressionCount() const
{
	return 14;
}

void ConvertedNodeSampleGradient::Connect(int targetInd, std::shared_ptr<ConvertedNode> outputNode, int32_t outputNodePinIndex)
{
}