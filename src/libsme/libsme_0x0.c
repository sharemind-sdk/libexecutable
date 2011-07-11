#include "libsme_0x0.h"

#include <assert.h>


void SME_Header_0x0_init(struct SME_Header_0x0 * header,
                         uint8_t number_of_units_minus_one,
                         uint8_t active_linking_unit)
{
    header->number_of_units_minus_one = number_of_units_minus_one;
    header->active_linking_unit = active_linking_unit;
    __builtin_bzero(header->zeroPadding, 4);
}

void SME_Unit_Header_0x0_init(struct SME_Unit_Header_0x0 * header,
                              uint8_t sections_minus_one)
{
    static const char magic[32] = "Linking Unit";
    __builtin_memcpy(&header->type, magic, 32);
    header->sections_minus_one = sections_minus_one;
    __builtin_bzero(&header->zeroPadding, 7);
}

void SME_Section_Header_0x0_init(struct SME_Section_Header_0x0 * header,
                                 enum SME_Section_Type type,
                                 uint32_t length)
{
    assert(type < SME_SECTION_TYPE_COUNT_0x0);

    static const char magic[SME_SECTION_TYPE_COUNT_0x0][32] = {
        "TEXT", "RODATA", "DATA", "BSS", "BIND", "DEBUG"
    };
    __builtin_memcpy(header->type, magic[type], 32);
    header->length = length;
    __builtin_bzero(header->zeroPadding, 4);
}
