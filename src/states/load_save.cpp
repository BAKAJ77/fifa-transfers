#include <states/load_save.h>
#include <states/main_menu.h>
#include <states/save_loading.h>

#include <interface/menu_button.h>
#include <serialization/json_loader.h>
#include <serialization/save_data.h>
#include <util/directory_system.h>

void LoadSave::Init()
{
    // Initialize the member variables
    this->goBack = false;
    this->logoOpacity = 0.0f;

    // Fetch the Bahnschrift Bold font
    this->font = FontLoader::GetInstance().GetFont("Bahnschrift Bold");

    // Query for any existing saves
    JSONLoader file(Util::GetAppDataDirectory() + "data/saves.json");
    for (const nlohmann::json& save : file.GetRoot())
        this->existingSaves.push_back({ save["filename"].get<std::string>(), save["playerCount"].get<int>(), save["growthSystem"].get<int>() });

    // Initialize the user interface
    this->userInterface = UserInterface(this->GetAppWindow(), 8.0f, 0.0f);
    this->userInterface.AddButton(new MenuButton({ 1745, 1005 }, { 300, 100 }, { 315, 115 }, "BACK"));

    this->userInterface.AddSelectionList("Existing Saves", { { 797.5f, 553 }, { 1545, 850 }, 80 });
    this->userInterface.GetSelectionList("Existing Saves")->AddCategory("Save Name");
    this->userInterface.GetSelectionList("Existing Saves")->AddCategory("Player Count");

    for (size_t index = 0; index < this->existingSaves.size(); index++)
    {
        const ExistingSave& save = this->existingSaves[index];

        // When adding the list element, we cut out the .json extension using the substring function so we have only the save name
        this->userInterface.GetSelectionList("Existing Saves")->AddElement({ save.fileName.substr(0, save.fileName.size() - 5),
            std::to_string(save.playerCount) }, (int)index);
    }
}

void LoadSave::Destroy() {}

void LoadSave::Update(const float& deltaTime)
{
    if (!this->goBack)
    {
        this->userInterface.Update(deltaTime);

        // When a save has been selected, attempt to load it
        if (this->userInterface.GetSelectionList("Existing Saves")->GetCurrentSelected() != -1)
            this->SwitchState(SaveLoading::GetAppState());
        
        // If the back button was clicked, start transition to go back to the play menu app state
        if (this->userInterface.GetButtons()[0]->WasClicked())
            this->goBack = true;
    }
    else
    {
        constexpr float transitionSpeed = 1000.0f;

        // Update the interface and small title logo fade out effect
        this->userInterface.SetOpacity(std::max(this->userInterface.GetOpacity() - (transitionSpeed * deltaTime), 0.0f));
        this->logoOpacity = this->userInterface.GetOpacity();

        if (this->userInterface.GetOpacity() == 0.0f)
        {
            // Update large title logo fade in effect
            MainMenu::GetAppState()->SetLogoOpacity(std::min(MainMenu::GetAppState()->GetLogoOpacity() + (transitionSpeed * deltaTime), 255.0f));
            if (MainMenu::GetAppState()->GetLogoOpacity() == 255.0f)
                this->PopState();
        }
    }
}

void LoadSave::Render() const
{
    // Render the small title logo
    Renderer::GetInstance().RenderSquare({ 169, 64 }, { 288, 78 }, TextureLoader::GetInstance().GetTexture("Title Logo Small"),
        { glm::vec3(255), this->logoOpacity });

    // Render the text description
    Renderer::GetInstance().RenderShadowedText({ 1300, 95 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 75, 
        "SAVE SELECTION", 5);

    // Render the user interface
    this->userInterface.Render();
}

bool LoadSave::OnStartupTransitionUpdate(const float deltaTime)
{
    constexpr float transitionSpeed = 1000.0f;

    // Update large title logo fade out effect
    MainMenu::GetAppState()->SetLogoOpacity(std::max(MainMenu::GetAppState()->GetLogoOpacity() - (transitionSpeed * deltaTime), 0.0f));

    if (MainMenu::GetAppState()->GetLogoOpacity() == 0.0f)
    {
        // Update fade in effect of the title logo
        this->logoOpacity = std::min(this->logoOpacity + (transitionSpeed * deltaTime), 255.0f);

        if (this->logoOpacity == 255.0f)
        {
            // Update the fade in effect of the user interface
            this->userInterface.SetOpacity(std::min(this->userInterface.GetOpacity() + (transitionSpeed * deltaTime), 255.0f));

            if (this->userInterface.GetOpacity() == 255.0f)
                return true;
        }
    }

    return false;
}

LoadSave* LoadSave::GetAppState()
{
    static LoadSave appState;
    return &appState;
}

const LoadSave::ExistingSave& LoadSave::GetSelectedExistingSave()
{
    return this->existingSaves[this->userInterface.GetSelectionList("Existing Saves")->GetCurrentSelected()];
}
