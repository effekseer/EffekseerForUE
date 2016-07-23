#include "EffekseerPrivatePCH.h"
#include "EffekseerEffect.h"
#include "EffekseerNative.h"

#include <string>

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

static ::Effekseer::Setting* CreateSetting()
{
	auto setting = ::Effekseer::Setting::Create();
	setting->SetTextureLoader(new TextureLoader());
	return setting;
}

void UEffekseerEffect::LoadEffect(const uint8_t* data, int32_t size, const TCHAR* path)
{
	::Effekseer::Setting* setting = CreateSetting();
	 
	auto textureLoader = (TextureLoader*)setting->GetTextureLoader();
	textureLoader->SetUObject(this);

	this->ColorTextures.Reset();

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

	// î•ñŽæ“¾
	if (effect != nullptr)
	{
		Version = effect->GetVersion();
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

void UEffekseerEffect::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	//const int32 myVersion = 1;
	//const FGuid myGUID(123, 444, 563, 464);
	//FCustomVersionRegistration registerMyAssetCustomVersion(myGUID, myVersion, TEXT("AssetVersion"));
	//
	//Ar.UsingCustomVersion(myGUID);

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