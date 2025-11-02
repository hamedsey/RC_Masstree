def count_remaps(connection_list):
    # Dictionary to store the current core of each connection
    connection_to_core = {}
    # Counter for remappings
    remap_count = 0

    # Iterate through each connection, core pair in the list
    for connection, core in connection_list:
        # Check if the connection exists and is mapped to a different core
        if connection in connection_to_core:
            if connection_to_core[connection] != core:
                remap_count += 1
                # Update the core for this connection
                connection_to_core[connection] = core
        else:
            # New connection, add to dictionary
            connection_to_core[connection] = core

    return remap_count

import numpy as np
from statistics import mean
import argparse
import csv
import os, glob
import sys


latencies = []
timestamps = []
conns = []
cores = []



##prep for csv
first_row = []
with open('rebalance-result.csv', 'w') as f:

    # create the csv writer
    writer = csv.writer(f)

    # write a row to the csv file
    first_row.append("bucket")
    first_row.append("size")
    first_row.append("99%")

    writer.writerow(first_row)
##


    # Open log file
    log_file = open("output.log", "w")

    # Redirect standard output and error to the log file
    sys.stdout = log_file
    sys.stderr = log_file


    #latencies = [8,4,2,1]  # Your latency values
    #timestamps = [8,4,2,1]  # Your timestamp values

    file_list_lat = glob.glob("*.result") 
    file_list_lat = sorted(file_list_lat, key = os.path.getmtime)

    file_list_lat_ts = glob.glob("*.timestamps") 
    file_list_lat_ts = sorted(file_list_lat_ts, key = os.path.getmtime) 

    file_list_core = glob.glob("*.core") 
    file_list_core = sorted(file_list_core, key = os.path.getmtime) 

    file_list_conn = glob.glob("*.conn") 
    file_list_conn = sorted(file_list_conn, key = os.path.getmtime) 

    #results = [[]]

    #p90_list  = []
    #p95_list  = []
    #p99_list  = []
    #p999_list = []

    for filename in file_list_lat:
        latencies = np.loadtxt(filename)
        #data_mean = mean(data)
        #maximum = max(data)
        #p90 = np.percentile(data, 90)
        p95 = np.percentile(latencies, 95)
        p99 = np.percentile(latencies, 99)
        #p999 = np.percentile(data, 99.9)

        print(filename)
        print(len(latencies))
        print("p95 = "+str(p95*4.6*16))
        print("p99 = "+str(p99*4.6*16))
        #result = [p90, p95, p99, p999]
        #print(result)
        #results.append(result)

    for filename in file_list_lat_ts:
        timestamps = np.loadtxt(filename)
        print(filename)
        print(len(timestamps))

    for filename in file_list_core:
        cores = np.loadtxt(filename)
        print(filename)
        print(len(cores))

    for filename in file_list_conn:
        conns = np.loadtxt(filename)
        print(filename)
        print(len(conns))

    #with open('result/result.csv', 'w') as f:
        # create the csv writer
    #    writer = csv.writer(f)

        # write a row to the csv file
    #    writer.writerow(["90%", "95%", "99%", "99.9%"])
    #    writer.writerows(results)


    numEpochs = 100
    epochRuntime = 1
    numBuckets = 100

    t = int(numEpochs*epochRuntime*1000000000)  # max time 10s
    n = int(t/numBuckets)  # interval size 0.1s

    # Initialize the buckets
    buckets = []
    for i in range(0, int(t*2), n):
        bucket = []  # Each bucket is labeled by its interval range
        buckets.append(bucket)

    print("created buckets")

    outOfBounds = 0
    # Sort latencies into buckets based on their timestamp range
    for timestamp, latency in zip(timestamps, latencies):
        #if timestamp < t:
        if int(timestamp//n) > len(buckets): 
            outOfBounds = outOfBounds + 1
            continue
        buckets[int(timestamp//n)].append(latency)

    print("placed latencies into buckets")
    print("outOfBounds = " + str(outOfBounds))


    # Calculate and print the 99th percentile latency for each bucket
    i = 0
    for latencies in buckets:
        results = []
        results.append(i)
        
        if latencies:  # Ensure the bucket is not empty
            percentile_99 = np.percentile(latencies, 99)
            print(f"{i}: size = {len(latencies)} ... p99 latency = {percentile_99*4.6*16}")
            results.append(len(latencies))
            results.append(percentile_99*4.6*16)
        else:
            print(f"{i}: No data")
            results.append(0)
            results.append(0)
        
        writer.writerow(results)
        i = i + 1


    '''
    timestamps = [8,2,4,1]  # Your timestamp values
    conns = [9,442,200,11]  # Your timestamp values
    cores = [81,40,22,15]  # Your timestamp values
    '''

    # Combine latencies and timestamps, sort by timestamps, then unzip
    sorted_pairs = zip(timestamps, conns, cores)
    
    # Converting to list
    sorted_pairs = list(sorted_pairs)
    
    # Using sorted and lambda
    res = sorted(sorted_pairs, key = lambda x: x[0])

    sorted_timestamps, sorted_conns, sorted_cores  = zip(*res)

    '''
    print(res)
    print(sorted_timestamps)
    print(sorted_latencies)
    print(sorted_conns)
    print(sorted_cores)
    '''

    # Initialize the buckets
    sortedBuckets = []
    for i in range(0, int(t*2), n):
        bucket = []  # Each bucket is labeled by its interval range
        sortedBuckets.append(bucket)

    print("created sortedBuckets")

    # Sort latencies into buckets based on their timestamp range
    for timestamp, conn, core in res:
        #if timestamp < t:
        if int(timestamp//n) > len(sortedBuckets): 
            outOfBounds = outOfBounds + 1
            continue

        sortedBuckets[int(timestamp)//int(n)].append((conn,core))

    print("outOfBounds = " + str(outOfBounds))

    second_row = []
    second_row.append("bucket")
    second_row.append("# of rebalances")
    writer.writerow(second_row)

    remaps = []
    connection_to_core = {}

    for i in range(len(sortedBuckets)):
        result = []

        # Dictionary to store the current core of each connection
        # Counter for remappings
        remap_count = 0

        # Iterate through each connection, core pair in the list
        for connection, core in sortedBuckets[i]:
            # Check if the connection exists and is mapped to a different core
            if connection in connection_to_core:
                if connection_to_core[connection] != core:
                    remap_count = remap_count + 1
                    # Update the core for this connection
                    connection_to_core[connection] = core
            else:
                # New connection, add to dictionary
                connection_to_core[connection] = core

        
        #numRemaps = count_remaps(sortedBuckets[i])
        
        result.append(i)
        result.append(remap_count)
        writer.writerow(result)

        remaps.append(remap_count)

    print(remaps)

# Close log file when done
log_file.close()