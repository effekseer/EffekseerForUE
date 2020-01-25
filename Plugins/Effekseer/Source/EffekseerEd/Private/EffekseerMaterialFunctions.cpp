#include "EffekseerMaterialFunctions.h"

#include "Materials/MaterialExpressionAbs.h"
#include "Materials/MaterialExpressionAdd.h"
#include "Materials/MaterialExpressionMultiply.h"
#include "Materials/MaterialExpressionAppendVector.h"
#include "Materials/MaterialExpressionTextureCoordinate.h"
#include "Materials/MaterialExpressionScalarParameter.h"
#include "Materials/MaterialExpressionVectorParameter.h"
#include "Materials/MaterialExpressionConstant.h"
#include "Materials/MaterialExpressionTextureSample.h"
#include "Materials/MaterialExpressionTextureObject.h"
#include "Materials/MaterialExpressionTextureObjectParameter.h"
#include "Materials/MaterialExpressionTextureCoordinate.h"
#include "Materials/MaterialExpressionMaterialFunctionCall.h"
#include "Materials/MaterialExpressionComponentMask.h"

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

class ConvertedNode
{
private:
public:
	ConvertedNode() = default;
	virtual ~ConvertedNode() = default;

	virtual UMaterialExpression* GetExpression() const { return nullptr; }

	virtual void Connect(int targetInd, std::shared_ptr<ConvertedNode> outputNode) {}
};

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

	void Connect(int targetInd, std::shared_ptr<ConvertedNode> outputNode) override
	{
		if (targetInd == effekseerNode_->GetInputPinIndex("BaseColor"))
		{
			material_->BaseColor.Expression = outputNode->GetExpression();
		}

		if (targetInd == effekseerNode_->GetInputPinIndex("Emissive"))
		{
			material_->EmissiveColor.Expression = outputNode->GetExpression();
		}
	}
};

class ConvertedNodeConstant1 : public ConvertedNode
{
private:
	std::shared_ptr<EffekseerMaterial::Node> effekseerNode_;
	UMaterialExpressionConstant* expression_ = nullptr;

public:
	ConvertedNodeConstant1(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
		: effekseerNode_(effekseerNode)
	{
		expression_ = NewObject<UMaterialExpressionConstant>(material);
		material->Expressions.Add(expression_);

		expression_->R = effekseerNode_->GetProperty("Value")->Floats[0];
	}

	UMaterialExpression* GetExpression() const override { return expression_; }

	void Connect(int targetInd, std::shared_ptr<ConvertedNode> outputNode) override
	{
	}
};

class ConvertedNodeParameter1 : public ConvertedNode
{
private:
	std::shared_ptr<EffekseerMaterial::Node> effekseerNode_;
	UMaterialExpressionScalarParameter* expression_ = nullptr;

public:
	ConvertedNodeParameter1(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
		: effekseerNode_(effekseerNode)
	{
		expression_ = NewObject<UMaterialExpressionScalarParameter>(material);
		material->Expressions.Add(expression_);
		expression_->ParameterName = FName(effekseerMaterial->uniformNames[effekseerNode_->GUID].c_str());
		expression_->DefaultValue = effekseerNode_->GetProperty("Value")->Floats[0];
	}

	UMaterialExpression* GetExpression() const override { return expression_; }

	void Connect(int targetInd, std::shared_ptr<ConvertedNode> outputNode) override
	{
	}
};

class ConvertedNodeParameter4 : public ConvertedNode
{
private:
	std::shared_ptr<EffekseerMaterial::Node> effekseerNode_;
	UMaterialExpressionVectorParameter* expression_ = nullptr;

public:
	ConvertedNodeParameter4(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
		: effekseerNode_(effekseerNode)
	{
		expression_ = NewObject<UMaterialExpressionVectorParameter>(material);
		material->Expressions.Add(expression_);
		expression_->ParameterName = FName(effekseerMaterial->uniformNames[effekseerNode_->GUID].c_str());
		expression_->DefaultValue.R = effekseerNode_->GetProperty("Value")->Floats[0];
		expression_->DefaultValue.G = effekseerNode_->GetProperty("Value")->Floats[1];
		expression_->DefaultValue.B = effekseerNode_->GetProperty("Value")->Floats[2];
		expression_->DefaultValue.A = effekseerNode_->GetProperty("Value")->Floats[3];
	}

	UMaterialExpression* GetExpression() const override { return expression_; }

	void Connect(int targetInd, std::shared_ptr<ConvertedNode> outputNode) override
	{
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
		FStringAssetReference assetPath(texturePath.c_str());
		UTexture* texture = Cast<UTexture>(assetPath.TryLoad());

		expression_->Texture = texture;
	}

	UMaterialExpression* GetExpression() const override { return expression_; }

	void Connect(int targetInd, std::shared_ptr<ConvertedNode> outputNode) override
	{
		if (targetInd == effekseerNode_->GetInputPinIndex("Texture"))
		{
			expression_->TextureObject.Expression = outputNode->GetExpression();
		}

		if (targetInd == effekseerNode_->GetInputPinIndex("UV"))
		{
			expression_->Coordinates.Expression = outputNode->GetExpression();
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
		FStringAssetReference assetPath(texturePath.c_str());
		UTexture* texture = Cast<UTexture>(assetPath.TryLoad());

		expression_->Texture = texture;
	}

	UMaterialExpression* GetExpression() const override { return expression_; }

	void Connect(int targetInd, std::shared_ptr<ConvertedNode> outputNode) override
	{
	}
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
		FStringAssetReference assetPath(texturePath.c_str());
		UTexture* texture = Cast<UTexture>(assetPath.TryLoad());

		expression_->Texture = texture;
	}

	UMaterialExpression* GetExpression() const override { return expression_; }

	void Connect(int targetInd, std::shared_ptr<ConvertedNode> outputNode) override
	{
	}
};

class ConvertedNodeTextureCoordinate : public ConvertedNode
{
private:
	std::shared_ptr<EffekseerMaterial::Node> effekseerNode_;
	UMaterialExpressionMaterialFunctionCall* expression_ = nullptr;

public:
	ConvertedNodeTextureCoordinate(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
		: effekseerNode_(effekseerNode)
	{
		expression_ = NewObject<UMaterialExpressionMaterialFunctionCall>(material);
		material->Expressions.Add(expression_);

		auto index = static_cast<int32_t>(effekseerNode_->GetProperty("UVIndex")->Floats[0]);

		if (index == 0)
		{
			FStringAssetReference assetPath("/Effekseer/MaterialFunctions/EfkUV1.EfkUV1");
			UMaterialFunction* func = Cast<UMaterialFunction>(assetPath.TryLoad());
			expression_->SetMaterialFunction(func);
		}
		else
		{
			FStringAssetReference assetPath("/Effekseer/MaterialFunctions/EfkUV2.EfkUV2");
			UMaterialFunction* func = Cast<UMaterialFunction>(assetPath.TryLoad());
			expression_->SetMaterialFunction(func);
		}
	}

	UMaterialExpression* GetExpression() const override { return expression_; }

	void Connect(int targetInd, std::shared_ptr<ConvertedNode> outputNode) override
	{
	}
};


class ConvertedNodeAdd : public ConvertedNode
{
private:
	std::shared_ptr<EffekseerMaterial::Node> effekseerNode_;
	UMaterialExpressionAdd* expression_ = nullptr;

public:
	ConvertedNodeAdd(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
		: effekseerNode_(effekseerNode)
	{
		expression_ = NewObject<UMaterialExpressionAdd>(material);
		material->Expressions.Add(expression_);

		expression_->ConstA = effekseerNode_->GetProperty("Value1")->Floats[0];
		expression_->ConstB = effekseerNode_->GetProperty("Value2")->Floats[0];
	}

	UMaterialExpression* GetExpression() const override { return expression_; }

	void Connect(int targetInd, std::shared_ptr<ConvertedNode> outputNode) override
	{
		if (targetInd == effekseerNode_->GetInputPinIndex("Value1"))
		{
			expression_->A.Expression = outputNode->GetExpression();
		}

		if (targetInd == effekseerNode_->GetInputPinIndex("Value2"))
		{
			expression_->B.Expression = outputNode->GetExpression();
		}
	}
};

class ConvertedNodeAbs : public ConvertedNode
{
private:
	UMaterialExpressionAbs* expression_ = nullptr;

public:
	ConvertedNodeAbs(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
	{
		expression_ = NewObject<UMaterialExpressionAbs>(material);
		material->Expressions.Add(expression_);
	}

	UMaterialExpression* GetExpression() const override { return expression_; }

	void Connect(int targetInd, std::shared_ptr<ConvertedNode> outputNode) override
	{
		expression_->Input.Expression = outputNode->GetExpression();
	}
};

class ConvertedNodeCustomData1 : public ConvertedNode
{
private:
	UMaterialExpressionComponentMask* expression_ = nullptr;
	UMaterialExpressionMaterialFunctionCall* functions_ = nullptr;

public:
	ConvertedNodeCustomData1(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
	{
		expression_ = NewObject<UMaterialExpressionComponentMask>(material);
		material->Expressions.Add(expression_);

		functions_ = NewObject<UMaterialExpressionMaterialFunctionCall>(material);
		material->Expressions.Add(functions_);

		FStringAssetReference assetPath("/Effekseer/MaterialFunctions/EfkCustomData1.EfkCustomData1");
		UMaterialFunction* func = Cast<UMaterialFunction>(assetPath.TryLoad());
		functions_->SetMaterialFunction(func);

		expression_->Input.Expression = functions_;
		expression_->R = effekseerNode->GetProperty("R")->Floats[0] > 0 ? 1 : 0;
		expression_->G = effekseerNode->GetProperty("G")->Floats[0] > 0 ? 1 : 0;
		expression_->B = effekseerNode->GetProperty("B")->Floats[0] > 0 ? 1 : 0;
		expression_->A = effekseerNode->GetProperty("A")->Floats[0] > 0 ? 1 : 0;
	}

	UMaterialExpression* GetExpression() const override { return expression_; }

	void Connect(int targetInd, std::shared_ptr<ConvertedNode> outputNode) override
	{
	}
};

class ConvertedNodeCustomData2 : public ConvertedNode
{
private:
	UMaterialExpressionComponentMask* expression_ = nullptr;
	UMaterialExpressionMaterialFunctionCall* functions_ = nullptr;

public:
	ConvertedNodeCustomData2(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
	{
		expression_ = NewObject<UMaterialExpressionComponentMask>(material);
		material->Expressions.Add(expression_);

		functions_ = NewObject<UMaterialExpressionMaterialFunctionCall>(material);
		material->Expressions.Add(functions_);

		FStringAssetReference assetPath("/Effekseer/MaterialFunctions/EfkCustomData2.EfkCustomData2");
		UMaterialFunction* func = Cast<UMaterialFunction>(assetPath.TryLoad());
		functions_->SetMaterialFunction(func);

		expression_->Input.Expression = functions_;
		expression_->R = effekseerNode->GetProperty("R")->Floats[0] > 0 ? 1 : 0;
		expression_->G = effekseerNode->GetProperty("G")->Floats[0] > 0 ? 1 : 0;
		expression_->B = effekseerNode->GetProperty("B")->Floats[0] > 0 ? 1 : 0;
		expression_->A = effekseerNode->GetProperty("A")->Floats[0] > 0 ? 1 : 0;
	}

	UMaterialExpression* GetExpression() const override { return expression_; }

	void Connect(int targetInd, std::shared_ptr<ConvertedNode> outputNode) override
	{
	}
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

	nodeFactories["Output"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeOutput>>();
	nodeFactories["Add"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeAdd>>();
	nodeFactories["Abs"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeAbs>>();
	nodeFactories["Constant1"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeConstant1>>();
	nodeFactories["Parameter1"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeParameter1>>();
	nodeFactories["Parameter4"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeParameter4>>();
	nodeFactories["SampleTexture"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeTextureSample>>();
	nodeFactories["TextureObject"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeTextureObject>>();
	nodeFactories["TextureObjectParameter"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeTextureObjectParameter>>();
	nodeFactories["TextureCoordinate"] = std::make_shared< ConvertedNodeFactoryNormalNode<ConvertedNodeTextureCoordinate>>();
	nodeFactories["CustomData1"] = std::make_shared< ConvertedNodeFactoryNormalNode<ConvertedNodeCustomData1>>();
	nodeFactories["CustomData2"] = std::make_shared< ConvertedNodeFactoryNormalNode<ConvertedNodeCustomData2>>();

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

			if (n->GetExpression() != nullptr)
			{
				n->GetExpression()->MaterialExpressionEditorX = node->Pos.X;
				n->GetExpression()->MaterialExpressionEditorY = node->Pos.Y;
			}
			else
			{
				originalMaterial->EditorX = node->Pos.X;
				originalMaterial->EditorY = node->Pos.Y;
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

		convertedNodes[inputNode->GUID]->Connect(link->InputPin->PinIndex, convertedNodes[outputNode->GUID]);
	}

	originalMaterial->BlendMode = blendMode;

	originalMaterial->PostEditChange();

	return originalMaterial;
}

UMaterialInstance* CreateUE4MaterialFromEffekseerMaterial(UMaterial* parent, const FString& packageRoot, const FString& filename, const UE4MaterialFromEffekseerMaterialOption& option)
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
	MaterialEditorInstance->SetSourceFunction(nullptr);
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
		FString elementAssetName = material->Material->GetName() + TEXT("_Inst");

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
			elementMaterial = CreateUE4MaterialFromEffekseerMaterial(material->Material, elementParentName, elementAssetName, option);
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