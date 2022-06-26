#ifndef SPLASH_SCREEN_H
#define SPLASH_SCREEN_H

#include <core/application_state.h>

class SplashScreen : public AppState
{
protected:
	void Init() override;
	void Destroy() override;

	void Update(const float& deltaTime) override;
	void Render() const override;
public:
	static SplashScreen* GetAppState();
};

#endif
