#!/bin/bash
sql_index=1
parallel_cnt_max=1

printf "%10s|" "paraellel"

parallel_cnt=1
while [ $parallel_cnt -le $parallel_cnt_max ]
do
	printf "%9s|" $parallel_cnt
	parallel_cnt=$(( $parallel_cnt + 1 ))
done
echo ""

while [ $sql_index -le 22 ]
do
        query_sql=q$sql_index
        parallel_cnt=1
	printf "%10s|" $query_sql
        while [ $parallel_cnt -le $parallel_cnt_max ]
        do
		p_cnt=$(printf "%02d" $parallel_cnt)
		sql_name=$query_sql
		log=$p_cnt-$sql_name-mpstat_running.txt
		cpu_idle=$(cat $log | grep Average | grep all | awk '{print $12}')
		cpu_load=$( printf "%.2f" $(echo "100 - $cpu_idle" | bc) )

		printf "%10s" $(printf "%7.2f|" $cpu_load)

		parallel_cnt=$(( $parallel_cnt + 1 ))
	done
	echo ""
	sql_index=$(( $sql_index + 1 ))
done

