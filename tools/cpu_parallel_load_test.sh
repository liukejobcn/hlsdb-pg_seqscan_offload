#!/bin/bash
parallel_cnt_max=12
parallel_cnt=0
cpu_load_max=80
cpu_load=0

test_time=20
password=ubuntu

function start_pg_service()
{
parallel_cnt=$1
cpu_load=$2
p_cnt=$(printf "%02d" $parallel_cnt) 
c_load=$(printf "%02d" $cpu_load)

/usr/bin/expect << EOF
spawn ssh  -X ubuntu@192.168.1.5
expect "*password:" 
send "ubuntu\r"

expect "ubuntu@*"
send "cd /home/ubuntu/work/project/inspur-project/inspur_project_bin/tpch/db/postgresql-11.2-o2\r"

expect "ubuntu@*"
send "source setenv.sh\r"

expect "ubuntu@*"
send "./change_parallel_worker_conf.sh $parallel_cnt\r"

expect "ubuntu@*"
send "echo \"$password\" | sudo -S ./cpu_load_set.sh $cpu_load\r"

expect "ubuntu@*"
send "mpstat 1 10 > $c_load-$p_cnt-mpstat_base.txt\r"

expect "ubuntu@*"
send "sync\r"

expect "ubuntu@*"
send "exit\r"
EOF
}

function do_pgbench()
{
parallel_cnt=$1
cpu_load=$2
test_time=$3
p_cnt=$(printf "%02d" $parallel_cnt) 
c_load=$(printf "%02d" $cpu_load)

/usr/bin/expect << EOF
spawn ssh  -X ubuntu@192.168.1.10
expect "*password:" 
send "ubuntu\r"

expect "ubuntu@*"
send "cd /home/ubuntu/work/project/inspur-project/inspur_project_bin/tpch/db/postgresql-11.2-o2\r"

expect "ubuntu@*"
send "source setenv.sh\r"

expect "ubuntu@*"
send "nohup pgbench --host 192.168.1.5 -U ubuntu -rf q6.sql -T $test_time -n tpch1GB > $c_load-$p_cnt-cpu-parallel-pgbench.txt &\r"

expect "ubuntu@*"
send "exit\r"
EOF
}

function do_mpstat()
{
parallel_cnt=$1
cpu_load=$2
p_cnt=$(printf "%02d" $parallel_cnt) 
c_load=$(printf "%02d" $cpu_load)

/usr/bin/expect << EOF
spawn ssh  -X ubuntu@192.168.1.5
expect "*password:" 
send "ubuntu\r"

expect "ubuntu@*"
send "cd /home/ubuntu/work/project/inspur-project/inspur_project_bin/tpch/db/postgresql-11.2-o2\r"

expect "ubuntu@*"
send "mpstat 1 10 > $c_load-$p_cnt-mpstat_running.txt \r"

expect "ubuntu@*"
send "sync\r"

expect "ubuntu@*"
send "exit\r"
EOF
}

while [ $cpu_load -le $cpu_load_max ] 
do
	parallel_cnt=0
	while [ $parallel_cnt -le $parallel_cnt_max ]
	do
		start_pg_service $parallel_cnt $cpu_load

		do_pgbench $parallel_cnt $cpu_load $test_time

		sleep 2

		do_mpstat $parallel_cnt $cpu_load

		sleep 30

		parallel_cnt=$(( $parallel_cnt + 1 ))
	done
	cpu_load=$(( $cpu_load + 10 ))
done

