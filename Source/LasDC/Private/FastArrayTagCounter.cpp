// Fill out your copyright notice in the Description page of Project Settings.


#include "FastArrayTagCounter.h"
#include "GameplayTagContainer.h"

int32 FFastArrayTagCounter::GetTagCount(FGameplayTag InTag) const
{
	for (auto ItemIter = TagArray.CreateConstIterator(); ItemIter; ++ItemIter) {
		const FFastArrayTagCounterRecord& TagRecord = *ItemIter;

		if (TagRecord.Tag == InTag) {

			return TagRecord.Count;
		}
	}

	return 0;
}

void FFastArrayTagCounter::AddTagCount(FGameplayTag InTag, int32 Count)
{
    for (auto ItemIter = TagArray.CreateIterator(); ItemIter; ++ItemIter)
    {
        FFastArrayTagCounterRecord& Item = *ItemIter;
        if (Item.Tag == InTag)
        {
            Item.Count += Count;
            if (Item.Count <= 0)
            {
                ItemIter.RemoveCurrent();
                MarkArrayDirty();
            }
            else
            {
                MarkItemDirty(Item);
            }
            return;
        }
    }

    FFastArrayTagCounterRecord& Item = TagArray.AddDefaulted_GetRef();
    Item.Tag = InTag;
    Item.Count = Count;
    MarkItemDirty(Item);
}



const TArray<FFastArrayTagCounterRecord>& FFastArrayTagCounter::GetTagArray() const
{
	return TagArray;
}
