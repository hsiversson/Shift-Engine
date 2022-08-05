#pragma once

class SED_Window : public SC_ReferenceCounted
{
public:
	SED_Window();
	virtual ~SED_Window();

	void Update();
	void Draw();

	void Open();
	void Close();
	bool IsOpen() const;

	void SuppressUpdates(bool aValue);

	const SC_Vector2& GetPosition() const;
	const SC_Vector2& GetSize() const;
	const SC_Vector2& GetContentRegionMin() const;
	const SC_Vector2& GetContentRegionMax() const;
	const SC_Vector2& GetAvailableContentRegion() const;

	bool IsFocused() const;

	virtual const char* GetWindowName() const { return "<Unnamed Window>"; }
	virtual bool SuppressUpdatesIfClosed() const { return true; }
	virtual bool HasRightClickMenu() const { return false; }

protected:
	virtual void OnUpdate() {}
	virtual void OnDraw() = 0;
	virtual void OnRightClick() {}

	SC_Vector2 mPosition;
	SC_Vector2 mSize;
	SC_Vector2 mContentRegionMin;
	SC_Vector2 mContentRegionMax;
	SC_Vector2 mAvailableContentRegion;

	bool mIsOpen;
	bool mSuppressUpdates;
	bool mIsFocused;
	bool mIsCollapsed;
};