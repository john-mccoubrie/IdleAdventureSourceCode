


#include "UI/IdleUserWidget.h"

void UIdleUserWidget::SetWidgetController(UObject* InWidgetController)
{
	WidgetController = InWidgetController;
	WidgetControllerSet();
}
