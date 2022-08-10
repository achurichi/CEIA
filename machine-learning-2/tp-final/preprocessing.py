import pandas as pd
import numpy as np
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import StandardScaler
from sklearn.ensemble import RandomForestClassifier
from constants import RANDOM_STATE, CATEGORICAL_VARIABLES, COLUMNS_TO_REMOVE


def split(df):
    # Definimos la variable objetivo a predecir (y) y las feature (X)
    y = df["Attrition"]
    X = df.drop("Attrition", axis=1)

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
    # Eliminar columnas que no aportan informacion
    df.drop(COLUMNS_TO_REMOVE, axis=1, inplace=True)

    # Convertir variables categoricas
    df["Attrition"] = df["Attrition"].replace(to_replace=["No", "Yes"], value=[0, 1])
    df["OverTime"] = df["OverTime"].replace(to_replace=["No", "Yes"], value=[0, 1])
    df["Gender"] = df["Gender"].replace(to_replace=["Male", "Female"], value=[0, 1])
    df["BusinessTravel"] = df["BusinessTravel"].replace(
        to_replace=["Non-Travel", "Travel_Rarely", "Travel_Frequently"], value=[0, 0, 1]
    )

    df_numerical = df.drop(columns=CATEGORICAL_VARIABLES, axis=1)
    df_categorical = pd.get_dummies(df.drop(columns=df_numerical.columns))
    df = pd.concat([df_numerical, df_categorical], axis=1)

    # Random Forest para sacar las 20 features mas importantes
    X_train, _, y_train, _ = split(df)

    model = RandomForestClassifier(
        n_estimators=10, class_weight="balanced", random_state=RANDOM_STATE
    )
    model.fit(X_train, y_train)

    feature_importances = pd.DataFrame(
        {"features": X_train.columns, "feature_importance": model.feature_importances_}
    )
    feature_importances.sort_values("feature_importance", ascending=False, inplace=True)

    important_columns = feature_importances.iloc[:20]["features"].to_numpy()
    important_columns = np.append(important_columns, ["Attrition"])

    return df[important_columns]
