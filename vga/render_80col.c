#include <pico/stdlib.h>
#include "common/config.h"
#include "vga/vgabuf.h"
#include "vga/render.h"
#include "vga/vgaout.h"

static inline uint_fast8_t char_terminal_bits(uint_fast8_t ch, uint_fast8_t glyph_line) {
    uint_fast8_t bits = terminal_character_rom[((uint_fast16_t)ch << 3) | glyph_line];
    if(ch & 0x80) {
        // normal character
        return bits & 0x7f;
    }

    if((bits & 0x80) == 0) {
        // inverse character
        return bits ^ 0x7f;
    } else {
        // flashing character
        return (bits ^ text_flasher_mask) & 0x7f;
    }
}


void DELAYED_COPY_CODE(render_terminal)() {
    for(int line=0; line < 24; line++) {
        render_terminal_line(line);
    }
}


void DELAYED_COPY_CODE(render_terminal_line)(unsigned int line) {
    const uint8_t *page = (const uint8_t *)terminal_memory;
    const uint8_t *line_buf = page + (line * 80);

    for(uint glyph_line=0; glyph_line < 8; glyph_line++) {
        struct vga_scanline *sl = vga_prepare_scanline();
        uint sl_pos = 0;

        // Pad 40 pixels on the left to center horizontally
        sl->data[sl_pos++] = (text_border|THEN_EXTEND_7) | ((text_border|THEN_EXTEND_7) << 16); // 16 pixels per word
        sl->data[sl_pos++] = (text_border|THEN_EXTEND_7) | ((text_border|THEN_EXTEND_7) << 16); // 16 pixels per word
        sl->data[sl_pos++] = (text_border|THEN_EXTEND_3) | ((text_border|THEN_EXTEND_3) << 16); // 16 pixels per word

        for(uint col=0; col < 80; ) {
            // Grab 14 pixels from the next two characters
            uint32_t bits_a = char_terminal_bits(line_buf[col], glyph_line);
            col++;
            uint32_t bits_b = char_terminal_bits(line_buf[col], glyph_line);
            col++;

            uint32_t bits = (bits_a << 7) | bits_b;

            // Translate each pair of bits into a pair of pixels
            for(int i=0; i < 7; i++) {
                uint32_t pixeldata = (bits & 0x2000) ? (text_fore) : (text_back);
                pixeldata |= (bits & 0x1000) ?
                    (((uint32_t)text_fore) << 16) :
                    (((uint32_t)text_back) << 16);
                bits <<= 2;

                sl->data[sl_pos] = pixeldata;
                sl_pos++;
            }
        }

        // Pad 40 pixels on the right to center horizontally
        sl->data[sl_pos++] = (text_border|THEN_EXTEND_7) | ((text_border|THEN_EXTEND_7) << 16); // 16 pixels per word
        sl->data[sl_pos++] = (text_border|THEN_EXTEND_7) | ((text_border|THEN_EXTEND_7) << 16); // 16 pixels per word
        sl->data[sl_pos++] = (text_border|THEN_EXTEND_3) | ((text_border|THEN_EXTEND_3) << 16); // 16 pixels per word

        sl->length = sl_pos;
        sl->repeat_count = 1;
        vga_submit_scanline(sl);
    }
}

