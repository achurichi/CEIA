import requests
from sqlalchemy import create_engine


def predict(external_request):
    external_request = external_request.json
    user_id = external_request["user_id"]
    data = external_request["data"]

    response = requests.post(
        "http://<serving-vm-public-ip>:5000/invocations",
        headers={"Content-Type": "application/json"},
        json=data,
    )

    engine = create_engine(
        "postgresql+psycopg2://postgres:<db-password>@<db-public-ip>:5432/postgres"
    )
    connection = engine.connect()
    for entry, pred in zip(data, response.json()):
        keys = ", ".join(str(key) for key in entry.keys())
        values = ", ".join(str(value) for value in entry.values())
        connection.execute(
            f"INSERT INTO public.predictions (user_id, {keys}, prediction) VALUES ('{user_id}', {values}, {str(pred)})"
        )

    return response.text
