#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h>

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
#define NUM_THREAD 4
uint32_t random_buf[NUM_RANDOM * 4] = {0};
float uni_buf[NUM_RANDOM * 4] = {0};
float gauss_buf[NUM_RANDOM * 4] = {0};
uint32_t key_buf[NUM_THREAD * 2];
philox4x32_ctr_t *g_ctr = (philox4x32_ctr_t *)random_buf;
philox4x32_key_t *g_key = (philox4x32_key_t *)key_buf;

static void * random_thread(void *arg)
{
    uint32_t* p_index = (uint32_t *)arg;
    uint32_t num = NUM_RANDOM / NUM_THREAD;
    uint32_t offset = *p_index * num;
    uint32_t i;
    printf("Thread %d: %d\n", *p_index, offset);
    g_ctr[offset] = philox4x32_R(10, g_ctr[0], g_key[*p_index]);
    for (i = 0; i < num - 1; i++)
    {
        g_ctr[offset + i + 1] = philox4x32_R(10, g_ctr[offset + i], g_key[*p_index]);
    }
}

void init_keys()
{
    uint32_t i;
    for (i = 0; i < NUM_THREAD / 2; i++)
    {
        g_ctr[0] = philox4x32_R(10, g_ctr[0], g_key[0]);
        g_key[i * 2].v[0] = g_ctr[0].v[0];
        g_key[i * 2].v[1] = g_ctr[0].v[1];
        g_key[i * 2 + 1].v[0] = g_ctr[0].v[2];
        g_key[i * 2 + 1].v[1] = g_ctr[0].v[3];
    }
}

void uniform_transform()
{
    uint32_t i;
    float rand_max = pow(2, 32) - 1;
    for (i = 0; i < NUM_RANDOM * 4; i++)
    {
        uni_buf[i] = random_buf[i] / rand_max;
    }
}

void gaussian_transform()
{
    //Box-Muller transform
    uint32_t i;
    const float epsilon = 1.0e-7f;
    float f1, f2;
    for (i = 0; i < NUM_RANDOM * 2; i++)
    {
        if (uni_buf[i * 2] < epsilon)
        {
            uni_buf[i * 2] = epsilon;
        }
        f1 = sqrt(-2.0f * log(uni_buf[i * 2]));
        f2 = 2 * M_PI * uni_buf[i * 2 + 1];
        gauss_buf[i * 2] = f1 * sin(f2);
        gauss_buf[i * 2 + 1] = f1 * cos(f2);
    }
}

void test_random()
{
    philox4x32_ukey_t key = { 0xdeadbeef, 0x13579 };
    FILE* f = fopen("2.txt", "w");
    uint32_t i;
    pthread_t tidp[NUM_THREAD];
    uint32_t thread_id[NUM_THREAD];
    

    //generate random integer
    g_key[0] = philox4x32keyinit(key);
    init_keys();
    for (i = 0; i < NUM_THREAD; i++)
    {
        thread_id[i] = i;
        pthread_create(&(tidp[i]), NULL, random_thread, (void*)&(thread_id[i]));
    }
    for (i = 0; i < NUM_THREAD; i++)
    {
        pthread_join(tidp[i], NULL);
    }
    for (i = 0; i < NUM_RANDOM * 4; i++)
    {
        fprintf(f, "%u\n", random_buf[i]);
    }
    fclose(f);

    //generate Uniform Distribution on [0,1]
    f = fopen("3.txt", "w");
    uniform_transform();
    for (i = 0; i < NUM_RANDOM * 4; i++)
    {
        fprintf(f, "%f\n", uni_buf[i]);
    }
    fclose(f);

    //generate Gaussian Distribution on [0,1]
    f = fopen("4.txt", "w");
    gaussian_transform();
    for (i = 0; i < NUM_RANDOM * 4; i++)
    {
        fprintf(f, "%f\n", gauss_buf[i]);
    }
    fclose(f);
}

int upper_bound(float* a, int n, float x) {
    int l = 0;
    int h = n; // Not n - 1
    while (l < h) {
        int mid = (l + h) / 2;
        if (x >= a[mid]) {
            l = mid + 1;
        } else {
            h = mid;
        }
    }
    return l;
}

void test_upper_bound()
{
    float f[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    int index = upper_bound(f, 5, 0.6);
    printf("\x1b[33m%d\x1b[0m\n",index);
}

int main()
{
    //test_random();
    test_upper_bound();
    return 0;
}
