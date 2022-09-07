#ifndef SPLASH_SCREEN_H
#define SPLASH_SCREEN_H

#include <core/application_state.h>

class SplashScreen : public AppState
{
private:
	float devLogoOpacity;
	bool devLogoEffectDone;
private:
	void UpdateDevLogoEffect(const float& deltaTime);
protected:
	void Init() override;
	void Destroy() override;

	void Update(const float& deltaTime) override;
	void Render() const override;

	bool OnStartupTransitionUpdate(const float deltaTime) override;
public:
	static SplashScreen* GetAppState();
};

#endif
