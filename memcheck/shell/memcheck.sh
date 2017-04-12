#!/bin/bash
#usage: nohup ./memcheck.sh > /dev/null 2>&1 &
#set -x
PROC_NAME="memchk_test"   #进程名称
OUTPUT_PATH="/home/zhoulee/" #输出文件路径
let MEMORY_LIMIT=500000  #内存使用上限 单位:KB
let MONITOR_INTERVAL=5   #监控时间间隔 单位:Sec

while (true)
do
    PID=`ps aux | grep ${PROC_NAME} | grep -v grep | head -n 1 | awk '{print $2}'`
    if [ ! ${PID} ]; then
        echo "process ${PROC_NAME} not found"
    elif [ ${PID} -gt 0 ]; then
        MEM_USE=`cat /proc/${PID}/status | grep VmRSS | awk '{print $2}'`
        if [ ${MEM_USE} -gt ${MEMORY_LIMIT} ]; then
            echo "memory high, save process info...."
            TIME_SUFFIX=`date +%Y%m%d-%H%M%S`
            CORE_FILE=${OUTPUT_PATH}${PROC_NAME}"_"${TIME_SUFFIX}"_"${PID}".coredump"
            GDB_FILE=${OUTPUT_PATH}${PROC_NAME}"_"${TIME_SUFFIX}"_"${PID}"_gdb.info"
            gdb -ex "thread apply all bt" -batch -p ${PID} > ${GDB_FILE}
            gcore -o ${CORE_FILE} ${PID}
            break;
        else
            echo "memory ${MEM_USE} KB"
        fi
    fi
    sleep ${MONITOR_INTERVAL}
done
