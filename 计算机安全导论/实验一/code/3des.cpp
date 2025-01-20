#include <iostream>
#include <bitset>
#include <string>
#include <random>
#include <fstream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "windows.h"

using namespace std;

// 初始置换（IP）数组
int IP[] = {
    58, 50, 42, 34, 26, 18, 10, 2,
    60, 52, 44, 36, 28, 20, 12, 4,
    62, 54, 46, 38, 30, 22, 14, 6,
    64, 56, 48, 40, 32, 24, 16, 8,
    57, 49, 41, 33, 25, 17, 9, 1,
    59, 51, 43, 35, 27, 19, 11, 3,
    61, 53, 45, 37, 29, 21, 13, 5,
    63, 55, 47, 39, 31, 23, 15, 7 };

// 逆初始置换（IP^-1）数组
int IP_1[] = {
    40, 8, 48, 16, 56, 24, 64, 32,
    39, 7, 47, 15, 55, 23, 63, 31,
    38, 6, 46, 14, 54, 22, 62, 30,
    37, 5, 45, 13, 53, 21, 61, 29,
    36, 4, 44, 12, 52, 20, 60, 28,
    35, 3, 43, 11, 51, 19, 59, 27,
    34, 2, 42, 10, 50, 18, 58, 26,
    33, 1, 41, 9, 49, 17, 57, 25 };

// PC-1
int PC1[] = {57,49,41,33,25,17,9,
            1,58,50,42,34,26,18,
            10,2,59,51,43,35,27,
            19,11,3,60,52,44,36,
            63,55,47,39,31,23,15,
            7,62,54,46,38,30,22,
            14,6,61,53,45,37,29,
            21,13,5,28,20,12,4};

// PC-2
int PC2[] = {14,17,11,24,1,5,3,28,
            15,6,21,10,23,19,12,4,
            26,8,16,7,27,20,13,2,
            41,52,31,37,47,55,30,40,
            51,45,33,48,44,49,39,56,
            34,53,46,42,50,36,29,32};

// 左循环移位数组
int shiftbit[] = { 1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1 };

// 扩展矩阵
int E[] = {
    32, 1, 2, 3, 4, 5,
    4, 5, 6, 7, 8, 9,
    8, 9, 10, 11, 12, 13,
    12, 13, 14, 15, 16, 17,
    16, 17, 18, 19, 20, 21,
    20, 21, 22, 23, 24, 25,
    24, 25, 26, 27, 28, 29,
    28, 29, 30, 31, 32, 1 };

// S盒
int S[8][4][16] ={
    {  // S1
        {14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7},
        {0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8},
        {4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0},
        {15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13}
    },
    {  //S2
        {15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10},
        {3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5},
        {0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15},
        {13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9}
    },
    {  // S3
        {10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8},
        {13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1},
        {13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7},
        {1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12}
    },
    {  // S4
        {7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15},
        {13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9},
        {10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4},
        {3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14}
    },
    {  // S5
        {2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9},
        {14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6},
        {4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14},
        {11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3}
    },
    {  // S6
        {12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11},
        {10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8},
        {9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6},
        {4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13}
    },
    {  // S7
        {4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1},
        {13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6},
        {1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2},
        {6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12}
    },
    {   // S8
        {13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7},
        {1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2},
        {7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8},
        {2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11}
    }
};

// 置换操作P
int P[] = {16,7,20,21,29,12,28,17,
        1,15,23,26,5,18,31,10,
        2,8,24,14,32,27,3,9,
        19,13,30,6,22,11,4,25};

// 左循环移位操作
template <size_t N>
bitset<N> rotateLeft(const bitset<N>& bits, int positions) {
    bitset<N> result;
    positions %= N; // 确保移位数在有效范围内

    // 循环左移
    for (size_t i = 0; i < N; ++i) {
        result[i] = bits[(i + positions) % N];
    }

    return result;
}

// 生成子密钥，key为密钥，subkey装有16个子密钥
void subkeyGeneration(bitset<64> key, bitset<48> subkey[16]){
    bitset<56> keyAfterReplace; // 用于存储经过初始置换的密钥
    bitset<28> C0; // 左半部分
    bitset<28> D0; // 右半部分
    bitset<48> realSubKey; // 当前生成的子密钥

    // 根据PC-1进行初始置换，将64位密钥转换为56位
    for(int i = 0; i < 56; ++i) 
        keyAfterReplace[i] = key[PC1[i]-1];

    // 生成16个子密钥
    for(int i = 0; i < 16; ++i){
        // 将当前的56位密钥分为两个28位部分C和D
        for(int j = 0; j < 28; ++j) 
            C0[j] = keyAfterReplace[j];
        for(int j = 28; j < 56; ++j) 
            D0[j % 28] = keyAfterReplace[j];

        // 进行左循环移位
        C0 = rotateLeft(C0, shiftbit[i]);
        D0 = rotateLeft(D0, shiftbit[i]);

        // 合并C和D，更新keyAfterReplace
        for(int j = 0; j < 28; ++j) 
            keyAfterReplace[j] = C0[j];
        for(int j = 28; j < 56; ++j) 
            keyAfterReplace[j] = D0[j % 28];

        // 根据PC-2生成当前的48位子密钥
        for(int j = 0; j < 48; ++j) 
            realSubKey[j] = keyAfterReplace[PC2[j]-1];

        // 将生成的子密钥存储到subkey数组中
        subkey[i] = realSubKey;
    }
}

// 轮函数
bitset<32> F(bitset<32> bits, bitset<48> subKey){
    bitset<48> bitsAfterExtend; // 用于存储扩展后的48位数据
    bitset<32> bitsAfterCompress; // 用于存储压缩后的32位数据

    // 扩展运算：将32位输入扩展为48位
    for(int i = 0; i < 48; ++i) 
        bitsAfterExtend[i] = bits[E[i]-1];

    // 和子密钥进行异或运算
    bitsAfterExtend = bitsAfterExtend ^ subKey;

    // 压缩运算：使用S盒将48位数据压缩回32位
    for(int i = 0; i < 48; i += 6){
        // 计算行和列，用于查找S盒的值
        int row = bitsAfterExtend[i + 5] * 1 + bitsAfterExtend[i] * 2;
        int col = bitsAfterExtend[i + 4] * 1 + bitsAfterExtend[i + 3] * 2 + bitsAfterExtend[i + 2] * 4 + bitsAfterExtend[i + 1] * 8;
        int val = S[i / 6][row][col]; // 从S盒获取值
        bitset<4> binary(val); // 将值转换为4位二进制

        // 将4位二进制值存入压缩后的结果中
        bitsAfterCompress[i * 6] = binary[0]; 
        bitsAfterCompress[i * 6 + 1] = binary[1];
        bitsAfterCompress[i * 6 + 2] = binary[2]; 
        bitsAfterCompress[i * 6 + 3] = binary[3];
    }

    // 置换运算：根据P盒将32位数据重新排列
    for(int i = 0; i < 32; ++i) 
        bits[i] = bitsAfterCompress[P[i]-1];

    return bits; // 返回经过轮函数处理后的32位结果
}


// DES加密
bitset<64> encryptionDES(bitset<64> plain, bitset<64> key){
    bitset<48> subkey[16]; // 存储16个子密钥
    bitset<64> plainAfterDisplace; // 用于存储初始置换后的明文
    bitset<32> L; // 明文的左半部分
    bitset<32> R; // 明文的右半部分
    bitset<32> tmp; // 用于交换左右部分
    bitset<64> res; // 最终加密结果

    // 生成子密钥
    subkeyGeneration(key, subkey);

    // 初始置换：根据IP对明文进行初始置换
    for(int i = 0; i < 64; ++i) 
        plainAfterDisplace[i] = plain[IP[i]-1];
    // 分割为左右部分
    for(int i = 0; i < 32; ++i) 
        L[i] = plainAfterDisplace[i];
    for(int i = 32; i < 64; ++i) 
        R[i % 32] = plainAfterDisplace[i];

    // 16轮加密
    for(int i = 0; i < 16; ++i){
        tmp = R; // 暂存右半部分
        // 更新右半部分为L与F函数的异或，更新左半部分为之前的右半部分
        R = L ^ F(R, subkey[i]);
        L = tmp;
    }

    // 左右交换
    for(int i = 0; i < 32; ++i) 
        res[i] = R[i]; // 将R部分放到结果的前半部分
    for(int i = 32; i < 64; ++i) 
        res[i] = L[i % 32]; // 将L部分放到结果的后半部分

    // 逆初始置换：根据IP-1对结果进行逆置换
    plainAfterDisplace = res;
    for(int i = 0; i < 64; ++i) 
        res[i] = plainAfterDisplace[IP_1[i]-1];

    return res; // 返回加密后的密文
}


// DES解密
bitset<64> decryptDES(bitset<64> ciphertext, bitset<64> key){
    bitset<48> subkey[16]; // 存储16个子密钥
    bitset<64> plainAfterDisplace; // 用于存储初始置换后的密文
    bitset<32> L; // 密文的左半部分
    bitset<32> R; // 密文的右半部分
    bitset<32> tmp; // 用于交换左右部分
    bitset<64> res; // 最终解密结果

    // 生成子密钥
    subkeyGeneration(key, subkey);

    // 初始置换：根据IP对密文进行初始置换
    for(int i = 0; i < 64; ++i) 
        plainAfterDisplace[i] = ciphertext[IP[i]-1];
    // 分割为左右部分
    for(int i = 0; i < 32; ++i) 
        L[i] = plainAfterDisplace[i];
    for(int i = 32; i < 64; ++i) 
        R[i % 32] = plainAfterDisplace[i];

    // 16轮解密（使用反向子密钥）
    for(int i = 0; i < 16; ++i){
        tmp = R; // 暂存右半部分
        // 更新右半部分为L与F函数的异或，使用反向子密钥，更新左半部分为之前的右半部分
        R = L ^ F(R, subkey[15 - i]);
        L = tmp;
    }

    // 左右交换
    for(int i = 0; i < 32; ++i) 
        res[i] = R[i]; // 将R部分放到结果的前半部分
    for(int i = 32; i < 64; ++i) 
        res[i] = L[i % 32]; // 将L部分放到结果的后半部分

    // 逆初始置换：根据IP-1对结果进行逆置换
    plainAfterDisplace = res;
    for(int i = 0; i < 64; ++i) 
        res[i] = plainAfterDisplace[IP_1[i]-1];

    return res; // 返回解密后的明文
}


// 3DES加密
bitset<64> encryption3DES(bitset<64> plain, bitset<64> key[3]){
    bitset<64> ciphertext; // 存储加密后的密文

    // 使用第一个密钥进行加密
    ciphertext = encryptionDES(plain, key[0]);
    // 使用第二个密钥进行解密
    ciphertext = decryptDES(ciphertext, key[1]);
    // 使用第三个密钥进行加密
    ciphertext = encryptionDES(ciphertext, key[2]);

    return ciphertext; // 返回最终的密文
}

// 3DES解密
bitset<64> decrypt3DES(bitset<64> ciphertext, bitset<64> key[3]){
    bitset<64> plain; // 存储解密后的明文

    // 使用第三个密钥进行解密
    plain = decryptDES(ciphertext, key[2]);
    // 使用第二个密钥进行加密
    plain = encryptionDES(plain, key[1]);
    // 使用第一个密钥进行解密
    plain = decryptDES(plain, key[0]);

    return plain; // 返回最终的明文
}


// 将文件内容以64位比特读取
bitset<64> readBitsFromFile(FILE* file) {
    char buffer[8] = {0,0,0,0,0,0,0,0}; // 存储8个字节（64位）
    size_t bytesRead = fread(buffer, 1, 8, file); // 从文件中读取8个字节

    bitset<64> bits = 0; // 初始化64位比特集合
    // 将每个字节转换为对应的64位比特
    for (size_t i = 0; i < bytesRead; ++i) {
        bits[i * 8] = (buffer[i] >> 0) & 1; // 获取每一位
        bits[i * 8 + 1] = (buffer[i] >> 1) & 1;
        bits[i * 8 + 2] = (buffer[i] >> 2) & 1;
        bits[i * 8 + 3] = (buffer[i] >> 3) & 1;
        bits[i * 8 + 4] = (buffer[i] >> 4) & 1;
        bits[i * 8 + 5] = (buffer[i] >> 5) & 1;
        bits[i * 8 + 6] = (buffer[i] >> 6) & 1;
        bits[i * 8 + 7] = (buffer[i] >> 7) & 1;
    }

    return bits; // 返回读取到的64位比特
}

// 将64位比特写入文件
void writeBitsToFile(FILE* file, bitset<64> bits) {
    char byte = 0; // 初始化字节
    // 将64位比特转换为字节并写入文件
    for (int i = 0; i < 64; ++i) {
        byte |= (bits[i] << (i % 8)); // 将比特设置到相应的位
        if (i % 8 == 7) { // 每8位写入一个字节
            fputc(byte, file); // 写入文件
            byte = 0; // 重置字节
        }
    }
}


// 文件加密
void solve(const char* inputFile, const char* outputFile, bitset<64> key[3], int decrypt) {
    FILE* file = fopen(inputFile, "rb");
    if (!file) {
        perror("无法打开文件");
        return;
    }

    FILE* outFile = fopen(outputFile, "wb");
    if (!outFile) {
        perror("无法写入文件");
        fclose(file);
        return;
    }

    while (!feof(file)) {
        bitset<64> bits = readBitsFromFile(file);
        if(feof(file) && decrypt) break;
        bitset<64> result;
        if(decrypt == 0){
            result = encryption3DES(bits, key);
        }
        else{
            result = decrypt3DES(bits, key);
        }
        writeBitsToFile(outFile, result);
    }

    fclose(file);
    fclose(outFile);
}

// 密码分组链接模式加密
void encryptionFile(const char* inputFile, const char* outputFile, bitset<64> key[3], bitset<64> IV0) {
    FILE* file = fopen(inputFile, "rb"); // 打开输入文件以二进制模式读取
    if (!file) {
        perror("无法打开文件"); // 错误处理
        return;
    }

    FILE* outFile = fopen(outputFile, "wb"); // 打开输出文件以二进制模式写入
    if (!outFile) {
        perror("无法写入文件"); // 错误处理
        fclose(file);
        return;
    }

    bitset<64> C = IV0; // 初始化链块C为初始向量IV0
    bitset<64> bits, result;

    // 读取文件直到结束
    while (!feof(file)) {
        bits = readBitsFromFile(file); // 从文件中读取64位比特
        result = encryption3DES(bits ^ C, key); // 使用3DES进行加密，结合密码分组链接模式
        C = result; // 更新链块C为当前结果
        writeBitsToFile(outFile, result); // 将加密结果写入输出文件
    }

    fclose(file); // 关闭输入文件
    fclose(outFile); // 关闭输出文件

    printf("文件加密成功: %s -> %s\n", inputFile, outputFile);
}


// 解密工作线程
void decryptWorker(bitset<64> Ciphertext, bitset<64> C, int idx, bitset<64> key[3], vector<bitset<64>> & plain, mutex & mtx){
    bitset<64> result;
    // 使用3DES解密，并结合密码分组链接模式
    result = decrypt3DES(Ciphertext, key) ^ C; 
    {
        unique_lock<std::mutex> lock(mtx); // 锁定互斥量以保护共享资源
        plain[idx] = result; // 将解密结果存入输出向量
    }
    return;
}

// 密码分组链接模式解密
void decryptFile(const char* inputFile, const char* outputFile, bitset<64> key[3], bitset<64> IV0) {
    FILE* file = fopen(inputFile, "rb"); // 打开输入文件
    if (!file) {
        perror("无法打开文件"); // 错误处理
        return;
    }

    FILE* outFile = fopen(outputFile, "wb"); // 打开输出文件
    if (!outFile) {
        perror("无法写入文件"); // 错误处理
        fclose(file);
        return;
    }

    bitset<64> tmp, result; // 用于临时存储和结果
    vector<thread> threads; // 存储线程
    mutex mtx; // 互斥量用于线程安全
    SYSTEM_INFO sysInfo; // 获取系统信息
    GetSystemInfo(&sysInfo); // 获取处理器数量
    vector<bitset<64>> bits, Cset, plains; // 存储比特块、链块C和明文
    Cset.emplace_back(IV0); // 将初始向量添加到链块C

    // 读取文件直到结束
    while (!feof(file)) {
        int cnt = 0;

        // 读取处理器数量的64位比特块
        for(int i = 0; i < sysInfo.dwNumberOfProcessors && !feof(file); ++i, ++cnt){
            tmp = readBitsFromFile(file);
            if(feof(file)) break;
            bits.emplace_back(tmp); // 添加到比特块向量
            if(i != sysInfo.dwNumberOfProcessors - 1) Cset.emplace_back(tmp); // 更新链块C
        }

        plains.resize(cnt); // 调整明文向量大小
        threads.clear(); // 清空线程向量
        // 创建解密线程
        for (int i = 0; i < cnt; ++i) {
            threads.emplace_back(decryptWorker, bits[i], Cset[i], i, key, ref(plains), ref(mtx));
        }

        // 等待所有解密线程完成
        for (auto& thread : threads) {
            thread.join();
        }

        // 将解密后的明文写入输出文件
        for(int i = 0; i < cnt; ++i){
            writeBitsToFile(outFile, plains[i]);
        }

        Cset.clear(); // 清空链块C
        Cset.emplace_back(bits.back()); // 更新链块C为最后一个比特块
        bits.clear(); // 清空比特块
        plains.clear(); // 清空明文
    }

    fclose(file); // 关闭输入文件
    fclose(outFile); // 关闭输出文件

    printf("文件解密成功: %s -> %s\n", inputFile, outputFile);
}


int main(){
    // 
    std::random_device rd;
    std::mt19937_64 gen(rd());
    bitset<64> keys[3] = {bitset<64>((uint64_t)gen()), bitset<64>((uint64_t)gen()), bitset<64>((uint64_t)gen())}; // 生成三个密钥
    bitset<64> IV0 = bitset<64>((uint64_t)gen());    // 密码分组链接模式的初始向量

    // 以下是密码分组链接模式下加密和解密文件

    // 加密和解密txt文件
    // encryptionFile("test_txt.txt", "encrypted_txt.bin", keys, IV0);  // 密码分组链接模式下加密
    // decryptFile("encrypted_txt.bin", "decrypted_txt.txt", keys, IV0);    // 密码分组链接模式解密

    // 加密和解密word文件
    // encryptionFile("test_word.doc", "encrypted_word.bin", keys, IV0);    // 密码分组链接模式下加密
    // decryptFile("encrypted_word.bin", "decrypted_word.doc", keys, IV0);  // 密码分组链接模式解密

    // 加密和解密jpg文件
    encryptionFile("test_jpg.jpg", "encrypted_jpg.bin", keys, IV0);  // 密码分组链接模式下加密
    decryptFile("encrypted_jpg.bin", "decrypted_jpg.jpg", keys, IV0);    // 密码分组链接模式解密

    // 加密和解密mp3文件
    // encryptionFile("test_mp3.mp3", "encrypted_mp3.bin", keys, IV0); // 密码分组链接模式下加密
    // decryptFile("encrypted_mp3.bin", "decrypted_mp3.mp3", keys, IV0);   // 密码分组链接模式解密

    // 以下是电子密码本模式下加密和解密文件

    // 加密和解密txt文件
    // solve("test_txt.txt", "encrypted_txt.bin", keys, 0); // 加密文件
    // solve("encrypted_txt.bin", "decrypted_txt.txt", keys, 1);    // 解密文件

    // 加密和解密word文件
    // solve("test_word.doc", "encrypted_word.bin", keys, 0); // 加密文件
    // solve("encrypted_word.bin", "decrypted_word.doc", keys, 1);    // 解密文件

    // 加密和解密jpg文件
    // solve("test_jpg.jpg", "encrypted_jpg.bin", keys, 0); // 加密文件
    // solve("encrypted_jpg.bin", "decrypted_jpg.jpg", keys, 1);    // 解密文件

    // 加密和解密mp3文件
    // solve("test_mp3.mp3", "encrypted_mp3.bin", keys, 0); // 加密文件
    // solve("encrypted_mp3.bin", "decrypted_mp3.mp3", keys, 1);    // 解密文件

    return 0;
}