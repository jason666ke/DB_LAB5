//
// Created by 86153 on 2022/12/31.
//

#ifndef DB_LAB5_UTILS_H
#define DB_LAB5_UTILS_H

#include "extmem.h"

char *getSpecificDataFromBlk(unsigned char *blk, int num);

int getNextDiskNum(unsigned char *blk);

void updateDataBlks(unsigned char *data_blks[], Buffer *buf, int length, int start);

int linear_relationship_select(int num);

void sort_blk_data(int start, int end, int sorted_col, int write_base);

void TPMMS(int start, int end, int compare_col);

void create_index(int start, int end, int index_col);

void index_relationship_select(int start, int end, int num, int index_col);

void sort_merge_join(int start_1, int end_1, int select_col_1, int start_2, int end_2, int select_col_2, int write_base);

void test_324();

void intersect(int start_1, int end_1, int select_col_1, int start_2, int end_2, int select_col_2, int write_base);

#endif //DB_LAB5_UTILS_H
