const unsigned short XLAT_TABLE = {
    0x0, 0x1021, 0x2042, 0x3063,
    0x4084, 0x50a5, 0x60c6, 0x70e4, 0x8108,
    0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad,
    0xe1ce, 0xf1ef
};

unsigned short checksum(const unsigned char* in, int len) {
    if (len == 0) {
        return 0;
    }

    unsigned short last_sum = 0, last_med = 0;
    for (int i = 0; i < len; i++) {
        unsigned char curr_byte = in[i];
        last_med = XLAT_TABLE[(last_sum >> 12) ^ (curr_byte >> 4)] ^ 16 * last_sum;
        last_sum = XLAT_TABLE[(curr_byte ^ (last_med >> 12)) & 0xf] ^ 16 * last_med;
    }

    return last_sum;
}


