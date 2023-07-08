#include "EffekseerCurve.h"

#include "EffekseerCustomVersion.h"
#include "EffekseerInternalCurve.h"

void UEffekseerCurve::BeginDestroy()
{
	ReleaseModel();
	Super::BeginDestroy();
}

void UEffekseerCurve::LoadModel(const uint8_t* data, int32_t size, const TCHAR* path)
{
	auto curve = new EffekseerInternalCurve((void*)data, size);
	curvePtr = curve;
}

void UEffekseerCurve::ReleaseModel()
{
	auto p = (EffekseerInternalCurve*)curvePtr;
	ES_SAFE_RELEASE(p);
	curvePtr = nullptr;
}

void UEffekseerCurve::Load(const uint8_t* data, int32_t size, const TCHAR* path)
{
	buffer.Reset(0);
	buffer.Append(data, size);
	LoadModel(data, size, path);
}

void UEffekseerCurve::GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const
{
#if WITH_EDITORONLY_DATA
	if (AssetImportData)
	{
		OutTags.Add(FAssetRegistryTag(SourceFileTagName(), AssetImportData->GetSourceData().ToJson(), FAssetRegistryTag::TT_Hidden));
	}
#endif

	Super::GetAssetRegistryTags(OutTags);
}

void UEffekseerCurve::AssignInternalPtr()
{
	auto p = (EffekseerInternalCurve*)curvePtr;
	if (p == nullptr)
		return;

	p->UserData = this;
}

FEffekseerCurve UEffekseerCurve::GetCurve()
{
	FEffekseerCurve curve;

	auto p = (EffekseerInternalCurve*)curvePtr;
	if (p == nullptr)
		return curve;

	return curve;
}

void UEffekseerCurve::Serialize(FArchive& Ar)
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
		ReleaseModel();

		auto path = GetPathName();
		LoadModel(buffer.GetData(), buffer.Num(), *path);

		AssignInternalPtr();
	}
}