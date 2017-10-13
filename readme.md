Usage:
    1.cpu_memory.c 限制某进程内存及cpu占用率

    1.1 程序运行
    ./cpu_memory [允许最大内存 单位：M] [最大CPU占用率 单位：%]

    ./cpu_memory 100 20

    1.2 终端运行
    mkdir -p /tmp/cgroup/cpu
    mkdir -p /tmp/cgroup/memory

    mount -t cgroup -o cpu cpu /tmp/cgroup/cpu
    mount -t cgroup -o memory memory /tmp/cgroup/memory

    mkdir -p /tmp/cgroup/cpu/test
    mkdir -p /tmp/cgroup/memory/test

    echo 100M > /tmp/cgroup/memory/test/memory.limit_in_bytes
    echo 20000 > /tmp/cgroup/cpu/test/cpu.cfs_quota_us

    echo 0 > /tmp/cgroup/cpu/test/tasks
    echo 0 > /tmp/cgroup/memory/test/tasks

    2.cpuset_memory.c 限制某进程内存及cpu使用核数

    2.1 程序运行
    ./cpuset_memory [允许最大内存 单位：M] [cpu使用核数 如：0-1] [CPU内存节点 默认为0]

    ./cpuset_memory 100 0-1 [0]

    2.2 终端运行
    mkdir -p /tmp/cgroup/cpuset
    mkdir -p /tmp/cgroup/memory

    mount -t cgroup -o cpuset cpuset /tmp/cgroup/cpuset
    mount -t cgroup -o memory memory /tmp/cgroup/memory

    mkdir -p /tmp/cgroup/cpuset/test
    mkdir -p /tmp/cgroup/memory/test

    echo 100M > /tmp/cgroup/memory/test/memory.limit_in_bytes
    echo 0 > /tmp/cgroup/cpuset/test/cpuset.cpus
    echo 0 > /tmp/cgroup/cpuset/test/cpuset.mems

    echo 0 > /tmp/cgroup/cpuset/test/tasks
    echo 0 > /tmp/cgroup/memory/test/tasks
