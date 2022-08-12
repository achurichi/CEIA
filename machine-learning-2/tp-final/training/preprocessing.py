import pandas as pd
import numpy as np
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import StandardScaler
from constants import RANDOM_STATE, CATEGORICAL_VARIABLES, VARIABLES_TO_KEEP


def split(df):
    # Definimos la variable objetivo a predecir (y) y las feature (X)
    y = df["attrition"]
    X = df.drop("attrition", axis=1)

    # Dividimos el set de datos en entrenamiento y testeo
    X_train, X_test, y_train, y_test = train_test_split(
        X, y, test_size=0.20, random_state=RANDOM_STATE, stratify=y
    )

    # Estandarizamos los datos
    scaler = StandardScaler().fit(X_test)
    X_train = pd.DataFrame(scaler.transform(X_train), columns=X_train.columns)
    X_test = pd.DataFrame(scaler.transform(X_test), columns=X_test.columns)

    return X_train, X_test, y_train, y_test


def preprocess_dataset(df):
    # Convertir variables categoricas
    df["attrition"] = df["attrition"].replace(to_replace=["No", "Yes"], value=[0, 1])
    df["overtime"] = df["overtime"].replace(to_replace=["No", "Yes"], value=[0, 1])

    df_numerical = df.drop(columns=CATEGORICAL_VARIABLES, axis=1)
    df_categorical = pd.get_dummies(df[CATEGORICAL_VARIABLES], prefix_sep="")
    df_categorical = df_categorical.rename(columns=str.lower)

    df = pd.concat([df_numerical, df_categorical], axis=1)

    # Nos quedamos solamente con las columnas que aportan informacion
    df = df[VARIABLES_TO_KEEP]
    df = df.reindex(sorted(df.columns), axis=1)

    return df
