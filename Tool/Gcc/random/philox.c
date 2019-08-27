#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef PHILOX_W32_0
#define PHILOX_W32_0 ((uint32_t)0x9E3779B9)
#endif
#ifndef PHILOX_W32_1
#define PHILOX_W32_1 ((uint32_t)0xBB67AE85)
#endif

#ifndef PHILOX_M4x32_0
#define PHILOX_M4x32_0 ((uint32_t)0xD2511F53)
#endif
#ifndef PHILOX_M4x32_1
#define PHILOX_M4x32_1 ((uint32_t)0xCD9E8D57)
#endif

struct array4x32{
    uint32_t v[4];
};

struct array2x32 {
    uint32_t v[2];
};

typedef struct array4x32 philox4x32_ctr_t;
typedef struct array2x32 philox4x32_key_t;
typedef struct array2x32 philox4x32_ukey_t;

uint32_t mulhilo32(uint32_t a, uint32_t b, uint32_t* hip)
{
    uint64_t product = ((uint64_t)a)*((uint64_t)b);
    *hip = product>>32;
    return (uint32_t)product;
}

philox4x32_key_t philox4x32keyinit(philox4x32_ukey_t uk)
{
    return uk;
}

struct array2x32 _philox4x32bumpkey(struct array2x32 key)
{
    key.v[0] += PHILOX_W32_0;
    key.v[1] += PHILOX_W32_1;
    return key;
}

struct array4x32 _philox4x32round(struct array4x32 ctr, struct array2x32 key)
{
    uint32_t hi0;
    uint32_t hi1;
    uint32_t lo0 = mulhilo32(PHILOX_M4x32_0, ctr.v[0], &hi0);
    uint32_t lo1 = mulhilo32(PHILOX_M4x32_1, ctr.v[2], &hi1);
    struct array4x32 out = {{hi1^ctr.v[1]^key.v[0], lo1,
                              hi0^ctr.v[3]^key.v[1], lo0}};
    return out;
}

philox4x32_ctr_t philox4x32_R(uint32_t R, philox4x32_ctr_t ctr, philox4x32_key_t key)
{
    uint32_t i;
    for (i = 0; i < R; i++)
    {
        if (i != 0)
        {
            key = _philox4x32bumpkey(key);
        }
        ctr = _philox4x32round(ctr, key);
    }
    return ctr;
}

#define NUM_RANDOM 102400
philox4x32_ctr_t g_ctr;
philox4x32_key_t g_key;
char str[256];

void print_philox4x32(FILE* f, philox4x32_ctr_t ctr)
{
    fprintf(f, "%u\n", ctr.v[0]);
    fprintf(f, "%u\n", ctr.v[1]);
    fprintf(f, "%u\n", ctr.v[2]);
    fprintf(f, "%u\n", ctr.v[3]);
}

int main()
{
    philox4x32_ukey_t key = { 0xdeadbeef, 0x13579 };
    FILE* f = fopen("1.txt", "w");
    uint32_t i;
    g_key = philox4x32keyinit(key);
    for (i = 0; i < NUM_RANDOM; i++)
    {
        g_ctr = philox4x32_R(10, g_ctr, g_key);
        print_philox4x32(f, g_ctr);
    }
    fclose(f);
    return 0;
}
