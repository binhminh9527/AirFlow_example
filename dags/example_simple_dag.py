from airflow import DAG
from airflow.operators.python import PythonOperator
from datetime import datetime, timedelta
import logging


def print_hello():
    logging.info("Hello from Airflowwwwwwwwwwwwwwwww!")


def print_hello_seq():
    logging.info("Hello from sequential task")


def print_parallel():
    logging.info("Hello from parallel task")


def final_task_func():
    logging.info("Final task after parallel tasks complete")


with DAG(
    dag_id="example_simple_dag",
    start_date=datetime(2023, 1, 1),
    schedule_interval="@daily",
    catchup=False,
    default_args={
        "retries": 1,
        "retry_delay": timedelta(minutes=5),
    },
    description="A simple example DAG",
) as dag:
    # original task
    hello_task = PythonOperator(
        task_id="hello_task",
        python_callable=print_hello,
    )

    # sequential task that runs after hello_task
    hello_task_seq = PythonOperator(
        task_id="hello_task_seq",
        python_callable=print_hello_seq,
    )

    # a task that runs in parallel with the sequential task (both depend on hello_task)
    hello_task_parallel = PythonOperator(
        task_id="hello_task_parallel",
        python_callable=print_parallel,
    )

    # final task that runs after both branches complete
    final_task = PythonOperator(
        task_id="final_task",
        python_callable=final_task_func,
    )

    # Dependencies:

    hello_task >> hello_task_seq
    [hello_task_seq,hello_task_parallel] >> final_task