#pragma once

class SED_Window
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

	virtual const char* GetWindowName() const { return "<Unnamed Window>"; }
	virtual bool SuppressUpdatesIfClosed() const { return true; }

protected:
	virtual void OnUpdate() {}
	virtual void OnDraw() = 0;

	bool mIsOpen;
	bool mSuppressUpdates;
};