#pragma once

class SR_Texture;

///////////////////////////////////////
// Declarations

enum class SED_DrawDataType
{
	Int,
	Uint,
	Float,
};


///////////////////////////////////////
// Widgets

void SED_Text(const char* aFmtStr, ...);

bool SED_TypeSlider(const char* aLabel, SED_DrawDataType aType, void* aVariable, uint8 aVectorSize, void* aMin = nullptr, void* aMax = nullptr, const char* aFormat = nullptr);
bool SED_TypeField(const char* aLabel, SED_DrawDataType aType, void* aVariable, uint8 aVectorSize, void* aMin = nullptr, void* aMax = nullptr, float aStepRate = 1.0f, const char* aFormat = nullptr);

bool SED_FloatSlider(const char* aLabel, float& aVariable, float aMin = 0.0f, float aMax = 0.0f, const char* aFormat = "%.3f");
bool SED_FloatSlider(const char* aLabel, SC_Vector2& aVariable, float aMin = 0.0f, float aMax = 0.0f, const char* aFormat = "%.3f");
bool SED_FloatSlider(const char* aLabel, SC_Vector& aVariable, float aMin = 0.0f, float aMax = 0.0f, const char* aFormat = "%.3f");
bool SED_FloatSlider(const char* aLabel, SC_Vector4& aVariable, float aMin = 0.0f, float aMax = 0.0f, const char* aFormat = "%.3f");
bool SED_FloatField(const char* aLabel, float& aVariable, float aMin = 0.0f, float aMax = 0.0f, float aStepRate = 1.0f, const char* aFormat = "%.3f");
bool SED_FloatField(const char* aLabel, SC_Vector2& aVariable, float aMin = 0.0f, float aMax = 0.0f, float aStepRate = 1.0f, const char* aFormat = "%.3f");
bool SED_FloatField(const char* aLabel, SC_Vector& aVariable, float aMin = 0.0f, float aMax = 0.0f, float aStepRate = 1.0f, const char* aFormat = "%.3f");
bool SED_FloatField(const char* aLabel, SC_Vector4& aVariable, float aMin = 0.0f, float aMax = 0.0f, float aStepRate = 1.0f, const char* aFormat = "%.3f");

bool SED_IntSlider(const char* aLabel, int& aVariable, int aMin = 0, int aMax = 0);
bool SED_IntSlider(const char* aLabel, SC_IntVector2& aVariable, int aMin = 0, int aMax = 0);
bool SED_IntSlider(const char* aLabel, SC_IntVector& aVariable, int aMin = 0, int aMax = 0);
bool SED_IntSlider(const char* aLabel, SC_IntVector4& aVariable, int aMin = 0, int aMax = 0);
bool SED_IntField(const char* aLabel, int& aVariable, int aMin = 0, int aMax = 0, int aStepRate = 1);
bool SED_IntField(const char* aLabel, SC_IntVector2& aVariable, int aMin = 0, int aMax = 0, int aStepRate = 1);
bool SED_IntField(const char* aLabel, SC_IntVector& aVariable, int aMin = 0, int aMax = 0, int aStepRate = 1);
bool SED_IntField(const char* aLabel, SC_IntVector4& aVariable, int aMin = 0, int aMax = 0, int aStepRate = 1);

bool SED_UintSlider(const char* aLabel, uint32& aVariable, uint32 aMin = 0, uint32 aMax = 0);
bool SED_UintSlider(const char* aLabel, SC_Vector2u& aVariable, uint32 aMin = 0, uint32 aMax = 0);
bool SED_UintSlider(const char* aLabel, SC_Vector3u& aVariable, uint32 aMin = 0, uint32 aMax = 0);
bool SED_UintSlider(const char* aLabel, SC_Vector4u& aVariable, uint32 aMin = 0, uint32 aMax = 0);
bool SED_UintField(const char* aLabel, uint32& aVariable, uint32 aMin = 0, uint32 aMax = 0, uint32 aStepRate = 1);
bool SED_UintField(const char* aLabel, SC_Vector2u& aVariable, uint32 aMin = 0, uint32 aMax = 0, uint32 aStepRate = 1);
bool SED_UintField(const char* aLabel, SC_Vector3u& aVariable, uint32 aMin = 0, uint32 aMax = 0, uint32 aStepRate = 1);
bool SED_UintField(const char* aLabel, SC_Vector4u& aVariable, uint32 aMin = 0, uint32 aMax = 0, uint32 aStepRate = 1);

bool SED_Checkbox(const char* aLabel, bool& aBool);
bool SED_Button(const char* aLabel, const SC_Vector2& aSize = SC_Vector2(0));
bool SED_ImageButton(SR_Texture* aImage, const SC_Vector2& aSize = SC_Vector2(0), const SC_Vector2& aMinUV = SC_Vector2(0), const SC_Vector2& aMaxUV = SC_Vector2(1));

void SED_Image(SR_Texture* aImage, const SC_Vector2& aSize = SC_Vector2(0), const SC_Vector2& aMinUV = SC_Vector2(0), const SC_Vector2& aMaxUV = SC_Vector2(1));

bool SED_ColorPickerRGB(const char* aLabel, SC_LinearColor& aColor);
bool SED_ColorPickerRGBA(const char* aLabel, SC_LinearColor& aColor);
bool SED_ColorPickerRGB(const char* aLabel, SC_Color& aColor);
bool SED_ColorPickerRGBA(const char* aLabel, SC_Color& aColor);

bool SED_ColorEditRGB(const char* aLabel, SC_LinearColor& aColor);
bool SED_ColorEditRGBA(const char* aLabel, SC_LinearColor& aColor);
bool SED_ColorEditRGB(const char* aLabel, SC_Color& aColor);
bool SED_ColorEditRGBA(const char* aLabel, SC_Color& aColor);

bool SED_Selectable(const char* aLabel, bool aSelected = false);

bool SED_BeginComboBox(const char* aLabel, const char* aPreviewText = nullptr);
void SED_EndComboBox();

bool SED_BeginMenuBar();
void SED_EndMenuBar();
bool SED_BeginMenu(const char* aLabel, bool aEnabled = true);
void SED_EndMenu();
bool SED_MenuItem(const char* aLabel, const char* aShortcut = nullptr, bool aSelected = false, bool aEnabled = true);


///////////////////////////////////////
// Misc

struct SED_DragDropPayload
{
	SED_DragDropPayload() : mSize(0), mData(nullptr) {}
	~SED_DragDropPayload() { delete[] mData; }

	SED_DragDropPayload(const SED_DragDropPayload& aOther) 
		: mSize(aOther.mSize)
		, mData(nullptr) 
	{ 
		if (mSize > 0)
		{
			mData = new uint8[mSize];
			SC_Memcpy(mData, aOther.mData, mSize);
		}
	}

	uint32 mSize;
	void* mData;
};

bool SED_BeginDragDropTarget();
void SED_EndDragDropTarget();
bool SED_BeginDragDropSource();
void SED_EndDragDropSource();

bool SED_PushDragDropPayload(const char* aType, const void* aData, SC_SizeT aSize/*, ImGuiCond cond*/);
bool SED_RetrieveDragDropPayload(const char* aType, SED_DragDropPayload& aPayload);
