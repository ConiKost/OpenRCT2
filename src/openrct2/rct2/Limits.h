/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once
#include "../rct12/Limits.h"

namespace RCT2::Limits
{
    using namespace RCT12::Limits;
    constexpr uint8_t MaxStaff = 200;
    constexpr uint8_t MaxBanners = 250;
    constexpr uint8_t MaxTrainsPerRide = 32;
    constexpr uint8_t DowntimeHistorySize = 8;
    constexpr uint16_t MaxEntities = 10000;
    constexpr uint16_t MaxEntitiesRCTCExtended = 15000; // Used in files marked with “classic flag” 0xF
    constexpr uint32_t MaxTileElements = 0x30000;
    constexpr uint16_t MaxAnimatedObjects = 2000;
    constexpr uint8_t MaxResearchedRideTypeQuads = 8;  // With 32 bits per uint32_t, this means there is room for
                                                       // 256 types.
    constexpr uint8_t MaxResearchedRideEntryQuads = 8; // With 32 bits per uint32_t, this means there is room for
                                                       // 256 entries.
    constexpr uint8_t MaxResearchedSceneryItemQuads = 56;
    constexpr const uint16_t MaxResearchedSceneryItems = (MaxResearchedSceneryItemQuads * 32); // There are 32
                                                                                               // bits per
                                                                                               // quad.
    constexpr uint16_t MaxResearchItems = 500;

    constexpr uint16_t TD6MaxTrackElements = 8192;

    constexpr uint8_t MaxSmallSceneryObjects = 252;
    constexpr uint8_t MaxLargeSceneryObjects = 128;
    constexpr uint8_t MaxWallSceneryObjects = 128;
    constexpr uint8_t MaxBannerObjects = 32;
    constexpr uint8_t MaxPathObjects = 16;
    constexpr uint8_t MaxPathAdditionObjects = 15;
    constexpr uint8_t MaxSceneryGroupObjects = 19;
    constexpr uint8_t MaxParkEntranceObjects = 1;
    constexpr uint8_t MaxWaterObjects = 1;
    constexpr uint8_t MaxScenarioTextObjects = 1;
    constexpr uint8_t RideTypeCount = 91;
    constexpr uint16_t MaxMapSize = 256;
} // namespace RCT2::Limits
