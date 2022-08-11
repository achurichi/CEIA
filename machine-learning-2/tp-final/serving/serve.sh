gsutil cp -r gs://ml2-model-bucket/production/model /home/ml2-serving
screen -X -S inference quit
screen -S inference
mlflow models serve -m /home/ml2-serving/model --no-conda -h 0.0.0.0