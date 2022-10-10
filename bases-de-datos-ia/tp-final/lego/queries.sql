-- 1) ¿Cuáles son los 10 colores más utilizados en los 90?

with sets_with_colors as (
	select distinct s.set_num, s."year", c."name" color
	from lego."sets" s
	inner join lego.inventories i on s.set_num = i.set_num
	inner join lego.inventory_parts ip on i.id = ip.inventory_id
	inner join lego.colors c on ip.color_id = c.id
	where s."year" between 1990 and 1999
)

select swc.color, count(*)
from sets_with_colors swc
group by swc.color
order by count(*) desc
limit 10;


-- 2) ¿Cuántas colores tenemos que son únicos?

with sets_with_colors as (
	select distinct s.set_num, c."name" color
	from lego."sets" s
	inner join lego.inventories i on s.set_num = i.set_num
	inner join lego.inventory_parts ip on i.id = ip.inventory_id
	inner join lego.colors c on ip.color_id = c.id
)

select swc.color
from sets_with_colors swc
group by swc.color
having count(*) = 1;


-- 3) ¿Cómo es la tendencia en cantidad de piezas por sets a través de los años?

select s."year", avg(s.num_parts)
from lego."sets" s
group by s."year"
order by s."year" asc;


--4) ¿Cuáles son las temáticas más populares de los 2000?

select t."name" theme, count(*)
from lego."sets" s
inner join lego.themes t on s.theme_id = t.id
where s."year" between 2000 and 2009
group by theme
order by count(*) desc;

