PGPASSWORD=<db-password> psql -h <db-public-ip> -d postgres -U postgres -c "\copy employee_attrition TO 'data.csv' WITH (FORMAT CSV, HEADER)"
python train_model.py
latest=$(ls -t mlruns/0/ | head -1)
gsutil cp -r mlruns/0/$latest/artifacts/model gs://ml2-model-bucket/production
rm data.csv