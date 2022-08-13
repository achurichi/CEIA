import mlflow
from mlflow.tracking import MlflowClient
from pathlib import Path
from google.cloud import storage


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


connection_string = (
    "postgresql+psycopg2://postgres:<db-password>@<db-public-ip>:5432/postgres"
)

mlflow.set_tracking_uri(connection_string)
mlflow.set_registry_uri(connection_string)
client = MlflowClient()

model_path = client.get_latest_versions(
    name="sk-learn-logistic-regression", stages=["Production"]
)[0].source

download_files("ml2-model-bucket", model_path)
