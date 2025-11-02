import os
import pandas as pd
import matplotlib.pyplot as plt
import sys

import argparse

parser = argparse.ArgumentParser()
parser.add_argument("-t", "--tp", type=int, help="traffic pattern")
parser.add_argument("-c", "--column", type=int, help="column id of data to plot 2:p99latency, 3:numrebalances, 4:avgservicetime")

args = parser.parse_args()
tp = args.tp
column = args.column

# List of folder names
folders = [
    "ubench_ro_kernel_scq",
    "ubench_ro_kernel_sscq",
    "ubench_ro_kernel_bitvector",
    "ubench_ro_kernel_affgeq1",
    "ubench_ro_kernel_affgeq2",
    "ubench_ro_kernel_affgeq4"
]

titles = {
    "ubench_ro_kernel_affgeq4": "JAC_4",
    "ubench_ro_kernel_affgeq2": "JAC_2",
    "ubench_ro_kernel_affgeq1": "JAC_1",
    "ubench_ro_kernel_bitvector": "JSC",
    "ubench_ro_kernel_sscq": "grp_sh",
    "ubench_ro_kernel_scq": "grp_ex"
}

tps = {
    20: "20%",
    21: "10%",
    22: "100%"
}

yaxis = [
    "load",
    "tput",
    "p99latency",
    "numrebalances",
    "avgservicetime",
]

plt.figure(figsize=(3.5, 2.5))

# Load and plot data for each folder
for folder in folders:
    csv_filename = f"{folder}/{folder}_20_1024.csv"
    if not os.path.exists(csv_filename):
        print(f"Warning: {csv_filename} not found.")
        continue

    df = pd.read_csv(csv_filename)
    try:
        x = pd.to_numeric(df.iloc[:, 1], errors="coerce")  # 2nd column: throughput
        y = pd.to_numeric(df.iloc[:, column], errors="coerce")  # 3rd column: p99 latency
        lat = pd.to_numeric(df.iloc[:, 2], errors="coerce")

        index = 0
        while index < len(lat):
            if lat[index] > 100000:
                break  # Exit the loop if a value under 50 is found
            index += 1

        x = x[:index]
        y = y[:index]

        if column == 2 or column == 4:
            y = [num / 1E3 for num in y]
        elif column == 1:
            y = [req / reb if reb != 0 else float('inf') for req, reb in zip(x, y)]
        elif column == 3: 
            y = y
        else: print("wrong column argument")

        x = [num / 1E6 for num in x]

        plt.plot(x, y, linewidth = 2.0, label=titles[folder])
    except Exception as e:
        print(f"Error processing {csv_filename}: {e}")
        continue

plt.tick_params(axis='both', labelsize=16)

if column == 2:
    plt.ylabel('p99 latency (us)',fontsize=20)
elif column == 3:
    plt.ylabel('requests/rebalances',fontsize=20)
elif column == 4:
    plt.ylabel('avg service time (us)',fontsize=20)

plt.xlabel('Load (MRPS)',fontsize=20)

if column == 2:
    plt.ylim(0, 50)

plt.legend(["grp_ex", "grp_sh", "JSC", "JAC_1", "JAC_2", "JAC_4"], loc = "center left",bbox_to_anchor=(1,0.5),fontsize=14,framealpha=0.5, ncol=6)
plt.grid(True)
plt.tight_layout()
plt.savefig('ubench_ro_'+tps[tp]+'_'+yaxis[column]+'.pdf',bbox_inches='tight')


# Save the plot
plt.close()
