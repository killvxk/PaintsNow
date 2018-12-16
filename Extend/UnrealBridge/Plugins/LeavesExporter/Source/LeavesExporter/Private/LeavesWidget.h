// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "LeavesExporterPCH.h"

#include "LeavesExporter.h"

class SLeavesWidget : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SLeavesWidget) {}

	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs);
	FReply OnConnectClicked();

private:
	TSharedPtr<SEditableText> tunnelIdentifier;
};