﻿#include <stdio.h>
#include <stdint.h>

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

typedef uint32_t bool;

float input[100] = { 0.24405758545342704, 0.6823071261111106, 0.7125174614243092, 0.6764326879764893, 0.2057687974444613,
0.5148242648943889, 0.4415894991869841, 0.27060077944877126, 0.06172037394464147, 0.5164734134185364,
0.18515806765059206, 0.7963922143437836, 0.06152751216117103, 0.958735402447652, 0.7393856323597939,
0.5523319340121301, 0.556665414169253, 0.34817001996224106, 0.733762201043927, 0.6937707904103673,
0.4987256590077457, 0.2638537072587315, 0.635038161881618, 0.3061495156254107, 0.6862333198293101,
0.09922311661076488, 0.5049468852603233, 0.460508454640316, 0.9249518056096026, 0.03507939615315914,
0.6696781190128598, 0.33476739911265696, 0.4415078596393729, 0.4824733385364546, 0.14899859122344405,
0.17373230195717704, 0.7634301908555976, 0.7941249053380055, 0.2526843027002703, 0.11608431614130232,
0.8383041966020701, 0.4609995460094424, 0.0767314654198884, 0.5846037806569536, 0.541814394992107,
0.7830750864163383, 0.4004079471264289, 0.2260560049398299, 0.13474516892465338, 0.24507423085165514,
0.7304842913354657, 0.336440668074804, 0.04522411115399483, 0.06673448237662516, 0.20856642171988393,
0.353438132220802, 0.6103677151906084, 0.93612058907043, 0.621918385096673, 0.4557147943598707,
0.22241876982759612, 0.6212562027847534, 0.7583362464894365, 0.22067006541545298, 0.5068757537774735,
0.36649725082170714, 0.9607344618185378, 0.04417032491168971, 0.708072749895354, 0.7914280158250411,
0.032456786349089106, 0.5090107216633174, 0.979111318269498, 0.06077102563678327, 0.6469422242486804,
0.9821079404896634, 0.5102422954939342, 0.8217614525255671, 0.4070438344077978, 0.6386459984716917,
0.4518300849831902, 0.3390855645971781, 0.8422475848707272, 0.3525378908421799, 0.5744240875936495,
0.9557268914018588, 0.26604609286943504, 0.5216293103631433, 0.10680450967334443, 0.8999371552976716,
0.17431654924402173, 0.36531416318455334, 0.9575092288971582, 0.3991330238556654, 0.3735685315860141,
0.9328186219550156, 0.6130911641646289, 0.09231951566066865, 0.5632275243491878, 0.3379546092469432 };

float output[100] = { 0.9821079404896634, 0.979111318269498, 0.9607344618185378, 0.958735402447652, 0.9575092288971582,
0.9557268914018588, 0.93612058907043, 0.9328186219550156, 0.9249518056096026, 0.8999371552976716,
0.8422475848707272, 0.8383041966020701, 0.8217614525255671, 0.7963922143437836, 0.7941249053380055,
0.7914280158250411, 0.7830750864163383, 0.7634301908555976, 0.7583362464894365, 0.7393856323597939,
0.733762201043927, 0.7304842913354657, 0.7125174614243092, 0.708072749895354, 0.6937707904103673,
0.6862333198293101, 0.6823071261111106, 0.6764326879764893, 0.6696781190128598, 0.6469422242486804,
0.6386459984716917, 0.635038161881618, 0.621918385096673, 0.6212562027847534, 0.6130911641646289,
0.6103677151906084, 0.5846037806569536, 0.5744240875936495, 0.5632275243491878, 0.556665414169253,
0.5523319340121301, 0.541814394992107, 0.5216293103631433, 0.5164734134185364, 0.5148242648943889,
0.5102422954939342, 0.5090107216633174, 0.5068757537774735, 0.5049468852603233, 0.4987256590077457,
0.4824733385364546, 0.4609995460094424, 0.460508454640316, 0.4557147943598707, 0.4518300849831902,
0.4415894991869841, 0.4415078596393729, 0.4070438344077978, 0.4004079471264289, 0.3991330238556654,
0.3735685315860141, 0.36649725082170714, 0.36531416318455334, 0.353438132220802, 0.3525378908421799,
0.34817001996224106, 0.3390855645971781, 0.3379546092469432, 0.336440668074804, 0.33476739911265696,
0.3061495156254107, 0.27060077944877126, 0.26604609286943504, 0.2638537072587315, 0.2526843027002703,
0.24507423085165514, 0.24405758545342704, 0.2260560049398299, 0.22241876982759612, 0.22067006541545298,
0.20856642171988393, 0.2057687974444613, 0.18515806765059206, 0.17431654924402173, 0.17373230195717704,
0.14899859122344405, 0.13474516892465338, 0.11608431614130232, 0.10680450967334443, 0.09922311661076488,
0.09231951566066865, 0.0767314654198884, 0.06673448237662516, 0.06172037394464147, 0.06152751216117103,
0.06077102563678327, 0.04522411115399483, 0.04417032491168971, 0.03507939615315914, 0.032456786349089106 };

static int32_t partition_basic
(
    float* input,
    int32_t left,
    int32_t right,
    bool is_recursion
)
{
    float key;
    int32_t low = left;
    int32_t high = right;
    if (left < right)
    {
        key = input[left];
        while (low < high)
        {
            while (low < high && input[high] <= key)
            {
                high--;
            }
            input[low] = input[high];
            while (low < high && input[low] >= key)
            {
                low++;
            }
            input[high] = input[low];
        }
        input[low] = key;
        if (is_recursion)
        {
            partition_basic(input, left, low - 1, TRUE);
            partition_basic(input, low + 1, right, TRUE);
        }
    }
    return low;
}

static void find_top_k_basic
(
    float* input,
    uint32_t input_len,
    uint32_t k
)
{
    int32_t low = 0;
    int32_t high = input_len - 1;
    int32_t j = partition_basic(input, low, high, FALSE);

    //part_sort
    while (j != k)
    {
        if (k > j)
        {
            low = j + 1;
        }
        else
        {
            high = j;
        }
        j = partition_basic(input, low, high, FALSE);
    }
    //all_sort
    partition_basic(input, 0, k - 1, TRUE);
}

static void print_tensor
(
    float* input,
    uint32_t input_len
)
{
    uint32_t i;
    for (i = 0; i < input_len; i++)
    {
        printf("%f, ", input[i]);
        if (i % 5 == 4)
        {
            printf("\n");
        }
    }
}

static int32_t partition_complex
(
    float* input,
    int32_t left,
    int32_t right,
    bool is_recursion,
    uint32_t* indices
)
{
    float key;
    int32_t key_index;
    int32_t low = left;
    int32_t high = right;
    if (left < right)
    {
        key_index = indices[left];
        key = input[key_index];
        while (low < high)
        {
            while (low < high && input[indices[high]] <= key)
            {
                high--;
            }
            indices[low] = indices[high];
            while (low < high && input[indices[low]] >= key)
            {
                low++;
            }
            indices[high] = indices[low];
        }
        indices[low] = key_index;
        if (is_recursion)
        {
            partition_complex(input, left, low - 1, TRUE, indices);
            partition_complex(input, low + 1, right, TRUE, indices);
        }
    }
    return low;
}

static void find_top_k_1d
(
    float* input,
    uint32_t input_len,
    uint32_t k,
    float* value,
    uint32_t* indices
)
{
    int32_t low = 0;
    int32_t high = input_len - 1;
    int32_t j;

    for (j = 0; j < input_len; j++)
    {
        indices[j] = j;
    }
    j = partition_complex(input, low, high, FALSE, indices);
    //part_sort
    while (j != k)
    {
        if (k > j)
        {
            low = j + 1;
        }
        else
        {
            high = j;
        }
        j = partition_complex(input, low, high, FALSE, indices);
    }
    //all_sort
    partition_complex(input, 0, k - 1, TRUE, indices);

    for (j = 0; j < k; j++)
    {
        value[j] = input[indices[j]];
    }
}

int main()
{
    uint32_t indices[100];
    uint32_t i;
    float value[100];
    //find_top_k_basic(input, 100, 10);
    //print_tensor(input, 10);

    find_top_k_1d(input, 100, 10, value, indices);
    print_tensor(value, 10);
}