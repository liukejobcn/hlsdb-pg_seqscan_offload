\timing on
select
sum(l_extendedprice * l_discount) as revenue
from
lineitem
where
l_shipdate >= date '1994-01-01'
and l_shipdate < date '1994-01-01' + interval '1' year
and l_receiptdate < date '1995-01-01' + interval '1' year
and l_quantity < 8
limit 1;
