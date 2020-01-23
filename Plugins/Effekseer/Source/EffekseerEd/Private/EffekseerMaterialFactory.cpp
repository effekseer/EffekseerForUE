#include "EffekseerMaterialFactory.h"

#include "Editor/UnrealEd/Public/Editor.h"
#include "Runtime/Core/Public/HAL/FileManager.h"

#include "Runtime/Launch/Resources/Version.h"
#include "Factories/MaterialFactoryNew.h"
#include "AssetRegistryModule.h"
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
#include "EditorFramework/AssetImportData.h"
#include "EffekseerMaterial/efkMat.Models.h"
#include "EffekseerMaterial/efkMat.Library.h"
#include "EffekseerMaterial/efkMat.TextExporter.h"
#include "NativeEffekseerMaterialContext.h"
#include "Materials/MaterialInstance.h"
#include <map>
#include <memory>
#include <functional>
#include "Factories/MaterialInstanceConstantFactoryNew.h"

#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "Materials/MaterialInstanceConstant.h"

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
		texturePath = + "/" + texturePath.substr(0, ext_i);
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


void UEffekseerMaterialFactory::LoadData(UMaterial* targetMaterial, std::shared_ptr<NativeEffekseerMaterialContext> native)
{
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

	for (auto node : native->material->GetNodes())
	{
		auto it = nodeFactories.find(node->Parameter->TypeName);

		if (it == nodeFactories.end())
		{
			UE_LOG(LogTemp, Warning, TEXT("Unsupported node"));
		}
		else
		{
			auto n = it->second->Create(targetMaterial, native, node);
			convertedNodes[node->GUID] = n;

			if (n->GetExpression() != nullptr)
			{
				n->GetExpression()->MaterialExpressionEditorX = node->Pos.X;
				n->GetExpression()->MaterialExpressionEditorY = node->Pos.Y;
			}
			else
			{
				targetMaterial->EditorX = node->Pos.X;
				targetMaterial->EditorY = node->Pos.Y;
			}
		}
	}

	for (auto link : native->material->GetLinks())
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
}

UEffekseerMaterialFactory::UEffekseerMaterialFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UEffekseerMaterial::StaticClass();
	bCreateNew = false;
	bText = false;
	bEditorImport = true;
	Formats.Add(TEXT("efkmat;EffekseerMaterial"));
}
bool UEffekseerMaterialFactory::DoesSupportClass(UClass* Class)
{
	return (Class == UEffekseerMaterial::StaticClass());
}
UClass* UEffekseerMaterialFactory::ResolveSupportedClass()
{
	return UEffekseerMaterial::StaticClass();
}

UObject* UEffekseerMaterialFactory::FactoryCreateBinary(
	UClass* InClass,
	UObject* InParent,
	FName InName,
	EObjectFlags Flags,
	UObject* Context,
	const TCHAR* Type,
	const uint8*& Buffer,
	const uint8* BufferEnd,
	FFeedbackContext* Warn)
{
	TArray<UObject*> retAssets;

	// Start impoprting
	FEditorDelegates::OnAssetPreImport.Broadcast(this, InClass, InParent, InName, Type);

	UEffekseerMaterial* assetEfkMat = Cast<UEffekseerMaterial>(FindObject<UEffekseerMaterial>(InParent, *InName.ToString()));

	if (assetEfkMat == nullptr)
	{
		assetEfkMat = NewObject<UEffekseerMaterial>(InParent, InClass, FName(InName), Flags);

		// Add defaults
		FEffekseerMaterialElement e1, e2;
		e1.AlphaBlend = EEffekseerAlphaBlendType::Blend;
		e2.AlphaBlend = EEffekseerAlphaBlendType::Add;
		assetEfkMat->MaterialElements.Add(e1);
		assetEfkMat->MaterialElements.Add(e2);
	}

	if (assetEfkMat)
	{
		auto path = assetEfkMat->GetPathName();

		if (!assetEfkMat->AssetImportData)
		{
			assetEfkMat->AssetImportData = NewObject<UAssetImportData>(assetEfkMat, UAssetImportData::StaticClass());
		}

		assetEfkMat->AssetImportData->Update(CurrentFilename);
	}

	retAssets.Add(assetEfkMat);

	{
		// Create package
		UPackage* package = CreatePackage(NULL, *(InParent->GetFName().ToString() + TEXT("_Mat/") + InName.ToString()));

		auto MaterialFactory = NewObject<UMaterialFactoryNew>();
		UMaterial* originalMaterial = (UMaterial*)MaterialFactory->FactoryCreateNew(UMaterial::StaticClass(), package, FName(*(InName.ToString() + TEXT("_M"))), RF_Standalone | RF_Public, NULL, GWarn);
		FAssetRegistryModule::AssetCreated(originalMaterial);

#if ENGINE_MINOR_VERSION >= 23 // TODO Check correct version
		originalMaterial->AssetImportData = NewObject<UAssetImportData>(originalMaterial, UAssetImportData::StaticClass());
		originalMaterial->AssetImportData->Update(CurrentFilename);
#endif
		package->FullyLoad();
		package->SetDirtyFlag(true);

		
		auto native = NativeEffekseerMaterialContext::Load(Buffer, BufferEnd - Buffer, TCHAR_TO_ANSI(*originalMaterial->GetPathName()));

		LoadData(originalMaterial, native);

		for (auto u : native->result.Uniforms)
		{
			FEffekseerUniformProperty prop;
			prop.Name = FString(native->uniformNames[u->GUID].c_str());
			prop.Count = (int)u->Type + 1;
			assetEfkMat->Uniforms.Add(prop);
		}

		for (auto u : native->result.Textures)
		{
			FEffekseerTextureProperty prop;
			prop.Name = FString(native->textureNames[u->GUID].c_str());
			assetEfkMat->Textures.Add(prop);
		}

		assetEfkMat->Material = originalMaterial;
		assetEfkMat->StoreData(Buffer, BufferEnd - Buffer);
		assetEfkMat->LoadMaterial(Buffer, BufferEnd - Buffer, nullptr);
		assetEfkMat->ReassignSearchingMaps();
		retAssets.Add(originalMaterial);

		// generate instances for blends
		TMap<FString, int> generated;

		
		for (int32_t i = 0; i < assetEfkMat->MaterialElements.Num(); i++)
		{
			auto& e = assetEfkMat->MaterialElements[i];

			IAssetTools& AssetTools = FModuleManager::Get().LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
			UMaterialInstanceConstantFactoryNew* factory = NewObject<UMaterialInstanceConstantFactoryNew>();
			factory->InitialParent = originalMaterial;
			const FString elementParentName = InParent->GetName() + "_Mat/";
			FString elementAssetName = InName.ToString() + TEXT("_Inst");

			EBlendMode blendMode = EBlendMode::BLEND_Translucent;
			if (e.AlphaBlend == EEffekseerAlphaBlendType::Blend)
			{
				elementAssetName += "_Blend";
				blendMode = EBlendMode::BLEND_Translucent;
			}
			else if (e.AlphaBlend == EEffekseerAlphaBlendType::Add)
			{
				elementAssetName += "_Add";
				blendMode = EBlendMode::BLEND_Additive;
			}
			else if (e.AlphaBlend == EEffekseerAlphaBlendType::Mul)
			{
				elementAssetName += "_Mul";
				blendMode = EBlendMode::BLEND_Modulate;
			}
			else if (e.AlphaBlend == EEffekseerAlphaBlendType::Opacity)
			{
				elementAssetName += "_Ops";
				blendMode = EBlendMode::BLEND_Masked;
			}
			else if (e.AlphaBlend == EEffekseerAlphaBlendType::Sub)
			{
				elementAssetName += "_Sub";
				blendMode = EBlendMode::BLEND_Modulate;
			}

			if (auto found = generated.Find(elementAssetName))
			{
				e.Material = assetEfkMat->MaterialElements[*found].Material;
				continue;
			}

			UPackage* package = CreatePackage(NULL, *(elementParentName + elementAssetName));
			auto material = Cast<UMaterialInstanceConstant>(factory->FactoryCreateNew(UMaterialInstanceConstant::StaticClass(), package, FName(*elementAssetName), RF_Standalone | RF_Public, nullptr, nullptr));

			//auto material = NewObject<UMaterialInstanceConstant>(package, UMaterialInstanceConstant::StaticClass(), FName(*elementAssetName), RF_Standalone | RF_Public);
			material->PreEditChange(nullptr);
			material->BasePropertyOverrides.BlendMode = blendMode;
			material->BasePropertyOverrides.bOverride_BlendMode = true;
			material->UpdateOverridableBaseProperties();
			material->PostEditChange();
			
			FMaterialUpdateContext UpdateContext(FMaterialUpdateContext::EOptions::Default & ~FMaterialUpdateContext::EOptions::RecreateRenderStates);
			UpdateContext.AddMaterialInstance(material);

			retAssets.Add(material);
			e.Material = material;
			generated.Add(elementAssetName, i);
		}
	}

	// notify finished
	for (UObject* Object : retAssets)
	{
		if (Object)
		{
			FEditorDelegates::OnAssetPostImport.Broadcast(this, Object);
			Object->MarkPackageDirty();
			Object->PreEditChange(nullptr);
			Object->PostEditChange();
		}
	}

	return assetEfkMat;
}

bool UEffekseerMaterialFactory::CanReimport(UObject* Obj, TArray<FString>& OutFilenames)
{
	UEffekseerMaterial* asset = Cast<UEffekseerMaterial>(Obj);
	if (asset && asset->AssetImportData)
	{
		asset->AssetImportData->ExtractFilenames(OutFilenames);
		return true;
	}
	return false;
}

void UEffekseerMaterialFactory::SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths)
{
	UEffekseerMaterial* asset = Cast<UEffekseerMaterial>(Obj);
	if (asset && ensure(NewReimportPaths.Num() == 1))
	{
		asset->AssetImportData->UpdateFilenameOnly(NewReimportPaths[0]);
	}
}

EReimportResult::Type UEffekseerMaterialFactory::Reimport(UObject* Obj)
{
	UEffekseerMaterial* asset = Cast<UEffekseerMaterial>(Obj);
	if (!asset)
	{
		return EReimportResult::Failed;
	}

	const FString Filename = asset->AssetImportData->GetFirstFilename();
	
	if (!Filename.Len() || IFileManager::Get().FileSize(*Filename) == INDEX_NONE)
	{
		return EReimportResult::Failed;
	}

	EReimportResult::Type Result = EReimportResult::Failed;

	if (UFactory::StaticImportObject(
		asset->GetClass(),
		asset->GetOuter(),
		*asset->GetName(),
		RF_Public | RF_Standalone,
		*Filename,
		asset,
		this))
	{
		if (asset->GetOuter())
		{
			asset->GetOuter()->MarkPackageDirty();
		}
		else
		{
			asset->MarkPackageDirty();
		}

		return EReimportResult::Succeeded;
	}

	return EReimportResult::Failed;
}