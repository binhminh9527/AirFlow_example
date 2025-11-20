docker compose down
docker compose up -d polygons_db results_db postgres
docker compose run webserver airflow db init
docker compose run webserver airflow users create --username admin --password admin --firstname Admin --lastname User --role Admin --email admin@example.com
docker compose up -d webserver scheduler
