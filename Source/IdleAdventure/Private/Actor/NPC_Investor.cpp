
#include "Actor/NPC_Investor.h"
#include "Core/PlayFabClientDataModels.h"
#include "Core/PlayFabClientAPI.h"
#include <cstdlib>
#include <ctime>
#include <PlayerEquipment/BonusManager.h>
#include <PlayFab/PlayFabManager.h>

ANPC_Investor::ANPC_Investor()
{
    UDataTable* InvestingDataTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, TEXT("/Game/Blueprints/DataTables/DT_InvestingValues.DT_InvestingValues")));

    FName RowKey = "InvestingValues";
    FInvestingValues* InvestingValues = InvestingDataTable->FindRow<FInvestingValues>(RowKey, TEXT("Context String"), true);

    if (InvestingValues)
    {
        BaseSuccessChance = InvestingValues->BaseSuccessChance;
        WisdomInvestmentMultiplier = InvestingValues->WisdomInvestmentMultiplier;
        TemperanceInvestmentMultiplier = InvestingValues->TemperanceInvestmentMultiplier;
        JusticeInvestmentMultiplier = InvestingValues->JusticeInvestmentMultiplier;
        LegendaryInvestmentMultiplier = InvestingValues->LegendaryInvestmentMultiplier;
        PowerFactor = InvestingValues->PowerFactor;
        LossPercentage = InvestingValues->LossPercentage;
    }
}

//this needs to be called somewhere
void ANPC_Investor::Interact()
{
	UE_LOG(LogTemp, Warning, TEXT("NPC_Investor Interact"));
	OnInteractedWithInvestor.Broadcast();

	//Grab players essence counts and store in a struct
    GetCurrentEssenceCounts();
}

void ANPC_Investor::BeginPlay()
{
	Super::BeginPlay();

	clientAPI = IPlayFabModuleInterface::Get().GetClientAPI();

    //Seed the random number generator
    std::srand(std::time(nullptr));

    
}

int32 ANPC_Investor::HandleInvest(int32 PlayerWisdomAmt, int32 PlayerTemperanceAmt, int32 PlayerJusticeAmt, int32 PlayerLegendaryAmt)
{
    UE_LOG(LogTemp, Warning, TEXT("HandleInvest called..."));

    // Check if the player has enough essence to make the investment
    if (PlayerWisdomAmt > PlayFabWisdom || PlayerTemperanceAmt > PlayFabTemperance ||
        PlayerJusticeAmt > PlayFabJustice || PlayerLegendaryAmt > PlayFabLegendary)
    {
        UE_LOG(LogTemp, Warning, TEXT("Not enough essence for investment."));
        AGameChatManager* GameChatManager = AGameChatManager::GetInstance(GetWorld());
        GameChatManager->PostNotificationToUI(TEXT("You entered more essence than you have!"), FLinearColor::Red);
        return 0; // Return 0 or an appropriate value to indicate that the investment did not proceed
    }



    // Calculate total invested essence
    int32 TotalInvestment = PlayerWisdomAmt + PlayerTemperanceAmt + PlayerJusticeAmt + PlayerLegendaryAmt;
    //UE_LOG(LogTemp, Log, TEXT("Total Investment: %d"), TotalInvestment);

    // Calculate equipment bonus
    int32 EquipmentBonus = GetEquipmentBonus();
    //UE_LOG(LogTemp, Log, TEXT("Equipment Bonus: %d"), EquipmentBonus);

    // Calculate success chance
    float SuccessChance = CalculateSuccessChance(PlayerWisdomAmt, PlayerTemperanceAmt, PlayerJusticeAmt, PlayerLegendaryAmt, EquipmentBonus);
    //UE_LOG(LogTemp, Log, TEXT("Success Chance: %f"), SuccessChance);

    OnBroadcastInvestNumbers.Broadcast(TotalInvestment, EquipmentBonus, SuccessChance);


    // Determine if investment is successful
    bool bIsSuccessful = (std::rand() % 100) < (SuccessChance * 100);

    if (bIsSuccessful)
    {
        // Successful investment - calculate and return Courage essence
        int32 Points = PlayerWisdomAmt * 1 + PlayerTemperanceAmt * 2 + PlayerJusticeAmt * 3 + PlayerLegendaryAmt * 10;
        Points += EquipmentBonus;
        int32 CourageEssence = CalculateCourageEssenceReturn(Points);

        // Format the message before calling PostNotificationToUI
        FString FormattedMessage = FString::Printf(TEXT("SUCCESS! Received %d Courage Essence!"), CourageEssence);


        UE_LOG(LogTemp, Log, TEXT("Investment Successful: Returned Courage Essence: %d"), CourageEssence);
        AGameChatManager* GameChatManager = AGameChatManager::GetInstance(GetWorld());
        // Now pass the formatted message to PostNotificationToUI
        GameChatManager->PostNotificationToUI(FormattedMessage, FLinearColor::Green);

        UpdatePlayerEssenceCounts(PlayerWisdomAmt, PlayerTemperanceAmt, PlayerJusticeAmt, PlayerLegendaryAmt, CourageEssence, true);

        return CourageEssence;
    }
    else
    {
        // Unsuccessful investment - player loses a portion of their essence
        UE_LOG(LogTemp, Log, TEXT("Investment Unsuccessful"));

        // Calculate the loss for each essence type
        FInvestmentLossResult LossResult = HandleInvestmentLoss(PlayerWisdomAmt, PlayerTemperanceAmt, PlayerJusticeAmt, PlayerLegendaryAmt);

        // Sum up the individual losses
        int32 TotalLoss = LossResult.WisdomLoss + LossResult.TemperanceLoss + LossResult.JusticeLoss + LossResult.LegendaryLoss;

        // Format the message to include the amount lost of each essence type
        FString LossMessage = FString::Printf(TEXT("FAILURE! Lost %d Wisdom, %d Temperance, %d Justice, and %d Legendary Essence."),
            LossResult.WisdomLoss,
            LossResult.TemperanceLoss,
            LossResult.JusticeLoss,
            LossResult.LegendaryLoss);

        // Post the formatted loss message to the UI
        AGameChatManager* GameChatManager = AGameChatManager::GetInstance(GetWorld());
        GameChatManager->PostNotificationToUI(LossMessage, FLinearColor::Red);

        // Update the player's essence counts after the loss
        UpdatePlayerEssenceCounts(PlayerWisdomAmt, PlayerTemperanceAmt, PlayerJusticeAmt, PlayerLegendaryAmt, 0, false);

        // Return the total loss as a negative value to indicate failure
        return -TotalLoss;
    }
}

void ANPC_Investor::UpdatePlayerEssenceCounts(int32 WisdomAmt, int32 TemperanceAmt, int32 JusticeAmt, int32 LegendaryAmt, int32 CourageEssence, bool bIsSuccessful)
{
    // Subtract invested essences
    PlayFabWisdom = FMath::Max(0, PlayFabWisdom - WisdomAmt);
    PlayFabTemperance = FMath::Max(0, PlayFabTemperance - TemperanceAmt);
    PlayFabJustice = FMath::Max(0, PlayFabJustice - JusticeAmt);
    PlayFabLegendary = FMath::Max(0, PlayFabLegendary - LegendaryAmt);

    if (bIsSuccessful)
    {
        // Add Courage essence won to the total
        PlayFabCourage += CourageEssence;
    }

    // Send the updated essence counts back to PlayFab
    SendUpdatedEssenceCountsToPlayFab();

    // Broadcast the delegate with updated essence counts
    APlayFabManager* PlayFabManager = APlayFabManager::GetInstance(GetWorld());
    PlayFabManager->OnEssenceUpdate.Broadcast(PlayFabWisdom, PlayFabTemperance, PlayFabJustice, PlayFabCourage, PlayFabLegendary);
}

float ANPC_Investor::CalculateSuccessChance(int32 PlayerWisdomAmt, int32 PlayerTemperanceAmt, int32 PlayerJusticeAmt, int32 PlayerLegendaryAmt, int32 EquipmentBonus)
{

    // Calculate the weighted and powered contribution of each essence type to the success chance
    float WisdomContribution = FMath::Pow(PlayerWisdomAmt, PowerFactor) * WisdomInvestmentMultiplier;
    float TemperanceContribution = FMath::Pow(PlayerTemperanceAmt, PowerFactor) * TemperanceInvestmentMultiplier;
    float JusticeContribution = FMath::Pow(PlayerJusticeAmt, PowerFactor) * JusticeInvestmentMultiplier;
    float LegendaryContribution = FMath::Pow(PlayerLegendaryAmt, PowerFactor) * LegendaryInvestmentMultiplier;

    UE_LOG(LogTemp, Log, TEXT("Wisdom Contribution: %f"), WisdomContribution);
    UE_LOG(LogTemp, Log, TEXT("Temperance Contribution: %f"), TemperanceContribution);
    UE_LOG(LogTemp, Log, TEXT("Justice Contribution: %f"), JusticeContribution);
    UE_LOG(LogTemp, Log, TEXT("Legendary Contribution: %f"), LegendaryContribution);

    // Sum the contributions of each essence type
    float TotalContribution = WisdomContribution + TemperanceContribution + JusticeContribution + LegendaryContribution;
    UE_LOG(LogTemp, Log, TEXT("Total Essence Contribution: %f"), TotalContribution / 10000.0f);

    // Calculate the final success chance using the total contribution and equipment bonus
    float SuccessChance = BaseSuccessChance + (TotalContribution / 10000.0f) + (EquipmentBonus / 50.0f);

    return FMath::Clamp(SuccessChance, BaseSuccessChance, 0.95f); // Clamp between base chance and 95%
}

// This method now returns a struct or a tuple containing the losses of each essence
FInvestmentLossResult ANPC_Investor::HandleInvestmentLoss(int32 PlayerWisdomAmt, int32 PlayerTemperanceAmt, int32 PlayerJusticeAmt, int32 PlayerLegendaryAmt)
{

    // Calculate the essence loss for each type
    int32 WisdomLoss = static_cast<int32>(PlayerWisdomAmt * LossPercentage);
    int32 TemperanceLoss = static_cast<int32>(PlayerTemperanceAmt * LossPercentage);
    int32 JusticeLoss = static_cast<int32>(PlayerJusticeAmt * LossPercentage);
    int32 LegendaryLoss = static_cast<int32>(PlayerLegendaryAmt * LossPercentage);

    // Create a struct or a tuple to return the losses
    FInvestmentLossResult LossResult;
    LossResult.WisdomLoss = WisdomLoss;
    LossResult.TemperanceLoss = TemperanceLoss;
    LossResult.JusticeLoss = JusticeLoss;
    LossResult.LegendaryLoss = LegendaryLoss;

    return LossResult;
}

int32 ANPC_Investor::GetEquipmentBonus()
{
    //int32 Bonus = std::rand() % 10 + 1; // Random number between 1 and 10
    ABonusManager* BonusManager = ABonusManager::GetInstance(GetWorld());
    UE_LOG(LogTemp, Log, TEXT("Generated Equipment Bonus: %i"), BonusManager->MultiplierSet.InvestingBonusChance);
    return BonusManager->MultiplierSet.InvestingBonusChance;
}

int32 ANPC_Investor::CalculateCourageEssenceReturn(int32 Points)
{
    int32 BaseReturn = 10; // Base amount of Courage essence to return
    int32 BonusReturn = Points / 10; // Additional Courage essence based on points

    int32 TotalReturn = BaseReturn + BonusReturn;
    UE_LOG(LogTemp, Log, TEXT("Base Return: %d, Bonus Return: %d, Total Courage Essence Return: %d"), BaseReturn, BonusReturn, TotalReturn);

    return TotalReturn;
}


void ANPC_Investor::GetCurrentEssenceCounts()
{
	PlayFab::ClientModels::FGetUserDataRequest GetRequest;
	clientAPI->GetUserData(GetRequest,
		PlayFab::UPlayFabClientAPI::FGetUserDataDelegate::CreateUObject(this, &ANPC_Investor::OnSuccessFetchEssenceCounts),
		PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &ANPC_Investor::OnErrorFetchEssenceCounts));
}

void ANPC_Investor::OnSuccessFetchEssenceCounts(const PlayFab::ClientModels::FGetUserDataResult& Result)
{
    FString EssenceDataJsonString;

    // Assuming "EssenceAddedToCoffer" is the key for the essence counts data
    if (Result.Data.Contains(TEXT("EssenceAddedToCoffer")))
    {
        EssenceDataJsonString = Result.Data[TEXT("EssenceAddedToCoffer")].Value;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Essence counts data not found."));
        return;
    }

    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(EssenceDataJsonString);

    if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
    {
        // Parse and assign the values to your int32 variables
        PlayFabWisdom = JsonObject->GetIntegerField(TEXT("Wisdom"));
        PlayFabJustice = JsonObject->GetIntegerField(TEXT("Justice"));
        PlayFabTemperance = JsonObject->GetIntegerField(TEXT("Temperance"));
        PlayFabCourage = JsonObject->GetIntegerField(TEXT("Courage"));
        PlayFabLegendary = JsonObject->GetIntegerField(TEXT("Legendary"));

        // Log or use these values as needed
        UE_LOG(LogTemp, Log, TEXT("Wisdom: %d, Justice: %d, Temperance: %d, Courage: %d, Legendary: %d"), PlayFabWisdom, PlayFabJustice, PlayFabTemperance, PlayFabCourage, PlayFabLegendary);

        // Update any relevant game data structures or variables here
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to parse essence counts JSON."));
    }
}

void ANPC_Investor::OnErrorFetchEssenceCounts(const PlayFab::FPlayFabCppError& Error)
{
    FString ErrorMessage = Error.GenerateErrorReport();
    UE_LOG(LogTemp, Error, TEXT("Failed to get essence counts in NPC_Investor: %s"), *ErrorMessage);
}

void ANPC_Investor::SendUpdatedEssenceCountsToPlayFab()
{
    TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
    JsonObject->SetNumberField(TEXT("Wisdom"), PlayFabWisdom);
    JsonObject->SetNumberField(TEXT("Temperance"), PlayFabTemperance);
    JsonObject->SetNumberField(TEXT("Justice"), PlayFabJustice);
    JsonObject->SetNumberField(TEXT("Courage"), PlayFabCourage);
    JsonObject->SetNumberField(TEXT("Legendary"), PlayFabLegendary);

    FString UpdatedDataString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&UpdatedDataString);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

    PlayFab::ClientModels::FUpdateUserDataRequest UpdateRequest;
    UpdateRequest.Data.Add(TEXT("EssenceAddedToCoffer"), UpdatedDataString);

    clientAPI->UpdateUserData(UpdateRequest,
        PlayFab::UPlayFabClientAPI::FUpdateUserDataDelegate::CreateUObject(this, &ANPC_Investor::OnSuccessUpdateEssenceCounts),
        PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &ANPC_Investor::OnErrorUpdateEssenceCounts));
}

void ANPC_Investor::OnSuccessUpdateEssenceCounts(const PlayFab::ClientModels::FUpdateUserDataResult& Result)
{
    // Log success or handle accordingly
    UE_LOG(LogTemp, Log, TEXT("Successfully updated essence counts on PlayFab."));
}

void ANPC_Investor::OnErrorUpdateEssenceCounts(const PlayFab::FPlayFabCppError& Error)
{
    FString ErrorMessage = Error.GenerateErrorReport();
    UE_LOG(LogTemp, Error, TEXT("Failed to update essence counts on PlayFab: %s"), *ErrorMessage);
}