# 简介

- 本文介绍开发环境搭建方法



# 说明

## 目录说明

1. bin：二进制安装包
2. src：源代码
3. doc：使用文档
4. test：测试数据及测试报告
5. tools：常用脚本工具
6. debug：调试记录

## 硬件要求

- FPGA板卡：浪潮F37X板卡（Xilnux U280板卡需要自行配置）
- 内存：大于32GB
- 磁盘：大于200GB
- CPU：核心数大于12个



## 软件要求

- 操作系统：Ubuntu18.04 x64
- 内核：Linux ubuntu 5.0.0-23-generic



# 环境搭建

## 安装Vitis2020.2

1. 参考Xilinx 官方文档安装Vitis2020.2开发环境



## 安装Postgresql

1. 安装依赖库

   ```shell
   sudo apt-get install libreadline6-dev zlib1g-dev -y
   ```

2. 下载源码

   ```asm
   https://ftp.postgresql.org/pub/source/v11.2/postgresql-11.2.tar.gz
   ```

3. 解压

   ```shell
   tar xvzf postgresql-11.2.tar.gz
   ```

4. 配置

   ```shell
   ./configure --prefix=/usr/local/postgres
   ```

5. 编译

   ```shell
   make
   ```

6. 安装

   ```shell
   sudo make install
   ```

   

## 创建测试数据集

### 创建TPC-H数据

1. 下载dbgen源码

   ```asm
   https://www.tpc.org/tpch/
   ```

2. 添加如下patch

   ```asm
   diff --git a/makefile.suite b/makefile.suite
   index 45548a4..6014505 100755
   --- a/makefile.suite
   +++ b/makefile.suite
   @@ -100,15 +100,15 @@
    ################
    ## CHANGE NAME OF ANSI COMPILER HERE
    ################
   -CC      = 
   +CC      = gcc
    # Current values for DATABASE are: INFORMIX, DB2, TDAT (Teradata)
    #                                  SQLSERVER, SYBASE, ORACLE, VECTORWISE
    # Current values for MACHINE are:  ATT, DOS, HP, IBM, ICL, MVS, 
    #                                  SGI, SUN, U2200, VMS, LINUX, WIN32 
    # Current values for WORKLOAD are:  TPCH
   -DATABASE= 
   -MACHINE = 
   -WORKLOAD = 
   +DATABASE= SQLSERVER
   +MACHINE = LINUX
   +WORKLOAD = TPCH
    #
    CFLAGS = -g -DDBNAME=\"dss\" -D$(MACHINE) -D$(DATABASE) -D$(WORKLOAD) -DRNG_TEST -D_FILE_OFFSET_BITS=64 
    LDFLAGS = -O
   ```

3. 编译生成dbgen工具

   ```shell
   make -f makefile.suite
   ```

4. 生成tpc-h测试数据

   ```shell
   ./dbgen -s 1 -f
   ```

5. 将数据转换为csv格式

   ```shell
   for i in `ls *.tbl`; do sed 's/|$//' $i > ${i/tbl/csv}; echo $i; done;
   ```

   

### 导入数据到Postgresql

1. 调用tools目录下脚本，自动创建测试用数据库

   ```shel
   ./create_tpch_demo_db.sh csv文件路径/ tpch1GB tpch.sql
   ```



### 测试TPC-H Q6查询

1. 导出环境变量

   ```shell
   export PATH=/usr/local/postgres/bin
   ```

2. 启动数据库

   ```shell
   pg_ctl -D tpch1GB restart
   ```

3. 链接数据库

   ```shell
   psql -d tpch1GB
   ```

4. 启动Q6查询

   ```shell
   \timing on
   select
     sum(l_extendedprice * l_discount) as revenue
   from
     lineitem
   where
     l_shipdate >= date '1994-01-01'
           and l_shipdate < date '1994-01-01' + interval '1' year
     and l_discount between 0.06 - 0.01 and 0.06 + 0.01
     and l_quantity < 24
   limit 1;
   ```
   
   



# CPU+FPGA测试步骤

1. 进入bin目录，解压安装包

   ```shell
   tar xvzf pg_seqscan3.5.3-bin-relaese.tar.gz
   ```

2. 拷贝pg_pathlist_hook_demo.so到PostgreSQL v11.2安装目录

   ```shell
   sudo cp pg_pathlist_hook_demo.so /usr/local/postgres/lib
   ```

3. 修改数据库配置文件使能插件

   ```shell
   sed -i "/#shared_preload_libraries =/cshared_preload_libraries = pg_pathlist_hook_demo.so" tpch1GB/postgresql.conf
   ```

4. 导出Xilinx XRT环境变量（**按照自己的安装目录设置**）

   ```shell
   source /opt/Xilinx-2020.2/Vitis/2020.2/settings64.sh
   source /opt/xilinx/xrt/setup.sh
   export LIBRARY_PATH=/usr/lib/x86_64-linux-gnu
   ```

5. 启动中间件服务进程

   ```shell
   ./pg_filter_host -k pg_filter_kernel.xclbin -w 4 &
   ```

6. 重启数据库

   ```shell
   pg_ctl -D tpch1GB restart
   ```

7. 连接数据库

   ```shell
   psql -d tpch1GB
   ```

8. 启动Q6查询

   ```shell
   \timing on
   select
     sum(l_extendedprice * l_discount) as revenue
   from
     lineitem
   where
     l_shipdate >= date '1994-01-01'
           and l_shipdate < date '1994-01-01' + interval '1' year
     and l_discount between 0.06 - 0.01 and 0.06 + 0.01
     and l_quantity < 24
   limit 1;
   ```

