#include <graphics.h>

uint32_t graphics_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a, graphics_context_t ctx) {
    switch (ctx.color_type) {
        // ARGB
        case 0: return (uint32_t) ((a << 24) | (r << 16) | (g << 8) | b);
        // RGBA
        case 1: return (uint32_t) ((r << 24) | (g << 16) | (b << 8) | a);
        // ABGR
        case 2: return (uint32_t) ((a << 24) | (b << 16) | (g << 8) | r);
        // BGRA
        case 3: return (uint32_t) ((b << 24) | (g << 16) | (r << 8) | a);
    }

    return 0;
}

void graphics_put_px(uint32_t color, int x, int y, graphics_context_t ctx) {
    ctx.fb[(y * ctx.width) + x] = color;
}

void graphics_put_rect(uint32_t color, int x, int y, int w, int h, graphics_context_t ctx) {
    int x2, y2;
    for (y2 = y; y2 < y + h; y2++) {
        for (x2 = x; x2 < x + w; x2++) {
            ctx.fb[(y2 * ctx.width) + x2] = color;
        }
    }
}

void graphics_put_text(char *text, graphics_psf_font_t *font, uint32_t color, int x, int y, graphics_context_t ctx) {
    uint8_t *data = (uint8_t *) font + font->header_size;
    int off;
    for (off = 0; *text; text++, off++) {
        char c = *text;
        uint8_t *glyph = data + (c * font->bytes_per_glyph);
        int i;
        for (i = 0; i < font->bytes_per_glyph; i++) {
            int x2;
            for (x2 = 0; x2 < font->width; x2++) {
                int x_real = x + (off * font->width) + x2, y_real = y + i;
                ctx.fb[(y_real * ctx.width) + x_real] = glyph[i] & (1 << (font->width - x2)) ? color : 0;
            }
        }
    }
}
