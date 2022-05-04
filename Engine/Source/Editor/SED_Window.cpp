#include "SED_Window.h"

#include "imgui.h"

SED_Window::SED_Window()
	: mIsOpen(true)
	, mSuppressUpdates(false)
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
		OnDraw();
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
