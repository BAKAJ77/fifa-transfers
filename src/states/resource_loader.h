#ifndef RESOURCE_LOADER_H
#define RESOURCE_LOADER_H

#include <core/application_state.h>
#include <serialization/json_loader.h>

class ResourceLoader : public AppState
{
private:
	JSONLoader resourcesListFile;
	uint32_t workDonePercentage;
	bool splashScreenStarted;
private:
	void LoadTextures();
	void LoadFonts();
	void LoadAudio();
protected:
	void Init() override;
	void Destroy() override;

	void Update(const float& deltaTime) override;
	void Render() const override;
public:
	static ResourceLoader* GetAppState();
};

#endif