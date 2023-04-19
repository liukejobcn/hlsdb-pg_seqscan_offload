#!/bin/bash
export PATH=/usr/local/postgres-11.2/bin/:$PATH

function start_pg_service()
{
server_ip=$1
client_ip=$2
user_name=$3
password=$4
query_sql=$5
working_dir=$6
parallel_cnt=$7

p_cnt=$(printf "%02d" $parallel_cnt) 
sql_name=$(echo "$query_sql" | awk -F . '{print $1}')

echo "start_pg_with_parallel_count parallel_cnt=$parallel_cnt"

/usr/bin/expect << EOF
spawn ssh  -X $user_name@$server_ip
expect "*password:" 
send "$password\r"

expect "$user_name@*"
send "cd $working_dir\r"

expect "$user_name@*"
send "source setenv.sh\r"

expect "$user_name@*"
send "./start_pg_with_parallel_count.sh $parallel_cnt\r"

expect "ubuntu@*"
send "mpstat 1 10 > $p_cnt-$sql_name-mpstat_base.txt\r"

expect "$user_name@*"
send "sync\r"

expect "$user_name@*"
send "exit\r"
EOF
}

function do_pgbench()
{
server_ip=$1
user_name=$2
working_dir=$3
sql_dir=$4
query_sql=$5
test_time=$6
parallel_cnt=$7

p_cnt=$(printf "%02d" $parallel_cnt) 
sql_name=$(echo "$query_sql" | awk -F . '{print $1}')

echo "pgbench --host $server_ip -U $user_name -rf $sql_dir/$query_sql -T $test_time -n tpch1GB" 

nohup pgbench --host $server_ip -U $user_name -rf $sql_dir/$query_sql -T $test_time -n tpch1GB > $p_cnt-$sql_name-parallel-pgbench.txt &

}

function do_mpstat()
{
server_ip=$1
user_name=$2
password=$3

working_dir=$4
sql_dir=$5
query_sql=$6
test_time=$7
parallel_cnt=$8

p_cnt=$(printf "%02d" $parallel_cnt) 
sql_name=$(echo "$query_sql" | awk -F . '{print $1}')

/usr/bin/expect << EOF
spawn ssh  -X $user_name@$server_ip
expect "*password:" 
send "$password\r"

expect "$user_name@*"
send "cd $working_dir\r"

expect "$user_name@*"
send "mpstat 1 $test_time > $p_cnt-$sql_name-mpstat_running.txt \r"

expect "$user_name@*"
send "sync\r"

expect "ubuntu@*"
send "exit\r"
EOF
}

server_ip=192.168.1.5
client_ip=192.168.1.10
user_name=ubuntu
password=ubuntu

working_dir=/home/ubuntu/work/project/inspur-project/inspur_project_bin/tpch/db/postgresql-11.2-o2
sql_dir=./tpch-sql
pg_bench_test_time=20
mpstat_test_time=10
parallel_cnt_max=12
sql_index=1
while [ $sql_index -le 22 ]
do
	echo "sql_index=$sql_index"
	query_sql=q$sql_index.sql
	parallel_cnt=0
	while [ $parallel_cnt -le $parallel_cnt_max ]
	do
		echo "do query $query_sql with parallel_cnt=$parallel_cnt"

		start_pg_service $server_ip $client_ip $user_name $password $query_sql $working_dir $parallel_cnt

		do_pgbench $server_ip $user_name $working_dir $sql_dir $query_sql $pg_bench_test_time $parallel_cnt

		sleep 5 

		do_mpstat $server_ip $user_name $password $working_dir $sql_dir $query_sql $mpstat_test_time $parallel_cnt

		sleep 30

		parallel_cnt=$(( $parallel_cnt + 1 ))
	done
	sql_index=$(( $sql_index + 1 ))
done

