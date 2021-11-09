// intro2algo.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <tchar.h>
#include <iostream>
#include <vector>

using namespace std;

extern void insertion_sort_test();
extern void dec2bin_test();
extern void merge_sort_test();
extern void gcd_test(int, TCHAR* []);
extern void genetic_algorithm_test();
extern void pay_test();
extern void cut_test();
extern int proto_client(int, TCHAR* []);
extern int proto_server();
extern int proto_client_file(int, TCHAR* []);
extern int proto_server_file();
extern void sm_door_test();



int _tmain(int argc, _TCHAR* argv[])
{
    // insertion_sort_test();
    // dec2bin_test();
    // merge_sort_test();
    // gcd_test(argc, argv);
    // genetic_algorithm_test();
    // pay_test();
    // cut_test();
    /*
    if (argc == 1)
        // proto_server();
        proto_server_file();
    else
        //proto_client(argc, argv);
        proto_client_file(argc, argv);
        */

    sm_door_test();
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
