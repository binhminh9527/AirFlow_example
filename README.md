# Airflow Docker Compose Example

## Overview
This repository contains a minimal Docker Compose setup for running Apache Airflow (webserver and scheduler) with a Postgres metadata database. Place your DAGs in the `dags/` folder and they will be mounted into the Airflow container.

## Files
- `docker-compose.yaml`: Docker Compose configuration defining `postgres`, `webserver`, and `scheduler` services.
- `dags/`: Directory to place your DAG Python files.
- `logs/`: (Optional) Airflow logs directory (created/used by the container).
- `plugins/`: (Optional) Place custom Airflow plugins here.

## Prerequisites
- Docker
- Docker Compose (v1 or v2 compatible)

Verify Docker is available:

```
docker --version
docker-compose --version
```

## What the compose file does
- `postgres` service: runs Postgres 15 and exposes port `5432` to the host.
- `webserver` service: runs `apache/airflow:2.9.0`, maps host `./dags` to `/opt/airflow/dags`, and exposes port `8080` for the Airflow UI.
- `scheduler` service: runs `apache/airflow:2.9.0` and mounts the same `dags` folder so scheduled DAGs are discovered.

Important environment variable in the compose file:
- `AIRFLOW__CORE__SQL_ALCHEMY_CONN`: points Airflow to the Postgres metadata DB using the Compose service name `postgres`.

## Quick Start
From the repository root (where `docker-compose.yaml` is located):

- Start services in the background:

```
docker-compose up -d
```

- Initialize the Airflow database (run once):

```
docker-compose run --rm webserver airflow db init
```

- Create an admin user (update username/email/password as desired):

```
docker-compose run --rm webserver airflow users create \
  --username admin \
  --firstname Admin \
  --lastname User \
  --role Admin \
  --email admin@example.com \
  --password admin
```

- (Optional) If you didn't run `docker-compose up -d` earlier, start it now so the `webserver` and `scheduler` services run:

```
docker-compose up -d webserver scheduler
```

- Open the Airflow UI in your browser:


```
http://localhost:8080
```

If you are running Docker inside WSL2 (not Docker Desktop), and cannot access via `localhost`, get your WSL IP with:

```
hostname -I
```

Then open `http://<WSL_IP>:8080` in your Windows browser. For persistent access, see the troubleshooting section for port forwarding tips.

Log in with the user you created above.

## Stopping and cleaning up
- Stop containers:

```
docker-compose down
```

- Remove containers, networks and volumes (use with caution — this removes DB data):

```
docker-compose down -v
```

## View logs and troubleshooting
- Follow logs for the webserver:

```
docker-compose logs -f webserver
```

- Follow logs for the scheduler:

```
docker-compose logs -f scheduler
```

- If the webserver cannot connect to Postgres, ensure `postgres` is healthy and reachable. Confirm network and the `SQL_ALCHEMY_CONN` in `docker-compose.yaml`.

- If DAGs are not appearing, ensure:
  - DAG files are in the host `dags/` directory (not nested deeper).
  - File permissions allow the container to read them. If you see permission errors, try:

```
chown -R $USER:$USER ./dags
```

or adjust permissions appropriately for your environment.

## Adding DAGs, plugins and connections
- Add DAG files to the `dags/` directory on the host. They will be picked up automatically.
- Add custom plugins to the `plugins/` directory.
- To add connections from the CLI:

```
docker-compose run --rm webserver airflow connections add 'my_conn' --conn-uri 'postgresql://user:pw@host:5432/db'
```

Or add them from the Airflow UI under `Admin -> Connections`.

## Notes & Tips
- The `webserver` and `scheduler` images are both `apache/airflow:2.9.0`. If you need a different version, update `docker-compose.yaml`.
- If you plan on running this in production or sharing state across restarts, consider using named volumes for the Postgres data and a more robust production-ready Compose file or Kubernetes deployment.

---

If you want, I can:
- Add a sample example DAG to `dags/`.
- Extend `docker-compose.yaml` with more useful defaults (Airflow user UID/GID, persistent volumes, redis/flower, etc.).

