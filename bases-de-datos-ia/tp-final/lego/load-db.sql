-- themes

create table lego.themes (
	id serial not null,
	"name" varchar(100) null,
	parent_id int null,
	constraint themes_pkey primary key (id),
	constraint themes_parent_id_fkey foreign key (parent_id) references lego.themes(id)
);

copy lego.themes(id, "name", parent_id)
from '/lego/themes.csv'
delimiter ','
csv header;


-- sets

create table lego."sets" (
	set_num varchar(50) not null,
	"name" varchar(100) null,
	"year" int null,
	theme_id int null,
	num_parts int null,
	constraint sets_pkey primary key (set_num),
	constraint sets_theme_id_fkey foreign key (theme_id) references lego.themes(id)
);

copy lego."sets"(set_num, "name", "year", theme_id, num_parts)
from '/lego/sets.csv'
delimiter ','
csv header;


-- inventories

create table lego.inventories (
	id serial not null,
	"version" int null,
	set_num varchar(50) null,
	constraint inventories_pkey primary key (id),
	constraint inventories_set_num_fkey foreign key (set_num) references lego."sets"(set_num)
);

copy lego.inventories(id, "version", set_num)
from '/lego/inventories.csv'
delimiter ','
csv header;


-- inventory_sets

create table lego.inventory_sets (
	inventory_id int not null,
	set_num varchar(50) not null,
	quantity int null,
	constraint inventory_sets_pkey primary key (inventory_id, set_num),
	constraint inventory_sets_inventory_id_fkey foreign key (inventory_id) references lego.inventories(id),
	constraint inventory_sets_set_num_fkey foreign key (set_num) references lego."sets"(set_num)
);

copy lego.inventory_sets(inventory_id, set_num, quantity)
from '/lego/inventory_sets.csv'
delimiter ','
csv header;


-- colors

create table lego.colors (
	id serial not null,
	"name" varchar(100) null,
	rgb varchar(6) null,
	is_trans varchar(1) null,
	constraint colors_pkey primary key (id)
);

copy lego.colors(id, "name", rgb, is_trans)
from '/lego/colors.csv'
delimiter ','
csv header;

alter table lego.colors
alter column is_trans
set data type boolean
	using
case
	when is_trans = 't' then true
	when is_trans = 'f' then false
	else null
end;


-- part_categories

create table lego.part_categories (
	id serial not null,
	"name" varchar(100) null,
	constraint part_categories_pkey primary key (id)
);

copy lego.part_categories(id, "name")
from '/lego/part_categories.csv'
delimiter ','
csv header;


-- parts

create table lego.parts (
	part_num varchar(50) not null,
	"name" varchar(255) null,
	part_cat_id int null,
	constraint parts_pkey primary key (part_num),
	constraint parts_part_cat_id_fkey foreign key (part_cat_id) references lego.part_categories(id)
);

copy lego.parts(part_num, "name", part_cat_id)
from '/lego/parts.csv'
delimiter ','
csv header;


-- inventory_parts

create table lego.inventory_parts (
	id serial not null,
	inventory_id int null,
	part_num varchar(50) null,
	color_id int null,
	quantity int null,
	is_spare varchar(1) null,
	constraint inventory_parts_pkey primary key (id),
	constraint inventory_parts_inventory_id_fkey foreign key (inventory_id) references lego.inventories(id),
--	constraint inventory_parts_part_num_fkey foreign key (part_num) references lego.parts(part_num),
	constraint inventory_parts_color_id_fkey foreign key (color_id) references lego.colors(id)
);

copy lego.inventory_parts(inventory_id, part_num, color_id, quantity, is_spare)
from '/lego/inventory_parts.csv'
delimiter ','
csv header;

with missing_parts as (
	select ip.part_num
	from lego.inventory_parts ip
	where not exists (
		select
		from lego.parts p
		where p.part_num = ip.part_num 
	)
)

update lego.inventory_parts ip
set part_num = NULL
from missing_parts mp
where ip.part_num = mp.part_num;

alter table lego.inventory_parts
add constraint inventory_parts_part_num_fkey foreign key (part_num) references lego.parts(part_num),
alter column is_spare
set data type boolean
	using
case
	when is_spare = 't' then true
	when is_spare = 'f' then false
	else null
end;

