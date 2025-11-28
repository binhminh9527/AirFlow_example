from datetime import datetime, timedelta
import logging
import os

from airflow import DAG
from airflow.operators.python import PythonOperator
from airflow.operators.bash import BashOperator

from LoadParceltoPostgre.LoadParceltoPostgre import load_parcel_to_postgres_array as load_parcel_to_postgres_array

with DAG(
    dag_id="WildFire_Customers_DAG",
    start_date=datetime(2023, 1, 1),
    schedule_interval="@daily",
    catchup=False,
    default_args={
        "retries": 1,
        "retry_delay": timedelta(seconds=5),
    },
    description="A simple example DAG",
) as dag:
    # Polygon validation task
    PolygonValidator_bin = os.path.normpath(os.path.join(os.path.dirname(__file__),"bin", "PolygonValidator_bin"))
    
    Polygon_validate = BashOperator(
        task_id="Polygon_validation",
        bash_command=f"{PolygonValidator_bin} /opt/airflow/Parcel_Data/Parcel_data.shp && {PolygonValidator_bin} /opt/airflow/Dataset_Cali_Wildfire/Wildfires.shp",
    )

    # sequential task that runs after hello_task

    Download_task = PythonOperator(
        task_id="Download_task",
        python_callable=load_parcel_to_postgres_array,
    )

    # a task that runs in parallel with the sequential task (both depend on hello_task)
    IntersectCalculation_bin = os.path.normpath(os.path.join(os.path.dirname(__file__),"bin", "IntersectCalculation_bin"))

    IntersectCalculation = BashOperator(
        task_id="IntersectCalculation",
        bash_command=IntersectCalculation_bin,
    )

    VerifyDB = BashOperator(
        task_id="VerifyDB",
        bash_command="ls",
    )
    
    # Dependencies:
    Polygon_validate >> Download_task >> IntersectCalculation >> VerifyDB