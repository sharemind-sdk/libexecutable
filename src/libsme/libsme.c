#include "libsme.h"


static const char magic[32] = "Sharemind Executable";

void SME_Common_Header_init(struct SME_Common_Header * h, uint16_t version) {
    __builtin_memcpy(h->magic, magic, 32);
    h->byte_order_verification = 0x0123456789abcdef;
    h->file_format_version = version;
}

int SME_Common_Header_read(void * from, struct SME_Common_Header * to) {
    __builtin_memcpy(to, from, sizeof(struct SME_Common_Header));
    if (__builtin_memcmp(to->magic, magic, 32) != 0)
        return 0;
    static const char byte_order_verification[8] = "\xef\xcd\xab\x89\x67\x45\x23\x01";
    if (__builtin_memcmp(&to->byte_order_verification, byte_order_verification, 8) != 0)
        return 0;
    return 1;
}
