create table nation  ( n_nationkey  integer not null,
                            n_name       char(25) not null,
                            n_regionkey  integer not null,
                            n_comment    varchar(152));
alter table nation alter n_nationkey set storage plain;
alter table nation alter n_name set storage plain;
alter table nation alter n_regionkey set storage plain;
alter table nation alter n_comment set storage plain;

create table region  ( r_regionkey  integer not null,
                            r_name       char(25) not null,
                            r_comment    varchar(152));
alter table region alter r_regionkey set storage plain;
alter table region alter r_name set storage plain;
alter table region alter r_comment set storage plain;

create table part  ( p_partkey     integer not null,
                          p_name        varchar(55) not null,
                          p_mfgr        char(25) not null,
                          p_brand       char(10) not null,
                          p_type        varchar(25) not null,
                          p_size        integer not null,
                          p_container   char(10) not null,
                          p_retailprice float8 not null,
                          p_comment     varchar(23) not null );
alter table part alter p_partkey set storage plain;
alter table part alter p_name set storage plain;
alter table part alter p_mfgr set storage plain;
alter table part alter p_brand set storage plain;
alter table part alter p_type set storage plain;
alter table part alter p_size set storage plain;
alter table part alter p_container set storage plain;
alter table part alter p_retailprice set storage plain;
alter table part alter p_comment set storage plain;

create table supplier ( s_suppkey     integer not null,
                             s_name        char(25) not null,
                             s_address     varchar(40) not null,
                             s_nationkey   integer not null,
                             s_phone       char(15) not null,
                             s_acctbal     float8 not null,
                             s_comment     varchar(101) not null);
alter table supplier alter s_suppkey set storage plain;
alter table supplier alter s_name set storage plain;
alter table supplier alter s_address set storage plain;
alter table supplier alter s_nationkey set storage plain;
alter table supplier alter s_phone set storage plain;
alter table supplier alter s_acctbal set storage plain;
alter table supplier alter s_comment set storage plain;

create table partsupp ( ps_partkey     integer not null,
                             ps_suppkey     integer not null,
                             ps_availqty    integer not null,
                             ps_supplycost  float8  not null,
                             ps_comment     varchar(199) not null );
alter table partsupp alter ps_partkey set storage plain;
alter table partsupp alter ps_suppkey set storage plain;
alter table partsupp alter ps_availqty set storage plain;
alter table partsupp alter ps_supplycost set storage plain;
alter table partsupp alter ps_comment set storage plain;

create table customer ( c_custkey     integer not null,
                             c_name        varchar(25) not null,
                             c_address     varchar(40) not null,
                             c_nationkey   integer not null,
                             c_phone       char(15) not null,
                             c_acctbal     float8   not null,
                             c_mktsegment  char(10) not null,
                             c_comment     varchar(117) not null);
alter table customer alter c_custkey set storage plain;
alter table customer alter c_name set storage plain;
alter table customer alter c_address set storage plain;
alter table customer alter c_nationkey set storage plain;
alter table customer alter c_phone set storage plain;
alter table customer alter c_acctbal set storage plain;
alter table customer alter c_mktsegment set storage plain;
alter table customer alter c_comment set storage plain;

create table orders  ( o_orderkey       integer not null,
                           o_custkey        integer not null,
                           o_orderstatus    char(1) not null,
                           o_totalprice     float8 not null,
                           o_orderdate      date not null,
                           o_orderpriority  char(15) not null,  
                           o_clerk          char(15) not null, 
                           o_shippriority   integer not null,
                           o_comment        varchar(79) not null);
alter table orders alter o_orderkey set storage plain;
alter table orders alter o_custkey set storage plain;
alter table orders alter o_orderstatus set storage plain;
alter table orders alter o_totalprice set storage plain;
alter table orders alter o_orderdate set storage plain;
alter table orders alter o_orderpriority set storage plain;
alter table orders alter o_clerk set storage plain;
alter table orders alter o_shippriority set storage plain;
alter table orders alter o_comment set storage plain;

create table lineitem ( l_orderkey    integer not null,
                             l_partkey     integer not null,
                             l_suppkey     integer not null,
                             l_linenumber  integer not null,
                             l_quantity    float8 not null,
                             l_extendedprice  float8 not null,
                             l_discount    float8 not null,
                             l_tax         float8 not null,
                             l_returnflag  char(1) not null,
                             l_linestatus  char(1) not null,
                             l_shipdate    date not null,
                             l_commitdate  date not null,
                             l_receiptdate date not null,
                             l_shipinstruct char(25) not null,
                             l_shipmode     char(10) not null,
                             l_comment      varchar(44) not null);
alter table lineitem alter l_quantity set storage plain;
alter table lineitem alter l_discount set storage plain;
alter table lineitem alter l_extendedprice set storage plain;
alter table lineitem alter l_tax set storage plain;
alter table lineitem alter l_returnflag set storage plain;
alter table lineitem alter l_linestatus set storage plain;
alter table lineitem alter l_shipinstruct set storage plain;
alter table lineitem alter l_shipmode set storage plain;
alter table lineitem alter l_comment set storage plain;

\set customer_path :csv_dir customer.csv
\set supplier_path :csv_dir supplier.csv
\set nation_path :csv_dir nation.csv
\set region_path :csv_dir region.csv
\set part_path :csv_dir part.csv
\set partsupp_path :csv_dir partsupp.csv
\set orders_path :csv_dir orders.csv
\set lineitem_path :csv_dir lineitem.csv

copy customer from :'customer_path' delimiter '|';
copy supplier from :'supplier_path' delimiter '|';
copy nation from :'nation_path' delimiter '|';
copy region from :'region_path' delimiter '|';
copy part from :'part_path' delimiter '|';
copy partsupp from :'partsupp_path' delimiter '|';  
copy orders from :'orders_path' delimiter '|'; 
copy lineitem from :'lineitem_path' delimiter '|';

commit work;
