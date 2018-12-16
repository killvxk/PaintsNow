// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "LeavesExporterPCH.h"
#include "LeavesWidget.h"

#define LOCTEXT_NAMESPACE "SLeavesWidget"

void SLeavesWidget::Construct(const FArguments& InArgs)
{
	UE_LOG(LogTemp, Log, TEXT("SLeavesWidget::Construct..."));

	tunnelIdentifier = SNew(SEditableText).Text(NSLOCTEXT("Tunnel", "LeavesWingScenePoster", "LeavesWingScenePoster"));

	auto MessagesTextBox = SNew(SMultiLineEditableTextBox)
		.Style(FEditorStyle::Get(), "Log.TextBox")
		.TextStyle(FEditorStyle::Get(), "Log.Normal")
		.ForegroundColor(FLinearColor::Gray)
		.IsReadOnly(true)
		.AlwaysShowScrollbars(true)
		// .OnVScrollBarUserScrolled(this, &SOutputLog::OnUserScrolled)
		// .ContextMenuExtender(this, &SOutputLog::ExtendTextBoxMenu)
		;

	ChildSlot
	[
		SNew(SBorder)
		.Padding(3)
		.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
		[
			SNew(SVerticalBox)

			// Output Log Filter
			+SVerticalBox::Slot()
			.AutoHeight()
			.Padding(FMargin(0.0f, 0.0f, 0.0f, 4.0f))
			[
				SNew(SHorizontalBox)
			
				+SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(2, 5, 0, 0)
				[
					tunnelIdentifier->AsShared()
				]
				+SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(2, 0, 0, 0)
				[
					SNew(SButton)
					.ContentPadding(FMargin(6.0, 2.0))
					.Text(LOCTEXT("Connect to server", "Connect"))
					.OnClicked(this, &SLeavesWidget::OnConnectClicked)
				]
				+SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SComboButton)
					.ComboButtonStyle(FEditorStyle::Get(), "GenericFilters.ComboButtonStyle")
					.ForegroundColor(FLinearColor::White)
					.ContentPadding(0)
					.ToolTipText(LOCTEXT("AddFilterToolTip", "Add an output log filter."))
					// .OnGetMenuContent(this, &SOutputLog::MakeAddFilterMenu)
					.HasDownArrow(true)
					.ContentPadding(FMargin(1, 0))
					.ButtonContent()
					[
						SNew(SHorizontalBox)

						+SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(STextBlock)
							.TextStyle(FEditorStyle::Get(), "GenericFilters.TextStyle")
							.Font(FEditorStyle::Get().GetFontStyle("FontAwesome.9"))
							.Text(FText::FromString(FString(TEXT("\xf0b0"))) /*fa-filter*/)
						]

						+SHorizontalBox::Slot()
						.AutoWidth()
						.Padding(2, 0, 0, 0)
						[
							SNew(STextBlock)
							.TextStyle(FEditorStyle::Get(), "GenericFilters.TextStyle")
							.Text(LOCTEXT("Filters", "Filters"))
						]
						
					]
				]

	/*
				+SHorizontalBox::Slot()
				.Padding(4, 1, 0, 0)
				[
					SAssignNew(FilterTextBox, SSearchBox)
					.HintText(LOCTEXT("SearchLogHint", "Search Log"))
					.OnTextChanged(this, &SOutputLog::OnFilterTextChanged)
					.OnTextCommitted(this, &SOutputLog::OnFilterTextCommitted)
					.DelayChangeNotificationsWhileTyping(true)
				]*/
			]

			// Output log area
			+SVerticalBox::Slot()
			.FillHeight(1)
			[
				MessagesTextBox
			]

			// The console input box
			/*
			+SVerticalBox::Slot()
			.AutoHeight()
			.Padding(FMargin(0.0f, 4.0f, 0.0f, 0.0f))
			[
				SNew(SBox)
				.MaxDesiredHeight(180.0f)
				[
					SNew(SConsoleInputBox)
					.OnConsoleCommandExecuted(this, &SOutputLog::OnConsoleCommandExecuted)

					// Always place suggestions above the input line for the output log widget
					.SuggestionListPlacement(MenuPlacement_AboveAnchor)
				]
			]*/
		]
	];
}


FReply SLeavesWidget::OnConnectClicked() {
	FString target = tunnelIdentifier->GetText().ToString();
	return FReply::Handled();
}


#undef LOCTEXT_NAMESPACE