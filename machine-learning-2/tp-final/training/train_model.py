import pandas as pd
import mlflow
from imblearn.over_sampling import RandomOverSampler
from sklearn.linear_model import LogisticRegression
from preprocessing import split, preprocess_dataset
from utils import report_scores
from constants import RANDOM_STATE

# Cargar Dataset
df = pd.read_csv("data.csv")
# df = pd.read_csv("/tf/notebooks/CEIA/machine-learning-2/tp-final/training/data.csv")
df = preprocess_dataset(df)

# Entrenamiento del modelo
X_train, X_test, y_train, y_test = split(df)

oversampler = RandomOverSampler(sampling_strategy="minority", random_state=RANDOM_STATE)
X_train, y_train = oversampler.fit_resample(X_train, y_train)

model = LogisticRegression(class_weight="balanced")
mlflow.start_run()
model.fit(X_train, y_train)
mlflow.sklearn.log_model(model, "model")

y_pred = model.predict(X_test)
y_proba = model.predict_proba(X_test)

report_scores(y_test, y_pred, y_proba)
