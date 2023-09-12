#pragma once


#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"


//Congi DEFAULTENGINE.INI löytyy oikeat editorista tulevat termit ja nihiin liittyvät channelien default responset
// 
// Trace against Actors/Components which provide interactions.
#define Crawl_TraceChannel_Interaction					ECC_GameTraceChannel6

// Trace used by weapons, will hit physics assets instead of capsules
#define Crawl_TraceChannel_Weapon						ECC_GameTraceChannel7

// Trace used by by weapons, will hit pawn capsules instead of physics assets
#define Crawl_TraceChannel_Weapon_Capsule				ECC_GameTraceChannel8

// Trace used by by weapons, will trace through multiple pawns rather than stopping on the first hit
#define Crawl_TraceChannel_Weapon_Multi					ECC_GameTraceChannel9

