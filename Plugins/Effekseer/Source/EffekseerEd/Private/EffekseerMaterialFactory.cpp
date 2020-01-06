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
#include "EditorFramework/AssetImportData.h"
#include "EffekseerMaterial/efkMat.Models.h"
#include "EffekseerMaterial/efkMat.Library.h"
#include "EffekseerMaterial/efkMat.TextExporter.h"
#include <map>
#include <memory>
#include <functional>


struct NativeEffekseerMaterialParameter
{
	std::shared_ptr<EffekseerMaterial::Material> material;
	std::shared_ptr<EffekseerMaterial::Library> library;
	std::shared_ptr<EffekseerMaterial::Node> outputNode;
	EffekseerMaterial::TextExporterResult result;
	std::map<uint64_t, std::string> uniformNames;
	std::map<uint64_t, std::string> textureNames;
};

std::shared_ptr<NativeEffekseerMaterialParameter> LoadNativeEffekseerMaterialParameter(const uint8* data, int size, const char* basePath)
{
	auto ret = std::make_shared< NativeEffekseerMaterialParameter>();

	std::vector<uint8_t> dataVec;
	dataVec.resize(size);
	memcpy(dataVec.data(), data, size);

	ret->library = std::make_shared<EffekseerMaterial::Library>();
	ret->material = std::make_shared<EffekseerMaterial::Material>();
	ret->material->Initialize();

	// TODO : specify path
	ret->material->Load(dataVec, ret->library, basePath);

	for (auto node : ret->material->GetNodes())
	{
		if (node->Parameter->Type == EffekseerMaterial::NodeType::Output)
		{
			ret->outputNode = node;
		}
	}

	assert(ret->outputNode != nullptr);

	std::shared_ptr<EffekseerMaterial::TextExporter> textExporter = std::make_shared<EffekseerMaterial::TextExporter>();
	ret->result = textExporter->Export(ret->material, ret->outputNode, "$SUFFIX");

	std::unordered_set<std::string> used;

	// parse uniform names
	for (auto u : ret->result.Uniforms)
	{
		if (u->Name == "" || used.count(u->Name) > 0)
		{
			ret->uniformNames[u->GUID] = u->UniformName;
		}
		else
		{
			ret->uniformNames[u->GUID] = u->Name;
			used.insert(u->UniformName);
		}
	}

	// parse texture names
	for (auto t : ret->result.Textures)
	{
		if (t->Name == "" || used.count(t->Name) > 0)
		{
			ret->textureNames[t->GUID] = t->UniformName;
		}
		else
		{
			ret->textureNames[t->GUID] = t->Name;
			used.insert(t->UniformName);
		}
	}

	return ret;
}

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
	virtual std::shared_ptr<ConvertedNode> Create(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialParameter> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode) { return nullptr; }
};

class ConvertedNodeOutput : public ConvertedNode
{
private:
	std::shared_ptr<EffekseerMaterial::Node> effekseerNode_;
	UMaterial* material_ = nullptr;

public:
	ConvertedNodeOutput(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialParameter> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
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
	ConvertedNodeConstant1(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialParameter> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
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
	ConvertedNodeParameter1(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialParameter> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
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
	ConvertedNodeParameter4(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialParameter> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
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


class ConvertedNodeAdd : public ConvertedNode
{
private:
	std::shared_ptr<EffekseerMaterial::Node> effekseerNode_;
	UMaterialExpressionAdd* expression_ = nullptr;

public:
	ConvertedNodeAdd(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialParameter> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
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
	ConvertedNodeAbs(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialParameter> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
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

template<class T>
class ConvertedNodeFactoryNormalNode : public ConvertedNodeFactory
{
private:
public:
	std::shared_ptr<ConvertedNode> Create(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialParameter> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode) override
	{
		return std::make_shared<T>(material, effekseerMaterial, effekseerNode);
	}
};


void UEffekseerMaterialFactory::LoadData(UMaterial* targetMaterial, std::shared_ptr<NativeEffekseerMaterialParameter> native)
{
	std::map <std::string, std::shared_ptr<ConvertedNodeFactory>> nodeFactories;

	nodeFactories["Output"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeOutput>>();
	nodeFactories["Add"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeAdd>>();
	nodeFactories["Abs"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeAbs>>();
	nodeFactories["Constant1"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeConstant1>>();
	nodeFactories["Parameter1"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeParameter1>>();
	nodeFactories["Parameter4"] = std::make_shared<ConvertedNodeFactoryNormalNode<ConvertedNodeParameter4>>();

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

	UEffekseerMaterial* assetEfkMat = NewObject<UEffekseerMaterial>(InParent, InClass, FName(InName), Flags);

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
		UPackage* package = CreatePackage(NULL, *(InParent->GetFName().ToString() + TEXT("_M")));

		auto MaterialFactory = NewObject<UMaterialFactoryNew>();
		UMaterial* targetMaterial = (UMaterial*)MaterialFactory->FactoryCreateNew(UMaterial::StaticClass(), package, FName(*(InName.ToString() + TEXT("_M"))), RF_Standalone | RF_Public, NULL, GWarn);
		FAssetRegistryModule::AssetCreated(targetMaterial);

		targetMaterial->AssetImportData = NewObject<UAssetImportData>(targetMaterial, UAssetImportData::StaticClass());
		targetMaterial->AssetImportData->Update(CurrentFilename);

		package->FullyLoad();
		package->SetDirtyFlag(true);

		auto native = LoadNativeEffekseerMaterialParameter(Buffer, BufferEnd - Buffer, TCHAR_TO_ANSI(*InName.ToString()));

		LoadData(targetMaterial, native);

		for (auto u : native->result.Uniforms)
		{
			FEffekseerUniformProperty prop;
			prop.Name = FString(native->uniformNames[u->GUID].c_str());
			prop.Count = (int)u->Type + 1;
			assetEfkMat->Uniforms.Add(prop);
		}

		// how to load
		// FStringAssetReference DiffuseAssetPath("/Game/T_Texture");
		// UTexture* DiffuseTexture = Cast<UTexture>(DiffuseAssetPath.TryLoad());

		assetEfkMat->Material = targetMaterial;
		assetEfkMat->StoreData(Buffer, BufferEnd - Buffer);
		assetEfkMat->LoadMaterial(Buffer, BufferEnd - Buffer, nullptr);
		assetEfkMat->ReassignSearchingMaps();
		retAssets.Add(targetMaterial);
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
	return true;
}

void UEffekseerMaterialFactory::SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths)
{

}

EReimportResult::Type UEffekseerMaterialFactory::Reimport(UObject* Obj)
{
	return EReimportResult::Failed;
}