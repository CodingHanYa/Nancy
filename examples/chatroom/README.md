# 聊天室服务

- 概述
    - 服务器通过内容转发实现聊天室功能

- 模块
    - 客户端：发起连接，收到专属名，开始聊天
    - 服务端：接收连接，分配名称，转发内容

- 运行
    - 编译
    ```shell
    # 在"chatroom/"目录下
    cmake -B build
    cd build
    make
    ```
    - 启动服务端（以本地为例）
    ```shell
    ./server 127.0.0.1 9090
    ```
    - 客户端采用netcat服务（在本地另外终端）
    ```shell
    nc -v 127.0.0.1 9090
    # 后收到服务端分配的专属名
    # 开始聊天...
    ```

- Posix接口介绍
    - `alarm(timeout)`: 定时，在timeout之后触发信号SIGALRM。此时我们调用`sigflow::add_signal(SIGALRM,[]{/*callback*/})`即可在信号触发时执行回调。若在回调中继续调用`alarm()`，则可以实现周期执行某些功能。