# tools
代码调试小工具集合
# memcheck #
内存监控脚本，监控指定进程的内存占用，当进程使用内存超过指定阈值时，输出对应的coredump以及线程信息
# malloctest #
当进程初始时分配较大堆内存，使用std::map存储分配指针，然后遍历map释放，再在运行时分配比初始化时小的多的堆内存，使用top查看，进程占用内存始终保持初始分配的高内存；如果使用std::vector和标准数组则内存及时释放。
[http://www.cnblogs.com/lookof/archive/2013/03/26/2981768.html](http://www.cnblogs.com/lookof/archive/2013/03/26/2981768.html "glibc内存管理")