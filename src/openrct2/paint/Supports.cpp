/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Supports.h"

#include "../interface/Viewport.h"
#include "../object/FootpathRailingsObject.h"
#include "../sprites.h"
#include "../util/Math.hpp"
#include "../world/Surface.h"
#include "Boundbox.h"
#include "Paint.SessionFlags.h"
#include "Paint.h"
#include "tile_element/Paint.TileElement.h"

/** rct2: 0x0097AF20, 0x0097AF21 */
// clang-format off
static constexpr CoordsXY SupportBoundBoxes[] = {
    {  4,  4 },
    { 28,  4 },
    {  4, 28 },
    { 28, 28 },
    { 16, 16 },
    { 16,  4 },
    {  4, 16 },
    { 28, 16 },
    { 16, 28 },
};

/** rct2: 0x0097AF32 */
static constexpr uint8_t _97AF32[] = {
    5, 2, 5, 2, 5, 2, 5, 2,
    7, 1, 7, 1, 7, 1, 7, 1,
    6, 3, 6, 3, 6, 3, 6, 3,
    8, 0, 8, 0, 8, 0, 8, 0,
    5, 3, 6, 0, 8, 1, 7, 2,
    1, 2, 1, 2, 1, 2, 1, 2,
    0, 3, 0, 3, 0, 3, 0, 3,
    3, 1, 3, 1, 3, 1, 3, 1,
    2, 0, 2, 0, 2, 0, 2, 0,

    6, 1, 6, 1, 6, 1, 6, 1,
    5, 0, 5, 0, 5, 0, 5, 0,
    8, 2, 8, 2, 8, 2, 8, 2,
    7, 3, 7, 3, 7, 3, 7, 3,
    6, 0, 8, 1, 7, 2, 5, 3,
    0, 0, 0, 0, 0, 0, 0, 0,
    2, 1, 2, 1, 2, 1, 2, 1,
    1, 3, 1, 3, 1, 3, 1, 3,
    3, 2, 3, 2, 3, 2, 3, 2,

    1, 6, 1, 6, 1, 6, 1, 6,
    3, 5, 3, 5, 3, 5, 3, 5,
    0, 7, 0, 7, 0, 7, 0, 7,
    2, 4, 2, 4, 2, 4, 2, 4,
    8, 1, 7, 2, 5, 3, 6, 0,
    4, 1, 4, 1, 4, 1, 4, 1,
    4, 2, 4, 2, 4, 2, 4, 2,
    4, 0, 4, 0, 4, 0, 4, 0,
    4, 3, 4, 3, 4, 3, 4, 3,

    2, 5, 2, 5, 2, 5, 2, 5,
    0, 4, 0, 4, 0, 4, 0, 4,
    3, 6, 3, 6, 3, 6, 3, 6,
    1, 7, 1, 7, 1, 7, 1, 7,
    7, 2, 5, 3, 6, 0, 8, 1,
    8, 5, 8, 5, 8, 5, 8, 5,
    7, 6, 7, 6, 7, 6, 7, 6,
    6, 4, 6, 4, 6, 4, 6, 4,
    5, 7, 5, 7, 5, 7, 5, 7,
};

/** rct2: 0x0097B052, 0x0097B053 */
static constexpr CoordsXY Loc97B052[] = {
    { -15,  -1 },
    {   0,  -2 },
    {  -2,  -1 },
    {  -1, -15 },
    { -26,  -1 },
    {   0,  -2 },
    {  -2,  -1 },
    {  -1, -26 },
};

/** rct2: 0x0097B062, 0x0097B063 */
static constexpr CoordsXY _97B062[] = {
    { 18,  3 },
    {  3, 18 },
    { 18,  3 },
    {  3, 18 },
    { 32,  3 },
    {  3, 32 },
    { 32,  3 },
    {  3, 32 }
};

/** rct2: 0x0097B072 */
static constexpr uint32_t _metalSupportTypeToCrossbeamImages[][8] = {
    { 3370, 3371, 3370, 3371, 3372, 3373, 3372, 3373 }, // MetalSupportType::Tubes
    { 3374, 3375, 3374, 3375, 3376, 3377, 3376, 3377 }, // MetalSupportType::Fork
    { 3374, 3375, 3374, 3375, 3376, 3377, 3376, 3377 }, // MetalSupportType::ForkAlt
    { 3370, 3371, 3370, 3371, 3372, 3373, 3372, 3373 }, // MetalSupportType::Boxed
    { 3374, 3375, 3374, 3375, 3376, 3377, 3376, 3377 }, // MetalSupportType::Stick
    { 3374, 3375, 3374, 3375, 3376, 3377, 3376, 3377 }, // MetalSupportType::StickAlt
    { 3378, 3383, 3378, 3383, 3380, 3385, 3380, 3385 }, // MetalSupportType::ThickCentred
    { 3378, 3383, 3378, 3383, 3380, 3385, 3380, 3385 }, // MetalSupportType::Thick
    { 3382, 3379, 3382, 3379, 3384, 3381, 3384, 3381 }, // MetalSupportType::ThickAlt
    { 3382, 3379, 3382, 3379, 3384, 3381, 3384, 3381 }, // MetalSupportType::ThickAltCentred
    { 3378, 3379, 3378, 3379, 3380, 3381, 3380, 3381 }, // MetalSupportType::Truss
    { 3386, 3387, 3386, 3387, 3388, 3389, 3388, 3389 }, // MetalSupportType::TubesInverted
    { 3370, 3371, 3370, 3371, 3372, 3373, 3372, 3373 }, // MetalSupportType::BoxedCoated
};

/** rct2: 0x0097B142 */
static constexpr uint8_t supportTypeToHeight[] = {
    6, // MetalSupportType::Tubes
    3, // MetalSupportType::Fork
    3, // MetalSupportType::ForkAlt
    6, // MetalSupportType::Boxed
    3, // MetalSupportType::Stick
    3, // MetalSupportType::StickAlt
    6, // MetalSupportType::ThickCentred
    6, // MetalSupportType::Thick
    6, // MetalSupportType::ThickAlt
    6, // MetalSupportType::ThickAltCentred
    4, // MetalSupportType::Truss
    3, // MetalSupportType::TubesInverted
    6, // MetalSupportType::BoxedCoated
};

struct MetalSupportsImages {
    uint16_t base_id;
    uint16_t beam_id;
};

/** rct2: 0x0097B15C */
static constexpr MetalSupportsImages _97B15C[] = {
    { 3243, 3209 }, // MetalSupportType::Tubes
    { 3279, 3262 }, // MetalSupportType::Fork
    { 3298, 3262 }, // MetalSupportType::ForkAlt
    { 3334, 3317 }, // MetalSupportType::Boxed
    {    0, 3658 }, // MetalSupportType::Stick
    {    0, 3658 }, // MetalSupportType::StickAlt
    {    0, 3141 }, // MetalSupportType::ThickCentred
    {    0, 3158 }, // MetalSupportType::Thick
    {    0, 3175 }, // MetalSupportType::ThickAlt
    {    0, 3192 }, // MetalSupportType::ThickAltCentred
    {    0, 3124 }, // MetalSupportType::Truss
    { 3243, 3209 }, // MetalSupportType::TubesInverted
    { 3334, 3353 }, // MetalSupportType::BoxedCoated
};

/** rct2: 0x0097B190 */
static constexpr MetalSupportsImages _97B190[] = {
    { 3243, 3226 }, // 3209
    { 3279, 3262 },
    { 3298, 3262 },
    { 3334, 3317 },
    {    0, 3658 },
    {    0, 3658 },
    {    0, 3141 },
    {    0, 3158 },
    {    0, 3175 },
    {    0, 3192 },
    {    0, 3124 },
    { 3243, 3226 }, // 3209
    { 3334, 3353 },
};

/** rct2: 0x0097B404 */
static constexpr uint8_t metal_supports_slope_image_map[] = {
     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,  0,
     0,  0,  0,  0,  0,  0,  0, 15,  0,  0,  0, 16,  0, 17, 18,  0,
};

struct SupportsIdDescriptor {
    uint16_t full;
    uint16_t half;
    uint16_t flat;
    uint16_t slope;
};

/* 0x0097B1C4 */
static constexpr SupportsIdDescriptor WoodenSupportImageIds[] = {
    { 3392, 3393, 3394, 3536 },
    { 3390, 3391, 3394, 3514 },
    { 3558, 3559, 3560, 3570 },
    { 3561, 3562, 3563, 3592 },
    { 3564, 3565, 3566, 3614 },
    { 3567, 3568, 3569, 3636 },
    { 3677, 3678, 3680, 3739 },
    { 3675, 3676, 3679, 3717 },
    { 3761, 3762, 3763, 3773 },
    { 3764, 3765, 3766, 3795 },
    { 3767, 3768, 3769, 3817 },
    { 3770, 3771, 3772, 3839 },
};

static constexpr uint16_t WoodenCurveSupportImageIds0[81] = {
    3465,
    3466,
    3467,
    3468,
    3469,
    3470,
    3471,
    3472,
    3473,
    3474,
    3475,
    3476,
    3477,
    3478,
    3479,
    3480,
    3481,
    3482,
    3483,
    3484,
    3485,
    3486,
    3487,
    3488,
    3489,
    3490,
    3491,
    3492,
    3493,
    3494,
    3495,
    3496,
    3497,
    3498,
    3499,
    3500,
    3501,
    3502,
    3503,
    3504,
    3505,
    3506,
    3507,
    3508,
    3509,
    3510,
    3511,
    3512,
    3513,
    SPR_G2_SUPPORT_BEGIN,
    SPR_G2_SUPPORT_BEGIN + 1,
    SPR_G2_SUPPORT_BEGIN + 2,
    SPR_G2_SUPPORT_BEGIN + 3,
    SPR_G2_SUPPORT_BEGIN + 4,
    SPR_G2_SUPPORT_BEGIN + 5,
    SPR_G2_SUPPORT_BEGIN + 6,
    SPR_G2_SUPPORT_BEGIN + 7,
    SPR_G2_SUPPORT_BEGIN + 8,
    SPR_G2_SUPPORT_BEGIN + 9,
    SPR_G2_SUPPORT_BEGIN + 10,
    SPR_G2_SUPPORT_BEGIN + 11,
    SPR_G2_SUPPORT_BEGIN + 12,
    SPR_G2_SUPPORT_BEGIN + 13,
    SPR_G2_SUPPORT_BEGIN + 14,
    SPR_G2_SUPPORT_BEGIN + 15,
    SPR_G2_SUPPORT_BEGIN + 16,
    SPR_G2_SUPPORT_BEGIN + 17,
    SPR_G2_SUPPORT_BEGIN + 18,
    SPR_G2_SUPPORT_BEGIN + 19,
    SPR_G2_SUPPORT_BEGIN + 20,
    SPR_G2_SUPPORT_BEGIN + 21,
    SPR_G2_SUPPORT_BEGIN + 22,
    SPR_G2_SUPPORT_BEGIN + 23,
    SPR_G2_SUPPORT_BEGIN + 24,
    SPR_G2_SUPPORT_BEGIN + 25,
    SPR_G2_SUPPORT_BEGIN + 26,
    SPR_G2_SUPPORT_BEGIN + 27,
    SPR_G2_SUPPORT_BEGIN + 28,
    SPR_G2_SUPPORT_BEGIN + 29,
    SPR_G2_SUPPORT_BEGIN + 30,
    SPR_G2_SUPPORT_BEGIN + 31,
};

static constexpr uint16_t WoodenCurveSupportImageIds1[81] = {
    3681, 3682, 3683, 3684, 3685, 3686, 3687, 3688, 3689, 3690, 3691, 3692, 3693, 3694, 3695, 3696, 3697,
    3698, 3699, 3700, 3701, 3702, 3703, 3704, 3705, 3706, 3707, 3708, 3709, 3710, 3711, 3712, 3713, 3714,
    3715, 3716, 3717, 3718, 3719, 3720, 3721, 3722, 3723, 3724, 3725, 3726, 3727, 3728, 3729,
};

/* 0x0097B224 */
static constexpr const uint16_t* WoodenCurveSupportImageIds[12] = {
    WoodenCurveSupportImageIds0,
    WoodenCurveSupportImageIds0,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    WoodenCurveSupportImageIds1,
    WoodenCurveSupportImageIds1,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
};


struct UnkSupportsDescriptor {
    BoundBoxXYZ bounding_box;
    uint8_t var_6;
    uint8_t var_7;
};

/* 0x0097B23C */
static constexpr UnkSupportsDescriptor Byte97B23C[] = {
    {{{0,  0,  0}, {1,  1,  8}},  0, 1}, // Flat to gentle
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1}, // Gentle to flat
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1}, // Gentle slope
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1}, // Gentle to steep
    {{{10, 0,  0}, {10, 32, 44}}, 1, 1},
    {{{0,  10, 0}, {32, 10, 44}}, 1, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1}, // Steep to gentle
    {{{10, 0,  0}, {10, 32, 44}}, 1, 1},
    {{{0,  10, 0}, {32, 10, 44}}, 1, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1}, // Steep slope
    {{{10, 0,  2}, {10, 32, 76}}, 1, 1},
    {{{0,  10, 2}, {32, 10, 76}}, 1, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  4}},  0, 1}, // Slope
    {{{0,  0,  0}, {1,  1,  4}},  0, 1},
    {{{0,  0,  0}, {1,  1,  4}},  0, 1},
    {{{0,  0,  0}, {1,  1,  4}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1}, // Flat to steep small
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1}, // Steep to flat small
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1}, // ?
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1}, // ?
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1}, // ?
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{2,  2,  1}, {28, 28, 2}},  0, 1}, // Large scenery (all directions)
    {{{0,  0,  0}, {1,  1,  8}},  0, 1}, // Flat to steep large 1
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1}, // Flat to steep large 2
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1}, // Flat to steep large 3
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1}, // Flat to steep large 4
    {{{10, 0,  2}, {10, 32, 52}}, 1, 1},
    {{{0,  10, 2}, {32, 10, 52}}, 1, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1}, // Steep to flat large 1
    {{{10, 0,  2}, {10, 32, 76}}, 1, 1},
    {{{0,  10, 2}, {32, 10, 76}}, 1, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1}, // Steep to flat large 2
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1}, // Steep to flat large 3
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1}, // Steep to flat large 4
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
};

/* 0x0098D8D4 */
static constexpr UnkSupportsDescriptor Byte98D8D4[] = {
    {{{0, 0, 0}, {1, 1, 4}}, 0, 1},
    {{{0, 0, 0}, {1, 1, 4}}, 0, 1},
    {{{0, 0, 0}, {1, 1, 4}}, 0, 1},
    {{{0, 0, 0}, {1, 1, 4}}, 0, 1},
};

/* 0x0097B3C4 */
static constexpr uint16_t word_97B3C4[] = {
    0,
    0,
    1,
    2,
    3,
    4,
    5,
    6,
    7,
    8,
    9,
    10,
    11,
    12,
    13,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    14,
    0,
    0,
    0,
    17,
    0,
    16,
    15,
    0,
};

// clang-format on

static WoodenSupportSubType rotatedWoodenSupportSubTypes[6][NumOrthogonalDirections] = {
    {
        WoodenSupportSubType::NeSw,
        WoodenSupportSubType::NwSe,
        WoodenSupportSubType::NeSw,
        WoodenSupportSubType::NwSe,
    },
    {
        WoodenSupportSubType::NwSe,
        WoodenSupportSubType::NeSw,
        WoodenSupportSubType::NwSe,
        WoodenSupportSubType::NeSw,
    },
    {
        WoodenSupportSubType::Corner0,
        WoodenSupportSubType::Corner1,
        WoodenSupportSubType::Corner2,
        WoodenSupportSubType::Corner3,
    },
    {
        WoodenSupportSubType::Corner1,
        WoodenSupportSubType::Corner2,
        WoodenSupportSubType::Corner3,
        WoodenSupportSubType::Corner0,
    },
    {
        WoodenSupportSubType::Corner2,
        WoodenSupportSubType::Corner3,
        WoodenSupportSubType::Corner0,
        WoodenSupportSubType::Corner1,
    },
    {
        WoodenSupportSubType::Corner3,
        WoodenSupportSubType::Corner0,
        WoodenSupportSubType::Corner1,
        WoodenSupportSubType::Corner2,
    },
};

/**
 * Draw repeated supports for left over space
 *
 * @param supportType
 * @param imageTemplate
 * @param heightSteps
 * @param session
 * @param baseHeight
 * @param hasSupports
 */
static void WoodenABPaintRepeatedSupports(
    int32_t supportType, const ImageId& imageTemplate, int16_t heightSteps, PaintSession& session, uint16_t& baseHeight,
    bool& hasSupports)
{
    while (heightSteps > 0)
    {
        const bool isHalf = baseHeight & 0x10 || heightSteps == 1 || baseHeight + WATER_HEIGHT_STEP == session.WaterHeight;
        if (isHalf)
        {
            // Half support
            auto imageId = imageTemplate.WithIndex(WoodenSupportImageIds[supportType].half);
            uint8_t boundBoxHeight = (heightSteps == 1) ? 7 : 12;
            PaintAddImageAsParent(session, imageId, { 0, 0, baseHeight }, { 32, 32, boundBoxHeight });
            baseHeight += 16;
            heightSteps -= 1;
        }
        else
        {
            // Full support
            auto imageId = imageTemplate.WithIndex(WoodenSupportImageIds[supportType].full);
            uint8_t boundBoxHeight = (heightSteps == 2) ? 23 : 28;
            PaintAddImageAsParent(session, imageId, { 0, 0, baseHeight }, { 32, 32, boundBoxHeight });
            baseHeight += 32;
            heightSteps -= 2;
        }

        hasSupports = true;
    }
}

/**
 * Draw special pieces, e.g. curved supports.
 */
static bool WoodenABSupportPaintSetupPaintSpecial(
    PaintSession& session, int32_t supportType, int32_t special, const ImageId& imageTemplate, uint16_t baseHeight,
    bool hasSupports)
{
    uint16_t specialIndex = (special - 1) & 0xFFFF;

    const UnkSupportsDescriptor& supportsDesc = Byte97B23C[specialIndex];

    // TODO: Byte97B23C[specialIndex].var_7 is never 0
    if (WoodenCurveSupportImageIds[supportType] != nullptr && WoodenCurveSupportImageIds[supportType][specialIndex] != 0
        && supportsDesc.var_7 != 0)
    {
        hasSupports = true;
        auto imageId = imageTemplate.WithIndex(WoodenCurveSupportImageIds[supportType][specialIndex]);

        auto boundBox = supportsDesc.bounding_box;
        boundBox.offset.z += baseHeight;

        if (supportsDesc.var_6 == 0 || session.WoodenSupportsPrependTo == nullptr)
        {
            PaintAddImageAsParent(session, imageId, { 0, 0, baseHeight }, boundBox);
        }
        else
        {
            auto* paintStruct = PaintAddImageAsOrphan(session, imageId, { 0, 0, baseHeight }, boundBox);
            if (paintStruct != nullptr)
            {
                session.WoodenSupportsPrependTo->Children = paintStruct;
            }
        }
    }

    return hasSupports;
}

/**
 * Adds paint structs for wooden supports.
 *  rct2: 0x006629BC
 * @param supportType (edi) Type and direction of supports.
 * @param special (ax) Used for curved supports.
 * @param height (dx) The height of the supports.
 * @param imageTemplate (ebp) The colour and palette flags for the support sprites.
 * @param[out] underground (Carry flag) true if underground.
 * @returns (al) true if any supports have been drawn, otherwise false.
 */
bool WoodenASupportsPaintSetup(
    PaintSession& session, int32_t supportType, int32_t special, int32_t height, ImageId imageTemplate)
{
    if (!(session.Flags & PaintSessionFlags::PassedSurface))
    {
        return false;
    }

    if (session.ViewFlags & VIEWPORT_FLAG_HIDE_SUPPORTS)
    {
        if (session.ViewFlags & VIEWPORT_FLAG_INVISIBLE_SUPPORTS)
        {
            return false;
        }
        imageTemplate = ImageId().WithTransparency(FilterPaletteID::PaletteDarken1);
    }

    uint16_t baseHeight = Floor2(session.Support.height + 15, 16);
    int16_t supportLength = height - baseHeight;

    if (supportLength < 0)
    {
        return false;
    }

    int16_t heightSteps = supportLength / 16;

    bool hasSupports = false;
    bool drawFlatPiece = false;

    // Draw base support (usually shaped to the slope)
    auto slope = session.Support.slope;
    if (slope & SUPPORTS_SLOPE_5)
    {
        // Above scenery (just put a base piece above it)
        drawFlatPiece = true;
    }
    else if (slope & TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT)
    {
        // Steep diagonal (place the correct shaped support for the slope)
        heightSteps -= 2;
        if (heightSteps < 0)
        {
            return false;
        }

        auto imageIndex = WoodenSupportImageIds[supportType].slope;
        if (imageIndex == 0)
        {
            drawFlatPiece = true;
        }
        else
        {
            auto imageId = imageTemplate.WithIndex(imageIndex + word_97B3C4[slope & TILE_ELEMENT_SURFACE_SLOPE_MASK]);

            PaintAddImageAsParent(session, imageId, { 0, 0, baseHeight }, { { 0, 0, baseHeight + 2 }, { 32, 32, 11 } });
            PaintAddImageAsParent(
                session, imageId.WithIndexOffset(4), { 0, 0, baseHeight + 16 },
                { { 0, 0, baseHeight + 16 + 2 }, { 32, 32, 11 } });

            hasSupports = true;
        }

        baseHeight += 32;
    }
    else if ((slope & TILE_ELEMENT_SLOPE_ALL_CORNERS_UP) != 0)
    {
        // 1 to 3 quarters up
        heightSteps--;
        if (heightSteps < 0)
        {
            return false;
        }

        auto imageIndex = WoodenSupportImageIds[supportType].slope;
        if (imageIndex == 0)
        {
            drawFlatPiece = true;
        }
        else
        {
            auto imageId = imageTemplate.WithIndex(imageIndex + word_97B3C4[slope & TILE_ELEMENT_SURFACE_SLOPE_MASK]);
            PaintAddImageAsParent(session, imageId, { 0, 0, baseHeight }, { { 0, 0, baseHeight + 2 }, { 32, 32, 11 } });
            hasSupports = true;
        }

        baseHeight += 16;
    }

    // Draw flat base support
    if (drawFlatPiece)
    {
        auto imageId = imageTemplate.WithIndex(WoodenSupportImageIds[supportType].flat);
        PaintAddImageAsParent(session, imageId, { 0, 0, baseHeight - 2 }, { 32, 32, 0 });
        hasSupports = true;
    }

    WoodenABPaintRepeatedSupports(supportType, imageTemplate, heightSteps, session, baseHeight, hasSupports);

    if (special != 0)
    {
        hasSupports = WoodenABSupportPaintSetupPaintSpecial(session, supportType, special, imageTemplate, baseHeight, false);
    }

    return hasSupports;
}

static int32_t GetSpecialOffsetForTransitionType(WoodenSupportTransitionType transitionType, Direction direction)
{
    if (transitionType == WoodenSupportTransitionType::None)
        return 0;

    // "Special" values are an offset into tables like Byte97B23C, plus 1.
    // Save for WoodenSupportTransitionType::LargeScenery, there are four entries (one per direction) for every
    // transition type. While these tables will have to be refactored in due course, we can only do so once all
    // drawing functions use WoodenSupportTransitionType instead of passing the "special" value directly.
    int32_t specialOffset = 0;
    if (transitionType < WoodenSupportTransitionType::Scenery)
        specialOffset = (EnumValue(transitionType) * NumOrthogonalDirections) + direction;
    else if (transitionType == WoodenSupportTransitionType::Scenery)
        specialOffset = (EnumValue(transitionType) * NumOrthogonalDirections);
    else
        specialOffset = (EnumValue(transitionType) * NumOrthogonalDirections) + direction - 3;

    return specialOffset + 1;
}

bool WoodenASupportsPaintSetup(
    PaintSession& session, WoodenSupportType supportType, WoodenSupportSubType subType, int32_t height, ImageId imageTemplate,
    WoodenSupportTransitionType transitionType, Direction direction)
{
    int32_t oldSupportType = (EnumValue(supportType) * 6) + EnumValue(subType);
    int32_t special = GetSpecialOffsetForTransitionType(transitionType, direction);

    return WoodenASupportsPaintSetup(session, oldSupportType, special, height, imageTemplate);
}

bool WoodenASupportsPaintSetupRotated(
    PaintSession& session, WoodenSupportType supportType, WoodenSupportSubType subType, Direction direction, int32_t height,
    ImageId imageTemplate, WoodenSupportTransitionType transitionType)
{
    subType = rotatedWoodenSupportSubTypes[EnumValue(subType)][direction];
    return WoodenASupportsPaintSetup(session, supportType, subType, height, imageTemplate, transitionType, direction);
}

/**
 * Wooden supports
 *  rct2: 0x00662D5C
 *
 * @param supportType (edi)
 * @param special (ax)
 * @param height (dx)
 * @param imageTemplate (ebp)
 * @param[out] underground (Carry Flag)
 *
 * @return (al) whether supports have been drawn
 */
static bool WoodenBSupportsPaintSetup(
    PaintSession& session, int32_t supportType, int32_t special, int32_t height, ImageId imageTemplate)
{
    if (!(session.Flags & PaintSessionFlags::PassedSurface))
    {
        return false;
    }

    if (session.ViewFlags & VIEWPORT_FLAG_HIDE_SUPPORTS)
    {
        if (session.ViewFlags & VIEWPORT_FLAG_INVISIBLE_SUPPORTS)
        {
            return false;
        }
        imageTemplate = ImageId().WithTransparency(FilterPaletteID::PaletteDarken1);
    }

    uint16_t baseHeight = Floor2(session.Support.height + 15, 16);
    int16_t supportLength = height - baseHeight;

    if (supportLength < 0)
    {
        return false;
    }

    int16_t heightSteps = supportLength / 16;

    bool hasSupports = false;
    bool drawFlatPiece = false;

    // Draw base support (usually shaped to the slope)
    auto slope = session.Support.slope;
    if (slope & SUPPORTS_SLOPE_5)
    {
        // Above scenery (just put a base piece above it)
        drawFlatPiece = true;
    }
    else if (slope & TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT)
    {
        // Steep diagonal (place the correct shaped support for the slope)
        heightSteps -= 2;
        if (heightSteps < 0)
        {
            return false;
        }

        auto imageIndex = WoodenSupportImageIds[supportType].slope;
        if (imageIndex == 0)
        {
            drawFlatPiece = true;
        }
        else
        {
            auto imageId = imageTemplate.WithIndex(imageIndex + word_97B3C4[slope & TILE_ELEMENT_SURFACE_SLOPE_MASK]);

            PaintAddImageAsParent(session, imageId, { 0, 0, baseHeight }, { { 0, 0, baseHeight + 2 }, { 32, 32, 11 } });
            PaintAddImageAsParent(
                session, imageId.WithIndexOffset(4), { 0, 0, baseHeight + 16 },
                { { 0, 0, baseHeight + 16 + 2 }, { 32, 32, 3 } });

            hasSupports = true;
        }

        baseHeight += 32;
    }
    else if ((slope & TILE_ELEMENT_SLOPE_ALL_CORNERS_UP) != 0)
    {
        // 1 to 3 quarters up
        heightSteps--;
        if (heightSteps < 0)
        {
            return false;
        }

        auto imageIndex = WoodenSupportImageIds[supportType].slope;
        if (imageIndex == 0)
        {
            drawFlatPiece = true;
        }
        else
        {
            auto imageId = imageTemplate.WithIndex(imageIndex + word_97B3C4[slope & TILE_ELEMENT_SURFACE_SLOPE_MASK]);
            PaintAddImageAsParent(session, imageId, { 0, 0, baseHeight }, { { 0, 0, baseHeight + 2 }, { 32, 32, 3 } });
            hasSupports = true;
        }

        baseHeight += 16;
    }

    // Draw flat base support
    if (drawFlatPiece)
    {
        if (heightSteps > 0)
        {
            auto imageId = imageTemplate.WithIndex(WoodenSupportImageIds[supportType].flat);
            PaintAddImageAsParent(session, imageId, { 0, 0, baseHeight - 2 }, { 32, 32, 0 });
            hasSupports = true;
        }
    }

    WoodenABPaintRepeatedSupports(supportType, imageTemplate, heightSteps, session, baseHeight, hasSupports);

    if (special != 0)
    {
        hasSupports = WoodenABSupportPaintSetupPaintSpecial(session, supportType, special, imageTemplate, baseHeight, false);
    }

    return hasSupports;
}

bool WoodenBSupportsPaintSetup(
    PaintSession& session, WoodenSupportType supportType, WoodenSupportSubType subType, int32_t height, ImageId imageTemplate,
    WoodenSupportTransitionType transitionType, Direction direction)
{
    int32_t oldSupportType = (EnumValue(supportType) * 6) + EnumValue(subType);
    int32_t special = GetSpecialOffsetForTransitionType(transitionType, direction);

    return WoodenBSupportsPaintSetup(session, oldSupportType, special, height, imageTemplate);
}

bool WoodenBSupportsPaintSetupRotated(
    PaintSession& session, WoodenSupportType supportType, WoodenSupportSubType subType, Direction direction, int32_t height,
    ImageId imageTemplate, WoodenSupportTransitionType transitionType)
{
    subType = rotatedWoodenSupportSubTypes[EnumValue(subType)][direction];
    return WoodenBSupportsPaintSetup(session, supportType, subType, height, imageTemplate, transitionType, direction);
}

/**
 * Metal pole supports
 * @param supportType (edi)
 * @param segment (ebx)
 * @param special (ax)
 * @param height (edx)
 * @param imageTemplate (ebp)
 *  rct2: 0x00663105
 */
bool MetalASupportsPaintSetup(
    PaintSession& session, MetalSupportType supportTypeMember, MetalSupportPlace placement, int32_t special, int32_t height,
    ImageId imageTemplate)
{
    uint8_t segment = EnumValue(placement);
    auto supportType = EnumValue(supportTypeMember);
    SupportHeight* supportSegments = session.SupportSegments;

    if (!(session.Flags & PaintSessionFlags::PassedSurface))
    {
        return false;
    }

    if (session.ViewFlags & VIEWPORT_FLAG_HIDE_SUPPORTS)
    {
        if (session.ViewFlags & VIEWPORT_FLAG_INVISIBLE_SUPPORTS)
        {
            return false;
        }
        imageTemplate = ImageId(0).WithTransparency(FilterPaletteID::PaletteDarken1);
    }

    int16_t originalHeight = height;
    int32_t originalSegment = segment;

    const uint8_t rotation = session.CurrentRotation;
    int16_t unk9E3294 = -1;
    if (height < supportSegments[segment].height)
    {
        unk9E3294 = height;

        height -= supportTypeToHeight[supportType];
        if (height < 0)
            return false;

        const uint8_t* esi = &(_97AF32[rotation * 2]);

        uint8_t newSegment = esi[segment * 8];
        if (height <= supportSegments[newSegment].height)
        {
            esi += 72;
            newSegment = esi[segment * 8];
            if (height <= supportSegments[newSegment].height)
            {
                esi += 72;
                newSegment = esi[segment * 8];
                if (height <= supportSegments[newSegment].height)
                {
                    esi += 72;
                    newSegment = esi[segment * 8];
                    if (height <= supportSegments[newSegment].height)
                    {
                        return false;
                    }
                }
            }
        }

        uint8_t ebp = esi[segment * 8 + 1];

        auto offset = CoordsXYZ{ SupportBoundBoxes[segment] + Loc97B052[ebp], height };
        auto boundBoxLength = CoordsXYZ(_97B062[ebp], 1);

        auto image_id = imageTemplate.WithIndex(_metalSupportTypeToCrossbeamImages[supportType][ebp]);
        PaintAddImageAsParent(session, image_id, offset, boundBoxLength);

        segment = newSegment;
    }
    int16_t si = height;
    if (supportSegments[segment].slope & SUPPORTS_SLOPE_5 || height - supportSegments[segment].height < 6
        || _97B15C[supportType].base_id == 0)
    {
        height = supportSegments[segment].height;
    }
    else
    {
        int8_t xOffset = SupportBoundBoxes[segment].x;
        int8_t yOffset = SupportBoundBoxes[segment].y;

        uint32_t imageIndex = _97B15C[supportType].base_id;
        imageIndex += metal_supports_slope_image_map[supportSegments[segment].slope & TILE_ELEMENT_SURFACE_SLOPE_MASK];
        auto image_id = imageTemplate.WithIndex(imageIndex);

        PaintAddImageAsParent(session, image_id, { xOffset, yOffset, supportSegments[segment].height }, { 0, 0, 5 });

        height = supportSegments[segment].height + 6;
    }

    // Work out if a small support segment required to bring support to normal
    // size (aka floor2(x, 16))
    int16_t heightDiff = Floor2(height + 16, 16);
    if (heightDiff > si)
    {
        heightDiff = si;
    }

    heightDiff -= height;

    if (heightDiff > 0)
    {
        int8_t xOffset = SupportBoundBoxes[segment].x;
        int8_t yOffset = SupportBoundBoxes[segment].y;

        uint32_t imageIndex = _97B15C[supportType].beam_id;
        imageIndex += heightDiff - 1;
        auto image_id = imageTemplate.WithIndex(imageIndex);

        PaintAddImageAsParent(session, image_id, { xOffset, yOffset, height }, { 0, 0, heightDiff - 1 });
    }

    height += heightDiff;
    // 6632e6

    for (uint8_t count = 0;; count++)
    {
        if (count >= 4)
            count = 0;

        int16_t z = height + 16;
        if (z > si)
        {
            z = si;
        }

        z -= height;
        if (z <= 0)
            break;

        int8_t xOffset = SupportBoundBoxes[segment].x;
        int8_t yOffset = SupportBoundBoxes[segment].y;

        uint32_t imageIndex = _97B15C[supportType].beam_id;
        imageIndex += z - 1;

        if (count == 3 && z == 0x10)
            imageIndex++;

        auto image_id = imageTemplate.WithIndex(imageIndex);
        PaintAddImageAsParent(session, image_id, { xOffset, yOffset, height }, { 0, 0, z - 1 });

        height += z;
    }

    supportSegments[segment].height = unk9E3294;
    supportSegments[segment].slope = 0x20;

    height = originalHeight;
    segment = originalSegment;
    if (special == 0)
        return true;

    if (special < 0)
    {
        special = -special;
        height--;
    }

    CoordsXYZ boundBoxOffset = CoordsXYZ(SupportBoundBoxes[segment], height);
    si = height + special;

    while (1)
    {
        int16_t z = height + 16;
        if (z > si)
        {
            z = si;
        }

        z -= height;
        if (z <= 0)
            break;

        int8_t xOffset = SupportBoundBoxes[segment].x;
        int8_t yOffset = SupportBoundBoxes[segment].y;

        uint32_t imageIndex = _97B190[supportType].beam_id;
        imageIndex += z - 1;
        auto image_id = imageTemplate.WithIndex(imageIndex);

        PaintAddImageAsParent(session, image_id, { xOffset, yOffset, height }, { boundBoxOffset, { 0, 0, 0 } });

        height += z;
    }

    return true;
}

/**
 * Metal pole supports
 *  rct2: 0x00663584
 *
 * @param supportType (edi)
 * @param segment (ebx)
 * @param special (ax)
 * @param height (edx)
 * @param imageTemplate (ebp)
 *
 * @return (Carry Flag)
 */
bool MetalBSupportsPaintSetup(
    PaintSession& session, MetalSupportType supportTypeMember, MetalSupportPlace placement, int32_t special, int32_t height,
    ImageId imageTemplate)
{
    uint8_t segment = EnumValue(placement);
    auto supportType = EnumValue(supportTypeMember);
    SupportHeight* supportSegments = session.SupportSegments;
    uint8_t originalSegment = segment;

    if (!(session.Flags & PaintSessionFlags::PassedSurface))
    {
        return false; // AND
    }

    if (session.ViewFlags & VIEWPORT_FLAG_HIDE_SUPPORTS)
    {
        if (session.ViewFlags & VIEWPORT_FLAG_INVISIBLE_SUPPORTS)
        {
            return false;
        }
        imageTemplate = ImageId(0).WithTransparency(FilterPaletteID::PaletteDarken1);
    }

    uint16_t _9E3294 = 0xFFFF;
    int32_t baseHeight = height;

    if (height < supportSegments[segment].height)
    {
        _9E3294 = height;

        baseHeight -= supportTypeToHeight[supportType];
        if (baseHeight < 0)
        {
            return false; // AND
        }

        uint16_t baseIndex = session.CurrentRotation * 2;

        uint8_t ebp = _97AF32[baseIndex + segment * 8];
        if (baseHeight <= supportSegments[ebp].height)
        {
            baseIndex += 9 * 4 * 2; // 9 segments, 4 directions, 2 values
            uint8_t ebp2 = _97AF32[baseIndex + segment * 8];
            if (baseHeight <= supportSegments[ebp2].height)
            {
                baseIndex += 9 * 4 * 2;
                uint8_t ebp3 = _97AF32[baseIndex + segment * 8];
                if (baseHeight <= supportSegments[ebp3].height)
                {
                    baseIndex += 9 * 4 * 2;
                    uint8_t ebp4 = _97AF32[baseIndex + segment * 8];
                    if (baseHeight <= supportSegments[ebp4].height)
                    {
                        return true; // STC
                    }
                }
            }
        }

        ebp = _97AF32[baseIndex + segment * 8 + 1];
        if (ebp >= 4)
        {
            return true; // STC
        }

        PaintAddImageAsParent(
            session, imageTemplate.WithIndex(_metalSupportTypeToCrossbeamImages[supportType][ebp]),
            { SupportBoundBoxes[originalSegment] + Loc97B052[ebp], baseHeight }, { _97B062[ebp], 1 });
    }

    int32_t si = baseHeight;

    if ((supportSegments[segment].slope & 0x20) || (baseHeight - supportSegments[segment].height < 6)
        || (_97B15C[supportType].base_id == 0))
    {
        baseHeight = supportSegments[segment].height;
    }
    else
    {
        uint32_t imageOffset = metal_supports_slope_image_map[supportSegments[segment].slope & TILE_ELEMENT_SURFACE_SLOPE_MASK];
        uint32_t imageId = _97B15C[supportType].base_id + imageOffset;

        PaintAddImageAsParent(
            session, imageTemplate.WithIndex(imageId), { SupportBoundBoxes[segment], supportSegments[segment].height },
            { 0, 0, 5 });

        baseHeight = supportSegments[segment].height + 6;
    }

    int16_t heightDiff = Floor2(baseHeight + 16, 16);
    if (heightDiff > si)
    {
        heightDiff = si;
    }

    heightDiff -= baseHeight;
    if (heightDiff > 0)
    {
        PaintAddImageAsParent(
            session, imageTemplate.WithIndex(_97B15C[supportType].beam_id + (heightDiff - 1)),
            { SupportBoundBoxes[segment], baseHeight }, { 0, 0, heightDiff - 1 });
    }

    baseHeight += heightDiff;

    int16_t endHeight;

    int32_t i = 1;
    while (true)
    {
        endHeight = baseHeight + 16;
        if (endHeight > si)
        {
            endHeight = si;
        }

        int16_t beamLength = endHeight - baseHeight;

        if (beamLength <= 0)
        {
            break;
        }

        uint32_t imageId = _97B15C[supportType].beam_id + (beamLength - 1);

        if (i % 4 == 0)
        {
            // Each fourth run, draw a special image
            if (beamLength == 16)
            {
                imageId += 1;
            }
        }

        PaintAddImageAsParent(
            session, imageTemplate.WithIndex(imageId), { SupportBoundBoxes[segment], baseHeight }, { 0, 0, beamLength - 1 });

        baseHeight += beamLength;
        i++;
    }

    supportSegments[segment].height = _9E3294;
    supportSegments[segment].slope = 0x20;

    if (special != 0)
    {
        baseHeight = height;
        si = height + special;
        while (true)
        {
            endHeight = baseHeight + 16;
            if (endHeight > si)
            {
                endHeight = si;
            }

            int16_t beamLength = endHeight - baseHeight;
            if (beamLength <= 0)
            {
                break;
            }

            uint32_t imageId = _97B15C[supportType].beam_id + (beamLength - 1);
            PaintAddImageAsParent(
                session, imageTemplate.WithIndex(imageId), { SupportBoundBoxes[originalSegment], baseHeight },
                { { SupportBoundBoxes[originalSegment], height }, { 0, 0, 0 } });
            baseHeight += beamLength;
        }
    }

    return false; // AND
}

/**
 *  rct2: 0x006A2ECC
 *
 * @param supportType (edi)
 * @param special (ax)
 * @param height (dx)
 * @param imageTemplate (ebp)
 * @param railingsDescriptor (0x00F3EF6C)
 * @param[out] underground (Carry Flag)
 *
 * @return Whether supports were drawn
 */
bool PathASupportsPaintSetup(
    PaintSession& session, int32_t supportType, int32_t special, int32_t height, ImageId imageTemplate,
    const FootpathPaintInfo& pathPaintInfo, bool* underground)
{
    if (underground != nullptr)
    {
        *underground = false; // AND
    }

    if (!(session.Flags & PaintSessionFlags::PassedSurface))
    {
        return false;
    }

    if (session.ViewFlags & VIEWPORT_FLAG_HIDE_SUPPORTS)
    {
        if (session.ViewFlags & VIEWPORT_FLAG_INVISIBLE_SUPPORTS)
        {
            return false;
        }
        imageTemplate = ImageId().WithTransparency(FilterPaletteID::PaletteDarken1);
    }

    uint16_t baseHeight = Ceil2(session.Support.height, 16);
    int32_t supportLength = height - baseHeight;
    if (supportLength < 0)
    {
        if (underground != nullptr)
            *underground = true; // STC
        return false;
    }

    bool hasSupports = false;

    int16_t heightSteps = supportLength / 16;

    if (session.Support.slope & 0x20)
    {
        // save dx2
        PaintAddImageAsParent(
            session, imageTemplate.WithIndex(pathPaintInfo.BridgeImageId + 48), { 0, 0, baseHeight - 2 }, { 32, 32, 0 });
        hasSupports = true;
    }
    else if (session.Support.slope & 0x10)
    {
        heightSteps -= 2;
        if (heightSteps < 0)
        {
            if (underground != nullptr)
                *underground = true; // STC
            return false;
        }

        uint32_t imageId = (supportType * 24) + word_97B3C4[session.Support.slope & TILE_ELEMENT_SURFACE_SLOPE_MASK]
            + pathPaintInfo.BridgeImageId;

        PaintAddImageAsParent(
            session, imageTemplate.WithIndex(imageId), { 0, 0, baseHeight }, { { 0, 0, baseHeight + 2 }, { 32, 32, 11 } });
        baseHeight += 16;

        PaintAddImageAsParent(
            session, imageTemplate.WithIndex(imageId + 4), { 0, 0, baseHeight }, { { 0, 0, baseHeight + 2 }, { 32, 32, 11 } });
        baseHeight += 16;

        hasSupports = true;
    }
    else if (session.Support.slope & 0x0F)
    {
        heightSteps -= 1;
        if (heightSteps < 0)
        {
            if (underground != nullptr)
                *underground = true; // STC
            return false;
        }

        uint32_t ebx = (supportType * 24) + word_97B3C4[session.Support.slope & TILE_ELEMENT_SURFACE_SLOPE_MASK]
            + pathPaintInfo.BridgeImageId;

        PaintAddImageAsParent(
            session, imageTemplate.WithIndex(ebx), { 0, 0, baseHeight }, { { 0, 0, baseHeight + 2 }, { 32, 32, 11 } });

        hasSupports = true;
        baseHeight += 16;
    }

    while (heightSteps > 0)
    {
        if (baseHeight & 0x10 || heightSteps == 1 || baseHeight + WATER_HEIGHT_STEP == session.WaterHeight)
        {
            uint32_t imageId = (supportType * 24) + pathPaintInfo.BridgeImageId + 23;

            PaintAddImageAsParent(
                session, imageTemplate.WithIndex(imageId), { 0, 0, baseHeight }, { 32, 32, ((heightSteps == 1) ? 7 : 12) });
            heightSteps -= 1;
            baseHeight += 16;
            hasSupports = true;
        }
        else
        {
            uint32_t imageId = (supportType * 24) + pathPaintInfo.BridgeImageId + 22;

            PaintAddImageAsParent(
                session, imageTemplate.WithIndex(imageId), { 0, 0, baseHeight }, { 32, 32, ((heightSteps == 2) ? 23 : 28) });
            heightSteps -= 2;
            baseHeight += 32;
            hasSupports = true;
        }
    }

    if (special != 0)
    {
        uint16_t specialIndex = (special - 1) & 0xFFFF;

        ImageIndex imageIndex = pathPaintInfo.BridgeImageId + 55 + specialIndex;

        const UnkSupportsDescriptor& supportsDesc = Byte98D8D4[specialIndex];
        auto boundBox = supportsDesc.bounding_box;
        boundBox.offset.z += baseHeight;

        if (supportsDesc.var_6 == 0 || session.WoodenSupportsPrependTo == nullptr)
        {
            PaintAddImageAsParent(session, imageTemplate.WithIndex(imageIndex), { 0, 0, baseHeight }, boundBox);
            hasSupports = true;
        }
        else
        {
            PaintStruct* paintStruct = PaintAddImageAsOrphan(
                session, imageTemplate.WithIndex(imageIndex), { 0, 0, baseHeight }, boundBox);
            hasSupports = true;
            if (paintStruct != nullptr)
            {
                session.WoodenSupportsPrependTo->Children = paintStruct;
            }
        }
    }

    if (underground != nullptr)
        *underground = false; // AND

    return hasSupports;
}

/**
 *
 *  rct2: 0x006A326B
 *
 * @param segment (ebx)
 * @param special (ax)
 * @param height (dx)
 * @param imageTemplate (ebp)
 * @param railingsDescriptor (0x00F3EF6C)
 *
 * @return Whether supports were drawn
 */
bool PathBSupportsPaintSetup(
    PaintSession& session, int32_t segment, int32_t special, int32_t height, ImageId imageTemplate,
    const FootpathPaintInfo& pathPaintInfo)
{
    SupportHeight* supportSegments = session.SupportSegments;

    if (!(session.Flags & PaintSessionFlags::PassedSurface))
    {
        return false;
    }

    if (session.ViewFlags & VIEWPORT_FLAG_HIDE_SUPPORTS)
    {
        if (session.ViewFlags & VIEWPORT_FLAG_INVISIBLE_SUPPORTS)
        {
            return false;
        }
        imageTemplate = ImageId().WithTransparency(FilterPaletteID::PaletteDarken1);
    }

    if (height < supportSegments[segment].height)
    {
        return true; // STC
    }

    uint16_t baseHeight;

    if ((supportSegments[segment].slope & 0x20) || (height - supportSegments[segment].height < 6)
        || !(pathPaintInfo.RailingFlags & RAILING_ENTRY_FLAG_HAS_SUPPORT_BASE_SPRITE))
    {
        baseHeight = supportSegments[segment].height;
    }
    else
    {
        uint8_t imageOffset = metal_supports_slope_image_map[supportSegments[segment].slope & TILE_ELEMENT_SURFACE_SLOPE_MASK];
        baseHeight = supportSegments[segment].height;

        PaintAddImageAsParent(
            session, imageTemplate.WithIndex(pathPaintInfo.BridgeImageId + 37 + imageOffset),
            { SupportBoundBoxes[segment].x, SupportBoundBoxes[segment].y, baseHeight }, { 0, 0, 5 });
        baseHeight += 6;
    }

    // si = height
    // dx = baseHeight

    int16_t heightDiff = Floor2(baseHeight + 16, 16);
    if (heightDiff > height)
    {
        heightDiff = height;
    }

    heightDiff -= baseHeight;

    if (heightDiff > 0)
    {
        PaintAddImageAsParent(
            session, imageTemplate.WithIndex(pathPaintInfo.BridgeImageId + 20 + (heightDiff - 1)),
            { SupportBoundBoxes[segment], baseHeight }, { 0, 0, heightDiff - 1 });
    }

    baseHeight += heightDiff;

    bool keepGoing = true;
    while (keepGoing)
    {
        int16_t z;

        for (int32_t i = 0; i < 4; ++i)
        {
            z = baseHeight + 16;
            if (z > height)
            {
                z = height;
            }
            z -= baseHeight;

            if (z <= 0)
            {
                keepGoing = false;
                break;
            }

            if (i == 3)
            {
                // Only do the z check in the fourth run.
                break;
            }

            PaintAddImageAsParent(
                session, imageTemplate.WithIndex(pathPaintInfo.BridgeImageId + 20 + (z - 1)),
                { SupportBoundBoxes[segment], baseHeight }, { 0, 0, (z - 1) });

            baseHeight += z;
        }

        if (!keepGoing)
        {
            break;
        }

        ImageIndex imageIndex = pathPaintInfo.BridgeImageId + 20 + (z - 1);
        if (z == 16)
        {
            imageIndex += 1;
        }

        PaintAddImageAsParent(
            session, imageTemplate.WithIndex(imageIndex), { SupportBoundBoxes[segment], baseHeight }, { 0, 0, (z - 1) });

        baseHeight += z;
    }

    // Loc6A34D8
    supportSegments[segment].height = 0xFFFF;
    supportSegments[segment].slope = 0x20;

    if (special != 0)
    {
        int16_t si = special + baseHeight;

        while (true)
        {
            int16_t z = baseHeight + 16;
            if (z > si)
            {
                z = si;
            }

            z -= baseHeight;
            if (z <= 0)
            {
                break;
            }

            ImageIndex imageIndex = pathPaintInfo.BridgeImageId + 20 + (z - 1);
            PaintAddImageAsParent(
                session, imageTemplate.WithIndex(imageIndex), { SupportBoundBoxes[segment], baseHeight }, { 0, 0, 0 });

            baseHeight += z;
        }
    }

    return false; // AND
}

constexpr uint8_t MetalSupportTypeCount = 13;
constexpr MetalSupportType RotatedMetalSupports[MetalSupportTypeCount][NumOrthogonalDirections] = {
    { MetalSupportType::Tubes, MetalSupportType::Tubes, MetalSupportType::Tubes, MetalSupportType::Tubes },
    { MetalSupportType::Fork, MetalSupportType::ForkAlt, MetalSupportType::Fork, MetalSupportType::ForkAlt },
    { MetalSupportType::ForkAlt, MetalSupportType::Fork, MetalSupportType::ForkAlt, MetalSupportType::Fork },
    { MetalSupportType::Boxed, MetalSupportType::Boxed, MetalSupportType::Boxed, MetalSupportType::Boxed },
    { MetalSupportType::Stick, MetalSupportType::StickAlt, MetalSupportType::Stick, MetalSupportType::StickAlt },
    { MetalSupportType::StickAlt, MetalSupportType::Stick, MetalSupportType::StickAlt, MetalSupportType::Stick },
    { MetalSupportType::ThickCentred, MetalSupportType::ThickAltCentred, MetalSupportType::Thick, MetalSupportType::ThickAlt },
    { MetalSupportType::Thick, MetalSupportType::ThickAlt, MetalSupportType::ThickCentred, MetalSupportType::ThickAltCentred },
    { MetalSupportType::ThickAlt, MetalSupportType::ThickCentred, MetalSupportType::ThickAltCentred, MetalSupportType::Thick },
    { MetalSupportType::ThickAltCentred, MetalSupportType::Thick, MetalSupportType::ThickAlt, MetalSupportType::ThickCentred },
    { MetalSupportType::Truss, MetalSupportType::Truss, MetalSupportType::Truss, MetalSupportType::Truss },
    { MetalSupportType::TubesInverted, MetalSupportType::TubesInverted, MetalSupportType::TubesInverted,
      MetalSupportType::TubesInverted },
    { MetalSupportType::BoxedCoated, MetalSupportType::BoxedCoated, MetalSupportType::BoxedCoated,
      MetalSupportType::BoxedCoated },
};

void DrawSupportsSideBySide(
    PaintSession& session, Direction direction, uint16_t height, ImageId colour, MetalSupportType type, int32_t special)
{
    type = RotatedMetalSupports[EnumValue(type)][direction];

    if (direction & 1)
    {
        MetalASupportsPaintSetup(session, type, MetalSupportPlace::TopRightSide, special, height, colour);
        MetalASupportsPaintSetup(session, type, MetalSupportPlace::BottomLeftSide, special, height, colour);
    }
    else
    {
        MetalASupportsPaintSetup(session, type, MetalSupportPlace::TopLeftSide, special, height, colour);
        MetalASupportsPaintSetup(session, type, MetalSupportPlace::BottomRightSide, special, height, colour);
    }
}
