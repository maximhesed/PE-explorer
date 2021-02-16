#include "spoiler.h"

struct spoiler *
spl_alloc(const char *cp, char *data)
{
    struct spoiler *spl = malloc(sizeof(struct spoiler));
    char *items[256];
    int items_q = 0;
    int i;
    size_t cp_len = strlen(cp);
    size_t item_len;

    spl->caption = calloc(sizeof(char), cp_len + 1);
    strncpy(spl->caption, cp, cp_len);

    while ((items[items_q] = strtok(items_q ? NULL : data, "\n")))
        items_q++;

    spl->data = malloc(sizeof(char *) * items_q);
    for (i = 0; i < items_q; i++) {
        item_len = strlen(items[i]);

        spl->data[i] = calloc(sizeof(char), item_len + 1);
        strncpy(spl->data[i], items[i], item_len);
    }

    spl->items_q = items_q;
    spl->offset = cp_len + 1;
    spl->state = SCROLL_HIDDEN;

    free(data);

    return spl;
}

void
spl_draw(const struct spoiler *spl)
{
    buf_printf(spl->x, spl->y,
        FOREGROUND_WHITE | 0,
        spl->offset + 3,
        "%s ...",
        spl->caption);

    if (spl->state == SCROLL_SHOWN)
        spl_data_show(spl);
}

void
spl_move(struct spoiler *spl, int y)
{
    char *buf;
    size_t size = spl->offset + 3;

    buf = calloc(sizeof(char), size + 1);
    memset(buf, ' ', size);

    buf_printf(spl->x, spl->y,
        FOREGROUND_WHITE | 0,
        size,
        "%s",
        buf);

    if (spl->state == SCROLL_SHOWN)
        spl_data_hide(spl);

    spl->y += y;

    spl_draw(spl);

    free(buf);
}

void
spl_data_show(const struct spoiler *spl)
{
    int i;

    for (i = 0; i < spl->items_q; i++)
        buf_printf(spl->x + 4, spl->y + i + 1,
            FOREGROUND_WHITE | 0,
            strlen(spl->data[i]),
            "%s",
            spl->data[i]);
}

void
spl_data_hide(const struct spoiler *spl)
{
    char *buf;
    int i;
    size_t size;

    for (i = 0; i < spl->items_q; i++) {
        size = strlen(spl->data[i]);

        buf = calloc(sizeof(char), size + 1);
        memset(buf, ' ', size);

        buf_printf(spl->x + 4, spl->y + i + 1,
            FOREGROUND_WHITE | 0,
            size,
            "%s",
            buf);

        free(buf);
    }
}

void
spl_data_toggle(struct spoiler *spl)
{
    switch (spl->state) {
        case SCROLL_SHOWN:
            spl_data_hide(spl);

            spl->state = SCROLL_HIDDEN;

            break;

        case SCROLL_HIDDEN:
            spl_data_show(spl);

            spl->state = SCROLL_SHOWN;

            break;
    }
}

void
spl_scroll_update(const struct spoiler *spl, int x, int y)
{
    WORD attrs;

    if ((x >= spl->offset && x <= spl->offset + 2) && y == spl->y)
        attrs = 0 | BACKGROUND_WHITE;
    else
        attrs = FOREGROUND_WHITE | 0;

    buf_printf(spl->offset, spl->y, attrs, 3, "%s", "...");
}

void
spl_free(struct spoiler *spl)
{
    int i;

    free(spl->caption);

    for (i = 0; i < spl->items_q; i++)
        free(spl->data[i]);

    free(spl->data);
    free(spl);
}
