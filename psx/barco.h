enum TCodificacion {sin_codificacion,tim_rle4,tim_rle8,tim_rle4_2,tim_rle8_2,tim_rle4_3,tim_rle8_3,tim_rle4_4,tim_rle8_4,bin_rle8,bin_rle8_2,bin_rle8_3,bin_rle8_4};

struct TBinarioHex{
 unsigned int tamBuffer;
 unsigned int tamCodificado;
 enum TCodificacion tipoCodificacion;
 unsigned char *buffer;
};

unsigned char barco_array[2063]={
 0x10,0x00,0x00,0x00,0x09,0x00,0x00,0x00,0x0C,0x02,0x00,0x00,0x00,0x02,0x68,0x01,0x00,0x01,0x01,0x00,0x00,0x80,0xAE,0x1D,0xE7,0x0C,0x73,0x3A,0x83,0x0C,0xF3,0x21,0x99,0x36,0x4B,0x15,0x87,0x04,0x11,0x2A,0x77,0x3A,0xC5,0x10,0x42,0x04,0x6E,0x19,0xF0,0x25,0x85,0x04,0xB5,0x42,0x53,0x2E,0x2A,0x11,0x44,0x00,0x95,0x3A,0x8D,0x1D,0xB0,0x29,0xE9,0x08,0x85,0x10,0xFC,0x42,0x6B,0x25,0x94,0x3E,0x52,0x32,0xB6,0x3E,0x62,0x08,0xAE,0x2D,0x13,0x36,0x4D,0x15,0xC7,0x0C,0x11,0x3E,0x36,0x2E,0xE8,0x0C,0x73,0x3E,0x6C,0x19,0xA5,
 0x0C,0x22,0x00,0xA4,0x10,0x14,0x2A,0x31,0x32,0xC6,0x10,0x35,0x2A,0x64,0x00,0x09,0x11,0x3E,0x4F,0xCF,0x25,0xE6,0x18,0x84,0x08,0xDC,0x3E,0x4C,0x15,0x12,0x2A,0x77,0x42,0x43,0x00,0x8E,0x1D,0xF1,0x25,0x86,0x04,0x53,0x36,0x95,0x42,0xD0,0x21,0xA5,0x10,0xFD,0x4A,0xD6,0x46,0x63,0x04,0xC9,0x08,0x64,0x08,0x21,0x00,0xAF,0x1D,0x74,0x36,0xF3,0x2D,0x99,0x42,0xA7,0x04,0x11,0x32,0x98,0x3A,0x6F,0x15,0xEF,0x31,0x85,0x0C,0x8D,0x25,0xFC,0x4A,0xCE,0x31,0xC8,0x08,0x74,0x3E,0xC6,0x18,0x0A,0x11,0xE7,0x14,0x84,0x10,0xD1,
 0x25,0xA6,0x10,0x63,0x0C,0xAE,0x21,0xF3,0x25,0x99,0x3A,0x4B,0x19,0x11,0x2E,0x78,0x32,0xC5,0x14,0x42,0x08,0x6E,0x1D,0xF0,0x29,0x85,0x08,0xB5,0x46,0x53,0x32,0x4A,0x19,0x95,0x3E,0x8D,0x21,0xEA,0x0C,0xFC,0x46,0x6B,0x29,0x94,0x42,0xB6,0x42,0xAE,0x35,0x14,0x3A,0x31,0x36,0x57,0x2E,0x08,0x19,0x6C,0x1D,0xA6,0x08,0x22,0x04,0x14,0x2E,0xC6,0x14,0x64,0x04,0x84,0x0C,0x4C,0x1D,0x43,0x04,0x53,0x3A,0xD7,0x46,0x95,0x46,0xD0,0x25,0xA5,0x14,0x63,0x08,0x64,0x0C,0x21,0x04,0x74,0x3A,0xA7,0x08,0xC8,0x0C,0x74,0x42,0x0A,
 0x15,0xE7,0x18,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,
 0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,
 0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0xCC,0x09,0x00,0x00,0x00,0x02,0x04,0x01,0x14,0x00,0x3F,0x00,0xCF,0x23,0x4F,0x73,0xC5,0x23,0x49,0xC1,0x23,0x4F,0xC4,0x23,0x4F,0x49,0xC3,0x23,0xC0,0xC0,0xC5,0x23,0x49,0xC4,0x23,0x49,0xC5,0x23,0x4F,0xC1,0x23,0x4F,0xC4,0x23,0x49,0xC6,0x23,0x4F,0xC1,0x23,0xC0,0xC0,0xC2,0x23,0x49,0x4F,0xC1,0x23,0x4F,0x49,0xC1,0x23,0x4F,0xD0,0x23,0x4F,0x49,0xC7,0x23,
 0xC0,0xC0,0x23,0x4F,0xCB,0x23,0x49,0xC4,0x23,0x3C,0x0F,0x54,0xC2,0x23,0x49,0xCB,0x23,0xC0,0xC0,0xCF,0x23,0x4F,0xC1,0x23,0x08,0x0F,0xC1,0x44,0x17,0x7C,0xC6,0x23,0x4F,0x49,0xC2,0x23,0x49,0x23,0xC0,0xC0,0xC7,0x23,0x49,0x4F,0xC3,0x23,0x4F,0xC3,0x23,0x39,0x4B,0x78,0x08,0x6D,0xCE,0x23,0xC0,0xC0,0xC2,0x23,0x49,0x23,0x4F,0xCB,0x23,0x39,0x2F,0x44,0x78,0x2F,0x06,0x38,0x23,0x4F,0xC2,0x23,0x49,0xC5,0x23,0x4F,0x23,0xC0,0xC0,0xC1,0x23,0x4F,0xC7,0x23,0x49,0xC5,0x23,0x13,0x2F,0x39,0x4D,0x3C,0x4A,0x86,0xC2,0x23,
 0x49,0x4F,0xC8,0x23,0xC0,0xC0,0xCD,0x23,0x49,0xC2,0x23,0xC1,0x29,0x78,0x49,0x5A,0x0A,0x52,0xC7,0x23,0x4F,0x49,0xC3,0x23,0xC0,0xC0,0xC6,0x23,0x49,0xC3,0x23,0x4F,0xC4,0x23,0x39,0x29,0x21,0x57,0x7A,0x16,0x7F,0xCD,0x23,0xC0,0xC0,0xC3,0x23,0x49,0xC2,0x23,0x4F,0x49,0xC6,0x23,0x46,0x7B,0x62,0x24,0x7E,0x65,0x49,0xC5,0x23,0x4F,0x49,0xC4,0x23,0x4F,0x23,0xC0,0xC0,0x23,0x4F,0xCD,0x23,0x40,0x0C,0x00,0x5B,0x4A,0x49,0x23,0x79,0xC1,0x23,0x49,0xC7,0x23,0x49,0xC2,0x23,0xC0,0xC0,0xC3,0x23,0x4F,0xC5,0x23,0x4F,0xC2,
 0x23,0x28,0x0B,0x7D,0x58,0x33,0x58,0x73,0xC2,0x23,0x4F,0xCC,0x23,0xC0,0xC0,0xC4,0x23,0x49,0x4F,0xC6,0x23,0x0C,0x28,0x34,0x0C,0x33,0x76,0x33,0x7B,0x2A,0xC4,0x23,0x49,0xC6,0x23,0x4F,0xC1,0x23,0xC0,0xC0,0xC1,0x23,0x49,0xC5,0x23,0x49,0xC2,0x23,0x50,0x7C,0x63,0x7C,0x33,0x02,0x8D,0xC1,0x7B,0x2D,0x7B,0xC4,0x23,0x4F,0x49,0xC2,0x23,0x49,0xC3,0x23,0xC0,0xC0,0xCC,0x23,0x29,0x0C,0x28,0x0C,0x7B,0x2D,0x33,0x58,0x33,0x00,0x2D,0xC1,0x23,0x4F,0xC5,0x23,0x4F,0xC4,0x23,0xC0,0xC0,0xC1,0x23,0x4F,0xC6,0x23,0x4F,0xC2,
 0x23,0x45,0x34,0x28,0x2D,0x0B,0x28,0x63,0x8D,0x56,0x85,0xCC,0x23,0x4F,0x23,0xC0,0xC0,0xC4,0x23,0x49,0xC6,0x23,0x43,0x39,0x5B,0x7B,0x0B,0x34,0x2A,0x67,0x2D,0x63,0x7B,0x84,0xC5,0x23,0x49,0xC6,0x23,0xC0,0xC0,0xC5,0x23,0x4F,0x23,0x49,0xC3,0x23,0x0C,0x34,0x2D,0x0B,0x40,0x59,0x0C,0x7D,0xC1,0x2D,0x2A,0x63,0xC2,0x23,0x49,0xC1,0x23,0x4F,0xC2,0x23,0x49,0xC2,0x23,0xC0,0xC0,0xC2,0x23,0x49,0xC8,0x23,0xC1,0x34,0x0B,0x7B,0x2A,0x59,0x00,0x2D,0x63,0x33,0xC1,0x63,0x23,0x4F,0xC8,0x23,0x4F,0xC1,0x23,0xC0,0xC0,0xC1,
 0x23,0x4F,0xC5,0x23,0x4F,0xC1,0x23,0x43,0x2A,0x67,0x5B,0x7B,0x40,0x0B,0x28,0x2D,0x63,0x7B,0x2A,0x40,0xCD,0x23,0xC0,0xC0,0xCB,0x23,0x85,0x02,0x5B,0x0B,0x59,0x40,0x43,0x1E,0x67,0x7D,0xC1,0x7B,0x04,0xC2,0x23,0x4F,0xC2,0x23,0x49,0x4F,0xC4,0x23,0xC0,0xC0,0xC5,0x23,0x49,0xC3,0x23,0x0C,0x28,0x2D,0x0B,0x34,0x2A,0x43,0x0C,0x43,0x04,0x28,0x7B,0x0B,0x84,0xCA,0x23,0x49,0xC1,0x23,0xC0,0xC0,0x23,0x4F,0xC1,0x23,0x49,0x4F,0xC1,0x23,0x4F,0xC1,0x23,0x39,0x28,0x2D,0x63,0x58,0x7B,0x04,0x34,0x04,0x34,0x67,0x0B,0x7B,
 0x59,0xC2,0x23,0x49,0xC9,0x23,0xC0,0xC0,0xCA,0x23,0x0C,0x2D,0x02,0xC1,0x7B,0x2A,0x2F,0x2A,0x28,0x2A,0xC1,0x28,0x04,0x28,0x8D,0xC4,0x23,0x49,0xC6,0x23,0xC0,0xC0,0xCA,0x23,0x7C,0x76,0x33,0x58,0x02,0x2A,0x28,0x2D,0x0B,0x2D,0x63,0x8D,0x34,0x84,0x63,0x8D,0xC1,0x23,0x4F,0xC3,0x23,0x49,0x23,0x4F,0xC1,0x23,0xC0,0xC0,0xC1,0x23,0x49,0xC4,0x23,0x49,0xC1,0x23,0x45,0x7B,0x58,0x33,0x58,0x04,0x78,0x0B,0x2D,0x7B,0x85,0x7B,0x00,0x1E,0x28,0x02,0x8D,0xC4,0x23,0x4F,0xC4,0x23,0xC0,0xC0,0xC4,0x23,0x49,0x4F,0xC2,0x23,
 0x43,0x04,0x7B,0x02,0x7B,0x2D,0x34,0x2A,0x2D,0x78,0x0B,0x02,0x58,0x33,0x40,0x46,0x18,0x4A,0xCA,0x23,0xC0,0xC0,0xC1,0x23,0x4F,0xC5,0x23,0x4F,0x7F,0x2D,0x0B,0x8D,0x58,0x63,0x7C,0xC1,0x28,0x0B,0x2D,0x33,0x58,0x8D,0x2A,0x79,0x21,0x35,0x24,0x49,0xC4,0x23,0x49,0xC2,0x23,0xC0,0xC0,0xC9,0x23,0x85,0x0B,0x8D,0x58,0x33,0x7B,0x43,0x34,0x28,0x2D,0x0B,0x2D,0x8D,0x63,0x2D,0xC1,0x23,0x89,0x49,0xC1,0x23,0x4F,0xC3,0x23,0x4F,0xC1,0x23,0xC0,0xC0,0xC9,0x23,0x7F,0x45,0x2D,0x02,0x8D,0x2D,0x39,0x34,0xC1,0x28,0x2A,0xC1,
 0x2D,0x8D,0x33,0xC3,0x23,0x4F,0x23,0x49,0xC6,0x23,0xC0,0xC0,0x23,0x4F,0xC1,0x23,0x49,0x4F,0xC1,0x23,0x49,0xC2,0x23,0x06,0x41,0x46,0x2A,0x1E,0x43,0x04,0x28,0x67,0x28,0x2D,0x8D,0x33,0x5B,0xCC,0x23,0xC0,0xC0,0xC8,0x23,0x4F,0x23,0x4E,0x06,0x31,0xC1,0x0C,0x29,0x1E,0x13,0x34,0x2A,0x28,0x0B,0x02,0x8D,0x33,0xC7,0x23,0x49,0xC1,0x23,0x4F,0x23,0xC0,0xC0,0xCA,0x23,0x8C,0x5E,0x41,0xC3,0x0C,0x43,0x04,0x0F,0x2A,0x5B,0xC1,0x7B,0x8D,0x5B,0xC1,0x23,0x4F,0xC3,0x23,0x4F,0xC3,0x23,0xC0,0xC0,0xC2,0x23,0x49,0xC6,0x23,
 0x00,0x7A,0x35,0x43,0x29,0xC2,0x0C,0x2F,0x04,0xC1,0x28,0x0B,0x02,0x33,0x8D,0x18,0xC2,0x23,0x49,0xC6,0x23,0xC0,0xC0,0x23,0x4F,0xC3,0x23,0x49,0xC4,0x23,0x7F,0x23,0x43,0x1E,0x46,0x0C,0x39,0x1E,0x34,0x0F,0x34,0x2D,0x0B,0xC1,0x8D,0x84,0xC7,0x23,0x49,0xC1,0x23,0xC0,0xC0,0xC6,0x23,0x4F,0x23,0x49,0xC3,0x23,0x13,0x7C,0x1E,0x0C,0x46,0x43,0x0C,0x34,0x28,0x2A,0x2D,0x02,0x33,0x58,0x63,0xC1,0x23,0x4F,0xC6,0x23,0xC0,0xC0,0xC9,0x23,0x4F,0xC1,0x23,0x39,0xC1,0x04,0x39,0xC2,0x0C,0x1E,0x34,0x28,0x34,0x2D,0x0B,0x58,
 0x33,0x56,0xC9,0x23,0xC0,0xC0,0xC2,0x23,0x49,0x4F,0xC7,0x23,0x39,0xC2,0x34,0x1E,0x46,0x0C,0x43,0x2F,0x34,0xC1,0x28,0x2D,0x33,0xC1,0x58,0x63,0xC1,0x23,0x49,0xC3,0x23,0x4F,0x23,0xC0,0xC0,0x23,0x4F,0xC5,0x23,0x49,0xC3,0x23,0x0C,0xC1,0x34,0x2F,0x34,0x43,0x46,0xC1,0x0C,0x34,0x2A,0xC1,0x28,0x2D,0x8D,0x33,0x56,0xC8,0x23,0xC0,0xC0,0xC8,0x23,0x4F,0xC2,0x23,0x39,0x34,0xC1,0x28,0x04,0x43,0xC1,0x0C,0x1E,0x34,0x28,0x2A,0xC2,0x2D,0xC1,0x8D,0x2A,0x23,0x4F,0x23,0x49,0xC3,0x23,0xC0,0xC0,0xCC,0x23,0x1E,0xC2,0x28,
 0x2A,0x2F,0x1E,0x46,0x0C,0x39,0x04,0x67,0x28,0x2D,0x63,0x58,0x33,0x84,0xC4,0x23,0x4F,0xC1,0x23,0xC0,0xC0,0xC1,0x23,0x4F,0x23,0x49,0xC7,0x23,0x0C,0x28,0x2A,0xC1,0x28,0x04,0x43,0xC1,0x0C,0x1E,0x34,0x28,0x2A,0x2D,0x63,0x56,0x84,0xC8,0x23,0xC0,0xC0,0xC5,0x23,0x49,0xC2,0x23,0x49,0xC1,0x23,0xC1,0x85,0x67,0x28,0x7D,0x34,0x85,0x46,0x0C,0x29,0x43,0x34,0x28,0x2D,0x5B,0xC1,0x23,0x49,0xC3,0x23,0x49,0xC2,0x23,0xC0,0xC0,0xC7,0x23,0x4F,0xC2,0x23,0x0D,0x01,0x09,0x69,0x04,0x28,0x86,0x5C,0x7F,0x29,0x46,0x1E,0xC1,
 0x34,0x10,0x8B,0xCA,0x23,0xC0,0xC0,0xC3,0x23,0x4F,0xC6,0x23,0x36,0x3F,0x09,0x4C,0x1F,0x7E,0x23,0x4F,0xC2,0x23,0x12,0x01,0x1C,0x82,0x81,0x49,0x23,0x4F,0xC4,0x23,0x4F,0xC1,0x23,0xC0,0xC0,0xC2,0x23,0x49,0xC6,0x23,0x12,0x3F,0x09,0x4C,0x4F,0x53,0xC5,0x23,0x17,0x27,0x61,0x1B,0x1D,0xC5,0x23,0x4F,0x73,0xC2,0x23,0xC0,0xC0,0x23,0x4F,0xC3,0x23,0x49,0xC1,0x23,0x4F,0x23,0x47,0x37,0x69,0x73,0x80,0xC6,0x23,0x25,0x07,0x0E,0x6B,0x88,0xC3,0x23,0x49,0xC5,0x23,0xC0,0xC0,0xC9,0x23,0x36,0x3F,0x11,0x14,0x80,0x6F,0xC6,
 0x23,0x22,0x30,0x01,0x03,0x14,0xC6,0x23,0x4F,0xC2,0x23,0xC0,0xC0,0xC3,0x23,0x49,0x23,0x4F,0xC2,0x23,0x3A,0x3B,0x48,0x80,0x53,0xC2,0x23,0x4F,0xC2,0x23,0x4F,0x23,0x30,0x15,0x3D,0x71,0x8B,0xC2,0x23,0x4F,0xC5,0x23,0xC0,0xC0,0xC1,0x23,0x4F,0xC5,0x23,0x12,0x01,0x11,0x88,0x2C,0xC9,0x23,0x30,0x15,0x01,0x70,0x68,0xC9,0x23,0xC0,0xC0,0xC8,0x23,0x00,0x0E,0x27,0x6B,0x53,0xC9,0x23,0x22,0x6A,0x32,0x03,0x70,0xC3,0x23,0x00,0xC2,0x23,0x49,0x23,0xC0,0xC0,0xC2,0x23,0x49,0xC3,0x23,0x79,0xC1,0x00,0x02,0x6C,0xC2,0x23,
 0x1F,0xC7,0x72,0x12,0x15,0x74,0x42,0xC4,0x00,0x50,0xC3,0x23,0xC0,0xC0,0xC5,0x23,0x49,0xC1,0x23,0x7F,0x18,0xC2,0x00,0xC8,0x72,0x1F,0x72,0x0F,0x6A,0x61,0x75,0xC1,0x00,0x87,0x00,0x18,0x4F,0xC3,0x23,0xC0,0xC0,0xC1,0x23,0x4F,0xC4,0x23,0xC2,0x72,0x79,0xC4,0x00,0xC6,0x72,0xC1,0x23,0x22,0xC3,0x00,0x7F,0x64,0xC2,0x23,0x4F,0xC1,0x23,0xC0,0xC0,0xC5,0x23,0x4F,0xC5,0x23,0xCA,0x72,0xC1,0x23,0x72,0xC1,0x79,0xC9,0x23,0xC0,0xC0,0xCD,0x23,0xD4,0x72,0xC3,0x23,0xC0,0xC0,0xC3,0x23,0x49,0xC2,0x23,0x49,0xC1,0x23,0xD2,
 0x72,0x23,0x49,0xC2,0x23,0x49,0xC2,0x23,0xC0,0xC0,0xC1,0x23,0x4F,0xC2,0x23,0x4F,0xC2,0x23,0x4F,0xC2,0x72,0x1F,0xC1,0x72,0x23,0x4F,0xC2,0x23,0x4F,0x23,0xC1,0x72,0x1F,0x72,0xC1,0x23,0x4F,0xC2,0x23,0x4F,0xC3,0x23,0xC0,0xC0,0xCA,0x23,0xC3,0x72,0xD7,0x23,0xC0,0xC0,0xE6,0x23,0xC0,0xC0,0xE6,0x23,0xC0,0xC0,0xCF,0x23
};

struct TBinarioHex barco= {3040,2063,tim_rle8,barco_array};