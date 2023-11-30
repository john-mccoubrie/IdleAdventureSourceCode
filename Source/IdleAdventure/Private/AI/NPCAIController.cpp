
#include "AI/NPCAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

ANPCAIController::ANPCAIController()
{
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>("BlackboardComponent");
	check(BlackboardComponent);
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>("BehaviorTreeComponent");
	check(BehaviorTreeComponent);
}
