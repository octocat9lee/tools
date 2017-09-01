# tools
代码调试小工具集合
# memcheck #
内存监控脚本，监控指定进程的内存占用，当进程使用内存超过指定阈值时，输出对应的coredump以及线程信息
# malloctest #
当进程初始时分配较大堆内存，使用std::map存储分配指针，然后遍历map释放，再在运行时分配比初始化时小的多的堆内存，使用top查看，进程占用内存始终保持初始分配的高内存；如果使用std::vector和标准数组则内存及时释放。
[http://www.cnblogs.com/lookof/archive/2013/03/26/2981768.html](http://www.cnblogs.com/lookof/archive/2013/03/26/2981768.html "glibc内存管理")
# makefile #
- 将makefile基本选项以及生成规则进行封装在makefile.pub中，从而简化makefile文件的编写
- 使用预定义宏打印可执行文件或者动态库版本信息，方便版本管理
## 使用示例 ##
具体使用示例见shared、static以及exe目录下makefile文件
#log4cplusfacade#
对开源日志库log4cplus封装，实现更简单方便调用
## 使用示例 ##
step1: 定义日志实例  
    `DECLARE_GET_LOGGER("Logger.Global")`  
step2: 使用配置文件初始化实例  
    `INIT_LOGGER("../config/log.conf");`  

## C++风格使用示例 ##
    LOG_DEBUG("debug " << i);
## C风格使用示例 ##
    CLOG_DEBUG(" u [%u]", static_cast<unsigned int>(i));

