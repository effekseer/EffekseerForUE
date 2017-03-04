#include "EffekseerPrivatePCH.h"
#include "EffekseerEffect.h"
#include "EffekseerNative.h"

#include <string>
#include <functional>

static void GetParentDir(EFK_CHAR* dst, const EFK_CHAR* src)
{
	int i, last = -1;
	for (i = 0; src[i] != L'\0'; i++)
	{
		if (src[i] == L'/' || src[i] == L'\\')
			last = i;
	}
	if (last >= 0)
	{
		memcpy(dst, src, last * sizeof(EFK_CHAR));
		dst[last] = L'\0';
	}
	else
	{
		dst[0] = L'\0';
	}
}

static std::basic_string<EFK_CHAR> GetFileNameWithoutExtension(const EFK_CHAR* filepath)
{
	auto path = std::basic_string<EFK_CHAR>(filepath);
	size_t i = path.rfind('.', path.length());
	if (i != std::basic_string<EFK_CHAR>::npos)
	{
		return (path.substr(0, i));
	}
	return std::basic_string<EFK_CHAR>();
}

class TextureLoader
	: public ::Effekseer::TextureLoader
{
private:
	UEffekseerEffect*			m_uobject;
	::Effekseer::FileInterface* m_fileInterface;
	::Effekseer::DefaultFileInterface m_defaultFileInterface;

public:
	TextureLoader(::Effekseer::FileInterface* fileInterface = NULL);
	virtual ~TextureLoader();

public:
	void* Load(const EFK_CHAR* path, ::Effekseer::TextureType textureType) override;
	void Unload(void* data) override;
	void SetUObject(UEffekseerEffect* uobject)
	{
		m_uobject = uobject;
	}
};

TextureLoader::TextureLoader(::Effekseer::FileInterface* fileInterface)
	: m_fileInterface(fileInterface)
{
	if (m_fileInterface == NULL)
	{
		m_fileInterface = &m_defaultFileInterface;
	}
}

TextureLoader::~TextureLoader()
{

}

void* TextureLoader::Load(const EFK_CHAR* path, ::Effekseer::TextureType textureType)
{
	if (textureType == ::Effekseer::TextureType::Color)
	{
		auto path_we = GetFileNameWithoutExtension(path);
		auto epath_ = (const char16_t*)path_we.c_str();
		auto path_ = (const TCHAR*)epath_;

		auto texture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, path_));
		m_uobject->ColorTextures.Add(texture);
		return (void*)texture;
	}

	return NULL;
}

void TextureLoader::Unload(void* data)
{
	if (data != NULL)
	{

	}
}

class ModelLoader
	: public ::Effekseer::ModelLoader
{
private:
	UEffekseerEffect*			m_uobject;
	::Effekseer::FileInterface* m_fileInterface;
	::Effekseer::DefaultFileInterface m_defaultFileInterface;

public:
	ModelLoader(::Effekseer::FileInterface* fileInterface = NULL);
	virtual ~ModelLoader();

public:
	void* Load(const EFK_CHAR* path) override;
	void Unload(void* data) override;
	void SetUObject(UEffekseerEffect* uobject)
	{
		m_uobject = uobject;
	}
};

ModelLoader::ModelLoader(::Effekseer::FileInterface* fileInterface)
	: m_fileInterface(fileInterface)
{
	if (m_fileInterface == NULL)
	{
		m_fileInterface = &m_defaultFileInterface;
	}
}

ModelLoader::~ModelLoader()
{

}

void* ModelLoader::Load(const EFK_CHAR* path)
{
	auto path_we = GetFileNameWithoutExtension(path);
	auto epath_ = (const char16_t*)path_we.c_str();
	auto path_ = (const TCHAR*)epath_;

	auto model = Cast<UEffekseerModel>(StaticLoadObject(UEffekseerModel::StaticClass(), NULL, path_));
	m_uobject->Models.Add(model);

	if (model != nullptr)
	{
		return (void*)model->GetNativePtr();
	}
	
	return model;
}

void ModelLoader::Unload(void* data)
{
	if (data != NULL)
	{
	
	}
}

static ::Effekseer::Setting* CreateSetting()
{
	auto setting = ::Effekseer::Setting::Create();
	setting->SetTextureLoader(new TextureLoader());
	setting->SetModelLoader(new ModelLoader());
	return setting;
}

void UEffekseerEffect::LoadEffect(const uint8_t* data, int32_t size, const TCHAR* path)
{
	::Effekseer::Setting* setting = CreateSetting();
	 
	auto textureLoader = (TextureLoader*)setting->GetTextureLoader();
	textureLoader->SetUObject(this);

	auto modelLoader = (ModelLoader*)setting->GetModelLoader();
	modelLoader->SetUObject(this);

	this->ColorTextures.Reset();
	this->Models.Reset();

	EFK_CHAR* rootPath = (EFK_CHAR*)path;
	EFK_CHAR parentPath[300];

	if (path != nullptr)
	{
		GetParentDir(parentPath, (const EFK_CHAR*)path);
		rootPath = parentPath;
	}
	
	loadedScale = Scale;

	auto effect = ::Effekseer::Effect::Create(setting, (void*)data, size, Scale, rootPath);
	setting->Release();

	effectPtr = effect;

	// èÓïÒéÊìæ
	if (effect != nullptr)
	{
		Version = effect->GetVersion();
	}

	// çƒãN
	std::function<void(::Effekseer::EffectNode*,bool)> renode;

	renode = [this, &renode](::Effekseer::EffectNode* node, bool isRoot) -> void
	{
		if (!isRoot)
		{
			auto param = node->GetBasicRenderParameter();

			UTexture2D* texture = nullptr;

			if (0 <= param.ColorTextureIndex &&
				param.ColorTextureIndex < this->ColorTextures.Num())
			{
				texture = this->ColorTextures[param.ColorTextureIndex];
			}

			UEffekseerMaterial* mat = NewObject<UEffekseerMaterial>();
			mat->Texture = texture;
			mat->IsDepthTestDisabled = !param.ZTest;
			mat->AlphaBlend = (EEffekseerAlphaBlendType)param.AlphaBlend;

			this->Materials.Add(mat);

		}

		for (size_t i = 0; i < node->GetChildrenCount(); i++)
		{
			auto n = node->GetChild(i);
			renode(n, false);
		}
	};
	
	if (effect != nullptr)
	{
		renode(effect->GetRoot(), true);
	}

}

void UEffekseerEffect::ReleaseEffect()
{
	auto p = (::Effekseer::Effect*)effectPtr;
	ES_SAFE_RELEASE(p);
	effectPtr = nullptr;
}

void UEffekseerEffect::Load(const uint8_t* data, int32_t size, const TCHAR* path)
{
	ReleaseEffect();
	buffer.Reset(0);
	buffer.Append(data, size);
	LoadEffect(data, size, path);
}

void UEffekseerEffect::BeginDestroy()
{
	ReleaseEffect();
	Super::BeginDestroy();
}

void UEffekseerEffect::ReloadIfRequired()
{
	if (Scale != loadedScale)
	{
		auto path = GetPathName();
		LoadEffect(buffer.GetData(), buffer.Num(), *path);
	}
}

void UEffekseerEffect::AssignResources()
{
	auto path = GetPathName();
	LoadEffect(buffer.GetData(), buffer.Num(), *path);
}

void UEffekseerEffect::PostLoad()
{
	Super::PostLoad();

	// To load Model object
	AssignResources();
}

void UEffekseerEffect::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	const int32_t version = 1;
	const FGuid GUID(0x11334A12, 0x2E554231, 0xA36791A3, 0xC5A1082C);
	static FCustomVersionRegistration GRegisterPaperCustomVersion(GUID, version, TEXT("EffekseerVer"));
	Ar.UsingCustomVersion(GUID);

	/*
#if WITH_EDITORONLY_DATA
	if (Ar.IsLoading() && (Ar.UE4Ver() < VER_UE4_ASSET_IMPORT_DATA_AS_JSON) && (AssetImportData == nullptr))
	{
		// AssetImportData should always be valid
		AssetImportData = NewObject<UAssetImportData>(this, TEXT("AssetImportData"));
	}
#endif
	*/

	if (Ar.IsSaving() || Ar.IsLoading())
	{
		Ar << buffer;
	}

	if (Ar.IsLoading())
	{
		ReleaseEffect();

		auto path = GetPathName();
		LoadEffect(buffer.GetData(), buffer.Num(), *path);
	}
}