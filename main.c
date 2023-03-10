#include <stdio.h>
#include <stdlib.h>
#include "extmem.h"
#include "utils.h"

int main() {
    setvbuf(stdout, NULL, _IONBF, 0);

    printf("****************************************************************************************\n");
    printf("\n任务一: \n");
    linear_relationship_select(128);
    printf("****************************************************************************************\n");
    printf("****************************************************************************************\n");
    printf("\n任务二: \n");
    printf("对关系R进行TPMMS内排序\n");
    TPMMS(1, 16, 0);
    printf("对关系R进行TPMMS内排序\n");
    TPMMS(17, 48, 0);
    printf("****************************************************************************************\n");
    // 任务三
    printf("****************************************************************************************\n");
    printf("\n任务三：\n");
    index_relationship_select(17, 48, 128, 0);
    printf("****************************************************************************************\n");
    // 任务四
    printf("****************************************************************************************\n");
    printf("\n任务四:\n");
    sort_merge_join(1, 16, 0, 17, 48, 0, 600);
    printf("****************************************************************************************\n");
    // 任务五
    printf("****************************************************************************************\n");
    printf("\n任务五:\n");
    intersect(1, 16, 0, 17, 48, 0, 900);
    printf("****************************************************************************************\n");
    return 0;
}

