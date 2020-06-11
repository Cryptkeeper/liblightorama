/*
 * MIT License
 *
 * Copyright (c) 2020 Nick Krecklow
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "io.h"

size_t lor_write_heartbeat(unsigned char *ptr) {
    ptr[0] = LOR_UNIT_ID_BROADCAST;
    ptr[1] = LOR_MAGIC_AND;
    ptr[2] = LOR_MAGIC_HEARTBEAT;
    return 3;
}

size_t lor_write_brightness(lor_brightness_t brightness, unsigned char *ptr) {
    ptr[0] = brightness;
    return 1;
}

size_t lor_write_brightnessf(float normal, lor_brightness_curve_t brightness_curve, unsigned char *ptr) {
    return lor_write_brightness(brightness_curve(normal), ptr);
}

size_t lor_write_duration(lor_duration_t duration, unsigned char *ptr) {
    size_t n = 0;
    ptr[n++] = (duration & 0xFF00u) >> 8u;
    ptr[n++] = duration & 0x00FFu;
    return n;
}

size_t lor_write_durationf(float seconds, unsigned char *ptr) {
    return lor_write_duration(lor_duration_of(seconds), ptr);
}

size_t lor_write_channel(LORChannel channel, unsigned char *ptr) {
    size_t n = 0;
    if (channel.chain_index > 0) {
        ptr[n++] = channel.chain_index;
    }
    switch (channel.type) {
        case LOR_CHANNEL_ID:
            ptr[n++] = LOR_MAGIC_CHANNEL_ID_MASK | channel.bits;
            break;

        case LOR_CHANNEL_MASK8:
            ptr[n++] = channel.bits;
            break;

        case LOR_CHANNEL_MASK16: {
            ptr[n++] = (channel.bits & 0xFF00u) >> 8u;
            ptr[n++] = channel.bits & 0x00FFu;
            break;
        }
    }
    return n;
}

size_t lor_write_channel_action(lor_unit_t unit, lor_channel_action_t action, LORChannel channel, unsigned char *ptr) {
    size_t n = 0;
    ptr[n++] = unit;
    ptr[n++] = lor_get_channel_magic(channel) | action;
    n += lor_write_channel(channel, ptr + n);
    return n;
}

size_t lor_write_channel_fade(lor_unit_t unit, LORChannel channel, lor_brightness_t from, lor_brightness_t to, lor_duration_t duration, unsigned char *ptr) {
    size_t n = 0;
    ptr[n++] = unit;
    ptr[n++] = lor_get_channel_magic(channel) | LOR_ACTION_CHANNEL_FADE;
    n += lor_write_brightness(from, ptr + n);
    n += lor_write_brightness(to, ptr + n);
    n += lor_write_duration(duration, ptr + n);
    n += lor_write_channel(channel, ptr + n);
    return n;
}

size_t lor_write_channel_fade_with(lor_unit_t unit, lor_channel_action_t foreground_action, LORChannel channel, lor_brightness_t from, lor_brightness_t to, lor_duration_t duration, unsigned char *ptr) {
    size_t n = 0;
    ptr[n++] = unit;
    ptr[n++] = lor_get_channel_magic(channel) | foreground_action;
    n += lor_write_channel(channel, ptr + n);
    ptr[n++] = LOR_MAGIC_AND;
    ptr[n++] = lor_get_channel_magic(channel) | LOR_ACTION_CHANNEL_FADE;
    n += lor_write_brightness(from, ptr + n);
    n += lor_write_brightness(to, ptr + n);
    n += lor_write_duration(duration, ptr + n);
    return n;
}

size_t lor_write_channel_set_brightness(lor_unit_t unit, LORChannel channel, lor_brightness_t to, unsigned char *ptr) {
    size_t n = 0;
    ptr[n++] = unit;
    ptr[n++] = lor_get_channel_magic(channel) | LOR_ACTION_CHANNEL_SET_BRIGHTNESS;
    n += lor_write_brightness(to, ptr + n);
    n += lor_write_channel(channel, ptr + n);
    return n;
}

size_t lor_write_unit_action(lor_unit_t unit, lor_unit_action_t action, unsigned char *ptr) {
    ptr[0] = unit;
    ptr[1] = action;
    return 2;
}