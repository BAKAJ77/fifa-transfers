#ifndef RESOURCE_LOADER_H
#define RESOURCE_LOADER_H

#include <core/application_state.h>

class ResourceLoader : public AppState
{
private:
	uint32_t workDonePercentage;
private:
	void LoadTextures();
	void LoadAudio();
	void LoadFonts();
protected:
	void Init() override;
	void Destroy() override;

	void Update(const float& deltaTime) override;
	void Render() const override;
public:
	static ResourceLoader* GetAppState();
};

#endif