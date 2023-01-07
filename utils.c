//
// Created by 86153 on 2022/12/31.
//
#include <stdlib.h>
#include <stdio.h>
#include <intrin.h>
#include "extmem.h"

// 一个元素占四个字节

#define DATA_LENGTH 4
// 一个数据块存七个元组加一个地址
#define BLK_LENGTH 8
// 关系R所占磁盘块的起始下标
#define R_DISK_START_INDEX 1
// 关系R所占磁盘块最大下标
#define R_DISK_MAX_INDEX 16
// 关系S所占磁盘块的起始下标
#define S_DISK_START_INDEX 17
// 关系S所占磁盘块最大下标
#define S_DISK_MAX_INDEX 48

/**
 * 获取blk中等于num的数据
 * @param blk 磁盘块
 * @param num 特定值
 * @param result_matrix 结果数组
 */
void getSpecificDataFromBlk(unsigned char *blk, int (*result_matrix)[2], int num) {
    // 从blk读取出的数据
    char str_1[4];
    char str_2[4];
    // 输出的结果
    char str[8] = {"-1"};
    // 字符型转整形数据
    int x = -1, y = -1;

    // 结果数组
    int result_num = 0;

    // BLK_LENGTH - 1 是因为需要存放一个后继磁盘块地址
    for (int i = 0; i < BLK_LENGTH - 1; i++) {
        for (int j = 0; j < DATA_LENGTH; j++) {
            str_1[j] = *(blk + i * 8 + j);
        }
        x = atoi(str_1);
        for (int j = 0; j < DATA_LENGTH; j++) {
            str_2[j] = *(blk + i * 8 + DATA_LENGTH + j);
        }
        y = atoi(str_2);

        // 写入结果
        if (x == 128) {
//            printf("找到结果(%d, %d)\n", x, y);
            result_matrix[result_num][0] = x;
            result_matrix[result_num][1] = y;
            result_num += 1;
        }
    }

    // 没有该块符合条件的数据
    if (result_num == 0) {
        printf("当前块没有符合条件的数据\n");
    } else {
        printf("当前块找到的数据有: ");
        for (int i = 0; i < result_num; i++) {
            printf("(%d, %d)\t", result_matrix[i][0], result_matrix[i][1]);
        }
        printf("\n");
    }
}

/**
 * 从数据块中获取某一列数据并排序
 * @param data_blks 原始数据
 * @param sorted_col 用于排序的数据列
 */
void sortColFromBlk(unsigned char *data_blks[], int sorted_col) {
    char str_1[4];
    char str_2[4];
    char str_3[4];
    char str_4[4];

    // 缓冲区共有8个块
    int tuple_1[2] = {0};
    int tuple_2[2] = {0};
    int temp[2] = {0};

    // 原始数据
//    printf("原始数据为:\n");
    for (int i = 0; i < 64; i++) {
        // 计算块下标
        int blk_index = i / 8;
        // 块内下标
        int data_index = i - (blk_index) * 8;

        if (data_index == 7) {
//            printf("\n");
            continue;
        } else {
            for (int k = 0; k < 4; k++) {
                str_1[k] = *(data_blks[blk_index] + data_index * 8 + k);
                str_2[k] = *(data_blks[blk_index] + data_index * 8 + 4 + k);
            }
            tuple_1[0] = atoi(str_1);
            tuple_1[1] = atoi(str_2);
//            printf("(%d, %d)\t", tuple_1[0], tuple_1[1]);
        }
    }

    // 冒泡排序
    for (int i = 0; i < 63; i++) {
        for (int j = 0; j < 63 - i; j++) {
            // 计算块下标
            int blk_index = j / 8;
            // 块内下标
            int data_index = j - (blk_index) * 8;
            // 跳过数据块下标
            if (data_index == 7) {
                continue;
            }
            for (int k = 0; k < 4; k++) {
                str_1[k] = *(data_blks[blk_index] + data_index * 8 + k);
                str_2[k] = *(data_blks[blk_index] + data_index * 8 + 4 + k);
            }
            tuple_1[0] = atoi(str_1);
            tuple_1[1] = atoi(str_2);
            // 边界条件: 第7个数据时
            if (data_index == 6) {
                for (int k = 0; k < 4; k++) {
                    // 最后一个数据
                    if (blk_index == 7) {
                        str_3[k] = *(data_blks[blk_index] + (data_index) * 8 + k);
                        str_4[k] = *(data_blks[blk_index] + (data_index) * 8 + 4 + k);
                    } else {
                        str_3[k] = *(data_blks[blk_index + 1] + k);
                        str_4[k] = *(data_blks[blk_index + 1] + 4 + k);
                    }
                }
                tuple_2[0] = atoi(str_3);
                tuple_2[1] = atoi(str_4);
            } else {
                for (int k = 0; k < 4; k++) {
                    str_3[k] = *(data_blks[blk_index] + (data_index + 1) * 8 + k);
                    str_4[k] = *(data_blks[blk_index] + (data_index + 1) * 8 + 4 + k);
                }
                tuple_2[0] = atoi(str_3);
                tuple_2[1] = atoi(str_4);
            }
            // 交换对应位置数据
            if (tuple_1[sorted_col] > tuple_2[sorted_col]) {
                if (data_index == 6) {
                    for (int k = 0; k < 4; k++) {
                        // 当前数据所在位置
                        *(data_blks[blk_index] + data_index * 8 + k) = str_3[k];
                        *(data_blks[blk_index] + data_index * 8 + 4 + k) = str_4[k];
                        // 交换的数据所在位置
                        *(data_blks[blk_index + 1] + k) = str_1[k];
                        *(data_blks[blk_index + 1] + 4 + k) = str_2[k];
                    }
                } else {
                    for (int k = 0; k < 4; k++) {
                        // 当前数据所在位置
                        *(data_blks[blk_index] + data_index * 8 + k) = str_3[k];
                        *(data_blks[blk_index] + data_index * 8 + 4 + k) = str_4[k];
                        // 交换的数据所在位置
                        *(data_blks[blk_index] + (data_index + 1) * 8 + k) = str_1[k];
                        *(data_blks[blk_index] + (data_index + 1) * 8 + 4 + k) = str_2[k];
                    }
                }
            }
        }
    }

    // 排序后的数据
//    printf("经过排序的数据为: \n");
    for (int i = 0; i < 64; i++) {
        // 计算块下标
        int blk_index = i / 8;
        // 块内下标
        int data_index = i - (blk_index) * 8;

        if (data_index == 7) {
//            printf("\n");
            continue;
        } else {
            for (int k = 0; k < 4; k++) {
                str_1[k] = *(data_blks[blk_index] + data_index * 8 + k);
                str_2[k] = *(data_blks[blk_index] + data_index * 8 + 4 + k);
            }
            tuple_1[0] = atoi(str_1);
            tuple_1[1] = atoi(str_2);
//            printf("(%d, %d)\t", tuple_1[0], tuple_1[1]);
        }
    }

}


/**
 * 找到磁盘的后继数据块地址
 * @param blk 当前磁盘块
 * @return 后续磁盘块地址
 */
int getNextDiskNum(unsigned char *blk) {
    // 七个元组
    int tuple_num = 7;

    char str[5];
    for (int i = 0; i < 4; i++) {
        str[i] = *(blk + tuple_num * 8 + i);
    }
    int addr = atoi(str);
//    printf("next address: %d\n", addr);
    return addr;
}

/**
 * 更新数据缓冲区
 * @param data_blks 数据块数组
 * @param buf 缓冲区
 * @param length 数据块数组长度
 */
void updateDataBlks(unsigned char *data_blks[], Buffer *buf, int length) {
    // 计算下一块数据块编号
    int next_disk = getNextDiskNum(data_blks[length - 1]);
    // 解除块对缓冲区内存的占用
    for (int i = 0; i < length; i++) {
        freeBlockInBuffer(data_blks[i], buf);
    }

    // 更新数据块
    data_blks[0] = readBlockFromDisk(next_disk, buf);
    next_disk = getNextDiskNum(data_blks[0]);
    for (int i = 1; i < length; i++) {
        // 当计算出下一块内存为49时说明已经完全读取
        if (next_disk == 49) break;
        // 写入数据
        data_blks[i] = readBlockFromDisk(next_disk, buf);
        // 计算下一磁盘块地址
        next_disk = getNextDiskNum(data_blks[i]);
    }
}

/**
 * 将结果写入缓冲区BLK中
 * @param result_matrix
 * @param blk
 * @param nextDisk 磁盘后继块地址
 */
void writeResultToBlock(int (*result_matrix)[2], int *result_num, unsigned char *blk, int nextDisk) {
    char str[4];
    int finish = 0;
    if (*result_num == 7) {
        printf("当前数据块已经写满！\n");
        return;
    }
    // 写入七个数据元组
    while (*result_num < 7) {
        if (finish) break;
        for (int i = 0; i < 7; i++) {
            // 所有结果已经写完
            if (result_matrix[i][0] == 0) {
                finish = 1;
                break;
            }
            // 写入X
            itoa(result_matrix[i][0], str, 10);
            for (int j = 0; j < DATA_LENGTH; j++) {
                *(blk + *result_num * 8 + j) = str[j];
            }
            // 写入Y
            itoa(result_matrix[i][1], str, 10);
            for (int j = 0; j < DATA_LENGTH; j++) {
                *(blk + *result_num * 8 + DATA_LENGTH + j) = str[j];
            }
            // 更新数据个数
            *result_num += 1;
        }
    }

    // 写入磁盘后继块地址
    itoa(nextDisk, str, 10);
    for (int i = 0; i < 4; i++) {
        *(blk + 7 * 8 + i) = str[i];
    }
}

/**
 * 基于线性搜索的关系选择，
 * 缓冲区内：用6个块存放数据，2个块存放结果
 * @param num 需要选择的数值
 * @return null
 */
int linear_relationship_select(int num) {

    Buffer buf;
    // 缓冲区数据块
    unsigned char *data_blk[6];
    // 缓冲区结果块
    unsigned char *result_blk[2];
    // 结果计数
    int result_num[2] = {0};

    int answer_num = 0;
    // buffer初始化
    if (!initBuffer(520, 64, &buf)) {
        perror("Buffer Initialization Failed\n");
        return -1;
    }

    // 记录当前正在读的数据块数
    int count = 17;

    // 数据块初始化
    data_blk[0] = readBlockFromDisk(S_DISK_START_INDEX, &buf);
//    printf("读入数据块%d\n", S_DISK_START_INDEX);
    int cur_disk = S_DISK_START_INDEX;
    int next_disk = getNextDiskNum(data_blk[0]);
    for (int i = 1; i < 6; i++) {
        // 写入数据
        data_blk[i] = readBlockFromDisk(next_disk, &buf);
//        printf("读入数据块%d\n", next_disk);
        cur_disk = next_disk;
        // 计算下一磁盘块地址
        next_disk = getNextDiskNum(data_blk[i]);
    }

    // 结果块初始化
    result_blk[0] = getNewBlockInBuffer(&buf);
    result_blk[1] = getNewBlockInBuffer(&buf);

    // 完成遍历的标记变量
    int finish = 0;

    // 结果磁盘块起始下标
    int start_index = 100;

    while (count < 49) {
        for (int i = 0; i < 6; i++) {
            // 刷新result_matrix
            int result_matrix[8][2] = {0};
            printf("当前读取的数据块为%d \t", count);
            getSpecificDataFromBlk(data_blk[i], result_matrix, num);
            count += 1;
            if (result_num[0] < 7) {
                // 数据写回
                writeResultToBlock(result_matrix, &result_num[0], result_blk[0], start_index + 1);
            } else if (result_num[1] < 7) {
                // 数据写回
                writeResultToBlock(result_matrix, &result_num[1], result_blk[1], start_index + 2);
            }
                // 结果块都已写满
            else {
                writeBlockToDisk(result_blk[0], start_index, &buf);
                writeBlockToDisk(result_blk[1], start_index + 1, &buf);
                start_index += 2;
            }
            // 遍历结束
            if (count == 49) {
                finish = 1;
                break;
            }
        }
        if (finish) {
            // 写入数据
            writeBlockToDisk(result_blk[0], start_index, &buf);
            writeBlockToDisk(result_blk[1], start_index + 1, &buf);
            printf("将选择结果写入磁盘块 %d 与 %d\n", start_index, start_index + 1);
            break;
        }
        updateDataBlks(data_blk, &buf, 6);
    }

    // 输出IO读取次数
    printf("IO num: %d\n", buf.numIO);

    return 0;
}

/**
 * 内排序
 * 对于关系R，使用属性 R.A 进行排序
 * 对于关系S，使用属性 S.C 进行排序
 * @param start 起始磁盘块号
 * @param end 结束磁盘块号
 * @param sorted_col 用于排序的数据列
 * @param write_base 起始写入块号基底
 */
void sort_blk_data(int start, int end, int sorted_col, int write_base) {
    // 声明缓冲区并初始化
    Buffer buf;
    initBuffer(520, 64, &buf);

    // 8个块一组将原始数据进行排序
    unsigned char *data_blks[8];

    // 内排序轮数
    int round = (end + 1 - start) / 8;
    int cur_round = 0;
    // 数据块初始化
    int next_disk = start;
    while (cur_round < round) {
//        printf("****************************************************************************************\n");
//        printf("第 %d 轮内排序:\n", cur_round + 1);
        for (int i = 0; i < 8; i++) {
//            printf("读入数据块%d\n", next_disk);
            data_blks[i] = readBlockFromDisk(next_disk, &buf);
            next_disk = getNextDiskNum(data_blks[i]);
        }
        // 数据排序
        sortColFromBlk(data_blks, 0);
        // 数据写回
        for (int i = 0; i < 8; i++) {
            int write_blk = write_base + start + cur_round * 8 + i;
//            printf("写入数据块%d\n", write_blk);
            writeBlockToDisk(data_blks[i], write_blk, &buf);
        }
        cur_round += 1;
//        printf("****************************************************************************************\n");
    }
}

/**
 * 创建 end - start个指针分别指向每一个子表
 * 因为每个子表已经排好序，所以找到最小值的那个子表移入结果块然后进行排序即可
 * @param start
 * @param end
 * @param compare_col 比较的属性
 */
void TPMMS(int start, int end, int compare_col) {
    // 声明缓冲区并初始化
    Buffer buf;
    initBuffer(520, 64, &buf);
    // 对原始数据进行内排序
    sort_blk_data(start, end, 0, 200);
    // 组数
    int group_num = (end + 1 - start) / 8;

    // 声明指向每个组的指针
    unsigned char *group[group_num];
    memset(group, 0, sizeof(group));

    // 声明结果块
    int result_num[8 - group_num];
    memset(result_num, 0, sizeof(result_num));
    unsigned char *result_blk[8 - group_num];
    for (int i = 0; i < (8 - group_num); i++) {
        result_blk[i] = getNewBlockInBuffer(&buf);
    }

    // 声明写回磁盘块的下标
    int write_disk_index = 300 + start;

    // 记录当前组读取到的位置
    int cur_group_index[group_num];
    memset(cur_group_index, 0, sizeof(cur_group_index));

    // 所有组都已经完成的标记变量
    int all_finish = 0;

    // 每一个组单独的标记变量
    int finish[group_num];
    memset(finish, 0, sizeof(finish));

    int tuple[group_num][2];
    memset(tuple, 0, sizeof(tuple));

    while (1) {
        char str_1[4];
        char str_2[4];
        // 判断所有组是否已经完全遍历
        int finish_num = 0;
        while (finish_num < group_num && (finish[finish_num] == 1)) {
            finish_num++;
        }
        if (finish_num == group_num) {
            break;
        }
        // 当前循环所选择的组号
        int select_group = -1;
        // 选择最小的元组
        int min_tuple[2] = {1000, 1000};
        for (int i = 0; i < group_num; i++) {
            if (cur_group_index[i] == 63) {
                finish[i] = 1;
                continue;
            }
            // 初始化
            if (group[i] == NULL) {
                int cur_blk_num = 200 + start + i * 8 + cur_group_index[i] / 8;
                group[i] = readBlockFromDisk(cur_blk_num, &buf);
            }
            // 当前下标对应的块内偏移量
            int offset = cur_group_index[i] % 8;
            if (offset == 7) {
                // 计算下一磁盘块地址
                for (int j = 0; j < 4; j++) {
                    str_1[j] = *(group[i] + offset * 8 + j);
                }
                // 释放原有磁盘块
                freeBlockInBuffer(group[i], &buf);
                // 读入新的磁盘块
                int next_group = 200 + atoi(str_1);
                group[i] = readBlockFromDisk(next_group, &buf);
                cur_group_index[i] += 1;
                offset = cur_group_index[i] % 8;
            }
            // 计算当前指针所指的元组值
            for (int j = 0; j < 4; j++) {
                str_1[j] = *(group[i] + offset * 8 + j);
                str_2[j] = *(group[i] + offset * 8 + 4 + j);
            }
            tuple[i][0] = atoi(str_1);
            tuple[i][1] = atoi(str_2);
            // 找到最小的值
            if (tuple[i][compare_col] <= min_tuple[compare_col]) {
                min_tuple[0] = tuple[i][0];
                min_tuple[1] = tuple[i][1];
                // 更新所选择的组号
                select_group = i;
            }
        }
        // 所有的组都已经判断完毕
        if (select_group == -1) {
            // 所有结果块写回磁盘块
            for (int i = 0; i < 8 - group_num; i++) {
                char str_addr[4];
                // 所有磁盘块已经写完
                if (write_disk_index > (300 + end)) {
                    break;
                }
                // 写入下一个磁盘块地址
                itoa(write_disk_index + 1, str_addr, 10);
                for (int j = 0; j < 4; j++) {
                    *(result_blk[i] + result_num[i] * 8 + j) = str_addr[j];
                }
                writeBlockToDisk(result_blk[i], write_disk_index, &buf);
                printf("结果块%d 写入磁盘下标 %d\n", i, write_disk_index);
                write_disk_index += 1;
            }
        } else {
            // 写入结果块
            itoa(min_tuple[0], str_1, 10);
            itoa(min_tuple[1], str_2, 10);
            // 写入所有磁盘块的数据
            for (int i = 0; i < 8 - group_num; i++) {
                if (result_num[i] < 7) {
                    for (int j = 0; j < 4; j++) {
                        *(result_blk[i] + result_num[i] * 8 + j) = str_1[j];
                        *(result_blk[i] + result_num[i] * 8 + 4 + j) = str_2[j];
                    }
//                    printf("(%d, %d)\t", atoi(str_1), atoi(str_2));
//                    printf("写入结果块%d\t", i);
                    result_num[i] += 1;
                    break;
                }
            }
            // 更新对应的下标
            cur_group_index[select_group] += 1;
//            printf("group %d cur_index: %d\n", select_group, cur_group_index[select_group]);
            // 所有结果块写满写回磁盘块
            if (result_num[8 - group_num - 1] == 7) {
                for (int i = 0; i < 8 - group_num; i++) {
                    // 写入下一磁盘块地址
                    char str_addr[4];
                    itoa(write_disk_index + 1, str_addr, 10);
//                    printf("结果块%d 写入地址%d\n", i, write_disk_index + 1);
                    for (int j = 0; j < 4; j++) {
                        *(result_blk[i] + 7 * 8 + j) = str_addr[j];
                    }
                    // 写回磁盘
                    writeBlockToDisk(result_blk[i], write_disk_index, &buf);
                    printf("结果块%d 写入磁盘下标 %d\n", i, write_disk_index);
                    write_disk_index += 1;
                    // 申请新的磁盘块
                    result_blk[i] = getNewBlockInBuffer(&buf);
                    result_num[i] = 0;
                }
            }
        }
    }
}

/**
 * 为排好序的数据建立索引
 * @param start
 * @param end
 * @param index_col
 */
void create_index(int start, int end, int index_col) {
    // 声明缓冲区并初始化
    Buffer buf;
    initBuffer(520, 64, &buf);

    // 建立数据块
    unsigned char *data_blk;

    // 建立结果块
    int result_blk_num = (end + 1 - start) / 7 + 1;

    unsigned char *result_blks[result_blk_num];
    for (int i = 0; i < result_blk_num; i++) {
        result_blks[i] = getNewBlockInBuffer(&buf);
    }
    int result_num[result_blk_num];
    memset(result_num, 0, sizeof(result_num));

    // 进行排序
    TPMMS(start, end, index_col);

    for (int i = start; i < (end + 1); i++) {
        int sorted_blk_index = 300 + i;
        char str_1[4];
        char str_2[4];
        data_blk = readBlockFromDisk(sorted_blk_index, &buf);
        for (int j = 0; j < 4; j++) {
            str_1[j] = *(data_blk + j);
            str_2[j] = *(data_blk + 4 + j);
        }
//        printf("第%d块索引为(%d, %d)\n", i, atoi(str_1), atoi(str_2));
        // 写入索引数据
        for (int j = 0; j < result_blk_num; j++) {
            if (result_num[j] < 7) {
                for (int k = 0; k < 4; k++) {
                    *(result_blks[j] + result_num[j] * 8 + k) = str_1[k];
                    *(result_blks[j] + result_num[j] * 8 + 4 + k) = str_2[k];
                }
                result_num[j]++;
                break;
            }
        }
        // 刷新指针
        freeBlockInBuffer(data_blk, &buf);
    }

    int index_blk = 400 + start;
    for (int i = 0; i < result_blk_num; i++) {
        char str_addr[4];
        itoa(index_blk + 1, str_addr, 10);
        for (int j = 0; j < 4; j++) {
            *(result_blks[i] + 7 * 8 + j) = str_addr[j];
        }
        // 写入磁盘
        writeBlockToDisk(result_blks[i], index_blk, &buf);
        index_blk += 1;
    }
}

/**
 * 基于索引的关系选择算法
 * @param start 磁盘块起始下标
 * @param end 磁盘块结束下标
 * @param num 所需要选择的数
 * @param index_col 排序的索引列下标
 */
void index_relationship_select(int start, int end, int num, int index_col) {
    // 声明缓冲区并初始化
    Buffer buf;
    initBuffer(520, 64, &buf);

    // 建立索引
    create_index(start, end, index_col);

    // 根据索引读取到的数据块
    int disk_num = 300 + start;
    // 索引块下标
    int index_num = 400 + start;
    unsigned char *data_blk = readBlockFromDisk(400 + start, &buf);
    printf("****************************************************************************************\n");
    printf("完成TPMMS排序\n");
    printf("****************************************************************************************\n");
    printf("所需查找的值为: %d\n", num);

    // 统计符合条件的数值
    int answer_num = 0;

    // 找到磁盘块下标上界
    int upbound_found = 0;
    while (!upbound_found) {
        char str_1[4];
        char str_2[4];
        int tuple[2] = {0};
        for (int i = 0; i < 8; i++) {
            if (i == 7) {
                // 刷新数据块
                freeBlockInBuffer(data_blk, &buf);
                index_num++;
                data_blk = readBlockFromDisk(index_num, &buf);
            } else {
                for (int k = 0; k < 4; k++) {
                    str_1[k] = *(data_blk + i * 8 + k);
                    str_2[k] = *(data_blk + i * 8 + 4 + k);
                }
                tuple[0] = atoi(str_1);
                tuple[1] = atoi(str_2);
                printf("当前索引为(%d, %d)\t", tuple[0], tuple[1]);
                printf("对应磁盘块下标为%d\n", disk_num);
                if (tuple[index_col] > num) {
                    upbound_found = 1;
                    break;
                } else {
                    disk_num++;
                }
            }
        }
    }
    printf("\n");
    printf("找到对应的磁盘块下标上界为: %d\n", disk_num);
    freeBlockInBuffer(data_blk, &buf);

    // 找到磁盘块下标下界
    int lowBound_found = 0;
    while (!lowBound_found) {
        char str_1[4];
        char str_2[4];
        int tuple[2] = {0};
        disk_num--;
        data_blk = readBlockFromDisk(disk_num, &buf);
        for (int k = 0; k < 4; k++) {
            str_1[k] = *(data_blk + k);
            str_2[k] = *(data_blk + 4 + k);
        }
        tuple[0] = atoi(str_1);
        tuple[1] = atoi(str_2);
        if (tuple[index_col] < num) {
            lowBound_found = 1;
        }
    }
    printf("找到对应的磁盘块下标下界为: %d\n", disk_num);
    freeBlockInBuffer(data_blk, &buf);

    // 找到所有符合条件的值
    int all_found = 0;
    unsigned char *result_blk = getNewBlockInBuffer(&buf);
    int result_num = 0;
    int write_disk = 500 + start;

    while (!all_found) {
        data_blk = readBlockFromDisk(disk_num, &buf);
        printf("\n当前读取块号为%d\n", disk_num);
        int tuple[2];
        for (int i = 0; i < 7; i++) {
            char str_1[4] = {0};
            char str_2[4] = {0};
            tuple[0] = 0;
            tuple[1] = 0;
            for (int k = 0; k < 4; k++) {
                str_1[k] = *(data_blk + i * 8 + k);
                str_2[k] = *(data_blk + i * 8 + 4 + k);
            }
            tuple[0] = atoi(str_1);
            tuple[1] = atoi(str_2);
//            printf("(%d, %d)\t", tuple[0], tuple[1]);
            if (tuple[index_col] == num) {
                printf("找到符合条件的值 (%d, %d)\n", tuple[0], tuple[1]);
                answer_num++;
                if (result_num == 7) {
                    // 写入下一块地址
                    char str_addr[4];
                    itoa(write_disk + 1, str_addr, 10);
                    for (int k = 0; k < 4; k++) {
                        *(result_blk + result_num * 8 + k) = str_addr[k];
                    }
                    // 写回磁盘块
                    writeBlockToDisk(result_blk, write_disk, &buf);
                    write_disk++;
                    // 更新计数器
                    result_num = 0;
                }
                for (int k = 0; k < 4; k++) {
                    *(result_blk + result_num * 8 + k) = str_1[k];
                    *(result_blk + result_num * 8 + 4 + k) = str_2[k];
                }
                result_num++;
            } else if (tuple[index_col] > num) {
                all_found = 1;
                break;
            }
        }
        disk_num++;
    }
    // 清除最后一块原有的数据
    for (int i = result_num; i < 8; i++) {
        if (i == 7) {
            // 写入下一块地址
            char str_addr[4];
            itoa(write_disk + 1, str_addr, 10);
            for (int k = 0; k < 4; k++) {
                *(result_blk + 7 * 8 + k) = str_addr[k];
            }
        } else {
            for (int k = 0; k < 4; k++) {
                *(result_blk + result_num * 8 + k) = 0;
                *(result_blk + result_num * 8 + 4 + k) = 0;
            }
            result_num++;
        }
    }
    // 写回数据块
    writeBlockToDisk(result_blk, write_disk, &buf);
    freeBlockInBuffer(result_blk, &buf);
    printf("共有 %d 个符合条件的值\n", answer_num);
    printf("IO num: %d\n", buf.numIO);
}

void test_324() {
    // 声明缓冲区并初始化
    Buffer buf;
    initBuffer(520, 64, &buf);

    unsigned char *data_blk;
    data_blk = readBlockFromDisk(324, &buf);

    for (int i = 0; i < 8; i++) {
        char str_1[4];
        char str_2[4];
        int tuple[2];
        for (int k = 0; k < 4; k++) {
            str_1[k] = *(data_blk + i * 8 + k);
            str_2[k] = *(data_blk + i * 8 + 4 + k);
        }
        tuple[0] = atoi(str_1);
        tuple[1] = atoi(str_2);
        printf("(%d, %d)\t", tuple[0], tuple[1]);
    }
}

void read_from_blk(unsigned char *blk, int offset, char *str_1, char *str_2) {
    for (int k = 0; k < 4; k++) {
        str_1[k] = *(blk + offset * 8 + k);
        str_2[k] = *(blk + offset * 8 + 4 + k);
    }
}

/**
 * 基于排序的连接操作
 * @param start_1 关系1的起始下标
 * @param end_1 关系1的结束下标
 * @param select_col_1 关系1的排序索引列下标
 * @param start_2 关系2的起始下标
 * @param end_2 关系2的结束下标
 * @param select_col_2 关系2的排序索引列下标
 * @param write_base 写入磁盘块的基底
 */
void
sort_merge_join(int start_1, int end_1, int select_col_1, int start_2, int end_2, int select_col_2, int write_base) {
    Buffer buf;
    initBuffer(520, 64, &buf);

    // 对原始数据进行内排序
    printf("****************************************************************************************\n");
    sort_blk_data(start_1, end_1, select_col_1, write_base);
    sort_blk_data(start_2, end_2, select_col_2, write_base);
    printf("分别对关系R和关系S完成内排序\n");
    printf("****************************************************************************************\n");

    // 计算每个属性的组数
    int group_1_num = (end_1 + 1 - start_1) / 8;
    int group_2_num = (end_2 + 1 - start_2) / 8;

    // 指向每个组的指针
    unsigned char *group_1[group_1_num];
    unsigned char *group_2[group_2_num];
    memset(group_1, 0, sizeof group_1);
    memset(group_2, 0, sizeof group_2);

    // 指针指向对应的磁盘块
    for (int i = 0; i < group_1_num; i++) {
        int addr = write_base + start_1 + i * 8;
        group_1[i] = readBlockFromDisk(addr, &buf);
    }
    for (int i = 0; i < group_2_num; i++) {
        int addr = write_base + start_2 + i * 8;
        group_2[i] = readBlockFromDisk(addr, &buf);
    }

    // 定义每个组当前读取到的数据下标
    int cur_index_1[group_1_num];
    int cur_index_2[group_2_num];
    memset(cur_index_1, 0, sizeof cur_index_1);
    memset(cur_index_2, 0, sizeof cur_index_2);

    // 定义第二组当前读取到的磁盘下标
    int cur_blk_2[group_2_num];
    for (int i = 0; i < group_2_num; i++) {
        cur_blk_2[i] = write_base + start_2 + i * 8;
    }

    // 读取完毕的标记变量
    int finish_1[group_1_num];
    int finish_2[group_2_num];
    memset(finish_1, 0, group_1_num);
    memset(finish_2, 0, group_2_num);

    // 写回磁盘块的首下标
    int write_disk = write_base + 100;
    // 声明结果块
    unsigned char *result_blk[8 - group_1_num - group_2_num];
    for (int i = 0; i < (8 - group_1_num - group_2_num); i++) {
        result_blk[i] = getNewBlockInBuffer(&buf);
    }
    int result_num[8 - group_1_num - group_2_num];
    memset(result_num, 0, sizeof result_num);

    int join_num = 0;
    // 连接操作
    while (1) {
//        printf("*******************************\n");
        /*
         * 找到关系R当前两个指针所指的较小的值
         */
        int min_tuple[2] = {1000, 1000};
        int select_group = -1;
//        printf("选择关系R元组\n");
//        printf("当前选择的两组数据分别为: ");
        for (int group_index = 0; group_index < group_1_num; group_index++) {
            if (cur_index_1[group_index] == 63) {
//                printf("关系R元组的第 %d 组判断完毕\n", group_index);
                finish_1[group_index] = 1;
                continue;
            }
            // 计算当前下标所对应的块内偏移量
            int offset = cur_index_1[group_index] % 8;
            // 更新当前块指针
            if (offset == 7) {
                // 计算下一块下标
                char str_addr[4];
                for (int k = 0; k < 4; k++) {
                    str_addr[k] = *(group_1[group_index] + offset * 8 + k);
                }
                int next_disk = write_base + atoi(str_addr);
                // 释放原有磁盘块
                freeBlockInBuffer(group_1[group_index], &buf);
                // 读取新的磁盘块
                group_1[group_index] = readBlockFromDisk(next_disk, &buf);
                cur_index_1[group_index]++;
                // 更新offset
                offset = cur_index_1[group_index] % 8;
            }
            // 记录数据
            char str_1[4];
            char str_2[4];
            for (int k = 0; k < 4; k++) {
                str_1[k] = *(group_1[group_index] + offset * 8 + k);
                str_2[k] = *(group_1[group_index] + offset * 8 + 4 + k);
            }
            int cur_tuple[2] = {atoi(str_1), atoi(str_2)};
//            printf("(%d, %d)\t", cur_tuple[0], cur_tuple[1]);
            // 数据更小则更新
            if (cur_tuple[select_col_1] <= min_tuple[select_col_1]) {
                min_tuple[0] = cur_tuple[0];
                min_tuple[1] = cur_tuple[1];
                select_group = group_index;
            }
        }
//        printf("\n");
//        printf("最终选择数据为: (%d, %d)\n", min_tuple[0], min_tuple[1]);
//        printf("两组当前下标分别为:\n");
//        printf("group 0: %d, group 1: %d\n", cur_index_1[0], cur_index_1[1]);
        // 更新下标
        cur_index_1[select_group]++;

        /*
         * 判断关系R是否完全读取完毕
         */
        int finish_num = 0;
        while (finish_num < group_1_num && (finish_1[finish_num] == 1)) {
            finish_num++;
        }
        if (finish_num == group_1_num) {
            printf("\n关系R的元组读取完毕\n");
            printf("\n");
            break;
        }

        /*
         * 找到关系S中与关系R选出的值相等的元组
         */
//        printf("选择关系S元组\n");
        int last_group_2_index[group_2_num];
        int last_blk_2[group_2_num];

        for (int i = 0; i < group_2_num; i++) {
            last_group_2_index[i] = cur_index_2[i];
        }
        for (int i = 0; i < group_2_num; i++) {
            last_blk_2[i] = cur_blk_2[i];
        }

        int equal_num = 0;
        for (int group_index = 0; group_index < group_2_num; group_index++) {
            // 如果当前组已经读取完毕
            if (cur_index_2[group_index] == 63) {
                finish_2[group_index] = 1;
                continue;
            }
            // 当前组所有符合条件元素都已经选择完毕的标记变量
            int cur_group_all_found = 0;
            while (!cur_group_all_found) {
                // 计算当前下标所对应的块内偏移量
                int offset = cur_index_2[group_index] % 8;
                // 更新当前块指针
                if (offset == 7) {
                    char str_addr[4];
                    for (int k = 0; k < 4; k++) {
                        str_addr[k] = *(group_2[group_index] + offset * 8 + k);
                    }
                    int next_disk = write_base + atoi(str_addr);
                    // 释放原有磁盘块
                    freeBlockInBuffer(group_2[group_index], &buf);
                    // 读取新的磁盘块
                    group_2[group_index] = readBlockFromDisk(next_disk, &buf);
                    cur_index_2[group_index]++;
                    // 更新offset
                    offset = cur_index_2[group_index] % 8;
                    // 更新磁盘块下标
                    cur_blk_2[group_index] = write_base + start_2 + group_index * 8 + cur_index_2[group_index] / 8;
                }
                // 记录数据
                char str_1[4];
                char str_2[4];
                read_from_blk(group_2[group_index], offset, str_1, str_2);
                int cur_tuple[2] = {atoi(str_1), atoi(str_2)};
//                printf("当前组号为: %d\t 块号为: %d\t 偏移量为: %d\n", group_index, cur_blk_2[group_index], cur_index_2[group_index]);
//                printf("当前数据为: (%d, %d)\n", cur_tuple[0], cur_tuple[1]);
                // 判断结果块是否写满的标记变量
                int all_full = 0;
                // 找到小于的元素则继续往后移
                if (cur_tuple[select_col_2] < min_tuple[select_col_1]) {
//                    printf("当前选择数据为(%d, %d)\t 小于所选数据\n", cur_tuple[0], cur_tuple[1]);
                    // 移动当前指针对应下标
                    cur_index_2[group_index]++;
                    // 移动所需要恢复的下标位置
                    last_group_2_index[group_index]++;
                    // 移动需要恢复的磁盘块号
                    if (last_group_2_index[group_index] % 8 == 0) {
                        last_blk_2[group_index]++;
                    }
                }
                    // 找到相等的元素则写入结果块
                else if (cur_tuple[select_col_2] == min_tuple[select_col_1]) {
                    equal_num++;
                    join_num++;
//                    printf("当前选择数据为(%d, %d)\t 匹配成功\n", cur_tuple[0], cur_tuple[1]);
//                    printf("当前连接数为: %d\n", join_num);
                    char str_A[4];
                    char str_B[4];
                    char str_C[4];
                    char str_D[4];
                    itoa(min_tuple[0], str_A, 10);
                    itoa(min_tuple[1], str_B, 10);
                    itoa(cur_tuple[0], str_C, 10);
                    itoa(cur_tuple[1], str_D, 10);
                    for (int j = 0; j < 8 - group_1_num - group_2_num; j++) {
                        // 所有结果块都已经写满
                        if (result_num[j] == 6 && j == 7 - group_1_num - group_2_num) {
                            for (int k = 0; k < (8 - group_1_num - group_2_num); k++) {
                                // 写回当前磁盘块
                                char str_addr[4];
                                itoa(write_disk + 1, str_addr, 10);
                                for (int m = 0; m < 4; m++) {
                                    *(result_blk[k] + 7 * 8 + m) = str_addr[m];
                                }
                                writeBlockToDisk(result_blk[k], write_disk, &buf);
                                printf("结果块%d 写入磁盘下标 %d\n", k, write_disk);
                                write_disk++;
                                // 申请新的磁盘块
                                result_blk[k] = getNewBlockInBuffer(&buf);
                                result_num[k] = 0;
                            }
                        }
                            // 当前块写满
                        else if (result_num[j] == 6 && j < (7 - group_1_num - group_2_num)) {
                            continue;
                        }
                        for (int k = 0; k < 4; k++) {
                            *(result_blk[j] + result_num[j] * 8 + k) = str_C[k];
                            *(result_blk[j] + result_num[j] * 8 + 4 + k) = str_D[k];
                            *(result_blk[j] + result_num[j] * 8 + 8 + k) = str_A[k];
                            *(result_blk[j] + result_num[j] * 8 + 12 + k) = str_B[k];
                        }
                        result_num[j] += 2;
                        break;
                    }
                    cur_index_2[group_index]++;
                }
                    // 如果当前所指元素大于选定的元素则开始判断下一组
                else if (cur_tuple[select_col_2] > min_tuple[select_col_1]) {
//                    printf("当前元组数据大于所选数据，第 %d 组判断完毕\n", group_index);
                    cur_group_all_found = 1;
                }
            }
        }
//        printf("每个组需要恢复到的磁盘块号为: ");
        for (int i = 0; i < group_2_num; i++) {
//            printf("%d\t", last_blk_2[i]);
        }
//        printf("\n");
        // 还原每个指针读取的磁盘块下标
        for (int i = 0; i < group_2_num; i++) {
            cur_blk_2[i] = last_blk_2[i];
            freeBlockInBuffer(group_2[i], &buf);
            group_2[i] = readBlockFromDisk(cur_blk_2[i], &buf);
        }
//        printf("每个组需要恢复到的下标标号为: ");
        for (int i = 0; i < group_2_num; i++) {
//            printf("%d\t", last_group_2_index[i]);
        }
//        printf("\n");
        // 还原关系S对应组的下标
        for (int i = 0; i < group_2_num; i++) {
            cur_index_2[i] = last_group_2_index[i];
        }
//        printf("当前数据(%d, %d) 连接总数为: %d\n", min_tuple[0], min_tuple[1], equal_num);
//        printf("****************************");
    }
    // 最后数据未写满时也要写入结果
    int result_blk_num = join_num / 3;
    int last_blk_num = join_num % 3;
    for (int k = 0; k < (8 - group_1_num - group_2_num); k++) {
        // 最后一块
        if (write_disk == write_base + 100 + result_blk_num) {
            // 没写满需要删去部分内容
            for (int i = 2 * last_blk_num; i < 7; i++) {
                for (int m = 0; m < 4; m++) {
                    *(result_blk[k] + i * 8 + m) = 0;
                    *(result_blk[k] + i * 8 + 4 + m) = 0;
                }
            }
        }
            // 多余的块不写
        else if (write_disk > write_base + 100 + result_blk_num) {
            continue;
        }
        // 写回当前磁盘块
        char str_addr[4];
        itoa(write_disk + 1, str_addr, 10);
        for (int m = 0; m < 4; m++) {
            *(result_blk[k] + 7 * 8 + m) = str_addr[m];
        }
        writeBlockToDisk(result_blk[k], write_disk, &buf);
        printf("结果块%d 写入磁盘下标 %d\n", k, write_disk);
        write_disk++;
        // 申请新的磁盘块
        result_blk[k] = getNewBlockInBuffer(&buf);
        result_num[k] = 0;
    }
    printf("连接总数为: %d\n", join_num);
}

/**
 * 基于排序的两趟扫描算法实现交操作
 * @param start_1 关系1的起始下标
 * @param end_1 关系1的结束下标
 * @param select_col_1 关系1的排序索引列下标
 * @param start_2 关系2的起始下标
 * @param end_2 关系2的结束下标
 * @param select_col_2 关系2的排序索引列下标
 * @param write_base 写入磁盘块的基底
 */
void intersect(int start_1, int end_1, int select_col_1, int start_2, int end_2, int select_col_2, int write_base) {
    Buffer buf;
    initBuffer(520, 64, &buf);

    // 对原始数据进行内排序
    printf("****************************************************************************************\n");
    sort_blk_data(start_1, end_1, select_col_1, write_base);
    sort_blk_data(start_2, end_2, select_col_2, write_base);
    printf("分别对关系R和关系S完成内排序\n");
    printf("****************************************************************************************\n");

    // 计算每个属性的组数
    int group_1_num = (end_1 + 1 - start_1) / 8;
    int group_2_num = (end_2 + 1 - start_2) / 8;

    // 指向每个组的指针
    unsigned char *group_1[group_1_num];
    unsigned char *group_2[group_2_num];
    memset(group_1, 0, sizeof group_1);
    memset(group_2, 0, sizeof group_2);

    // 指针指向对应的磁盘块
    for (int i = 0; i < group_1_num; i++) {
        int addr = write_base + start_1 + i * 8;
        group_1[i] = readBlockFromDisk(addr, &buf);
    }
    for (int i = 0; i < group_2_num; i++) {
        int addr = write_base + start_2 + i * 8;
        group_2[i] = readBlockFromDisk(addr, &buf);
    }

    // 定义每个组当前读取到的数据下标
    int cur_index_1[group_1_num];
    int cur_index_2[group_2_num];
    memset(cur_index_1, 0, sizeof cur_index_1);
    memset(cur_index_2, 0, sizeof cur_index_2);

    // 定义第二组当前读取到的磁盘下标
    int cur_blk_2[group_2_num];
    for (int i = 0; i < group_2_num; i++) {
        cur_blk_2[i] = write_base + start_2 + i * 8;
    }

    // 读取完毕的标记变量
    int finish_1[group_1_num];
    int finish_2[group_2_num];
    memset(finish_1, 0, group_1_num);
    memset(finish_2, 0, group_2_num);

    // 写回磁盘块的首下标
    int write_disk = write_base + 100;
    // 声明结果块
    unsigned char *result_blk[8 - group_1_num - group_2_num];
    for (int i = 0; i < (8 - group_1_num - group_2_num); i++) {
        result_blk[i] = getNewBlockInBuffer(&buf);
    }
    int result_num[8 - group_1_num - group_2_num];
    memset(result_num, 0, sizeof result_num);

    int join_num = 0;
    // 连接操作
    while (1) {
//        printf("*******************************\n");
        /*
         * 找到关系R当前两个指针所指的较小的值
         */
        int min_tuple[2] = {1000, 1000};
        int select_group = -1;
//        printf("选择关系R元组\n");
//        printf("当前选择的两组数据分别为: ");
        for (int group_index = 0; group_index < group_1_num; group_index++) {
            if (cur_index_1[group_index] == 63) {
//                printf("关系R元组的第 %d 组判断完毕\n", group_index);
                finish_1[group_index] = 1;
                continue;
            }
            // 计算当前下标所对应的块内偏移量
            int offset = cur_index_1[group_index] % 8;
            // 更新当前块指针
            if (offset == 7) {
                // 计算下一块下标
                char str_addr[4];
                for (int k = 0; k < 4; k++) {
                    str_addr[k] = *(group_1[group_index] + offset * 8 + k);
                }
                int next_disk = write_base + atoi(str_addr);
                // 释放原有磁盘块
                freeBlockInBuffer(group_1[group_index], &buf);
                // 读取新的磁盘块
                group_1[group_index] = readBlockFromDisk(next_disk, &buf);
                cur_index_1[group_index]++;
                // 更新offset
                offset = cur_index_1[group_index] % 8;
            }
            // 记录数据
            char str_1[4];
            char str_2[4];
            for (int k = 0; k < 4; k++) {
                str_1[k] = *(group_1[group_index] + offset * 8 + k);
                str_2[k] = *(group_1[group_index] + offset * 8 + 4 + k);
            }
            int cur_tuple[2] = {atoi(str_1), atoi(str_2)};
//            printf("(%d, %d)\t", cur_tuple[0], cur_tuple[1]);
            // 数据更小则更新
            if (cur_tuple[select_col_1] <= min_tuple[select_col_1]) {
                min_tuple[0] = cur_tuple[0];
                min_tuple[1] = cur_tuple[1];
                select_group = group_index;
            }
        }
//        printf("\n");
//        printf("最终选择数据为: (%d, %d)\n", min_tuple[0], min_tuple[1]);
//        printf("两组当前下标分别为:\n");
//        printf("group 0: %d, group 1: %d\n", cur_index_1[0], cur_index_1[1]);
        // 更新下标
        cur_index_1[select_group]++;

        /*
         * 判断关系R是否完全读取完毕
         */
        int finish_num = 0;
        while (finish_num < group_1_num && (finish_1[finish_num] == 1)) {
            finish_num++;
        }
        if (finish_num == group_1_num) {
            break;
        }

        /*
         * 找到关系S中与关系R选出的值相等的元组
         */
//        printf("选择关系S元组\n");
        int last_group_2_index[group_2_num];
        int last_blk_2[group_2_num];

        for (int i = 0; i < group_2_num; i++) {
            last_group_2_index[i] = cur_index_2[i];
        }
        for (int i = 0; i < group_2_num; i++) {
            last_blk_2[i] = cur_blk_2[i];
        }

        int equal_num = 0;
        for (int group_index = 0; group_index < group_2_num; group_index++) {
            // 如果当前组已经读取完毕
            if (cur_index_2[group_index] == 63) {
                finish_2[group_index] = 1;
                continue;
            }
            // 当前组所有符合条件元素都已经选择完毕的标记变量
            int cur_group_all_found = 0;
            while (!cur_group_all_found) {
                // 计算当前下标所对应的块内偏移量
                int offset = cur_index_2[group_index] % 8;
                // 更新当前块指针
                if (offset == 7) {
                    char str_addr[4];
                    for (int k = 0; k < 4; k++) {
                        str_addr[k] = *(group_2[group_index] + offset * 8 + k);
                    }
                    int next_disk = write_base + atoi(str_addr);
                    // 释放原有磁盘块
                    freeBlockInBuffer(group_2[group_index], &buf);
                    // 读取新的磁盘块
                    group_2[group_index] = readBlockFromDisk(next_disk, &buf);
                    cur_index_2[group_index]++;
                    // 更新offset
                    offset = cur_index_2[group_index] % 8;
                    // 更新磁盘块下标
                    cur_blk_2[group_index] = write_base + start_2 + group_index * 8 + cur_index_2[group_index] / 8;
                }
                // 记录数据
                char str_1[4];
                char str_2[4];
                read_from_blk(group_2[group_index], offset, str_1, str_2);
                int cur_tuple[2] = {atoi(str_1), atoi(str_2)};
//                printf("当前组号为: %d\t 块号为: %d\t 偏移量为: %d\n", group_index, cur_blk_2[group_index], cur_index_2[group_index]);
//                printf("当前数据为: (%d, %d)\n", cur_tuple[0], cur_tuple[1]);
                // 判断结果块是否写满的标记变量
                int all_full = 0;
                // 找到小于的元素则继续往后移
                if (cur_tuple[select_col_2] < min_tuple[select_col_1]) {
//                    printf("当前选择数据为(%d, %d)\t 小于所选数据\n", cur_tuple[0], cur_tuple[1]);
                    // 移动当前指针对应下标
                    cur_index_2[group_index]++;
                    // 移动所需要恢复的下标位置
                    last_group_2_index[group_index]++;
                    // 移动需要恢复的磁盘块号
                    if (last_group_2_index[group_index] % 8 == 0) {
                        last_blk_2[group_index]++;
                    }
                }
                    // 找到相等的元素则写入结果块
                else if (cur_tuple[select_col_2] == min_tuple[select_col_1]) {
                    int other_col_1 = 1 - select_col_1;
                    int other_col_2 = 1 - select_col_2;
                    if (cur_tuple[other_col_2] == min_tuple[other_col_1]) {
                        equal_num++;
                        join_num++;
//                        printf("当前选择数据为(%d, %d)\t 匹配成功\n", cur_tuple[0], cur_tuple[1]);
//                        printf("当前连接数为: %d\n", join_num);
                        char str_A[4];
                        char str_B[4];
                        char str_C[4];
                        char str_D[4];
                        itoa(min_tuple[0], str_A, 10);
                        itoa(min_tuple[1], str_B, 10);
                        itoa(cur_tuple[0], str_C, 10);
                        itoa(cur_tuple[1], str_D, 10);
                        for (int j = 0; j < 8 - group_1_num - group_2_num; j++) {
                            // 所有结果块都已经写满
                            if (result_num[j] == 6 && j == 7 - group_1_num - group_2_num) {
                                for (int k = 0; k < (8 - group_1_num - group_2_num); k++) {
                                    // 写回当前磁盘块
                                    char str_addr[4];
                                    itoa(write_disk + 1, str_addr, 10);
                                    for (int m = 0; m < 4; m++) {
                                        *(result_blk[k] + 7 * 8 + m) = str_addr[m];
                                    }
                                    writeBlockToDisk(result_blk[k], write_disk, &buf);
                                    printf("结果块%d 写入磁盘下标 %d\n", k, write_disk);
                                    write_disk++;
                                    // 申请新的磁盘块
                                    result_blk[k] = getNewBlockInBuffer(&buf);
                                    result_num[k] = 0;
                                }
                            }
                                // 当前块写满
                            else if (result_num[j] == 6 && j < (7 - group_1_num - group_2_num)) {
                                continue;
                            }
                            for (int k = 0; k < 4; k++) {
                                *(result_blk[j] + result_num[j] * 8 + k) = str_C[k];
                                *(result_blk[j] + result_num[j] * 8 + 4 + k) = str_D[k];
                                *(result_blk[j] + result_num[j] * 8 + 8 + k) = str_A[k];
                                *(result_blk[j] + result_num[j] * 8 + 12 + k) = str_B[k];
                            }
                            result_num[j] += 2;
                            break;
                        }
                    }
                    cur_index_2[group_index]++;
                }
                    // 如果当前所指元素大于选定的元素则开始判断下一组
                else if (cur_tuple[select_col_2] > min_tuple[select_col_1]) {
//                    printf("当前元组数据大于所选数据，第 %d 组判断完毕\n", group_index);
                    cur_group_all_found = 1;
                }
            }
        }
//        printf("每个组需要恢复到的磁盘块号为: ");
//        for (int i = 0; i < group_2_num; i++) {
//            printf("%d\t", last_blk_2[i]);
//        }
//        printf("\n");
        // 还原每个指针读取的磁盘块下标
        for (int i = 0; i < group_2_num; i++) {
            cur_blk_2[i] = last_blk_2[i];
            freeBlockInBuffer(group_2[i], &buf);
            group_2[i] = readBlockFromDisk(cur_blk_2[i], &buf);
        }
//        printf("每个组需要恢复到的下标标号为: ");
//        for (int i = 0; i < group_2_num; i++) {
//            printf("%d\t", last_group_2_index[i]);
//        }
//        printf("\n");
        // 还原关系S对应组的下标
        for (int i = 0; i < group_2_num; i++) {
            cur_index_2[i] = last_group_2_index[i];
        }
//        printf("当前数据(%d, %d) 交总数为: %d\n", min_tuple[0], min_tuple[1], equal_num);
//        printf("****************************");
    }
    // 最后数据未写满时也要写入结果
    int result_blk_num = join_num / 3;
    int last_blk_num = join_num % 3;
    for (int k = 0; k < (8 - group_1_num - group_2_num); k++) {
        // 最后一块
        if (write_disk == write_base + 100 + result_blk_num) {
            // 没写满需要删去部分内容
            for (int i = 2 * last_blk_num; i < 7; i++) {
                for (int m = 0; m < 4; m++) {
                    *(result_blk[k] + i * 8 + m) = 0;
                    *(result_blk[k] + i * 8 + 4 + m) = 0;
                }
            }
        }
            // 多余的块不写
        else if (write_disk > write_base + 100 + result_blk_num) {
            continue;
        }
        // 写回当前磁盘块
        char str_addr[4];
        itoa(write_disk + 1, str_addr, 10);
        for (int m = 0; m < 4; m++) {
            *(result_blk[k] + 7 * 8 + m) = str_addr[m];
        }
        writeBlockToDisk(result_blk[k], write_disk, &buf);
        printf("结果块%d 写入磁盘下标 %d\n", k, write_disk);
        write_disk++;
        // 申请新的磁盘块
        result_blk[k] = getNewBlockInBuffer(&buf);
        result_num[k] = 0;
    }
    printf("交总数为: %d\n", join_num);
}

