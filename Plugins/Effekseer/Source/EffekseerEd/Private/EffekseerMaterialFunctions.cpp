#include "EffekseerMaterialFunctions.h"

#include "Materials/MaterialExpressionAppendVector.h"
#include "Materials/MaterialExpressionTextureCoordinate.h"
#include "Materials/MaterialExpressionTextureSample.h"
#include "Materials/MaterialExpressionTextureObject.h"
#include "Materials/MaterialExpressionTextureObjectParameter.h"
#include "Materials/MaterialExpressionTextureCoordinate.h"
#include "Materials/MaterialExpressionMaterialFunctionCall.h"
#include "Materials/MaterialExpressionComponentMask.h"
#include "Materials/MaterialExpressionTime.h"
#include "Materials/MaterialExpressionRotator.h"
#include "Materials/MaterialExpressionFresnel.h"
#include "Materials/MaterialExpressionConstant2Vector.h"
#include "EffekseerMaterial/efkMat.Models.h"
#include "EffekseerMaterial/efkMat.Library.h"
#include "EffekseerMaterial/efkMat.TextExporter.h"

#include "NativeEffekseerMaterialContext.h"

#include "Factories/MaterialFactoryNew.h"
#include "AssetRegistryModule.h"

#include "IAssetTools.h"
#include "Factories/MaterialInstanceConstantFactoryNew.h"
#include "AssetToolsModule.h"
#include "MaterialEditor/MaterialEditorInstanceConstant.h"
#include "Materials/MaterialInstanceConstant.h"

#include "Runtime/Launch/Resources/Version.h"
#include "MaterialNodeConverter/ConvertedNode.h"
#include "MaterialNodeConverter/ConvertedNodeConstant.h"
#include "MaterialNodeConverter/ConvertedNodeParameter.h"
#include "MaterialNodeConverter/ConvertedNodeMath.h"
#include "MaterialNodeConverter/ConvertedNodeModel.h"


class ConvertedNodeFactory
{
private:
public:
	ConvertedNodeFactory() = default;
	virtual ~ConvertedNodeFactory() = default;
	virtual std::shared_ptr<ConvertedNode> Create(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode) { return nullptr; }
};

class ConvertedNodeOutput : public ConvertedNode
{
private:
	UMaterial* material_ = nullptr;
	std::shared_ptr<EffekseerMaterial::Node> effekseerNode_;
	UMaterialExpressionMaterialFunctionCall* baseColorPower_ = nullptr;
	UMaterialExpressionMaterialFunctionCall* emissiveColorPower_ = nullptr;
	UMaterialExpressionMaterialFunctionCall* opacityCullingFunction_ = nullptr;
	UMaterialExpressionMaterialFunctionCall* opacityMaskCullingFunction_ = nullptr;
	UMaterialExpressionMaterialFunctionCall* opacityFunction_ = nullptr;

public:
	ConvertedNodeOutput(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
		: material_(material)
		, effekseerNode_(effekseerNode)
	{
		auto shadingModel = static_cast<EffekseerMaterial::ShadingModelType>(static_cast<int>(effekseerNode_->GetProperty("ShadingModel")->Floats[0]));

		if (shadingModel == EffekseerMaterial::ShadingModelType::Lit)
		{
			material->SetShadingModel(EMaterialShadingModel::MSM_DefaultLit);
		}
		else if (shadingModel == EffekseerMaterial::ShadingModelType::Unlit)
		{
			material->SetShadingModel(EMaterialShadingModel::MSM_Unlit);
		}

		opacityFunction_ = NewObject<UMaterialExpressionMaterialFunctionCall>(material);
		material->Expressions.Add(opacityFunction_);
		opacityCullingFunction_ = NewObject<UMaterialExpressionMaterialFunctionCall>(material);
		material->Expressions.Add(opacityCullingFunction_);
		opacityMaskCullingFunction_ = NewObject<UMaterialExpressionMaterialFunctionCall>(material);
		material->Expressions.Add(opacityMaskCullingFunction_);


		{
			FStringAssetReference assetPath("/Effekseer/MaterialFunctions/EfkCullingMask.EfkCullingMask");
			UMaterialFunction* func = Cast<UMaterialFunction>(assetPath.TryLoad());
			opacityCullingFunction_->SetMaterialFunction(func);
			opacityMaskCullingFunction_->SetMaterialFunction(func);
		}

		{
			FStringAssetReference assetPath("/Effekseer/MaterialFunctions/EfkOpacity.EfkOpacity");
			UMaterialFunction* func = Cast<UMaterialFunction>(assetPath.TryLoad());
			opacityFunction_->SetMaterialFunction(func);
		}

		material_->Opacity.Expression = opacityCullingFunction_;
		material_->OpacityMask.Expression = opacityMaskCullingFunction_;
		opacityCullingFunction_->GetInput(0)->Expression = opacityFunction_;
		opacityCullingFunction_->GetInput(0)->OutputIndex = 0;
	}

	void Connect(int targetInd, std::shared_ptr<ConvertedNode> outputNode, int32_t outputNodePinIndex) override
	{
		if (targetInd == effekseerNode_->GetInputPinIndex("BaseColor"))
		{
			baseColorPower_ = NewObject<UMaterialExpressionMaterialFunctionCall>(material_);
			material_->Expressions.Add(baseColorPower_);
			FStringAssetReference assetPath("/Effekseer/MaterialFunctions/EfkToLinear3.EfkToLinear3");
			UMaterialFunction* func = Cast<UMaterialFunction>(assetPath.TryLoad());
			baseColorPower_->SetMaterialFunction(func);
			material_->Expressions.Add(baseColorPower_);

			outputNode->GetNodeOutputConnector(outputNodePinIndex).Apply(*baseColorPower_->GetInput(0));
			material_->BaseColor.Expression = baseColorPower_;
		}

		if (targetInd == effekseerNode_->GetInputPinIndex("Emissive"))
		{
			emissiveColorPower_ = NewObject<UMaterialExpressionMaterialFunctionCall>(material_);
			material_->Expressions.Add(emissiveColorPower_);
			FStringAssetReference assetPath("/Effekseer/MaterialFunctions/EfkToLinear3.EfkToLinear3");
			UMaterialFunction* func = Cast<UMaterialFunction>(assetPath.TryLoad());
			emissiveColorPower_->SetMaterialFunction(func);
			material_->Expressions.Add(emissiveColorPower_);

			outputNode->GetNodeOutputConnector(outputNodePinIndex).Apply(*emissiveColorPower_->GetInput(0));
			material_->EmissiveColor.Expression = emissiveColorPower_;
		}

		if (targetInd == effekseerNode_->GetInputPinIndex("Opacity"))
		{
			outputNode->GetNodeOutputConnector(outputNodePinIndex).Apply(*opacityFunction_->GetInput(0));
		}

		if (targetInd == effekseerNode_->GetInputPinIndex("OpacityMask"))
		{
			outputNode->GetNodeOutputConnector(outputNodePinIndex).Apply(*opacityMaskCullingFunction_->GetInput(0));
			outputNode->GetNodeOutputConnector(outputNodePinIndex).Apply(*opacityFunction_->GetInput(1));
		}

		if (targetInd == effekseerNode_->GetInputPinIndex("Roughness"))
		{
			outputNode->GetNodeOutputConnector(outputNodePinIndex).Apply(material_->Roughness);
		}

		if (targetInd == effekseerNode_->GetInputPinIndex("AmbientOcclusion"))
		{
			outputNode->GetNodeOutputConnector(outputNodePinIndex).Apply(material_->AmbientOcclusion);
		}

		if (targetInd == effekseerNode_->GetInputPinIndex("Metallic"))
		{
			outputNode->GetNodeOutputConnector(outputNodePinIndex).Apply(material_->Metallic);
		}

		if (targetInd == effekseerNode_->GetInputPinIndex("WorldPositionOffset"))
		{
			outputNode->GetNodeOutputConnector(outputNodePinIndex).Apply(material_->WorldPositionOffset);
		}
	}

	void SetEditorPosition(int32_t x, int32_t y) override
	{
		material_->EditorX = x + 200;
		material_->EditorY = y;

		if (baseColorPower_ != nullptr)
		{
			baseColorPower_->MaterialExpressionEditorX = x;
			baseColorPower_->MaterialExpressionEditorY = y + 50;
		}

		if (emissiveColorPower_ != nullptr)
		{
			emissiveColorPower_->MaterialExpressionEditorX = x;
			emissiveColorPower_->MaterialExpressionEditorY = y + 50;
		}

		opacityCullingFunction_->MaterialExpressionEditorX = x;
		opacityCullingFunction_->MaterialExpressionEditorY = y + 200;
		opacityMaskCullingFunction_->MaterialExpressionEditorX = x;
		opacityMaskCullingFunction_->MaterialExpressionEditorY = y + 250;
		opacityFunction_->MaterialExpressionEditorX = x;
		opacityFunction_->MaterialExpressionEditorY = y + 300;
	}
};

class ConvertedNodeFresnel : public ConvertedNode
{
private:
	std::shared_ptr<EffekseerMaterial::Node> effekseerNode_;
	UMaterialExpressionFresnel* expression_ = nullptr;

public:
	ConvertedNodeFresnel(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
		: effekseerNode_(effekseerNode)
	{
		expression_ = NewObject<UMaterialExpressionFresnel>(material);
		material->Expressions.Add(expression_);

		expression_->Exponent = effekseerNode_->GetProperty("Exponent")->Floats[0];
		expression_->BaseReflectFraction = effekseerNode_->GetProperty("BaseReflectFraction")->Floats[0];
	}

	UMaterialExpression* GetExpression() const override { return expression_; }

	void Connect(int targetInd, std::shared_ptr<ConvertedNode> outputNode, int32_t outputNodePinIndex) override
	{
		if (targetInd == effekseerNode_->GetInputPinIndex("Exponent"))
		{
			outputNode->GetNodeOutputConnector(outputNodePinIndex).Apply(expression_->ExponentIn);
		}

		if (targetInd == effekseerNode_->GetInputPinIndex("BaseReflectFraction"))
		{
			outputNode->GetNodeOutputConnector(outputNodePinIndex).Apply(expression_->BaseReflectFractionIn);
		}
	}
};

class ConvertedNodeRotator : public ConvertedNode
{
private:
	std::shared_ptr<EffekseerMaterial::Node> effekseerNode_;
	UMaterialExpressionMaterialFunctionCall* expression_ = nullptr;

public:
	ConvertedNodeRotator(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
		: effekseerNode_(effekseerNode)
	{
		expression_ = NewObject<UMaterialExpressionMaterialFunctionCall>(material);
		material->Expressions.Add(expression_);

		FStringAssetReference assetPath("/Effekseer/MaterialFunctions/EfkRotator.EfkRotator");
		UMaterialFunction* func = Cast<UMaterialFunction>(assetPath.TryLoad());
		expression_->SetMaterialFunction(func);
	}

	UMaterialExpression* GetExpression() const override { return expression_; }

	void Connect(int targetInd, std::shared_ptr<ConvertedNode> outputNode, int32_t outputNodePinIndex) override
	{
		if (targetInd == effekseerNode_->GetInputPinIndex("UV"))
		{
			outputNode->GetNodeOutputConnector(outputNodePinIndex).Apply(*expression_->GetInput(0));
		}

		if (targetInd == effekseerNode_->GetInputPinIndex("RotationCenter"))
		{
			outputNode->GetNodeOutputConnector(outputNodePinIndex).Apply(*expression_->GetInput(1));
		}

		if (targetInd == effekseerNode_->GetInputPinIndex("RotationAngle"))
		{
			outputNode->GetNodeOutputConnector(outputNodePinIndex).Apply(*expression_->GetInput(2));
		}
	}
};

class ConvertedNodePolarCoords : public ConvertedNode
{
private:
	std::shared_ptr<EffekseerMaterial::Node> effekseerNode_;
	UMaterialExpressionMaterialFunctionCall* expression_ = nullptr;

	UMaterialExpressionConstant2Vector* expression1_ = nullptr;
	UMaterialExpressionConstant2Vector* expression2_ = nullptr;
	UMaterialExpressionConstant* expression3_ = nullptr;
	
public:
	ConvertedNodePolarCoords(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
		: effekseerNode_(effekseerNode)
	{
		expression_ = NewObject<UMaterialExpressionMaterialFunctionCall>(material);
		material->Expressions.Add(expression_);

		FStringAssetReference assetPath("/Effekseer/MaterialFunctions/EfkPolarCoords.EfkPolarCoords");
		UMaterialFunction* func = Cast<UMaterialFunction>(assetPath.TryLoad());
		expression_->SetMaterialFunction(func);

		if (effekseerMaterial->material->GetConnectedPins(effekseerNode->InputPins[0]).size() == 0)
		{
			expression1_ = NewObject<UMaterialExpressionConstant2Vector>(material);
			material->Expressions.Add(expression1_);
			expression1_->R = effekseerNode->Properties[0]->Floats[0];
			expression1_->G = effekseerNode->Properties[0]->Floats[1];
			expression_->GetInput(0)->Connect(0, expression1_);
		}

		if (effekseerMaterial->material->GetConnectedPins(effekseerNode->InputPins[1]).size() == 0)
		{
			expression2_ = NewObject<UMaterialExpressionConstant2Vector>(material);
			material->Expressions.Add(expression2_);
			expression2_->R = effekseerNode->Properties[1]->Floats[0];
			expression2_->G = effekseerNode->Properties[1]->Floats[1];
			expression_->GetInput(1)->Connect(0, expression2_);
		}

		if (effekseerMaterial->material->GetConnectedPins(effekseerNode->InputPins[2]).size() == 0)
		{
			expression3_ = NewObject<UMaterialExpressionConstant>(material);
			material->Expressions.Add(expression3_);
			expression3_->R = effekseerNode->Properties[2]->Floats[0];
			expression_->GetInput(2)->Connect(0, expression3_);
		}
	}

	UMaterialExpression* GetExpression() const override { return expression_; }

	UMaterialExpression* GetExpressions(int32_t ind) const override {
		if (ind == 0) return expression_;
		if (ind == 1) return expression1_;
		if (ind == 2) return expression2_;
		if (ind == 3) return expression3_;
		return nullptr;
	}

	int32_t GetExpressionCount() const { return 4; }

	void Connect(int targetInd, std::shared_ptr<ConvertedNode> outputNode, int32_t outputNodePinIndex) override
	{
		if (targetInd == effekseerNode_->GetInputPinIndex("Tile"))
		{
			outputNode->GetNodeOutputConnector(outputNodePinIndex).Apply(*expression_->GetInput(0));
		}

		if (targetInd == effekseerNode_->GetInputPinIndex("Offset"))
		{
			outputNode->GetNodeOutputConnector(outputNodePinIndex).Apply(*expression_->GetInput(1));
		}

		if (targetInd == effekseerNode_->GetInputPinIndex("PitchV"))
		{
			outputNode->GetNodeOutputConnector(outputNodePinIndex).Apply(*expression_->GetInput(2));
		}
	}
};


class ConvertedNodeDepthFade : public ConvertedNode
{
private:
	std::shared_ptr<EffekseerMaterial::Node> effekseerNode_;
	UMaterialExpressionMaterialFunctionCall* expression_ = nullptr;
	UMaterialExpressionConstant* expression1_ = nullptr;

public:
	ConvertedNodeDepthFade(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
		: effekseerNode_(effekseerNode)
	{
		expression_ = NewObject<UMaterialExpressionMaterialFunctionCall>(material);
		material->Expressions.Add(expression_);

		FStringAssetReference assetPath("/Effekseer/MaterialFunctions/EfkDepthFade.EfkDepthFade");
		UMaterialFunction* func = Cast<UMaterialFunction>(assetPath.TryLoad());
		expression_->SetMaterialFunction(func);

		if (effekseerMaterial->material->GetConnectedPins(effekseerNode->InputPins[0]).size() == 0)
		{
			expression1_ = NewObject<UMaterialExpressionConstant>(material);
			material->Expressions.Add(expression1_);
			expression1_->R = effekseerNode->Properties[0]->Floats[0];
			expression_->GetInput(0)->Connect(0, expression1_);
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
		return nullptr;
	}

	int32_t GetExpressionCount() const
	{
		return 2;
	}

	void Connect(int targetInd, std::shared_ptr<ConvertedNode> outputNode, int32_t outputNodePinIndex) override
	{
		if (targetInd == effekseerNode_->GetInputPinIndex("FadeDistance"))
		{
			outputNode->GetNodeOutputConnector(outputNodePinIndex).Apply(*expression_->GetInput(0));
		}
	}
};


class ConvertedNodeTextureSample : public ConvertedNode
{
private:
	UMaterial* material_ = nullptr;
	std::shared_ptr<EffekseerMaterial::Node> effekseerNode_;
	UMaterialExpressionTextureSample* expression_ = nullptr;
	UMaterialExpressionMaterialFunctionCall* expressionToLinear_ = nullptr;
	UMaterialExpressionConstant* expressionConstant_ = nullptr;
	EffekseerMaterial::TextureValueType valueType_;

public:
	ConvertedNodeTextureSample(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
		: material_(material), effekseerNode_(effekseerNode)
	{
		expression_ = NewObject<UMaterialExpressionTextureSample>(material);
		material->Expressions.Add(expression_);

		{
			expressionToLinear_ = NewObject<UMaterialExpressionMaterialFunctionCall>(material);
			material->Expressions.Add(expressionToLinear_);

			FStringAssetReference assetPath("/Effekseer/MaterialFunctions/EfkToLinear.EfkToLinear");
			UMaterialFunction* func = Cast<UMaterialFunction>(assetPath.TryLoad());
			expressionToLinear_->SetMaterialFunction(func);
		}

		// Sampler
		{
			auto sampler = static_cast<EffekseerMaterial::TextureSamplerType>(static_cast<int>(effekseerNode_->GetProperty("Sampler")->Floats[0]));

			if (sampler == EffekseerMaterial::TextureSamplerType::Repeat)
			{
				expression_->SamplerSource = ESamplerSourceMode::SSM_Wrap_WorldGroupSettings;
			}
			else if (sampler == EffekseerMaterial::TextureSamplerType::Wrap)
			{
				expression_->SamplerSource = ESamplerSourceMode::SSM_Clamp_WorldGroupSettings;
			}
		}

		for(int i = 0; i < 5; i++)
		{
			expressionToLinear_->GetInput(i)->Expression = expression_;
			expressionToLinear_->GetInput(i)->OutputIndex = i;
		}

		auto originalTexturePath = effekseerNode_->GetProperty("Texture")->Str;
		auto info = effekseerMaterial->material->FindTexture(originalTexturePath.c_str());
		valueType_ = info->Type;

		if (originalTexturePath != "")
		{
			int ext_i = originalTexturePath.find_last_of(".");
			auto texturePath = originalTexturePath.substr(0, ext_i);

			int slash_i = texturePath.find_last_of("/");
			texturePath = texturePath + "." + texturePath.substr(slash_i + 1, texturePath.size());

			FStringAssetReference assetPath(texturePath.c_str());
			UTexture* texture = Cast<UTexture>(assetPath.TryLoad());

			expression_->Texture = texture;

			if (texture != nullptr)
			{
				if (texture->IsNormalMap())
				{
					expression_->SamplerType = SAMPLERTYPE_Normal;
				}
				else if (texture->SRGB)
				{
					expression_->SamplerType = SAMPLERTYPE_Color;
				}
				else
				{
					expression_->SamplerType = SAMPLERTYPE_LinearColor;
				}
			}
		}

		if (expression_->Texture == nullptr)
		{
			FStringAssetReference assetPath("/Engine/EngineResources/DefaultTexture.DefaultTexture");
			UTexture* texture = Cast<UTexture>(assetPath.TryLoad());

			expression_->Texture = texture;

			if (texture != nullptr)
			{
				if (texture->IsNormalMap())
				{
					expression_->SamplerType = SAMPLERTYPE_Normal;
				}
				else if (texture->SRGB)
				{
					expression_->SamplerType = SAMPLERTYPE_Color;
				}
				else
				{
					expression_->SamplerType = SAMPLERTYPE_LinearColor;
				}
			}
		}
	}

	void MakeLinear()
	{
		if (expressionConstant_ == nullptr)
		{
			expressionConstant_ = NewObject<UMaterialExpressionConstant>(material_);
			material_->Expressions.Add(expressionConstant_);
			expressionToLinear_->GetInput(6)->Expression = expressionConstant_;
			expressionToLinear_->GetInput(6)->OutputIndex = 0;
		}

		if (valueType_ == EffekseerMaterial::TextureValueType::Color)
		{
			expressionConstant_->R = 1.0f / 2.2f;
		}
		else
		{
			expressionConstant_->R = 1.0f;
		}
	}

	void MakeSRGB()
	{
		if (expressionConstant_ != nullptr)
		{
			if (valueType_ == EffekseerMaterial::TextureValueType::Color)
			{
				expressionConstant_->R = 2.2f;
			}
			else
			{
				expressionConstant_->R = 1.0f;
			}
		}
	}

	UMaterialExpression* GetExpression() const override { return expressionToLinear_; }

	void Connect(int targetInd, std::shared_ptr<ConvertedNode> outputNode, int32_t outputNodePinIndex) override
	{
		if (targetInd == effekseerNode_->GetInputPinIndex("Texture"))
		{
			auto exp = outputNode->GetExpression();

			auto texObj = Cast<UMaterialExpressionTextureObject>(exp);
			auto texObjParam = Cast<UMaterialExpressionTextureObjectParameter>(exp);

			if (texObj != nullptr)
			{
				if (texObj->Texture->SRGB)
				{
					MakeSRGB();
				}
				else
				{
					MakeLinear();
				}
			}

			if (texObjParam != nullptr)
			{
				if (texObjParam->Texture->SRGB)
				{
					MakeSRGB();
				}
				else
				{
					MakeLinear();
				}
			}

			outputNode->GetNodeOutputConnector(outputNodePinIndex).Apply(expression_->TextureObject);
		}

		if (targetInd == effekseerNode_->GetInputPinIndex("UV"))
		{
			outputNode->GetNodeOutputConnector(outputNodePinIndex).Apply(expression_->Coordinates);
		}
	}

	virtual ConvertedNodeOutputConnector GetNodeOutputConnector(int32_t index) const
	{
		return ConvertedNodeOutputConnector(expressionToLinear_, index);
	}

	int32_t GetExpressionCount() const { return expressionConstant_ != nullptr ? 3: 2; }

	UMaterialExpression* GetExpressions(int32_t ind) const override {
		if (ind == 0) return expressionToLinear_;
		if (ind == 1) return expression_;
		if (ind == 2) return expressionConstant_;

		return nullptr;
	}
};

class ConvertedNodeTextureObject : public ConvertedNode
{
private:
	std::shared_ptr<EffekseerMaterial::Node> effekseerNode_;
	UMaterialExpressionTextureObject* expression_ = nullptr;

public:
	ConvertedNodeTextureObject(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
		: effekseerNode_(effekseerNode)
	{
		expression_ = NewObject<UMaterialExpressionTextureObject>(material);
		material->Expressions.Add(expression_);

		auto originalTexturePath = effekseerNode_->GetProperty("Texture")->Str;

		if (originalTexturePath != "")
		{
			auto texturePath = effekseerNode_->GetProperty("Texture")->Str;
			int ext_i = texturePath.find_last_of(".");
			texturePath = texturePath.substr(0, ext_i);

			int slash_i = texturePath.find_last_of("/");
			texturePath = texturePath + "." + texturePath.substr(slash_i + 1, texturePath.size());

			FStringAssetReference assetPath(texturePath.c_str());
			UTexture* texture = Cast<UTexture>(assetPath.TryLoad());

			expression_->Texture = texture;

			if (texture != nullptr)
			{
				if (texture->IsNormalMap())
				{
					expression_->SamplerType = SAMPLERTYPE_Normal;
				}
				else if (texture->SRGB)
				{
					expression_->SamplerType = SAMPLERTYPE_Color;
				}
				else
				{
					expression_->SamplerType = SAMPLERTYPE_LinearColor;
				}
			}
		}

		if (expression_->Texture == nullptr)
		{
			FStringAssetReference assetPath("/Engine/EngineResources/DefaultTexture.DefaultTexture");
			UTexture* texture = Cast<UTexture>(assetPath.TryLoad());

			expression_->Texture = texture;

			if (texture != nullptr)
			{
				if (texture->IsNormalMap())
				{
					expression_->SamplerType = SAMPLERTYPE_Normal;
				}
				else if (texture->SRGB)
				{
					expression_->SamplerType = SAMPLERTYPE_Color;
				}
				else
				{
					expression_->SamplerType = SAMPLERTYPE_LinearColor;
				}
			}
		}
	}

	UMaterialExpression* GetExpression() const override { return expression_; }
};

class ConvertedNodeTextureObjectParameter : public ConvertedNode
{
private:
	std::shared_ptr<EffekseerMaterial::Node> effekseerNode_;
	UMaterialExpressionTextureObjectParameter* expression_ = nullptr;

public:
	ConvertedNodeTextureObjectParameter(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
		: effekseerNode_(effekseerNode)
	{
		expression_ = NewObject<UMaterialExpressionTextureObjectParameter>(material);
		material->Expressions.Add(expression_);

		expression_->ParameterName = FName(effekseerMaterial->textureNames[effekseerNode_->GUID].c_str());

		auto originalTexturePath = effekseerNode_->GetProperty("Texture")->Str;

		if (originalTexturePath != "")
		{
			auto texturePath = effekseerNode_->GetProperty("Texture")->Str;
			int ext_i = texturePath.find_last_of(".");
			texturePath = texturePath.substr(0, ext_i);

			int slash_i = texturePath.find_last_of("/");
			texturePath = texturePath + "." + texturePath.substr(slash_i + 1, texturePath.size());

			FStringAssetReference assetPath(texturePath.c_str());
			UTexture* texture = Cast<UTexture>(assetPath.TryLoad());

			expression_->Texture = texture;

			if (texture != nullptr)
			{
				if (texture->IsNormalMap())
				{
					expression_->SamplerType = SAMPLERTYPE_Normal;
				}
				else if (texture->SRGB)
				{
					expression_->SamplerType = SAMPLERTYPE_Color;
				}
				else
				{
					expression_->SamplerType = SAMPLERTYPE_LinearColor;
				}
			}
		}
		
		if(expression_->Texture == nullptr)
		{
			FStringAssetReference assetPath("/Engine/EngineResources/DefaultTexture.DefaultTexture");
			UTexture* texture = Cast<UTexture>(assetPath.TryLoad());

			expression_->Texture = texture;

			if (texture != nullptr)
			{
				if (texture->IsNormalMap())
				{
					expression_->SamplerType = SAMPLERTYPE_Normal;
				}
				else if (texture->SRGB)
				{
					expression_->SamplerType = SAMPLERTYPE_Color;
				}
				else
				{
					expression_->SamplerType = SAMPLERTYPE_LinearColor;
				}
			}
		}
	}

	UMaterialExpression* GetExpression() const override { return expression_; }
};

template<class T>
class ConvertedNodeFactoryNormalNode : public ConvertedNodeFactory
{
private:
public:
	std::shared_ptr<ConvertedNode> Create(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode) override
	{
		return std::make_shared<T>(material, effekseerMaterial, effekseerNode);
	}
};

UMaterial* CreateUE4MaterialFromEffekseerMaterial(const std::shared_ptr<NativeEffekseerMaterialContext>& context, const FString& packageRoot, const FString& filename, const UE4MaterialFromEffekseerMaterialOption& option)
{
	EBlendMode blendMode = EBlendMode::BLEND_Translucent;
	if (option.AlphaBlend == EEffekseerAlphaBlendType::Blend)
	{
		blendMode = EBlendMode::BLEND_Translucent;
	}
	else if (option.AlphaBlend == EEffekseerAlphaBlendType::Add)
	{
		blendMode = EBlendMode::BLEND_Additive;
	}
	else if (option.AlphaBlend == EEffekseerAlphaBlendType::Mul)
	{
		blendMode = EBlendMode::BLEND_Modulate;
	}
	else if (option.AlphaBlend == EEffekseerAlphaBlendType::Opacity)
	{
		blendMode = EBlendMode::BLEND_Masked;
	}
	else if (option.AlphaBlend == EEffekseerAlphaBlendType::Sub)
	{
		blendMode = EBlendMode::BLEND_Modulate;
	}

	auto package = CreatePackage(NULL, *(packageRoot + filename));

	auto MaterialFactory = NewObject<UMaterialFactoryNew>();
	UMaterial* originalMaterial = (UMaterial*)MaterialFactory->FactoryCreateNew(UMaterial::StaticClass(), package, FName(*(filename)), RF_Standalone | RF_Public, NULL, GWarn);
	FAssetRegistryModule::AssetCreated(originalMaterial);

#if ENGINE_MINOR_VERSION >= 23 // TODO Check correct version
	originalMaterial->AssetImportData = NewObject<UAssetImportData>(originalMaterial, UAssetImportData::StaticClass());
	originalMaterial->AssetImportData->Update(filename);
#endif
	package->FullyLoad();
	package->SetDirtyFlag(true);

	originalMaterial->PreEditChange(nullptr);

	std::map <std::string, std::shared_ptr<ConvertedNodeFactory>> nodeFactories;

	nodeFactories["Constant1"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeConstant1>>();
	nodeFactories["Constant2"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeConstant2>>();
	nodeFactories["Constant3"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeConstant3>>();
	nodeFactories["Constant4"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeConstant4>>();

	nodeFactories["Parameter1"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeParameter1>>();
	nodeFactories["Parameter2"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeParameter2>>();
	nodeFactories["Parameter3"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeParameter3>>();
	nodeFactories["Parameter4"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeParameter4>>();

	nodeFactories["EffectScale"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeEffectScale>>();

	nodeFactories["Abs"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeAbs>>();
	nodeFactories["Sine"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeSine>>();
	nodeFactories["Arctangent2"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeArctangent2>>();

	nodeFactories["Add"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeAdd>>();
	nodeFactories["Subtract"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeSubtract>>();
	nodeFactories["Multiply"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeMultiply>>();
	nodeFactories["Divide"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeDivide>>();
	nodeFactories["Fmod"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeFmod>>();
	nodeFactories["Step"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeStep>>();

#if ENGINE_MINOR_VERSION >= 19 // TODO Check correct version
	nodeFactories["Ceil"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeCeil>>();
	nodeFactories["Floor"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeFloor>>();
	nodeFactories["Frac"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeFrac>>();
#endif
	nodeFactories["Min"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeMin>>();
	nodeFactories["Max"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeMax>>();
	nodeFactories["Power"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodePower>>();
	nodeFactories["SquareRoot"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeSquareRoot>>();
	nodeFactories["Clamp"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeClamp>>();
	nodeFactories["DotProduct"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeDotProduct>>();
	nodeFactories["CrossProduct"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeCrossProduct>>();
	nodeFactories["Normalize"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeNormalize>>();
	nodeFactories["LinearInterpolate"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeLinearInterpolate>>();
	nodeFactories["OneMinus"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeOneMinus>>();
	nodeFactories["ComponentMask"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeComponentMask>>();
	nodeFactories["AppendVector"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeAppendVector>>();

	nodeFactories["TextureCoordinate"] = std::make_shared< ConvertedNodeFactoryNormalNode<ConvertedNodeTextureCoordinate>>();
	nodeFactories["Panner"] = std::make_shared< ConvertedNodeFactoryNormalNode<ConvertedNodePanner>>();

	nodeFactories["TextureObject"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeTextureObject>>();
	nodeFactories["TextureObjectParameter"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeTextureObjectParameter>>();
	nodeFactories["SampleTexture"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeTextureSample>>();

	nodeFactories["Time"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeTime>>();
	nodeFactories["CameraPositionWS"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeCameraPositionWS>>();

	nodeFactories["WorldPosition"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeWorldPosition>>();
	nodeFactories["VertexNormalWS"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeVertexNormalWS>>();
	nodeFactories["PixelNormalWS"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodePixelNormalWS>>();
	nodeFactories["VertexColor"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeVertexColor>>();
	nodeFactories["ObjectScale"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeObjectScale>>();

	nodeFactories["CustomData1"] = std::make_shared< ConvertedNodeFactoryNormalNode<ConvertedNodeCustomData1>>();
	nodeFactories["CustomData2"] = std::make_shared< ConvertedNodeFactoryNormalNode<ConvertedNodeCustomData2>>();

	nodeFactories["Output"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeOutput>>();

	nodeFactories["Fresnel"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeFresnel>>();
	nodeFactories["Rotator"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeRotator>>();
	nodeFactories["PolarCoords"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodePolarCoords>>();

	nodeFactories["DepthFade"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeDepthFade>>();

	std::map<uint64_t, std::shared_ptr<ConvertedNode>> convertedNodes;

	for (auto node : context->material->GetNodes())
	{
		auto it = nodeFactories.find(node->Parameter->TypeName);

		if (it == nodeFactories.end())
		{
			UE_LOG(LogTemp, Warning, TEXT("Unsupported node"));
		}
		else
		{
			auto n = it->second->Create(originalMaterial, context, node);
			convertedNodes[node->GUID] = n;
		}
	}

	for (auto link : context->material->GetLinks())
	{
		auto outputNode = link->OutputPin->Parent.lock();
		auto inputNode = link->InputPin->Parent.lock();

		if (outputNode == nullptr || inputNode == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Invalid link"));
		}

		if (convertedNodes.count(outputNode->GUID) == 0) continue;
		if (convertedNodes.count(inputNode->GUID) == 0) continue;

		convertedNodes[inputNode->GUID]->Connect(link->InputPin->PinIndex, convertedNodes[outputNode->GUID], link->OutputPin->PinIndex);
	}

	// specify positions(because internal nodes are added while connecting)
	for (auto node : context->material->GetNodes())
	{
		auto it = nodeFactories.find(node->Parameter->TypeName);

		if (it == nodeFactories.end())
		{
			UE_LOG(LogTemp, Warning, TEXT("Unsupported node"));
		}
		else
		{
			auto n = convertedNodes[node->GUID];
			n->SetEditorPosition(node->Pos.X, node->Pos.Y);
		}
	}

	originalMaterial->BlendMode = blendMode;

	originalMaterial->TwoSided = 1;
	originalMaterial->OpacityMaskClipValue = 0.0f;
	originalMaterial->PostEditChange();

	return originalMaterial;
}

UMaterialInstance* CreateUE4MaterialInstanceFromUE4Material(UMaterial* parent, const FString& packageRoot, const FString& filename, const UE4MaterialFromEffekseerMaterialOption& option)
{
	EBlendMode blendMode = EBlendMode::BLEND_Translucent;
	if (option.AlphaBlend == EEffekseerAlphaBlendType::Blend)
	{
		blendMode = EBlendMode::BLEND_Translucent;
	}
	else if (option.AlphaBlend == EEffekseerAlphaBlendType::Add)
	{
		blendMode = EBlendMode::BLEND_Additive;
	}
	else if (option.AlphaBlend == EEffekseerAlphaBlendType::Mul)
	{
		blendMode = EBlendMode::BLEND_Modulate;
	}
	else if (option.AlphaBlend == EEffekseerAlphaBlendType::Opacity)
	{
		blendMode = EBlendMode::BLEND_Masked;
	}
	else if (option.AlphaBlend == EEffekseerAlphaBlendType::Sub)
	{
		blendMode = EBlendMode::BLEND_Modulate;
	}

	IAssetTools& AssetTools = FModuleManager::Get().LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	UMaterialInstanceConstantFactoryNew* factory = NewObject<UMaterialInstanceConstantFactoryNew>();
	factory->InitialParent = parent;

	UPackage* package = CreatePackage(NULL, *(packageRoot + filename));
	auto material = Cast<UMaterialInstanceConstant>(factory->FactoryCreateNew(UMaterialInstanceConstant::StaticClass(), package, FName(*filename), RF_Standalone | RF_Public, nullptr, nullptr));

	auto MaterialEditorInstance = NewObject<UMaterialEditorInstanceConstant>(GetTransientPackage(), NAME_None, RF_Transactional);
	MaterialEditorInstance->SetSourceInstance(material);

#if ENGINE_MINOR_VERSION >= 23 // TODO Check correct version
	MaterialEditorInstance->SetSourceFunction(nullptr);
#endif
	MaterialEditorInstance->BasePropertyOverrides.BlendMode = blendMode;
	MaterialEditorInstance->BasePropertyOverrides.bOverride_BlendMode = true;
	MaterialEditorInstance->PostEditChange();
	FAssetRegistryModule::AssetCreated(material);

	return material;
}


TArray<UObject*> AssignElementMaterials(UEffekseerMaterial* material, bool isInstance)
{
	TArray<UObject*> retAssets;

	// generate instances for blends
	TMap<FString, int> generated;


	for (int32_t i = 0; i < material->MaterialElements.Num(); i++)
	{
		auto& e = material->MaterialElements[i];

		const FString elementParentName = material->Material->GetOuter()->GetPathName();
		FString elementAssetName = material->Material->GetName();

		if (isInstance)
		{
			elementAssetName += TEXT("_Inst");
		}

		if (e.AlphaBlend == EEffekseerAlphaBlendType::Blend)
		{
			elementAssetName += "_Blend";
		}
		else if (e.AlphaBlend == EEffekseerAlphaBlendType::Add)
		{
			elementAssetName += "_Add";
		}
		else if (e.AlphaBlend == EEffekseerAlphaBlendType::Mul)
		{
			elementAssetName += "_Mul";
		}
		else if (e.AlphaBlend == EEffekseerAlphaBlendType::Opacity)
		{
			elementAssetName += "_Ops";
		}
		else if (e.AlphaBlend == EEffekseerAlphaBlendType::Sub)
		{
			elementAssetName += "_Sub";
		}

		if (auto found = generated.Find(elementAssetName))
		{
			e.Material = material->MaterialElements[*found].Material;
			continue;
		}

		UE4MaterialFromEffekseerMaterialOption option;
		option.AlphaBlend = e.AlphaBlend;
		UMaterialInterface* elementMaterial = nullptr;

		if (isInstance)
		{
			elementMaterial = CreateUE4MaterialInstanceFromUE4Material(material->Material, elementParentName, elementAssetName, option);
		}
		else
		{
			const auto& data = material->GetData();
			auto native = NativeEffekseerMaterialContext::Load(data.GetData(), data.Num(), TCHAR_TO_ANSI(*material->GetPathName()));

			elementMaterial = CreateUE4MaterialFromEffekseerMaterial(native, elementParentName, elementAssetName, option);
		}

		retAssets.Add(elementMaterial);
		e.Material = elementMaterial;
		generated.Add(elementAssetName, i);
	}

	return retAssets;
}