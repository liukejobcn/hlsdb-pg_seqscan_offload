#!/bin/bash
export PATH=/usr/local/postgres/bin:$PATH

function start_pg_service()
{
working_dir=$1
database=$2
parallel_cnt=$3
sql_dir=$4
query_sql=$5
test_time=$6

p_cnt=$(printf "%02d" $parallel_cnt) 
sql_name=$(echo "$query_sql" | awk -F . '{print $1}')

echo "start_pg_with_parallel_count parallel_cnt=$parallel_cnt"

cd $working_dir

sed -i "/max_worker_processes =/cmax_worker_processes = $parallel_cnt" $database/postgresql.conf
sed -i "/max_parallel_workers_per_gather =/cmax_parallel_workers_per_gather = $parallel_cnt" $database/postgresql.conf
sed -i "/max_parallel_workers =/cmax_parallel_workers = $parallel_cnt" $database/postgresql.conf
sed -i "/min_parallel_table_scan_size =/cmin_parallel_table_scan_size = 8kB" $database/postgresql.conf

pg_ctl -D $database restart

mpstat -P ON 1 $test_time > $p_cnt-$sql_name-mpstat_base.txt 
}

function do_pgbench()
{
working_dir=$1
database=$2
parallel_cnt=$3
sql_dir=$4
query_sql=$5
test_time=$6

cd $working_dir
p_cnt=$(printf "%02d" $parallel_cnt) 
sql_name=$(echo "$query_sql" | awk -F . '{print $1}')

echo "pgbench -rf $sql_dir/$query_sql -T $test_time -n $database" 

pgbench -rf $sql_dir/$query_sql -T $test_time -n $database > $p_cnt-$sql_name-parallel-pgbench.txt &

}

function do_mpstat()
{
working_dir=$1
database=$2
parallel_cnt=$3
sql_dir=$4
query_sql=$5
test_time=$6

cd $working_dir
p_cnt=$(printf "%02d" $parallel_cnt) 
sql_name=$(echo "$query_sql" | awk -F . '{print $1}')

mpstat -P ON 1 $test_time > $p_cnt-$sql_name-mpstat_running.txt 

}

sql_dir=$1
parallel_cnt_max=$2
server_working_dir=/home/ubuntu/work/project/inspur-project/inspur_project_bin/tpch/db/postgresql-11.2-o2
pgbench_working_dir=$(pwd)
database=tpch1GB
mpstat_base_time=5
pg_bench_test_time=15
mpstat_running_time=5

sql_index=1
while [ $sql_index -le 22 ]
do
	echo "sql_index=$sql_index"
	query_sql=q$sql_index.sql
	parallel_cnt=0
	while [ $parallel_cnt -le $parallel_cnt_max ]
	do
		echo "do query $query_sql with parallel_cnt=$parallel_cnt"

		start_pg_service $server_working_dir $database $parallel_cnt $sql_dir $query_sql $mpstat_base_time
		do_pgbench $pgbench_working_dir $database $parallel_cnt $sql_dir $query_sql $pg_bench_test_time
		sleep 5 
		do_mpstat $server_working_dir $database $parallel_cnt $sql_dir $query_sql $mpstat_running_time

		sleep 20

		while [ 1 ]
		do
			pgbench_pid=$(pidof pgbench)
			if [ -z $pgbench_pid ] ; then
				echo "pgbench is done ..."
				break;
			else
				echo "wait pgbench done..."
				sleep 1
			fi
		done

		parallel_cnt=$(( $parallel_cnt + 1 ))
	done
	sql_index=$(( $sql_index + 1 ))
done

