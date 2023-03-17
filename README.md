# 一个采用C++11编写轻量级Linux网络库

Nancy是采用c++11编写的Linux轻量级网络库，主要对Linux下的IO复用接口epoll和socket接口进行封装，同时提供了一些高性能组件如

基于红黑树的定时器、基于哈希表的内存池和轻量级低延迟的异步日志系统。

## 模块与功能 

### Net：对epoll、socket、signal等的封装

- reactor ： 基于Epoll的Linux反应堆，采用事件回调的方式简化网络编程接口，可以自由选择epoll的ET/LT模式。

- socket：封装服务端和客户端socket，客户端可以一行实现连接请求。

### Timer： 基于红黑树的轻量级定时器

- 高效：在log(n)时间复杂度下的查询及修改性能（具体表现见下）
- 低内存：采用红黑树，在内存方面相比哈希表等额外开销更少

### Memorys: 基于哈希表的轻量级内存池

- 高速存取：采用哈希表存储内存单元，在O1复杂度下实现增删查的功能。

### Logger：轻量级异步日志系统

- 线程安全：支持**多生产者单消费者模型**进行异步记录日志。
- 低延迟：采用**序列化技术**对用户层的流进行**压缩存储**，减少异步同步数据的开销。
- 内存分配：通过**栈内存+大缓冲**来避免在堆上申请小块内存，减少长时间运行时产生的**内存碎片**。
- 日志安全：支持[info]、[warn]、[critical]三种级别的日志记录，其中[critical]级别确保日志被及时写入硬盘中。
- 滚动日志：支持滚动日志，能够根据用户设定的字节数自动分割文件。

## demo

### echo-server

```C++
#include "nancy/net/tcp_server.h"

int main() {
    nc::net::tcp_server serv(nc::net::localhost);  // Nancy的默认端口为: 9090
    const int buf_sz = 1025;
    char buf[buf_sz];
    serv.set_readable_cb([&](int fd) {
        int rcv = 0;
        while ((rcv = read(fd, buf, buf_sz - 1)) > 0) {
            write(fd, buf, rcv);
        }
        serv.add_event(fd, nc::net::event::readable);
    });
    serv.activate();
}
```

可以看到事件驱动的回调让事情变得简单。而基于reactor开发的固定的服务器模板tcp_server，能省去许多面向Linux接口的烦恼。



## Benchmark

测试环境：

- 编译器：g++ 9.4.0 ，开启O2优化
- 系统：ubuntu20.04
- 处理器：AMD Ryzen 7 5800H with Radeon Graphics     3.20 GHz

### IO复用模块

#### pingpong

在pingpong测试中，我们在客户端和服务端都开了一个reactor(epoll)，在客户端不断发送16k的数据包，在服务端不断读取16k的数据包。最终得到结果（单位: Mb/s）：

| connect | 50      | 100     | 150     | 200     | 500     | 1000   |
| ------- | ------- | ------- | ------- | ------- | ------- | ------ |
| 1       | 2802.59 | 2844.28 | 3303.67 | 3229.41 | 3094.65 | 2765.4 |

其中**connect**代表并发连接数，1代表服务端和客户端都是单线程。以上结果只说明Nancy在编写reactor模块时没有犯什么大错误，不能说明Nancy的吞吐量有何过人之处。因为其实对epoll进行封装的写法是相对固定的，无法在编码上做太多改进。作为网络库的一个组件来说，与其它组件协同的难易程度，其接口的安全性、易用性等也许更值得考虑。



### 日志系统

#### 并发延迟百分比测试

| 线程数 | 日志系统 | 50%  | 75%  | 90%  | 99.00% | 99.90% | 最差 | 平均值  |
| ------ | -------- | ---- | ---- | ---- | ------ | ------ | ---- | ------- |
| 1      | nclog    | 0    | 0    | 1    | 8      | 11     | 100  | 0.2579  |
| 2      | nclog    | 0    | 0    | 1    | 5      | 13     | 106  | 0.2825  |
| 3      | nclog    | 0    | 0    | 1    | 9      | 13     | 56   | 0.3848  |
| 4      | nclog    | 0    | 0    | 1    | 9      | 18     | 618  | 0.5952  |
| 1      | spdlog   | 0    | 1    | 1    | 1      | 14     | 50   | 0.2956  |
| 2      | spdlog   | 0    | 1    | 30   | 45     | 91     | 217  | 4.3377  |
| 3      | spdlog   | 42   | 47   | 56   | 118    | 184    | 409  | 42.9147 |
| 4      | spdlog   | 51   | 56   | 70   | 146    | 229    | 4967 | 52.6044 |

测试说明：并发调用日志的记录接口多次，记录每条线程的结果并并将线程结果进行平均处理，得到最终结果。

结果说明：可以看到Nancy的日志系统nclog在总体的延迟水平远低于spdlog，且线程数越多，nclog的表现和spdlog的表现相差越大。







