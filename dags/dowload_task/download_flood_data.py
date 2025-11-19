import requests
import os

def download_flood_data(url: str, output_path: str):
    print(f"Downloading flood data from {url} to {output_path}")
    # os.makedirs(os.path.dirname(output_path), exist_ok=True)
    # response = requests.get(url)
    # response.raise_for_status()
    # with open(output_path, 'wb') as f:
    #     f.write(response.content)
    # print(f"Downloaded flood data to {output_path}")

if __name__ == "__main__":
    # Example usage
    url = "https://example.com/flood_data.csv"  # Replace with real data source
    output_path = "data/flood_data.csv"
    download_flood_data(url, output_path)
