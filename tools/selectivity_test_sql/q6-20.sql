select
  sum(l_extendedprice * l_discount) as revenue
from
  lineitem
where
  l_shipdate >= date '1984-01-01'
        and l_shipdate < date '1994-01-01' + interval '1' year
  and l_discount between 0.06 - 0.02 and 0.06 + 0.02
  and l_quantity < 80
limit 1;
