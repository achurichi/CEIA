import requests

def predict(external_request):
    headers = {
        'Content-Type': 'application/json',
        'format': 'pandas-records'
    }

    response = requests.post('http://34.102.116.204:5000/invocations', headers=headers, json=external_request.json)

    return response.text
