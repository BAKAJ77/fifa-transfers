#ifndef FINANCIALS_GENERATION_H
#define FINANCIALS_GENERATION_H

#include <core/application_state.h>
#include <interface/user_interface.h>

class FinancialsGeneration : public AppState
{
private:
	struct UserFinancials
	{
		int totalRevenue = 0, totalExpenses = 0, totalWages = 0, previousTransferBudget = 0, newTransferBudget = 0, previousWageBudget = 0, newWageBudget = 0;
	};
private:
	UserInterface userInterface;
	FontPtr font;
	std::vector<UserFinancials> calculatedUserFinancials;
	int userIndex;
protected:
	void Init() override;
	void Destroy() override;

	void Update(const float& deltaTime) override;
	void Render() const override;

	bool OnStartupTransitionUpdate(const float deltaTime) override;
	bool OnPauseTransitionUpdate(const float deltaTime) override;
public:
	static FinancialsGeneration* GetAppState();
};

#endif