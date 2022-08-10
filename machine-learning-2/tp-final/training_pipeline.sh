PGPASSWORD=<password> psql -h <public-ip> -d postgres -U postgres -c "\copy employee_attrition TO 'data.csv' WITH (FORMAT csv)"
python train_model.py
rm data.csv