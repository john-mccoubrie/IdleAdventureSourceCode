


#include "Quest/QuestLog.h"

void UQuestLog::AddQuest(UQuest* Quest)
{
    if (Quest)
    {
        Quests.AddUnique(Quest);
        // Additional logic to update UI or state
    }
}

void UQuestLog::CompleteQuest(UQuest* Quest)
{
    if (Quest)
    {
        Quest->Complete();
        // Additional logic for quest completion such as removing from the list, updating UI, etc.
    }
}