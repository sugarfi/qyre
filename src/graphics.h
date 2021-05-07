#ifndef _GRAPHICS_H
#define _GRAPHICS_H

#include <stdint.h>

typedef struct __attribute__((packed)) {
    uint32_t magic;
    uint32_t version;
    uint32_t header_size;
    uint32_t flags;
    uint32_t num_glyphs;
    uint32_t bytes_per_glyph;
    uint32_t height;
    uint32_t width;
} graphics_psf_font_t;

typedef struct {
    int width;
    int height;
    uint32_t *fb;
    uint8_t color_type;
} graphics_context_t;

uint32_t graphics_color(uint8_t, uint8_t, uint8_t, uint8_t, graphics_context_t);
void graphics_put_px(uint32_t, int, int, graphics_context_t);
void graphics_put_rect(uint32_t, int, int, int, int, graphics_context_t);
void graphics_put_text(char *, graphics_psf_font_t *, uint32_t, int, int, graphics_context_t);

#endif
