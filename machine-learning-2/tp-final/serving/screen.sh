screen -ls | awk -vFS='\t|[.]' '/serve/ {system("screen -S "$2" -X quit")}'
screen -dmS serve sh serve.sh