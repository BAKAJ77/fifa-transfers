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
	// Loads all texture resources.
	void LoadTextures();

	// Loads all font resources.
	void LoadFonts();

	// Loads all audio resources.
	void LoadAudio();

	// Loads the default database.
	void LoadDefaultDatabase();
protected:
	void Init() override;
	void Destroy() override;

	void Update(const float& deltaTime) override;
	void Render() const override;
public:
	static ResourceLoader* GetAppState();
};

#endif