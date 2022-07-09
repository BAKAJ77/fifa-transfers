#include <states/main_game.h>

void MainGame::Init()
{
}

void MainGame::Destroy()
{
}

void MainGame::Update(const float& deltaTime)
{
}

void MainGame::Render() const
{
}

bool MainGame::OnStartupTransitionUpdate(const float deltaTime)
{
    return true;
}

MainGame* MainGame::GetAppState()
{
    static MainGame appState;
    return &appState;
}
