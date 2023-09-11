select
sum(l_extendedprice * l_discount) as revenue
from
lineitem
where
l_shipdate >= date '1998-01-01'
and l_quantity < 10
limit 1;
