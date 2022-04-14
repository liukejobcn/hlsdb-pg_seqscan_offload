#!/bin/bash
        export PATH=/usr/local/postgres/bin:$PATH
        csv_dir=$1
        data_name=$2
        sql=$3

        cp $csv_dir/lineitem.csv $csv_dir/lineitem.csv.bak

        #we cut the data to 1GB for postgresql sotreage engine
        head -n 5687923 $csv_dir/lineitem.csv.bak  > $csv_dir/lineitem.csv

        pg_ctl  -D "$data_name" stop
        rm -rf "$data_name"

        initdb "$data_name"
        pg_ctl  -D "$data_name" start
        sleep 2

        createdb "$data_name"
        psql -v csv_dir=$csv_dir "$data_name" < $sql

        pg_ctl  -D "$data_name" stop

