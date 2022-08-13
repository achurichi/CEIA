import pandas as pd
import mlflow
from mlflow.tracking import MlflowClient
from imblearn.over_sampling import RandomOverSampler
from sklearn.linear_model import LogisticRegression
from preprocessing import split, preprocess_dataset
from utils import upload_files, report_scores
from constants import RANDOM_STATE

connection_string = (
    "postgresql+psycopg2://postgres:<db-password>@<db-public-ip>:5432/postgres"
)

mlflow.set_tracking_uri(connection_string)
mlflow.set_registry_uri(connection_string)
client = MlflowClient()

# Cargar Dataset
df = pd.read_csv("data.csv")
df = preprocess_dataset(df)

# Separacion en train y test
X_train, X_test, y_train, y_test = split(df)
oversampler = RandomOverSampler(sampling_strategy="minority", random_state=RANDOM_STATE)
X_train, y_train = oversampler.fit_resample(X_train, y_train)

# Entrenamiento del modelo
with mlflow.start_run() as run:
    model = LogisticRegression(class_weight="balanced")
    model.fit(X_train, y_train)
    mlflow.sklearn.log_model(model, "model")
    run_id = run.info.run_id

result = mlflow.register_model(
    f"mlruns/0/{run_id}/artifacts/model", "sk-learn-logistic-regression"
)

# Testing
y_pred = model.predict(X_test)
y_proba = model.predict_proba(X_test)
report_scores(y_test, y_pred, y_proba)

# Promote
client.transition_model_version_stage(
    name="sk-learn-logistic-regression",
    version=result.version,
    stage="Production",
    archive_existing_versions=True,
)

# Guardar el modelo
upload_files("ml2-model-bucket", f"mlruns/0/{run_id}/artifacts/model")
