from datetime import datetime, timedelta
import logging
import os

from airflow import DAG
from airflow.operators.python import PythonOperator
from airflow.operators.bash import BashOperator

from dowload_task.download_flood_data import download_flood_data as download_flood_data

def print_hello():
    logging.info("Hello from Airflow!")


with DAG(
    dag_id="Flood_Customers_DAG",
    start_date=datetime(2023, 1, 1),
    schedule_interval="@daily",
    catchup=False,
    default_args={
        "retries": 1,
        "retry_delay": timedelta(seconds=5),
    },
    description="A simple example DAG",
) as dag:
    # original task
    hello_task = PythonOperator(
        task_id="hello_task",
        python_callable=print_hello,
    )

    # sequential task that runs after hello_task

    Download_task = PythonOperator(
        task_id="Download_task",
        python_callable=download_flood_data,
        op_kwargs={"url": "https://example.com/flood_data.csv", "output_path": "data/flood_data.csv"},
    )

    # a task that runs in parallel with the sequential task (both depend on hello_task)
    IntersectCalculation_bin = os.path.normpath(os.path.join(os.path.dirname(__file__),"bin", "IntersectCalculation_bin"))

    IntersectCalculation = BashOperator(
        task_id="IntersectCalculation",
        bash_command=IntersectCalculation_bin,
    )
    # Dependencies:

    hello_task >> Download_task
    Download_task >> IntersectCalculation