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


def calculate_and_plot_percentage_over_epochs(data, epochs):
    # Prepare a list to store percentages for each epoch
    epoch_percentages = []

    for epoch in range(epochs):
        # Count the occurrences of each number in the current epoch
        counts = Counter(data[epoch])
        total = len(data[epoch])
        percentages = [(counts.get(i, 0) / total) * 100 for i in range(12)]
        epoch_percentages.append(percentages)

    # Convert to numpy array for easy plotting
    epoch_percentages = np.array(epoch_percentages).T

    # Plot the stacked bar graph
    x = range(1, epochs + 1)  # Epoch numbers
    bottom = np.zeros(epochs)
    colors = [plt.cm.tab20(i / 11) for i in range(12)]

    for i, percentages in enumerate(epoch_percentages):
        plt.bar(x, percentages, bottom=bottom, color=colors[i], label=str(i))
        bottom += percentages

    # Set y-axis to have a max value of 100
    plt.ylim(0, 100)

    # Add labels and legend
    plt.xlabel("Epoch")
    plt.ylabel("Percentage (%)")
    plt.title("Percentage Distribution of Integers (0-11) Over Epochs")
    plt.legend(title="Integers", bbox_to_anchor=(1.05, 1), loc='upper left')
    plt.tight_layout()
    plt.savefig('corePercentage.png', dpi=300, transparent=True)

    #plt.show()

    plt.clf()  # Clear the figure


import numpy as np
from statistics import mean
import argparse
import csv
import os, glob
import sys
import matplotlib.pyplot as plt
from collections import Counter

latencies = []
timestamps = []
conns = []
cores = []
#qds = []
#diffqds = []
avgServiceTime = []

first_row = ["load","p99","num rebalances","avg service time"]
only_result = []

##prep for csv

with open('result.csv', 'w') as f:

    # create the csv writer
    writer = csv.writer(f)

    # write a row to the csv file
    #first_row.append("bucket")
    #first_row.append("size")
    #first_row.append("99%")

    writer.writerow(first_row)
##


    # Open log file
    log_file = open("output2.log", "w")

    # Redirect standard output and error to the log file
    sys.stdout = log_file
    sys.stderr = log_file


    #latencies = [8,4,2,1]  # Your latency values
    #timestamps = [8,4,2,1]  # Your timestamp values

    file_list_lat = glob.glob("*.result") 
    file_list_lat = sorted(file_list_lat, key = os.path.getmtime)

    for filename in file_list_lat:
        name, ext = filename.split(".")
        junk0, measuredLoad, junk1, junk2 = name.split("_")
        print(measuredLoad)
        only_result.append(int(measuredLoad))


    file_list_lat_ts = glob.glob("*.timestamps") 
    file_list_lat_ts = sorted(file_list_lat_ts, key = os.path.getmtime) 

    file_list_core = glob.glob("*.core") 
    file_list_core = sorted(file_list_core, key = os.path.getmtime) 

    file_list_conn = glob.glob("*.conn") 
    file_list_conn = sorted(file_list_conn, key = os.path.getmtime) 

    '''
    file_list_qd = glob.glob("*.qd") 
    file_list_qd = sorted(file_list_qd, key = os.path.getmtime) 

    file_list_diffqd = glob.glob("*.diffqd") 
    file_list_diffqd = sorted(file_list_diffqd, key = os.path.getmtime) 
    '''

    file_list_avgServiceTime = glob.glob("*.servtime") 
    file_list_avgServiceTime = sorted(file_list_avgServiceTime, key = os.path.getmtime) 

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
        print("p95 = "+str(p95*4.6*16))
        print("p99 = "+str(p99*4.6*16))

        only_result.append(float(p99*4.6*16))

        print(len(latencies))
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
        print("now reading connections")
        conns = np.loadtxt(filename)
        print(filename)
        print(len(conns))

    '''
    for filename in file_list_qd:
        print("now reading qds")
        qds = np.loadtxt(filename)
        print(filename)
        mean_qd = mean(qds)
        p95_qd = np.percentile(qds, 95)
        p99_qd = np.percentile(qds, 99)
        p100_qd = np.percentile(qds,100)
        print("p95_qd = "+str(p95_qd))
        print("p99_qd = "+str(p99_qd))
        print("p100_qd = "+str(p100_qd))
        print(len(qds))

    for filename in file_list_diffqd:
        print("now reading diffqds")
        diffqds = np.loadtxt(filename)
        print(filename)
        p95_diffqd = np.percentile(diffqds, 95)
        p99_diffqd = np.percentile(diffqds, 99)
        p100_diffqd = np.percentile(diffqds,100)
        print("p95_diffqd = "+str(p95_diffqd))
        print("p99_diffqd = "+str(p99_diffqd))
        print("p100_diffqd = "+str(p100_diffqd))
        print(len(diffqds))
    '''
    for filename in file_list_avgServiceTime:
        print(filename)
        avgServiceTime = np.loadtxt(filename)
        print("printing avgServiceTime")
        print(avgServiceTime)

    #with open('result/result.csv', 'w') as f:
        # create the csv writer
    #    writer = csv.writer(f)

        # write a row to the csv file
    #    writer.writerow(["90%", "95%", "99%", "99.9%"])
    #    writer.writerows(results)


    numEpochs = 30 #4
    epochRuntime = 1 #25
    numBuckets = 30 #4

    t = int(numEpochs*epochRuntime*1000000000)  # max time 100s
    n = int(t/numBuckets)  # interval size 1s

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
            #results.append(len(latencies))
            #results.append(percentile_99*4.6*16)
        else:
            print(f"{i}: No data")
            #results.append(0)
            #results.append(0)
        
        #writer.writerow(results)
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
    #writer.writerow(second_row)

    remaps = []
    connection_to_core = {}

    print("len(sortedBuckets) = " + str(len(sortedBuckets)))

    for i in range(len(sortedBuckets)):
        #print("i = " + str(i))
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
        #writer.writerow(result)

        remaps.append(remap_count)


    # Slice the list to get the first n elements
    first_n = remaps[:numBuckets]
    mean_remaps = mean(first_n)
    print("mean remaps = " + str(mean_remaps))

    only_result.append(int(mean_remaps))
    
    if not avgServiceTime: only_result.append(int(0))
    else : only_result.append(int(avgServiceTime))
    writer.writerow(only_result)

    print(remaps)

    print("starting to plot QDs")
    '''
    print("sortedBuckets[i]")
    print(sortedBuckets[i])
    print("sortedBuckets[i][2]")
    print(sortedBuckets[i][2])
    print("sortedBuckets[i][0][2]")
    print(sortedBuckets[i][0][2])
    '''
    

    #plottimg core percentage per epoch

    # Example usage
    # Generate example data for 100 epochs
    #np.random.seed(42)
    #data = [np.random.randint(0, 12, 100) for _ in range(100)]
    '''
    allCores = []
    for i in range(0, int(t*2), n):
        bucket = []  # Each bucket is labeled by its interval range
        allCores.append(bucket)

    # Sort latencies into buckets based on their timestamp range
    for timestamp, conn, core, qd, diffqd in res:
        #if timestamp < t:
        if int(timestamp//n) > len(sortedBuckets): 
            continue

        allCores[int(timestamp)//int(n)].append(core)

    calculate_and_plot_percentage_over_epochs(allCores, numEpochs)
    '''
    
    #plotting qds per bucket
    '''
    x = []
    y = []
    qd_err_min = []
    qd_err_max = []
    result = []

    result.append("epoch number")
    result.append("qd mean")
    result.append("qd min")
    result.append("qd max")
    writer.writerow(result)
    
    print("len(sortedBuckets) = " + str(len(sortedBuckets)))

    for i in range(len(sortedBuckets)):
        #print("i = " + str(i))
        qds = []
        for connection, core, qd, diffqd in sortedBuckets[i]:
            #print("qd = " + str(qd))
            qds.append(qd)
            
        result = []

        if qds:
            x.append(i)
            result.append(i)

            qd_mean = mean(qds)
            y.append(qd_mean)
            result.append(qd_mean)

            qd_min = min(qds)
            qd_err_min.append(qd_min)
            result.append(qd_min)

            qd_max = max(qds)
            qd_err_max.append(qd_max)
            result.append(qd_max)

            writer.writerow(result)

    # Create the plot
    plt.errorbar(x, y, yerr=[qd_err_min, qd_err_max], fmt='o') #, #capsize=5)
    plt.xlabel('epoch')
    plt.ylabel('core queue depth')
    
    plt.axhline(y=p99_qd, color='r', linestyle=':')
    plt.axhline(y=p95_qd, color='r', linestyle='-')

    plt.savefig('qd.png', dpi=300, transparent=True)
    #plt.title('Max-Min Error Bars')
    plt.clf()


    x = []
    y = []
    diffqd_err_min = []
    diffqd_err_max = []
    result = []

    result.append("epoch number")
    result.append("diffqd mean")
    result.append("diffqd min")
    result.append("diffqd max")
    writer.writerow(result)
    
    print("len(sortedBuckets) = " + str(len(sortedBuckets)))

    for i in range(len(sortedBuckets)):
        #print("i = " + str(i))
        diffqds = []
        for connection, core, qd, diffqd in sortedBuckets[i]:
            diffqds.append(diffqd)
            
        result = []

        if diffqds:
            x.append(i)
            result.append(i)

            diffqd_mean = mean(diffqds)
            y.append(diffqd_mean)
            result.append(diffqd_mean)

            diffqd_min = min(diffqds)
            diffqd_err_min.append(diffqd_min)
            result.append(diffqd_min)

            diffqd_max = max(diffqds)
            diffqd_err_max.append(diffqd_max)
            result.append(diffqd_max)

            writer.writerow(result)

    # Create the plot
    plt.errorbar(x, y, yerr=[diffqd_err_min, diffqd_err_max], fmt='o') #, #capsize=5)
    plt.xlabel('epoch')
    plt.ylabel('core queue depth')
    
    plt.axhline(y=p99_diffqd, color='r', linestyle=':')
    plt.axhline(y=p95_diffqd, color='r', linestyle='-')

    plt.savefig('diffqd.png', dpi=300, transparent=True)

    plt.clf() 

    ##plotting one line per core (avg, min, max)

    #x = []


    #result = [p90, p95, p99, p999]
    writer.writerow(["diffQDs"])
    writer.writerow(["epoch", "core", "min", "mean", "max", "p95", "p99"])

    
    perCoreStatsMin = [[],[],[],[],[],[],[],[],[],[],[],[]] #stores stats per core for all epochs
    perCoreStatsMean = [[],[],[],[],[],[],[],[],[],[],[],[]] #stores stats per core for all epochs
    perCoreStatsMax = [[],[],[],[],[],[],[],[],[],[],[],[]] #stores stats per core for all epochs
    perCoreStatsp95 = [[],[],[],[],[],[],[],[],[],[],[],[]] #stores stats per core for all epochs
    perCoreStatsp99 = [[],[],[],[],[],[],[],[],[],[],[],[]] #stores stats per core for all epochs
    
    for i in range(len(sortedBuckets)):
        #x.append(i)
        perCoreQD = [[],[],[],[],[],[],[],[],[],[],[],[]] #stores the QD for each epoch
        for connection, core, qd, diffqd in sortedBuckets[i]:
            perCoreQD[int(core)].append(diffqd)
        #print("len(perCoreQD)" + str(len(perCoreQD)))
        for j in range(len(perCoreQD)):
            result = []
            result.append(i)
            result.append(j)
            
            #print(f"Index j : {j}, size perCoreQD[j]: {len(perCoreQD[j])}")

            if perCoreQD[j]: 
                perCoreQD_min = min(perCoreQD[j])
                perCoreStatsMin[j].append(perCoreQD_min)
                result.append(perCoreQD_min)

                perCoreQD_mean = mean(perCoreQD[j])
                perCoreStatsMean[j].append(perCoreQD_mean)
                result.append(perCoreQD_mean)

                perCoreQD_max = max(perCoreQD[j])
                perCoreStatsMax[j].append(perCoreQD_max)
                result.append(perCoreQD_max)

                perCoreQD_p95 = np.percentile(perCoreQD[j], 95)
                perCoreStatsp95[j].append(perCoreQD_p95)
                result.append(perCoreQD_p95)

                perCoreQD_p99 = np.percentile(perCoreQD[j], 99)
                perCoreStatsp99[j].append(perCoreQD_p99)
                result.append(perCoreQD_p99)

            writer.writerow(result)

    for j in range(len(perCoreQD)):
        plt.plot(x, perCoreStatsMin[j]) #, #capsize=5)

    # Create the plot
    plt.xlabel('epoch')
    plt.ylabel('diff core queue depth')
    plt.title('min')
    plt.savefig('diffqd_min.png', dpi=300, transparent=True)
    plt.clf()  # Clear the figure

    for j in range(len(perCoreQD)):
        plt.plot(x, perCoreStatsMean[j]) #, #capsize=5)

    # Create the plot
    plt.xlabel('epoch')
    plt.ylabel('diff core queue depth')
    plt.title('mean')
    plt.savefig('diffqd_mean.png', dpi=300, transparent=True)
    plt.clf()  # Clear the figure


    for j in range(len(perCoreQD)):
        plt.plot(x, perCoreStatsMax[j]) #, #capsize=5)

    # Create the plot
    plt.xlabel('epoch')
    plt.ylabel('diff core queue depth')
    plt.title('max')
    plt.savefig('diffqd_max.png', dpi=300, transparent=True)
    plt.clf()  # Clear the figure

    for j in range(len(perCoreQD)):
        plt.plot(x, perCoreStatsp95[j]) #, #capsize=5)

    # Create the plot
    plt.xlabel('epoch')
    plt.ylabel('diff core queue depth')
    plt.title('p95')
    plt.savefig('diffqd_p95.png', dpi=300, transparent=True)
    plt.clf()  # Clear the figure

    for j in range(len(perCoreQD)):
        plt.plot(x, perCoreStatsp99[j]) #, #capsize=5)

    # Create the plot
    plt.xlabel('epoch')
    plt.ylabel('core queue depth')
    plt.title('p99')
    plt.savefig('diffqd_p99.png', dpi=300, transparent=True)
    plt.clf()  # Clear the figure
    '''
# Close log file when done
log_file.close()