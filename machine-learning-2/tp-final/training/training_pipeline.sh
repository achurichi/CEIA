PGPASSWORD=<db-password> psql -h <db-public-ip> -d postgres -U postgres -c "\copy employee_attrition TO 'data.csv' WITH (FORMAT CSV, HEADER)"
python train_model.py
rm data.csv