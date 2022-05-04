
class SED_ViewportWindow;
class SGfx_World;
class SGfx_MaterialNodeGraph;

class SED_MaterialEditor
{
public:
	SED_MaterialEditor();
	~SED_MaterialEditor();

	bool Init();

	void OnUpdate();
	void OnRender();

private:
	SC_UniquePtr<SGfx_World> mGfxWorld;
	SC_UniquePtr<SED_ViewportWindow> mViewport;

	SGfx_MaterialNodeGraph* mCurrentGraph;

	bool mIsOpen;
};
