#include <iostream>
#include <cstdlib>
#include <ctime>
#include <fstream>　
#include <iomanip>　
#include<vector>
#include <chrono>
#include<random>
#include <thread>
using namespace std;

// 选择排序
void Selection_Sort(int arr[], int n) {
    for (int i = 0; i < 10; i++) {
        int max = i;
        for (int j = i + 1; j < n; j++)
            if (arr[j] > arr[max])
                max = j;
        swap(arr[i], arr[max]);
    }
}

int main()
{
    int n_val[] = { 1000000, 3000000,5000000, 10000000,30000000,50000000,100000000, 300000000, 500000000, 1000000000 };
    for (int i = 0; i < 10; i++) {
        double tot_time = 0;
        for (int j = 1; j <= 10; j++) {
            int n = n_val[i];     // n为数据规模
            int* arr = new int[n];     // arr为待排序数组
            int min = 0, max = n;
            random_device seed;//硬件生成随机数种子
            ranlux48 engine(seed());//利用种子生成随机数引擎
            uniform_int_distribution<> distrib(min, max);//设置随机数范围，并为正态分布
            for (int i = 0; i < n; i++)
            {
                arr[i] = distrib(engine) % n;    // 生成随机数据
            }
            clock_t StartTime, EndTime; //定义计时开始和计时结束变量
            double time;        // 记录运行时间    
            StartTime = clock();    //开始计时
            Selection_Sort(arr, n);    //排序
            EndTime = clock();      //结束计时
            time = (double)(EndTime - StartTime) / CLOCKS_PER_SEC;    //计算差值并转换为秒
            cout << fixed;
            cout << "数据规模为" << n << "时在第" << j << "组测试样本下所需运行时间为" << time << "秒" << endl;
            tot_time += time;
            delete[]arr;
        }
        cout << "数据规模为" << n_val[i] << "时平均运行时间为" << tot_time / 10 << "秒" << endl;
        cout << endl;
    }

    return 0;
}
