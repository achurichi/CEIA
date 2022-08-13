python download_models.py
export MLFLOW_TRACKING_URI="postgresql+psycopg2://postgres:<db-password>@<db-public-ip>:5432/postgres"
mlflow models serve -m "models:/sk-learn-logistic-regression/Production" --no-conda -h 0.0.0.0