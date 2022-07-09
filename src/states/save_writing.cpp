#include <states/save_writing.h>
#include <serialization/save_data.h>

void SaveWriting::Init()
{
    // Fetch the Bahnschrift Bold font
    this->font = FontLoader::GetInstance().GetFont("Bahnschrift Bold");

    int x = 5;
    std::mutex mutex;

    SaveData::GetInstance().Write(x, mutex);
    this->PopState();
}

void SaveWriting::Destroy()
{
}

void SaveWriting::Update(const float& deltaTime)
{
}

void SaveWriting::Render() const
{
}

bool SaveWriting::OnStartupTransitionUpdate(const float deltaTime)
{
    return true;
}

SaveWriting* SaveWriting::GetAppState()
{
    static SaveWriting appState;
    return &appState;
}
