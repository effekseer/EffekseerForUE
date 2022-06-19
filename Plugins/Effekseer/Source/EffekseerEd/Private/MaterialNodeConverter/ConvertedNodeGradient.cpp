#include "ConvertedNodeGradient.h"

#include "Materials/MaterialExpressionAppendVector.h"
#include "Materials/MaterialExpressionConstant4Vector.h"
#include "Materials/MaterialExpressionVectorParameter.h"

std::array<std::array<float, 4>, 13> GetFixedGradient(const EffekseerMaterial::Gradient& gradient)
{
	std::array<std::array<float, 4>, 13> ret;

	for (size_t i = 0; i < 13; i++)
	{
		ret[i].fill(0.0f);
	}

	ret[0][0] = gradient.ColorCount;
	ret[0][1] = gradient.AlphaCount;
	ret[0][2] = 0.0f;
	ret[0][3] = 0.0f;

	std::stringstream ss;

	for (int32_t i = 0; i < gradient.Colors.size(); i++)
	{
		ret[i + 1][0] = gradient.Colors[i].Color[0] * gradient.Colors[i].Intensity;
		ret[i + 1][1] = gradient.Colors[i].Color[1] * gradient.Colors[i].Intensity;
		ret[i + 1][2] = gradient.Colors[i].Color[2] * gradient.Colors[i].Intensity;
		ret[i + 1][3] = gradient.Colors[i].Position;
	}

	for (int32_t i = 0; i < gradient.Alphas.size(); i++)
	{
		if (i % 2 == 0)
		{
			ret[i / 2 + 9][0] = gradient.Alphas[i].Alpha;
			ret[i / 2 + 9][1] = gradient.Alphas[i].Position;
		}
		else
		{
			ret[i / 2 + 9][2] = gradient.Alphas[i].Alpha;
			ret[i / 2 + 9][3] = gradient.Alphas[i].Position;
		}
	}

	return ret;
}

ConvertedNodeSampleGradient::ConvertedNodeSampleGradient(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
	: effekseerNode_(effekseerNode)
{
	parameters_.fill(nullptr);
	appends_.fill(nullptr);

	expression_ = NewObject<UMaterialExpressionMaterialFunctionCall>(material);
	material->Expressions.Add(expression_);

	FStringAssetReference assetPath("/Effekseer/MaterialFunctions/EfkSampleGradient.EfkSampleGradient");
	UMaterialFunction* func = Cast<UMaterialFunction>(assetPath.TryLoad());
	expression_->SetMaterialFunction(func);

	auto connectedPins = effekseerMaterial->material->GetConnectedPins(effekseerNode->InputPins[0]);
	if (effekseerMaterial->material->GetConnectedPins(effekseerNode->InputPins[0]).size() == 0)
	{
		const auto& gradientData = effekseerNode->Properties[0]->GradientData;

		const auto params = GetFixedGradient(*gradientData);

		for (size_t i = 0; i < 13; i++)
		{
			// TODO : Refactor

			auto paramExpression = NewObject<UMaterialExpressionConstant4Vector>(material);
			paramExpression->Constant.R = params[i][0];
			paramExpression->Constant.G = params[i][1];
			paramExpression->Constant.B = params[i][2];
			paramExpression->Constant.A = params[i][3];
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
			// TODO : Refactor
			const auto& gradientData = node->Properties[0]->GradientData;

			const auto params = GetFixedGradient(*gradientData);

			for (size_t i = 0; i < 13; i++)
			{
				auto paramExpression = NewObject<UMaterialExpressionConstant4Vector>(material);
				paramExpression->Constant.R = params[i][0];
				paramExpression->Constant.G = params[i][1];
				paramExpression->Constant.B = params[i][2];
				paramExpression->Constant.A = params[i][3];
				material->Expressions.Add(paramExpression);
				parameters_[i] = paramExpression;
				expression_->GetInput(i)->Connect(0, paramExpression);
			}
		}
		else if (node->Parameter->Type == EffekseerMaterial::NodeType::GradientParameter)
		{
			const auto uniformName = node->Properties[0]->Str;

			for (size_t i = 0; i < 13; i++)
			{
				auto append = NewObject<UMaterialExpressionAppendVector>(material);
				material->Expressions.Add(append);
				appends_[i] = append;

				auto paramExpression = NewObject<UMaterialExpressionVectorParameter>(material);
				paramExpression->ParameterName = FName((uniformName + "_" + std::to_string(i)).c_str());
				paramExpression->DefaultValue.R = 0.0f;
				paramExpression->DefaultValue.G = 0.0f;
				paramExpression->DefaultValue.B = 0.0f;
				paramExpression->DefaultValue.A = 0.0f;
				material->Expressions.Add(paramExpression);
				parameters_[i] = paramExpression;

				append->A.Connect(0, paramExpression);
				append->B.Connect(4, paramExpression);

				expression_->GetInput(i)->Connect(0, append);
			}
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
	else if (ind <= 13)
	{
		return parameters_[ind - 1];
	}
	else
	{
		return appends_[ind - 1 - 13];
	}
}

int32_t ConvertedNodeSampleGradient::GetExpressionCount() const
{
	if (appends_[0] != nullptr)
	{
		return 14 + 13;
	}

	return 14;
}

void ConvertedNodeSampleGradient::Connect(int targetInd, std::shared_ptr<ConvertedNode> outputNode, int32_t outputNodePinIndex)
{
	if (targetInd == effekseerNode_->GetInputPinIndex("Alpha"))
	{
		outputNode->GetNodeOutputConnector(outputNodePinIndex).Apply(*expression_->GetInput(13));
	}
}