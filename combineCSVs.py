import os
import csv
from pathlib import Path


first_row = ["load","throughput","p99","num rebalances","avg service time"]

def get_folders_by_creation_order():
    """Returns a list of folders in the current directory sorted by creation time."""
    parent_dir = Path.cwd()
    #folders = [f for f in parent_dir.iterdir() if f.is_dir()]
    #folders.sort(key=lambda f: f.stat().st_ctime)

    folders = sorted([f for f in parent_dir.iterdir() if f.is_dir()], key=lambda f: f.name)
    return folders

def merge_csv_files(output_file):
    """Merges all lines of result.csv files from folders in creation order into one CSV file,
       prepending the folder name to each row."""
    folders = get_folders_by_creation_order()
    
    with open(output_file, 'w', newline='', encoding='utf-8') as outfile:
        writer = csv.writer(outfile)
        writer.writerow(first_row)
        for folder in folders:
            csv_file = folder / "result.csv"
            if csv_file.exists():
                with open(csv_file, 'r', encoding='utf-8') as infile:
                    reader = csv.reader(infile)
                    headers = next(reader, None)  # Read header
                        
                    for row in reader:
                        writer.writerow([folder.name] + row)  # Prepend folder name and append row

if __name__ == "__main__":
    one, two, three, four, dir, tp, seven = str(Path.cwd()).split("/")
    output_csv = dir+"_"+tp+"_merged_results.csv"
    merge_csv_files(output_csv)
    print(f"Merged CSV file saved as: {output_csv}")
