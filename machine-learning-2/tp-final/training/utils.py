import os
from pathlib import Path
from google.cloud import storage
from sklearn.metrics import (
    balanced_accuracy_score,
    roc_auc_score,
    precision_score,
    recall_score,
)


def upload_files(bucket_name, folder_name):
    storage_client = storage.Client()
    bucket = storage_client.get_bucket(bucket_name)
    for path, _, files in os.walk(folder_name):
        for name in files:
            path_local = os.path.join(path, name)
            blob_path = path_local.replace("\\", "/")
            blob = bucket.blob(blob_path)
            blob.upload_from_filename(path_local)


def download_files(bucket_name, bucket_dir):
    storage_client = storage.Client()
    bucket = storage_client.bucket(bucket_name)
    blobs = bucket.list_blobs(prefix=bucket_dir)
    for blob in blobs:
        if blob.name.endswith("/"):
            continue
        file_split = blob.name.split("/")
        directory = "/".join(file_split[0:-1])
        Path(directory).mkdir(parents=True, exist_ok=True)
        blob.download_to_filename(blob.name)


def report_scores(y_test, y_pred, y_proba):
    message = [
        f"Scores Report:",
        f"AUC ROC: {roc_auc_score(y_test, y_proba[:, 1]):.4f}",
        f"Balanced Accuracy: {balanced_accuracy_score(y_test, y_pred):.4f}",
        f"Precision: {precision_score(y_test, y_pred):.4f}",
        f"Recall: {recall_score(y_test, y_pred):.4f}",
    ]
    message = "\n".join(message)

    print(message)
