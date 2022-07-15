#include "SED_Window.h"

#include "imgui.h"

SED_Window::SED_Window()
	: mIsOpen(true)
	, mSuppressUpdates(false)
	, mIsFocused(false)
	, mIsCollapsed(false)
{
}

SED_Window::~SED_Window()
{
}

void SED_Window::Update()
{
	if ((SuppressUpdatesIfClosed() && !mIsOpen) || mSuppressUpdates)
		return;

	OnUpdate();
}

void SED_Window::Draw()
{
	if (!mIsOpen)
		return;

	ImGui::SetNextWindowSize(ImVec2(100, 100), ImGuiCond_FirstUseEver);
	if (ImGui::Begin(GetWindowName(), &mIsOpen))
	{
		mPosition = ImGui::GetWindowPos();
		mSize = ImGui::GetWindowSize();
		mContentRegionMin = ImGui::GetWindowContentRegionMin();
		mContentRegionMax = ImGui::GetWindowContentRegionMax();
		mAvailableContentRegion = ImGui::GetContentRegionAvail();
		mIsFocused = ImGui::IsWindowFocused();
		mIsCollapsed = ImGui::IsWindowCollapsed();

		if (HasRightClickMenu() && ImGui::BeginPopupContextWindow())
		{
			OnRightClick();
			ImGui::EndPopup();
		}

		OnDraw();
	}
	ImGui::End();
}

void SED_Window::Open()
{
	mIsOpen = true;
}

void SED_Window::Close()
{
	mIsOpen = false;
}

bool SED_Window::IsOpen() const
{
	return mIsOpen;
}

void SED_Window::SuppressUpdates(bool aValue)
{
	mSuppressUpdates = aValue;
}

const SC_Vector2& SED_Window::GetPosition() const
{
	return mPosition;
}

const SC_Vector2& SED_Window::GetSize() const
{
	return mSize;
}

const SC_Vector2& SED_Window::GetContentRegionMin() const
{
	return mContentRegionMin;
}

const SC_Vector2& SED_Window::GetContentRegionMax() const
{
	return mContentRegionMax;
}

const SC_Vector2& SED_Window::GetAvailableContentRegion() const
{
	return mAvailableContentRegion;
}

bool SED_Window::IsFocused() const
{
	return mIsFocused;
}
