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
	std::shared_ptr<EffekseerMaterial::Node> effekseerNode_;
	UMaterial* material_ = nullptr;

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
	}

	void Connect(int targetInd, std::shared_ptr<ConvertedNode> outputNode, int32_t outputNodePinIndex) override
	{
		if (targetInd == effekseerNode_->GetInputPinIndex("BaseColor"))
		{
			material_->BaseColor.Expression = outputNode->GetExpressions(outputNodePinIndex);
		}

		if (targetInd == effekseerNode_->GetInputPinIndex("Opacity"))
		{
			material_->Opacity.Expression = outputNode->GetExpressions(outputNodePinIndex);
		}

		if (targetInd == effekseerNode_->GetInputPinIndex("Roughness"))
		{
			material_->Roughness.Expression = outputNode->GetExpressions(outputNodePinIndex);
		}

		if (targetInd == effekseerNode_->GetInputPinIndex("AmbientOcclusion"))
		{
			material_->AmbientOcclusion.Expression = outputNode->GetExpressions(outputNodePinIndex);
		}

		if (targetInd == effekseerNode_->GetInputPinIndex("Metallic"))
		{
			material_->Metallic.Expression = outputNode->GetExpressions(outputNodePinIndex);
		}

		if (targetInd == effekseerNode_->GetInputPinIndex("OpacityMask"))
		{
			material_->OpacityMask.Expression = outputNode->GetExpressions(outputNodePinIndex);
		}

		if (targetInd == effekseerNode_->GetInputPinIndex("WorldPositionOffset"))
		{
			material_->WorldPositionOffset.Expression = outputNode->GetExpressions(outputNodePinIndex);
		}

		if (targetInd == effekseerNode_->GetInputPinIndex("Emissive"))
		{
			material_->EmissiveColor.Expression = outputNode->GetExpressions(outputNodePinIndex);
		}
	}
};

class ConvertedNodeTextureSample : public ConvertedNode
{
private:
	std::shared_ptr<EffekseerMaterial::Node> effekseerNode_;
	UMaterialExpressionTextureSample* expression_ = nullptr;

public:
	ConvertedNodeTextureSample(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
		: effekseerNode_(effekseerNode)
	{
		expression_ = NewObject<UMaterialExpressionTextureSample>(material);
		material->Expressions.Add(expression_);

		auto texturePath = effekseerNode_->GetProperty("Texture")->Str;
		int ext_i = texturePath.find_last_of(".");
		texturePath = +"/" + texturePath.substr(0, ext_i);

		int slash_i = texturePath.find_last_of("/");
		texturePath = texturePath + "." + texturePath.substr(slash_i + 1, texturePath.size());

		FStringAssetReference assetPath(texturePath.c_str());
		UTexture* texture = Cast<UTexture>(assetPath.TryLoad());

		expression_->Texture = texture;

		if (texture != nullptr)
		{
			if (texture->SRGB)
			{
				expression_->SamplerType = SAMPLERTYPE_Color;
			}
			else
			{
				expression_->SamplerType = SAMPLERTYPE_LinearColor;
			}
		}
	}

	UMaterialExpression* GetExpression() const override { return expression_; }

	void Connect(int targetInd, std::shared_ptr<ConvertedNode> outputNode, int32_t outputNodePinIndex) override
	{
		if (targetInd == effekseerNode_->GetInputPinIndex("Texture"))
		{
			expression_->TextureObject.Expression = outputNode->GetExpressions(outputNodePinIndex);
		}

		if (targetInd == effekseerNode_->GetInputPinIndex("UV"))
		{
			expression_->Coordinates.Expression = outputNode->GetExpressions(outputNodePinIndex);
		}
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

		auto texturePath = effekseerNode_->GetProperty("Texture")->Str;
		int ext_i = texturePath.find_last_of(".");
		texturePath = +"/" + texturePath.substr(0, ext_i);

		int slash_i = texturePath.find_last_of("/");
		texturePath = texturePath + "." + texturePath.substr(slash_i + 1, texturePath.size());

		FStringAssetReference assetPath(texturePath.c_str());
		UTexture* texture = Cast<UTexture>(assetPath.TryLoad());

		expression_->Texture = texture;

		if (texture != nullptr)
		{
			if (texture->SRGB)
			{
				expression_->SamplerType = SAMPLERTYPE_Color;
			}
			else
			{
				expression_->SamplerType = SAMPLERTYPE_LinearColor;
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

		auto texturePath = effekseerNode_->GetProperty("Texture")->Str;
		int ext_i = texturePath.find_last_of(".");
		texturePath = +"/" + texturePath.substr(0, ext_i);

		int slash_i = texturePath.find_last_of("/");
		texturePath = texturePath + "." + texturePath.substr(slash_i + 1, texturePath.size());

		FStringAssetReference assetPath(texturePath.c_str());
		UTexture* texture = Cast<UTexture>(assetPath.TryLoad());

		expression_->Texture = texture;

		if (texture != nullptr)
		{
			if (texture->SRGB)
			{
				expression_->SamplerType = SAMPLERTYPE_Color;
			}
			else
			{
				expression_->SamplerType = SAMPLERTYPE_LinearColor;
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

	nodeFactories["Abs"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeAbs>>();
	nodeFactories["Sine"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeSine>>();
	nodeFactories["Arctangent2"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeArctangent2>>();

	nodeFactories["Add"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeAdd>>();
	nodeFactories["Subtract"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeSubtract>>();
	nodeFactories["Multiply"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeMultiply>>();
	nodeFactories["Divide"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeDivide>>();
	nodeFactories["Fmod"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeFmod>>();

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

			for (int32_t i = 0; i < n->GetExpressionCount(); i++)
			{
				if (n->GetExpressions(i) != nullptr)
				{
					n->GetExpressions(i)->MaterialExpressionEditorX = node->Pos.X;
					n->GetExpressions(i)->MaterialExpressionEditorY = node->Pos.Y;
				}
				else
				{
					// for output
					originalMaterial->EditorX = node->Pos.X;
					originalMaterial->EditorY = node->Pos.Y;
				}
			}
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

	originalMaterial->BlendMode = blendMode;

	originalMaterial->TwoSided = 1;

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