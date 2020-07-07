#include "EffekseerEffect.h"
#include "EffekseerNative.h"
#include "EffekseerRendererNative.h"
#include "EffekseerMaterial.h"

#include <string>
#include <functional>
#include <vector>

#include "EffekseerRendererShader.h"
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

static void GetParentDir(char16_t* dst, const char16_t* src)
{
	int i, last = -1;
	for (i = 0; src[i] != L'\0'; i++)
	{
		if (src[i] == u'/' || src[i] == u'\\')
			last = i;
	}
	if (last >= 0)
	{
		memcpy(dst, src, last * sizeof(char16_t));
		dst[last] = L'\0';
	}
	else
	{
		dst[0] = L'\0';
	}
}

static std::basic_string<char16_t> GetFileNameWithoutExtension(const char16_t* filepath)
{
	auto path = Effekseer::PathHelper::Normalize(std::u16string(filepath));
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
	auto path_ = tStr<512>(path_we.c_str());


	if (textureType == ::Effekseer::TextureType::Color)
	{	
		if (m_requiredToCreateResource)
		{
			auto texture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, path_.c_str()));
			m_uobject->ColorTextures.Add(texture);

			if (texture == nullptr)
			{
				UE_LOG(LogScript, Warning, TEXT("Failed to load %s"), path_.c_str());
			}

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
			auto texture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, path_.c_str()));
			m_uobject->DistortionTextures.Add(texture);

			if (texture == nullptr)
			{
				UE_LOG(LogScript, Warning, TEXT("Failed to load %s"), path_.c_str());
			}

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
	auto path_ = tStr<512>(epath_);

	if (m_requiredToCreateResource)
	{
		auto model = Cast<UEffekseerModel>(StaticLoadObject(UEffekseerModel::StaticClass(), NULL, path_.c_str()));
		m_uobject->Models.Add(model);

		if (model != nullptr)
		{
			return (void*)model->GetNativePtr();
		}
		else
		{
			UE_LOG(LogScript, Warning, TEXT("Failed to load %s"), path_.c_str());
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

class MaterialLoader
	: public ::Effekseer::MaterialLoader
{
private:
	UEffekseerEffect* uobject_ = nullptr;
	bool requiredToCreateResource_ = false;
	int32_t loadingIndex_ = 0;

public:
	MaterialLoader() = default;
	virtual ~MaterialLoader() = default;

public:
	::Effekseer::MaterialData* Load(const EFK_CHAR* path) override;
	void Unload(::Effekseer::MaterialData* data) override;
	void SetUObject(UEffekseerEffect* uobject, bool requiredToCreateResource)
	{
		uobject_ = uobject;

		requiredToCreateResource_ = requiredToCreateResource;
		if (!requiredToCreateResource)
		{
			loadingIndex_ = 0;
		}
	}
};

::Effekseer::MaterialData* MaterialLoader::Load(const EFK_CHAR* path)
{
	auto path_we = GetFileNameWithoutExtension(path);
	auto path_ = tStr<512>(path_we.c_str());

	if (requiredToCreateResource_)
	{
		auto material = Cast<UEffekseerMaterial>(StaticLoadObject(UEffekseerMaterial::StaticClass(), NULL, path_.c_str()));
		uobject_->Materials.Add(material);

		if (material != nullptr)
		{
			auto data = new ::Effekseer::MaterialData();
			data->UserPtr = new EffekseerRendererUE4::Shader(material, false, false);
			data->ModelUserPtr = new EffekseerRendererUE4::Shader(material, true, false);

			if (material->GetNativePtr()->GetHasRefraction())
			{
				data->RefractionUserPtr = new EffekseerRendererUE4::Shader(material, false, true);
				data->RefractionModelUserPtr = new EffekseerRendererUE4::Shader(material, true, true);
			}

			data->IsSimpleVertex = material->GetNativePtr()->GetIsSimpleVertex();
			data->IsRefractionRequired = material->GetNativePtr()->GetHasRefraction();
			data->CustomData1 = material->GetNativePtr()->GetCustomData1Count();
			data->CustomData2 = material->GetNativePtr()->GetCustomData2Count();
			data->TextureCount = std::min(material->GetNativePtr()->GetTextureCount(), Effekseer::UserTextureSlotMax);
			data->UniformCount = material->GetNativePtr()->GetUniformCount();
			data->ShadingModel = material->GetNativePtr()->GetShadingModel();

			for (int32_t i = 0; i < data->TextureCount; i++)
			{
				data->TextureWrapTypes.at(i) = material->GetNativePtr()->GetTextureWrap(i);
			}

			return data;
		}
		else
		{
			UE_LOG(LogScript, Warning, TEXT("Failed to load %s"), path_.c_str());
		}

		return nullptr;
	}
	else
	{
		if (uobject_->Materials.Num() <= loadingIndex_) return nullptr;

		auto o = uobject_->Materials[loadingIndex_];
		loadingIndex_++;

		if (o != nullptr)
		{
			auto data = new ::Effekseer::MaterialData();
			data->UserPtr = new EffekseerRendererUE4::Shader(o, false, false);
			data->ModelUserPtr = new EffekseerRendererUE4::Shader(o, true, false);

			auto material = o;
			if (material->GetNativePtr()->GetHasRefraction())
			{
				data->RefractionUserPtr = new EffekseerRendererUE4::Shader(o, false, true);
				data->RefractionModelUserPtr = new EffekseerRendererUE4::Shader(o, true, true);
			}

			data->IsSimpleVertex = material->GetNativePtr()->GetIsSimpleVertex();
			data->IsRefractionRequired = material->GetNativePtr()->GetHasRefraction();
			data->CustomData1 = material->GetNativePtr()->GetCustomData1Count();
			data->CustomData2 = material->GetNativePtr()->GetCustomData2Count();
			data->TextureCount = std::min(material->GetNativePtr()->GetTextureCount(), Effekseer::UserTextureSlotMax);
			data->UniformCount = material->GetNativePtr()->GetUniformCount();
			data->ShadingModel = material->GetNativePtr()->GetShadingModel();

			for (int32_t i = 0; i < data->TextureCount; i++)
			{
				data->TextureWrapTypes.at(i) = material->GetNativePtr()->GetTextureWrap(i);
			}

			return data;
		}

		return nullptr;
	}
}

void MaterialLoader::Unload(::Effekseer::MaterialData* data)
{
	if (data == nullptr) return;
	auto p1 = (EffekseerRendererUE4::Shader*)data->UserPtr;
	auto p2 = (EffekseerRendererUE4::Shader*)data->ModelUserPtr;
	auto p3 = (EffekseerRendererUE4::Shader*)data->RefractionUserPtr;
	auto p4 = (EffekseerRendererUE4::Shader*)data->RefractionModelUserPtr;
	ES_SAFE_DELETE(p1);
	ES_SAFE_DELETE(p2);
	ES_SAFE_DELETE(p3);
	ES_SAFE_DELETE(p4);
	ES_SAFE_DELETE(data);
}

static ::Effekseer::Setting* CreateSetting()
{
	auto setting = ::Effekseer::Setting::Create();
	setting->SetTextureLoader(new TextureLoader());
	setting->SetModelLoader(new ModelLoader());
	setting->SetMaterialLoader(new MaterialLoader());
	return setting;
}

void UEffekseerEffect::LoadEffect(const uint8_t* data, int32_t size, const TCHAR* path, bool isResourceReset)
{
	auto uPath = uStr<260>(path);

	Name = tStr<260>(GetFileNameWithoutExtension(uPath.c_str()).c_str()).c_str();

	::Effekseer::Setting* setting = CreateSetting();
	 
	if (isResourceReset)
	{
		this->ColorTextures.Reset();
		this->DistortionTextures.Reset();
		this->Models.Reset();
		this->Materials.Reset();
	}

	auto textureLoader = (TextureLoader*)setting->GetTextureLoader();
	textureLoader->SetUObject(this, isResourceReset);

	auto modelLoader = (ModelLoader*)setting->GetModelLoader();
	modelLoader->SetUObject(this, isResourceReset);

	auto materialLoader = (MaterialLoader*)setting->GetMaterialLoader();
	materialLoader->SetUObject(this, isResourceReset);

	auto rootPath = uPath.c_str();
	EFK_CHAR parentPath[300];

	if (path != nullptr)
	{
		GetParentDir(parentPath, uPath.c_str());
		rootPath = parentPath;
	}
	
	loadedScale = Scale;

	auto effect = ::Effekseer::Effect::Create(setting, (void*)data, size, Scale, rootPath);
	setting->Release();

	effectPtr = effect;

#ifdef __EFFEKSEER_BUILD_VERSION16__
	if (effect != nullptr)
	{
		SetTextureAddressMode(effect->GetRoot());
	}
#endif

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
#ifdef __EFFEKSEER_BUILD_VERSION16__
			UTexture2D* alphaTexture = nullptr;
			UTexture2D* uvDistortionTexture = nullptr;
			UTexture2D* blendTexture = nullptr;
			UTexture2D* blendAlphaTexture = nullptr;
			UTexture2D* blendUVDistortionTexture = nullptr;
#endif
			if (param.Distortion)
			{
				if (0 <= param.ColorTextureIndex &&
					param.ColorTextureIndex < this->DistortionTextures.Num())
				{
					texture = this->DistortionTextures[param.ColorTextureIndex];
				}

#ifdef __EFFEKSEER_BUILD_VERSION16__
				if (0 <= param.AlphaTextureIndex && param.AlphaTextureIndex < this->DistortionTextures.Num())
				{
					alphaTexture = this->DistortionTextures[param.AlphaTextureIndex];
				}

				if (0 <= param.UVDistortionIndex && param.UVDistortionIndex < this->DistortionTextures.Num())
				{
					uvDistortionTexture = this->DistortionTextures[param.UVDistortionIndex];
				}

				if (0 <= param.BlendTextureIndex && param.BlendTextureIndex < this->DistortionTextures.Num())
				{
					blendTexture = this->DistortionTextures[param.BlendTextureIndex];
				}

				if (0 <= param.BlendAlphaTextureIndex && param.BlendAlphaTextureIndex < this->DistortionTextures.Num())
				{
					blendAlphaTexture = this->DistortionTextures[param.BlendAlphaTextureIndex];
				}

				if (0 <= param.BlendUVDistortionTextureIndex && param.BlendUVDistortionTextureIndex < this->DistortionTextures.Num())
				{
					blendUVDistortionTexture = this->DistortionTextures[param.BlendUVDistortionTextureIndex];
				}
#endif
			}
			else
			{
				if (0 <= param.ColorTextureIndex &&
					param.ColorTextureIndex < this->ColorTextures.Num())
				{
					texture = this->ColorTextures[param.ColorTextureIndex];
				}

#ifdef __EFFEKSEER_BUILD_VERSION16__
				if (0 <= param.AlphaTextureIndex && param.AlphaTextureIndex < this->ColorTextures.Num())
				{
					alphaTexture = this->ColorTextures[param.AlphaTextureIndex];
				}

				if (0 <= param.UVDistortionIndex && param.UVDistortionIndex < this->ColorTextures.Num())
				{
					uvDistortionTexture = this->ColorTextures[param.UVDistortionIndex];
				}

				if (0 <= param.BlendTextureIndex && param.BlendTextureIndex < this->ColorTextures.Num())
				{
					blendTexture = this->ColorTextures[param.BlendTextureIndex];
				}

				if (0 <= param.BlendAlphaTextureIndex && param.BlendAlphaTextureIndex < this->ColorTextures.Num())
				{
					blendAlphaTexture = this->ColorTextures[param.BlendAlphaTextureIndex];
				}

				if (0 <= param.BlendUVDistortionTextureIndex && param.BlendUVDistortionTextureIndex < this->ColorTextures.Num())
				{
					blendUVDistortionTexture = this->ColorTextures[param.BlendUVDistortionTextureIndex];
				}
#endif
			}

			UEffekseerEffectMaterialParameterHolder* mat = NewObject<UEffekseerEffectMaterialParameterHolder>();
			mat->Texture = texture;
#ifdef __EFFEKSEER_BUILD_VERSION16__
			mat->TextureAddressType = static_cast<int32>(param.WrapType);

			mat->AlphaTexture = alphaTexture;
			mat->AlphaTextureAddressType = static_cast<int32>(param.AlphaTexWrapType);

			mat->UVDistortionTexture = uvDistortionTexture;
			mat->UVDistortionTextureAddressType = static_cast<int32>(param.UVDistortionTexWrapType);
			
			mat->BlendTexture = blendTexture;
			mat->BlendTextureAddress = static_cast<int32>(param.BlendTexWrapType);

			mat->BlendAlphaTexture = blendAlphaTexture;
			mat->BlendAlphaTextureAddress = static_cast<int32>(param.BlendAlphaTexWrapType);

			mat->BlendUVDistortionTexture = blendUVDistortionTexture;
			mat->BlendUVDistortionTextureAddress = static_cast<int32>(param.BlendUVDistortionTexWrapType);

			mat->FlipbookParams.Enable = param.FlipbookParams.Enable;
			mat->FlipbookParams.LoopType = param.FlipbookParams.LoopType;
			mat->FlipbookParams.DivideX = param.FlipbookParams.DivideX;
			mat->FlipbookParams.DivideY = param.FlipbookParams.DivideY;

			mat->UVDistortionIntensity = param.UVDistortionIntensity;

			mat->TextureBlendType = param.TextureBlendType;

			mat->BlendUVDistortionIntensity = param.BlendUVDistortionIntensity;

			mat->EnableFalloff = param.EnableFalloff;
			mat->FalloffParam.ColorBlendType = param.FalloffParam.ColorBlendType;
			mat->FalloffParam.BeginColor = FLinearColor(param.FalloffParam.BeginColor[0], param.FalloffParam.BeginColor[1], param.FalloffParam.BeginColor[2], param.FalloffParam.BeginColor[3]);
			mat->FalloffParam.EndColor = FLinearColor(param.FalloffParam.EndColor[0], param.FalloffParam.EndColor[1], param.FalloffParam.EndColor[2], param.FalloffParam.EndColor[3]);
			mat->FalloffParam.Pow = param.FalloffParam.Pow;

			mat->EmissiveScaling = param.EmissiveScaling;

			mat->EdgeParams.Color = FLinearColor(param.EdgeParam.Color[0], param.EdgeParam.Color[1], param.EdgeParam.Color[2], param.EdgeParam.Color[3]);
			mat->EdgeParams.Threshold = param.EdgeParam.Threshold;
			mat->EdgeParams.ColorScaling = param.EdgeParam.ColorScaling;
#endif
			mat->IsDepthTestDisabled = !param.ZTest;
			mat->AlphaBlend = (EEffekseerAlphaBlendType)param.AlphaBlend;
#ifdef __EFFEKSEER_BUILD_VERSION16__
			mat->IsLighting = param.MaterialType == ::Effekseer::RendererMaterialType::Lighting;
#else
			mat->IsLighting = modelParam.Lighting;
#endif
			mat->IsDistorted = param.Distortion;

			this->EffekseerMaterials.Add(mat);

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

#ifdef __EFFEKSEER_BUILD_VERSION16__
void UEffekseerEffect::SetTextureAddressMode(::Effekseer::EffectNode* node)
{
	auto SetAtTextureAddressWrap = [](::Effekseer::EffectNode* node, int tex_index, bool is_distotion)
	{
		auto texture = (is_distotion == false) ? node->GetEffect()->GetColorImage(tex_index) : node->GetEffect()->GetDistortionImage(tex_index);
		if (texture == nullptr || ((UTexture2D*)texture->UserPtr) == nullptr)
		{
			return;
		}

		auto u_texture = ((UTexture2D*)texture->UserPtr);

		// ※クランプはマテリアルでUVを計算して行うので、テクスチャはインポート時に強制的にWrapに変更する
		u_texture->AddressX = TextureAddress::TA_Wrap;
		u_texture->AddressY = TextureAddress::TA_Wrap;

		u_texture->RefreshSamplerStates();
	};

	// 再帰
	for (int i = 0; i < node->GetChildrenCount(); i++)
	{
		SetTextureAddressMode(node->GetChild(i));
	}

	Effekseer::EffectBasicRenderParameter param = node->GetBasicRenderParameter();
	if (param.ColorTextureIndex >= 0)
	{
		SetAtTextureAddressWrap(node, param.ColorTextureIndex, param.Distortion);
	}
	if (param.AlphaTextureIndex >= 0)
	{
		SetAtTextureAddressWrap(node, param.AlphaTextureIndex, param.Distortion);
	}
}
#endif

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