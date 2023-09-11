# Introduction

- This document introduces the method of setting up development environment



# Description

## Directory description

1. bin：Binary installation package
2. src：Source code
3. doc：Document
4. test：Test data and test report
5. tools：Common scripting tools
6. debug：Debug log

## Hardware requirement

- FPGA：Inspur F73X Card(Alveo U280)
- DDR：more than 32GB
- Disk：more than 200GB
- CPU：more than 12 cores



## Software requirement

- OS：Ubuntu18.04 x64
  - Kernel：Linux ubuntu 5.0.0-23-generic
- Vitis: Vitis 2020.2



# Environment setup

## Install Vitis2020.2

1. Refer to the Xilinx official documentation to install the Vitis2020.2 development environment



## Install Postgresql

1. Install dependent library

   ```shell
   sudo apt-get install libreadline6-dev zlib1g-dev -y
   ```

2. Download source code

   ```asm
   https://ftp.postgresql.org/pub/source/v11.2/postgresql-11.2.tar.gz
   ```

3. Unzip 

   ```shell
   tar xvzf postgresql-11.2.tar.gz
   ```

4. Configure

   ```shell
   ./configure --prefix=/usr/local/postgres
   ```

5. Compile

   ```shell
   make
   ```

6. Install

   ```shell
   sudo make install
   ```

   

## Create a test dataset

### Create TPC-H Raw data

1. Download `dbgen` source code

   ```asm
   https://www.tpc.org/tpch/
   ```

2. Add the following patch(Using gcc as compiler)

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

3. Compile to create the `dbgen` tool

   ```shell
   make -f makefile.suite
   ```

4. create TPC-H raw data

   ```shell
   ./dbgen -s 1 -f
   ```

5. Convert data to csv format by the following shell scirpt

   ```shell
   for i in `ls *.tbl`; do sed 's/|$//' $i > ${i/tbl/csv}; echo $i; done;
   ```

   

### Import data to Postgresql

1. Using the script in the tools directory to automatically create a test database

   ```shel
   ./create_tpch_demo_db.sh <your csv dir path>/ tpch1GB tpch.sql
   ```



### Test the TPC-H Q6 query

1. Export environment variable

   ```shell
   export PATH=/usr/local/postgres/bin
   ```

2. Start the Database

   ```shell
   pg_ctl -D tpch1GB restart
   ```

3. Connect to Database

   ```shell
   psql -d tpch1GB
   ```

4. Start Q6 query

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
   
   



# CPU+FPGA test procedure

1. Go to the bin directory and decompress the installation package

   ```shell
   tar xvzf pg_seqscan3.5.4-bin-relaese.tar.gz
   ```

2. Copy pg_pathlist_hook_demo.so to the PostgreSQL v11.2 installation directory

   ```shell
   sudo cp pg_pathlist_hook_demo.so /usr/local/postgres/lib
   ```

3. Modify the database configuration file to enable the hook

   ```shell
   sed -i "/#shared_preload_libraries =/cshared_preload_libraries = pg_pathlist_hook_demo.so" tpch1GB/postgresql.conf
   ```

4. Export Xilinx XRT environment variables (**Set in your own installation directory !**)

   ```shell
   source /opt/Xilinx-2020.2/Vitis/2020.2/settings64.sh
   source /opt/xilinx/xrt/setup.sh
   export LIBRARY_PATH=/usr/lib/x86_64-linux-gnu
   ```

5. Start the middleware service process

   ```shell
   ./pg_filter_host -k pg_filter_kernel.xclbin -w 4 &
   ```

6. Restart the database

   ```shell
   pg_ctl -D tpch1GB restart
   ```

7. Connect database

   ```shell
   psql -d tpch1GB
   ```

8. Start Q6 query

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

