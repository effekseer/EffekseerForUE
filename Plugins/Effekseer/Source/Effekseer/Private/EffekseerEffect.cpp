#include "EffekseerEffect.h"
#include "EffekseerNative.h"

#include <string>
#include <functional>
#include <vector>

#include "EffekseerCustomVersion.h"

template <size_t size_>
struct tStr
{
	enum { size = size_ };
	TCHAR data[size];
	tStr(const char16_t* u16str)
	{
		data[size - 1] = 0;
		for (int i = 0; i < size - 1; i++)
		{
			data[i] = (TCHAR)u16str[i];
			if (data[i] == 0) break;
		}
	}
	const TCHAR* c_str() const
	{
		return data;
	}
};

template <size_t size_>
struct uStr
{
	enum { size = size_ };
	char16_t data[size];
	uStr(const TCHAR* u16str)
	{
		data[size - 1] = 0;
		for (int i = 0; i < size - 1; i++)
		{
			data[i] = (char16_t)u16str[i];
			if (data[i] == 0) break;
		}
	}
	const char16_t* c_str() const
	{
		return data;
	}
};

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

static std::u16string getFilenameWithoutExt(const char16_t* path)
{
	int start = 0;
	int end = 0;

	for (int i = 0; path[i] != 0; i++)
	{
		if (path[i] == u'/' || path[i] == u'\\')
		{
			start = i + 1;
		}
	}

	for (int i = start; path[i] != 0; i++)
	{
		if (path[i] == u'.')
		{
			end = i;
		}
	}

	std::vector<char16_t> ret;

	for (int i = start; i < end; i++)
	{
		ret.push_back(path[i]);
	}
	ret.push_back(0);

	return std::u16string(ret.data());
}

class TextureLoader
	: public ::Effekseer::TextureLoader
{
private:
	UEffekseerEffect*			m_uobject;
	bool						m_requiredToCreateResource = false;
	int32_t						m_loadingColorIndex = 0;
	int32_t						m_loadingDistortionIndex = 0;

public:
	TextureLoader(::Effekseer::FileInterface* fileInterface = NULL);
	virtual ~TextureLoader();

public:
	Effekseer::TextureData* Load(const EFK_CHAR* path, ::Effekseer::TextureType textureType) override;
	void Unload(Effekseer::TextureData* data) override;
	void SetUObject(UEffekseerEffect* uobject, bool requiredToCreateResource)
	{
		m_uobject = uobject;

		m_requiredToCreateResource = requiredToCreateResource;
		if (!requiredToCreateResource)
		{
			m_loadingColorIndex = 0;
			m_loadingDistortionIndex = 0;
		}
	}
};

TextureLoader::TextureLoader(::Effekseer::FileInterface* fileInterface)
{
}

TextureLoader::~TextureLoader()
{

}

Effekseer::TextureData* TextureLoader::Load(const EFK_CHAR* path, ::Effekseer::TextureType textureType)
{
	if (textureType != ::Effekseer::TextureType::Color &&
		textureType != ::Effekseer::TextureType::Distortion) return nullptr;

	auto path_we = GetFileNameWithoutExtension(path);
	auto epath_ = (const char16_t*)path_we.c_str();
	auto path_ = (const TCHAR*)epath_;


	if (textureType == ::Effekseer::TextureType::Color)
	{	
		if (m_requiredToCreateResource)
		{
			auto texture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, path_));
			m_uobject->ColorTextures.Add(texture);

			Effekseer::TextureData* data = new Effekseer::TextureData();
			data->UserPtr = texture;
			return data;
		}
		else
		{
			if (m_uobject->ColorTextures.Num() <= m_loadingColorIndex) return nullptr;

			auto o = m_uobject->ColorTextures[m_loadingColorIndex];
			m_loadingColorIndex++;

			Effekseer::TextureData* data = new Effekseer::TextureData();
			data->UserPtr = o;
			return data;
		}
	}

	if (textureType == ::Effekseer::TextureType::Distortion)
	{
		if (m_requiredToCreateResource)
		{
			auto texture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, path_));
			m_uobject->DistortionTextures.Add(texture);

			Effekseer::TextureData* data = new Effekseer::TextureData();
			data->UserPtr = texture;
			return data;
		}
		else
		{
			if (m_uobject->DistortionTextures.Num() <= m_loadingDistortionIndex) return nullptr;

			auto o = m_uobject->DistortionTextures[m_loadingDistortionIndex];
			m_loadingDistortionIndex++;

			Effekseer::TextureData* data = new Effekseer::TextureData();
			data->UserPtr = o;
			return data;
		}
	}

	return NULL;
}

void TextureLoader::Unload(Effekseer::TextureData* data)
{
	if (data != NULL)
	{
		auto p = (Effekseer::TextureData*)data;
		delete p;
	}
}

class ModelLoader
	: public ::Effekseer::ModelLoader
{
private:
	UEffekseerEffect*			m_uobject;
	bool						m_requiredToCreateResource = false;
	int32_t						m_loadingIndex = 0;

public:
	ModelLoader(::Effekseer::FileInterface* fileInterface = NULL);
	virtual ~ModelLoader();

public:
	void* Load(const EFK_CHAR* path) override;
	void Unload(void* data) override;
	void SetUObject(UEffekseerEffect* uobject, bool requiredToCreateResource)
	{
		m_uobject = uobject;

		m_requiredToCreateResource = requiredToCreateResource;
		if (!requiredToCreateResource)
		{
			m_loadingIndex = 0;
		}
	}
};

ModelLoader::ModelLoader(::Effekseer::FileInterface* fileInterface)
{
}

ModelLoader::~ModelLoader()
{

}

void* ModelLoader::Load(const EFK_CHAR* path)
{
	auto path_we = GetFileNameWithoutExtension(path);
	auto epath_ = (const char16_t*)path_we.c_str();
	auto path_ = (const TCHAR*)epath_;

	if (m_requiredToCreateResource)
	{
		auto model = Cast<UEffekseerModel>(StaticLoadObject(UEffekseerModel::StaticClass(), NULL, path_));
		m_uobject->Models.Add(model);

		if (model != nullptr)
		{
			return (void*)model->GetNativePtr();
		}

		return model;
	}
	else
	{
		if (m_uobject->Models.Num() <= m_loadingIndex) return nullptr;

		auto o = m_uobject->Models[m_loadingIndex];
		m_loadingIndex++;
		
		if (o != nullptr)
		{
			return (void*)o->GetNativePtr();
		}

		return o;
	}
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

void UEffekseerEffect::LoadEffect(const uint8_t* data, int32_t size, const TCHAR* path, bool isResourceReset)
{
	Name = (const TCHAR*)GetFileNameWithoutExtension((const EFK_CHAR*)path).c_str();

	::Effekseer::Setting* setting = CreateSetting();
	 
	if (isResourceReset)
	{
		this->ColorTextures.Reset();
		this->DistortionTextures.Reset();
		this->Models.Reset();
	}

	auto textureLoader = (TextureLoader*)setting->GetTextureLoader();
	textureLoader->SetUObject(this, isResourceReset);

	auto modelLoader = (ModelLoader*)setting->GetModelLoader();
	modelLoader->SetUObject(this, isResourceReset);

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

	// Get information
	if (effect != nullptr)
	{
		Version = effect->GetVersion();

		effect->SetName(getFilenameWithoutExt(uStr<260>(path).c_str()).c_str());
	}

	std::function<void(::Effekseer::EffectNode*,bool)> renode;

	renode = [this, &renode](::Effekseer::EffectNode* node, bool isRoot) -> void
	{
		if (!isRoot)
		{
			auto param = node->GetBasicRenderParameter();
			auto modelParam = node->GetEffectModelParameter();

			UTexture2D* texture = nullptr;

			if (param.Distortion)
			{
				if (0 <= param.ColorTextureIndex &&
					param.ColorTextureIndex < this->DistortionTextures.Num())
				{
					texture = this->DistortionTextures[param.ColorTextureIndex];
				}
			}
			else
			{
				if (0 <= param.ColorTextureIndex &&
					param.ColorTextureIndex < this->ColorTextures.Num())
				{
					texture = this->ColorTextures[param.ColorTextureIndex];
				}
			}

			UEffekseerMaterial* mat = NewObject<UEffekseerMaterial>();
			mat->Texture = texture;
			mat->IsDepthTestDisabled = !param.ZTest;
			mat->AlphaBlend = (EEffekseerAlphaBlendType)param.AlphaBlend;
			mat->IsLighting = modelParam.Lighting;
			mat->IsDistorted = param.Distortion;

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
	LoadEffect(data, size, path, true);
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
		LoadEffect(buffer.GetData(), buffer.Num(), *path, true);
	}
}

void UEffekseerEffect::AssignResources()
{
	auto path = GetPathName();
	LoadEffect(buffer.GetData(), buffer.Num(), *path, true);
}

void UEffekseerEffect::PostLoad()
{
	Super::PostLoad();

	if (isDirty)
	{
		ReleaseEffect();
		auto path = GetPathName();
		LoadEffect(buffer.GetData(), buffer.Num(), *path, false);
		isDirty = false;
	}
}

void UEffekseerEffect::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
	Ar.UsingCustomVersion(FEffekseerCustomVersion::GUID);

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
		isDirty = true;
	}
}