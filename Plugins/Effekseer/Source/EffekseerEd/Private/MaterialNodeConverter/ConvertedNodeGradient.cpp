#include "ConvertedNodeGradient.h"

#include "Materials/MaterialExpressionAppendVector.h"
#include "Materials/MaterialExpressionConstant4Vector.h"
#include "Materials/MaterialExpressionVectorParameter.h"

std::array<std::array<float, 4>, 13> GetFixedGradient(const EffekseerMaterial::Gradient& gradient)
{
	std::array<std::array<float, 4>, 13> ret;
	ret[0][0] = gradient.ColorCount;
	ret[0][1] = gradient.AlphaCount;
	ret[0][2] = 0.0F;
	ret[0][3] = 0.0F;

	const auto getColorKey = [](const EffekseerMaterial::Gradient& gradient, size_t index)
	{
		if (gradient.ColorCount == 0)
		{
			EffekseerMaterial::Gradient::ColorMarker key;
			key.Color = { 1.0f, 1.0f, 1.0f };
			key.Intensity = 1.0f;
			key.Position = 0.0;
			return key;
		}
		else
		{
			if (gradient.ColorCount <= index)
			{
				auto key = gradient.Colors[gradient.ColorCount - 1];
				key.Position += index;
				return key;
			}

			return gradient.Colors[index];
		}
	};

	const auto getAlphaKey = [](const EffekseerMaterial::Gradient& gradient, size_t index)
	{
		if (gradient.AlphaCount == 0)
		{
			EffekseerMaterial::Gradient::AlphaMarker key;
			key.Alpha = 1.0f;
			key.Position = 0.0;
			return key;
		}
		else
		{
			if (gradient.AlphaCount <= index)
			{
				auto key = gradient.Alphas[gradient.AlphaCount - 1];
				key.Position += index;
				return key;
			}

			return gradient.Alphas[index];
		}
	};

	for (size_t i = 0; i < gradient.Colors.size(); i++)
	{
		const auto colorKey = getColorKey(gradient, i);
		ret[1 + i][0] = colorKey.Color[0] * colorKey.Intensity;
		ret[1 + i][1] = colorKey.Color[1] * colorKey.Intensity;
		ret[1 + i][2] = colorKey.Color[2] * colorKey.Intensity;
		ret[1 + i][3] = colorKey.Position;
	}

	for (size_t i = 0; i < 4; i++)
	{
		const auto alphaKey0 = getAlphaKey(gradient, i * 2 + 0);
		const auto alphaKey1 = getAlphaKey(gradient, i * 2 + 1);
		ret[9 + i][0] = alphaKey0.Alpha;
		ret[9 + i][1] = alphaKey0.Position;
		ret[9 + i][2] = alphaKey1.Alpha;
		ret[9 + i][3] = alphaKey1.Position;
	}

	return ret;
}

ConvertedNodeSampleGradient::ConvertedNodeSampleGradient(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
	: effekseerNode_(effekseerNode)
{
	parameters_.fill(nullptr);
	appends_.fill(nullptr);

	expression_ = NewObject<UMaterialExpressionMaterialFunctionCall>(material);
	ConvertedNodeHelper::AddExpression(material, expression_);

	EffekseerUE::UEFSoftObjectPath assetPath("/Effekseer/MaterialFunctions/EfkSampleGradient.EfkSampleGradient");
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
			ConvertedNodeHelper::AddExpression(material, paramExpression);
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
				ConvertedNodeHelper::AddExpression(material, paramExpression);
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
				ConvertedNodeHelper::AddExpression(material, append);

				appends_[i] = append;

				auto paramExpression = NewObject<UMaterialExpressionVectorParameter>(material);
				paramExpression->ParameterName = FName((uniformName + "_" + std::to_string(i)).c_str());
				paramExpression->DefaultValue.R = 0.0f;
				paramExpression->DefaultValue.G = 0.0f;
				paramExpression->DefaultValue.B = 0.0f;
				paramExpression->DefaultValue.A = 0.0f;
				ConvertedNodeHelper::AddExpression(material, paramExpression);
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