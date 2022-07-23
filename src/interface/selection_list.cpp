#include <interface/selection_list.h>
#include <graphics/renderer.h>

SelectionList::SelectionList() :
    fontSize(0), listOffset(0), opacity(0.0f), maxListSelectionsVisible(0), buttonHeight(0.0f), currentSelected(nullptr)
{}

SelectionList::SelectionList(const glm::vec2& pos, const glm::vec2& size, float buttonHeight, float opacity, float fontSize) :
    position(pos), size(size), buttonHeight(buttonHeight), opacity(opacity), listOffset(0), currentSelected(nullptr),
    fontSize(fontSize > 0.0f ? fontSize : (buttonHeight / 2.75f))
{
    // Load the font to be used
    this->font = FontLoader::GetInstance().GetFont("Bahnschrift Bold");

    // Calculate the max amount of list selection options that can be displayed at a time
    this->maxListSelectionsVisible = (int)((size.y - buttonHeight) / buttonHeight);

    // Setup the page navigation buttons
    this->previousPageButton = ButtonBase({ pos.x - (size.x / 2) + (buttonHeight / 3), pos.y + (size.y / 2) + (buttonHeight / 3) + 20 },
        glm::vec2(buttonHeight / 1.5f), glm::vec2((buttonHeight / 1.5f) + 10), glm::vec3(60), glm::vec3(80), glm::vec3(100), 255, 2.5f);

    this->nextPageButton = ButtonBase({ pos.x + (size.x / 2) - (buttonHeight / 3), pos.y + (size.y / 2) + (buttonHeight / 3) + 20 },
        glm::vec2(buttonHeight / 1.5f), glm::vec2((buttonHeight / 1.5f) + 10), glm::vec3(60), glm::vec3(80), glm::vec3(100), 255, 2.5f);
}

void SelectionList::SetOpacity(float opacity)
{
    this->opacity = opacity;
    this->previousPageButton.SetOpacity(opacity);
    this->nextPageButton.SetOpacity(opacity);

    for (Element& element : this->listElements)
        element.button.SetOpacity(opacity);
}

void SelectionList::AddCategory(const std::string_view& name)
{
    this->listCategories.push_back({ name.data(), Renderer::GetInstance().GetTextSize(this->font, (uint32_t)this->fontSize, name) });
}

void SelectionList::AddElement(const std::vector<std::string>& categoryValues, int value, const glm::vec3& baseColor, const glm::vec3& highlightColor, 
    const glm::vec3& edgeColor)
{
    // Calculate the sizes of the category text values
    std::vector<Category> categoryVals;
    for (const std::string& categoryValue : categoryValues)
        categoryVals.push_back({ categoryValue, Renderer::GetInstance().GetTextSize(this->font, (uint32_t)this->fontSize, categoryValue) });

    // Push the element into the list
    this->listElements.push_back({ categoryVals,
        ButtonBase({ this->position.x, this->position.y - ((this->size.y + this->buttonHeight) / 2) + (2 * this->buttonHeight) +
            (((this->listElements.size()) % (this->maxListSelectionsVisible)) * this->buttonHeight) }, { this->size.x, this->buttonHeight }, 
            { this->size.x, this->buttonHeight }, baseColor, highlightColor, edgeColor, 255.0f, 2.5f), value });
}

void SelectionList::Clear()
{
    this->listElements.clear();
}

void SelectionList::Reset()
{
    this->currentSelected = nullptr;
}

void SelectionList::Update(const float& deltaTime)
{
    if (this->opacity > 0)
    {
        for (int index = this->listOffset; index < std::min(this->listOffset + this->maxListSelectionsVisible, (int)this->listElements.size()); index++)
        {
            // Update the list buttons
            this->listElements[index].button.Update(deltaTime, 8.0f);

            // Check if a list button has been clicked
            if (this->listElements[index].button.WasClicked())
                this->currentSelected = &this->listElements[index];
        }

        // Update the page navigation buttons
        if (this->listOffset > 0)
        {
            this->previousPageButton.Update(deltaTime, 8.0f);
            if (this->previousPageButton.WasClicked())
                this->listOffset -= this->maxListSelectionsVisible;
        }

        if (this->listOffset + this->maxListSelectionsVisible < (int)this->listElements.size())
        {
            this->nextPageButton.Update(deltaTime, 8.0f);
            if (this->nextPageButton.WasClicked())
                this->listOffset += this->maxListSelectionsVisible;
        }
    }
}

void SelectionList::Render(float masterOpacity) const
{
    // Render the list category identifier bar and its shadow
    Renderer::GetInstance().RenderSquare({ this->position.x + 5.0f, this->position.y - ((this->size.y - this->buttonHeight) / 2) + 5.0f },
        { this->size.x, this->buttonHeight }, { glm::vec3(0), (((this->opacity * masterOpacity) / 255.0f) * 0.5f) });

    Renderer::GetInstance().RenderSquare({ this->position.x, this->position.y - ((this->size.y - this->buttonHeight) / 2) }, 
        { this->size.x, this->buttonHeight }, { glm::vec3(60), (this->opacity * masterOpacity) / 255.0f });

    // Render the list buttons
    for (int index = this->listOffset; index < std::min(this->listOffset + this->maxListSelectionsVisible, (int)this->listElements.size()); index++)
        this->listElements[index].button.Render(masterOpacity);

    // Render the category dividor lines
    for (int index = 1; index < this->listCategories.size(); index++)
    {
        Renderer::GetInstance().RenderSquare({ (this->position.x - (this->size.x / 2)) + (index * (this->size.x / (float)this->listCategories.size())),
            this->position.y - ((this->size.y - this->buttonHeight) / 2) }, { 2, this->buttonHeight * 0.9f }, { glm::vec3(80),
            (this->opacity * masterOpacity) / 255.0f });
    }

    // Render the list category indentifier text
    constexpr float textOffsetFromEdge = 30.0f;

    for (int index = 0; index < this->listCategories.size(); index++)
    {
        const Category& category = this->listCategories[index];

        Renderer::GetInstance().RenderText({ 
            this->position.x - (this->size.x / 2) + (index * (this->size.x / (float)this->listCategories.size())) + textOffsetFromEdge,
            this->position.y - ((this->size.y - this->buttonHeight) / 2) + (category.textSize.y / 2) },
            { glm::vec3(255), (this->opacity * masterOpacity) / 255.0f }, this->font, (uint32_t)this->fontSize, category.text);
    }

    // Render each list button's text
    for (int index = this->listOffset; index < std::min(this->listOffset + this->maxListSelectionsVisible, (int)this->listElements.size()); index++)
    {
        const Element& element = this->listElements[index];

        for (int jIndex = 0; jIndex < element.categoryValues.size(); jIndex++)
        {
            const Category& category = element.categoryValues[jIndex];

            Renderer::GetInstance().RenderText({ 
                this->position.x - (this->size.x / 2) + (jIndex * (this->size.x / (float)this->listCategories.size())) + textOffsetFromEdge,
                element.button.GetPosition().y + (category.textSize.y / 2) }, { glm::vec3(255), (this->opacity * masterOpacity) / 255.0f }, this->font, 
                (uint32_t)this->fontSize, category.text);
        }
    }

    // Render the page navigation buttons
    if (this->listOffset > 0)
    {
        this->previousPageButton.Render(masterOpacity);
        Renderer::GetInstance().RenderTriangle({ this->previousPageButton.GetPosition().x - (this->previousPageButton.GetCurrentSize().x / 3.5f), 
            this->previousPageButton.GetPosition().y }, { this->previousPageButton.GetCurrentSize().x / 1.5f, this->previousPageButton.GetCurrentSize().y }, 
            { glm::vec3(40), (this->opacity * masterOpacity) / 255.0f }, 90);
    }

    if (this->listOffset + this->maxListSelectionsVisible < (int)this->listElements.size())
    {
        this->nextPageButton.Render(masterOpacity);
        Renderer::GetInstance().RenderTriangle({ this->nextPageButton.GetPosition().x + (this->nextPageButton.GetCurrentSize().x / 3.5f),
            this->nextPageButton.GetPosition().y }, { this->nextPageButton.GetCurrentSize().x / 1.5f, this->nextPageButton.GetCurrentSize().y },
            { glm::vec3(40), (this->opacity * masterOpacity) / 255.0f }, 270);
    }
}

int SelectionList::GetCurrentSelected() const
{
    if (this->currentSelected)
        return this->currentSelected->value;

    return -1;
}

std::vector<SelectionList::Element>& SelectionList::GetListElements()
{
    return this->listElements;
}

const glm::vec2& SelectionList::GetPosition() const
{
    return this->position;
}

const glm::vec2& SelectionList::GetSize() const
{
    return this->size;
}

const float& SelectionList::GetOpacity() const
{
    return this->opacity;
}
