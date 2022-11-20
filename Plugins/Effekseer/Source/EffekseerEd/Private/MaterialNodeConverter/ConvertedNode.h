
#pragma once

#include "CoreMinimal.h"
#include "EffekseerUECompatibility.h"
#include "Materials/MaterialExpression.h"
#include <memory>

struct ConvertedNodeHelper
{
	static void AddExpression(UMaterial* material, UMaterialExpression* expression)
	{
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 1
		material->GetEditorOnlyData()->ExpressionCollection.AddExpression(expression);
#else
		material->Expressions.Add(expression);
#endif
	}

	static void ConnectToOpacity(UMaterial* material, UMaterialExpression* expression)
	{
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 1
		material->GetEditorOnlyData()->Opacity.Expression = expression;
#else
		material->Opacity.Expression = expression;
#endif
	}

	static void ConnectToOpacityMask(UMaterial* material, UMaterialExpression* expression)
	{
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 1
		material->GetEditorOnlyData()->OpacityMask.Expression = expression;
#else
		material->OpacityMask.Expression = expression;
#endif
	}

	static void ConnectToBaseColor(UMaterial* material, UMaterialExpression* expression)
	{
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 1
		material->GetEditorOnlyData()->BaseColor.Expression = expression;
#else
		material->BaseColor.Expression = expression;
#endif
	}

	static void ConnectToEmissiveColor(UMaterial* material, UMaterialExpression* expression)
	{
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 1
		material->GetEditorOnlyData()->EmissiveColor.Expression = expression;
#else
		material->EmissiveColor.Expression = expression;
#endif
	}

	static FScalarMaterialInput& GetRoughness(UMaterial* material)
	{
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 1
		return material->GetEditorOnlyData()->Roughness;
#else
		return material->Roughness;
#endif
	}

	static FScalarMaterialInput& GetAmbientOcclusion(UMaterial* material)
	{
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 1
		return material->GetEditorOnlyData()->AmbientOcclusion;
#else
		return material->AmbientOcclusion;
#endif
	}

	static FScalarMaterialInput& GetMetallic(UMaterial* material)
	{
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 1
		return material->GetEditorOnlyData()->Metallic;
#else
		return material->Metallic;
#endif
	}

	static FVectorMaterialInput& GetWorldPositionOffset(UMaterial* material)
	{
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 1
		return material->GetEditorOnlyData()->WorldPositionOffset;
#else
		return material->WorldPositionOffset;
#endif
	}
};

struct ConvertedNodeOutputConnector
{
	UMaterialExpression* Expression = nullptr;
	int32_t OutputIndex = 0;

	ConvertedNodeOutputConnector(UMaterialExpression* e, int32_t ind)
		: Expression(e)
		, OutputIndex(ind)
	{
	}

	void Apply(FExpressionInput& input)
	{
		input.Expression = Expression;
		input.OutputIndex = OutputIndex;
	}
};

class ConvertedNode
{
private:
public:
	ConvertedNode() = default;
	virtual ~ConvertedNode() = default;

	virtual UMaterialExpression* GetExpression() const
	{
		return nullptr;
	}

	virtual UMaterialExpression* GetExpressions(int32_t i) const
	{
		return GetExpression();
	}

	virtual int32_t GetExpressionCount() const
	{
		return 1;
	}

	virtual ConvertedNodeOutputConnector GetNodeOutputConnector(int32_t index) const
	{
		return ConvertedNodeOutputConnector(GetExpressions(index), 0);
	}

	virtual void Connect(int targetInd, std::shared_ptr<ConvertedNode> outputNode, int32_t outputNodePinIndex)
	{
	}

	virtual void SetEditorPosition(int32_t x, int32_t y)
	{
		for (int32_t i = 0; i < GetExpressionCount(); i++)
		{
			auto expression = GetExpressions(i);
			if (expression != nullptr)
			{
				expression->MaterialExpressionEditorX = x;
				expression->MaterialExpressionEditorY = y;
			}
		}
	}
};

template <class T>
class ConvertedNodeSimple : public ConvertedNode
{
private:
	T* expression_ = nullptr;

public:
	ConvertedNodeSimple(UMaterial* material, std::shared_ptr<NativeEffekseerMaterialContext> effekseerMaterial, std::shared_ptr<EffekseerMaterial::Node> effekseerNode)
	{
		expression_ = NewObject<T>(material);
		ConvertedNodeHelper::AddExpression(material, expression_);
	}

	UMaterialExpression* GetExpression() const override
	{
		return expression_;
	}
};