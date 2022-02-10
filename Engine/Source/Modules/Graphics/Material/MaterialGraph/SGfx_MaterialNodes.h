#pragma once
#include "SGfx_MaterialNode.h"
#include "RenderCore/ShaderCompiler/SR_ShaderGenerationProperties.h"

class SGfx_MaterialNode_Constant : public SGfx_MaterialNode
{
public:
	SGfx_MaterialNode_Constant();
	bool Compile(SGfx_MaterialCompiler* aCompiler) override;

	void SetValue(bool  aValue);
	void SetValue(float aValue);
	void SetValue(const SC_Vector2& aValue);
	void SetValue(const SC_Vector&  aValue);
	void SetValue(const SC_Vector4& aValue);

public:
	enum class Type
	{
		Bool,
		Float,
		Float2,
		Float3,
		Float4,
	};
	enum OutputPinIndex
	{
		Out
	};

private:
	union Value
	{
		Value() { SC_ZeroMemory(this, sizeof(Value)); }
		~Value() {}

		bool mBool;
		float mFloat;
		SC_Vector2 mFloat2;
		SC_Vector mFloat3;
		SC_Vector4 mFloat4;
	} mValue;

	Type mType;
	SR_ShaderVariableProperties mVariableProps;
};

class SGfx_MaterialNode_Add : public SGfx_MaterialNode
{
public:
	SGfx_MaterialNode_Add();
	bool Compile(SGfx_MaterialCompiler* aCompiler) override;

public:
	enum InputPinIndex
	{
		In1,
		In2
	};
	enum OutputPinIndex
	{
		Out
	};

};

class SGfx_MaterialNode_Subtract : public SGfx_MaterialNode
{
public:
	SGfx_MaterialNode_Subtract();
	bool Compile(SGfx_MaterialCompiler* aCompiler) override;

public:
	enum PinIndex
	{
		In1,
		In2
	};
	enum OutputPinIndex
	{
		Out
	};
};

class SGfx_MaterialNode_Multiply : public SGfx_MaterialNode
{
public:
	SGfx_MaterialNode_Multiply();
	bool Compile(SGfx_MaterialCompiler* aCompiler) override;

public:
	enum PinIndex
	{
		In1,
		In2
	};
	enum OutputPinIndex
	{
		Out
	};
};

class SGfx_MaterialNode_Divide : public SGfx_MaterialNode
{
public:
	SGfx_MaterialNode_Divide();
	bool Compile(SGfx_MaterialCompiler* aCompiler) override;

public:
	enum PinIndex
	{
		In1,
		In2
	};
	enum OutputPinIndex
	{
		Out
	};
};

class SGfx_MaterialNode_Reciprocal : public SGfx_MaterialNode
{
public:
	SGfx_MaterialNode_Reciprocal();
	bool Compile(SGfx_MaterialCompiler* aCompiler) override;
public:
	enum InputPinIndex
	{
		In
	};
	enum OutputPinIndex
	{
		Out
	};
};

class SGfx_MaterialNode_Abs : public SGfx_MaterialNode
{
public:
	SGfx_MaterialNode_Abs();
	bool Compile(SGfx_MaterialCompiler* aCompiler) override;
public:
	enum InputPinIndex
	{
		In
	};
	enum OutputPinIndex
	{
		Out
	};
};

class SGfx_MaterialNode_Length : public SGfx_MaterialNode
{
public:
	SGfx_MaterialNode_Length();
	bool Compile(SGfx_MaterialCompiler* aCompiler) override;
public:
	enum InputPinIndex
	{
		In
	};
	enum OutputPinIndex
	{
		Out
	};
};

class SGfx_MaterialNode_Normalize : public SGfx_MaterialNode
{
public:
	SGfx_MaterialNode_Normalize();
	bool Compile(SGfx_MaterialCompiler* aCompiler) override;
public:
	enum InputPinIndex
	{
		In
	};
	enum OutputPinIndex
	{
		Out
	};
};

class SGfx_MaterialNode_Saturate : public SGfx_MaterialNode
{
public:
	SGfx_MaterialNode_Saturate();
	bool Compile(SGfx_MaterialCompiler* aCompiler) override;
public:
	enum InputPinIndex
	{
		In
	};
	enum OutputPinIndex
	{
		Out
	};
};

class SGfx_MaterialNode_Dot : public SGfx_MaterialNode
{
public:
	SGfx_MaterialNode_Dot();
	bool Compile(SGfx_MaterialCompiler* aCompiler) override;
public:
	enum InputPinIndex
	{
		In1,
		In2
	};
	enum OutputPinIndex
	{
		Out
	};
};

class SGfx_MaterialNode_Min : public SGfx_MaterialNode
{
public:
	SGfx_MaterialNode_Min();
	bool Compile(SGfx_MaterialCompiler* aCompiler) override;
public:
	enum InputPinIndex
	{
		In1,
		In2
	};
	enum OutputPinIndex
	{
		Out
	};
};

class SGfx_MaterialNode_Max : public SGfx_MaterialNode
{
public:
	SGfx_MaterialNode_Max();
	bool Compile(SGfx_MaterialCompiler* aCompiler) override;
public:
	enum InputPinIndex
	{
		In1,
		In2
	};
	enum OutputPinIndex
	{
		Out
	};
};

class SGfx_MaterialNode_Clamp : public SGfx_MaterialNode
{
public:
	SGfx_MaterialNode_Clamp();
	bool Compile(SGfx_MaterialCompiler* aCompiler) override;
public:
	enum InputPinIndex
	{
		InValue,
		MinClamp,
		MaxClamp
	};
	enum OutputPinIndex
	{
		Out
	};
};
