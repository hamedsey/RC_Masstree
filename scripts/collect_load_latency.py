import numpy as np
from statistics import mean
import argparse
import csv
import os, glob
import sys


# Initialize parser
parser = argparse.ArgumentParser()

# Adding optional argument
parser.add_argument("-t", "--trafficPattern", help = "index corresponding to traffic pattern (e.g. FB(5),PC(6),SQ(7),NC(8),EXP(9),PCR(10))")
parser.add_argument("-q", "--queues", help = "number of queues")
parser.add_argument("-m", "--mech", help = "notification mechanism")

#2 cores
#generatedLoad = ["200000", "400000" ,"500000","600000", "700000", "725000", "750000", "775000", "800000", "825000", "850000", "875000", "900000", "1000000", "1100000", "1200000", "1300000", "1400000", "1500000", "1600000", "1700000", "1800000", "1900000", "2000000"]

#4 cores
#generatedLoad = ["200000", "600000", "1000000", "1400000", "1500000", "1600000", "1700000", "1800000", "1900000", "2000000", "2100000", "2200000", "2300000", "2400000", "2500000", "2600000", "2700000", "2800000", "2900000", "3000000"]
#generatedLoad = ["100000", "200000", "300000", "400000", "500000", "600000", "700000", "800000", "900000", "1000000", "1100000", "1200000", "1300000", "1400000", "1500000", "1600000", "1700000", "1800000", "1900000", "2000000"]

#8 cores
#generatedLoad = ["100000", "200000", "300000", "400000", "500000", "600000", "700000", "800000", "900000", "1000000", "1100000", "1200000", "1300000", "1400000", "1500000", "1600000", "1700000", "1800000", "1900000", "2000000"]
#generatedLoad = ["400000", "1200000", "2000000", "2800000", "3000000", "3200000", "3400000", "3600000", "3800000", "4000000", "4200000", "4400000", "4600000", "4800000", "5000000", "5200000", "5400000", "5600000", "5800000", "6000000";do


#12 cores
#generatedLoad = ["600000", "1800000", "3000000", "4200000", "4500000", "4800000", "5100000", "5400000", "5700000", "6000000", "6300000", "6600000", "6900000", "7200000", "7500000", "7800000", "8100000", "8400000", "8700000", "9000000"]
#generatedLoad = ["100000", "200000", "300000", "400000", "500000", "600000", "700000", "800000", "900000", "1000000", "1100000", "1200000", "1300000", "1400000", "1500000", "1600000", "1700000", "1800000", "1900000", "2000000"]

#14TP
#generatedLoad = ["2500000", "3000000", "3300000", "3600000", "3900000", "4200000", "4500000", "4800000", "5100000", "5400000", "5700000", "6000000"]
#generatedLoad = ["100000", "200000", "300000", "400000", "500000", "600000", "700000", "800000", "900000", "1000000", "1100000", "1200000"]
#generatedLoad = ["600000", "1800000", "3000000", "4200000", "4500000"]
#generatedLoad = ["100000", "200000", "300000", "400000", "500000", "600000", "700000", "800000"]

#generatedLoad = ["3000000", "4200000", "4500000", "4800000", "5100000", "5400000", "5700000", "6000000", "6300000", "6600000", "6900000", "7200000", "7500000", "7800000", "8100000"]
#fixed
#generatedLoad = ["3000000" , "4000000", "5000000" ,"5500000", "6000000", "6200000", "6400000", "6600000", "6800000", "7000000", "7200000", "7400000", "7600000", "7800000", "8000000"]

#bm
#generatedLoad = ["500000" , "1000000", "1500000" ,"2000000", "2200000", "2400000", "2600000", "2800000", "3000000", "3200000", "3400000", "3600000", "3800000", "4000000", "4200000", "4400000", "4600000"]
#rebal64
#generatedLoad = ["6000000", "6300000", "6600000", "6900000", "7200000", "7500000", "7800000", "8100000"]
#generatedLoad = ["500000", "1000000", "1500000", "2000000", "2200000", "2400000", "2600000", "2800000", "3000000", "3200000", "3400000", "3600000", "3800000", "4000000", "4200000", "4400000", "4600000"]
#rebalance32
#generatedLoad = ["1000000", "2000000", "2500000", "3000000", "3500000", "4000000", "4200000", "4400000", "4600000", "4800000", "5000000", "5200000", "5400000", "5600000", "5800000", "6000000"]
#rebalance16   
#generatedLoad = ["1000000", "2000000", "3000000", "4000000", "4200000", "4400000", "4600000", "4800000", "5000000", "5200000", "5400000", "5600000", "5800000", "6000000", "6200000", "6400000", "6600000"]     
#rebalance1
#generatedLoad = ["1000000", "2000000", "3000000", "4000000", "5000000", "5500000", "6000000", "6300000", "6600000", "6900000", "7200000", "7500000", "7800000", "8100000"]     
#bimodal 5x-10p
#generatedLoad = ["1000000", "2000000", "3000000", "4000000", "4500000", "4800000", "5100000", "5400000", "5700000", "6000000", "6300000", "6600000"]

#kernel
#generatedLoad = ["250000", "500000", "750000", "1000000", "1250000", "1500000", "1750000", "2000000", "2250000", "2500000", "2750000", "3000000", "3250000", "3500000", "3750000", "4000000", "4250000", "4500000", "4750000", "5000000"]
#fixed
#generatedLoad = ["500000", "1000000", "2000000", "3000000", "4000000", "5000000", "5500000", "6000000", "6200000", "6400000", "6600000", "6800000", "7000000", "7200000", "7400000", "7600000", "7800000", "8000000"]
generatedLoad = ["500000"]
'''

python3 collect_load_latency.py -t 21 -q 1024 -m 0 &
python3 collect_load_latency.py -t 21 -q 1024 -m 1 &
python3 collect_load_latency.py -t 21 -q 1024 -m 2 &

'''

# Read arguments from command line
args = parser.parse_args()
id = int(args.trafficPattern)
queues = int(args.queues)
mech = int(args.mech)

mechanisms = ["fixedSTsharedcqwarmup","sharedcq100bm", "bitvector100pagain", "sharedsharedcq100bm","bitvector100rebal64fixed","bitvector100rebal32fixed","bitvector100rebal16fixed","bitvector100rebal8fixed","bitvector100rebal1again","bitvector100rebal4fixed",
                "bitvector100bm5x10p","sharedcq100bm5x10p","sharedcqsharedcq100bm5x10p","bitvector100connaff","bitvector100connaf1", "bitvector100connaf4","bitvector100connaf8","bitvector100connaf16","bitvector100connaf32",
                "bitvectorfixedST","sharedcqfixedST","sharedsharedcqfixedST"]
trafficpatterns = ["NULL", "NULL", "NULL", "NULL", "NULL", "FB", "PC", "SQ", "NC", "EXP", "PCR","RR","TWENTYP","TENP","CONTWENTYP","CONTENP","FIFTYP","CONFIFTYP","MIXED", "FIFTYPRAND","TWENTYPRAND","TENPRAND","HUNDREDPRAND"]

print(mech)
mechanism = mechanisms[mech]

path = mechanism+"/"+str(id)+"/"+str(queues)+"/"

#Create results and graphs folders"
if not os.path.exists(path+"result"):
    os.makedirs(path+"result")
	#for mechanism in mechanisms:
    #os.makedirs('result/'+mechanisms[index])
          
#if not os.path.exists('result/'+mechanisms[index]):
#    os.makedirs('result/'+mechanisms[index])

clk_period = 4.6


#for mechanism in mechanisms: 
#for id in trafficPatternID:
print("now doing "+trafficpatterns[id])
p50_list  = []
p95_list  = []
p99_list  = []
p999_list = []

first_row = ["client load","measured load","median","95%", "99%", "99.9%"]

#for queues in queuesList:
print(queues)
#with open('result/'+mechanism+'/'+trafficpatterns[id]+'-'+mechanism+'-'+str(queues)+'-result.csv', 'w') as f:
with open(path+'result/'+trafficpatterns[id]+'-'+generatedLoad[0]+'-result1.csv', 'w') as f:

    # create the csv writer
    writer = csv.writer(f)

    # write a row to the csv file
    for i in range(queues):
        first_row.append("median")
        first_row.append("95%")
        first_row.append("99%")
        first_row.append("99.9%")

    writer.writerow(first_row)

    clientLoad = generatedLoad[0]
    #for clientLoad in generatedLoad:
    results = []

    file_list = glob.glob(path+str(clientLoad)+"/*.result");
    count = 0
    allData = []
    data = []

    for filename in file_list:
        print(filename)
        #if count == 0:
        name, ext = filename.split(".")
        junk0, measuredLoad, junk1, junk2 = name.split("_")
        print(measuredLoad)
        count = count + 1
        data = np.loadtxt(filename)

        if data.size > 0:
            results.append(clientLoad)
            results.append(int(measuredLoad))
            #print(len(allData))
            results.append((16*clk_period/1000)*np.percentile(data, 50))
            results.append((16*clk_period/1000)*np.percentile(data, 95))
            results.append((16*clk_period/1000)*np.percentile(data, 99))
            results.append((16*clk_period/1000)*np.percentile(data, 99.9))
        
            writer.writerow(results)

'''
# Initialize parser
parser = argparse.ArgumentParser()

# Adding optional argument
parser.add_argument("-d", "--directory", help = "input and output directory")

# Read arguments from command line
args = parser.parse_args()
input_dir = args.directory
if input_dir[-1] == "/":
    input_dir = input_dir[:-1]

folder_path_list = glob.glob(input_dir+"*")
if len(folder_path_list) == 0:
    sys.exit('Input directory incorrect')

for folder_path in folder_path_list:
    print(folder_path)
    file_list = glob.glob(folder_path+"/*.result") 
    file_list = sorted(file_list,key = os.path.getmtime)
    results = [[]]
    load_list = []
    p50_list  = []
    p95_list  = []
    p99_list  = []
    p999_list = []
    for filename in file_list:
        #print(filename)
        data = np.loadtxt(filename)
        data_mean = mean(data)
        maximum = max(data)
        p50 = np.percentile(data, 50)
        p95 = np.percentile(data, 95)
        p99 = np.percentile(data, 99)
        p999 = np.percentile(data, 99.9)
        config = filename.split('/')[1].split('.')[0]
        configs = config.split('_')
        result = [configs[0], configs[1], configs[2], p50, p95, p99, p999]
        results.append(result)
        load_list.append(int(configs[2])/1000000)
        p50_list.append(p50)
        p95_list.append(p95)
        p99_list.append(p99)
        p999_list.append(p999)
    with open('result/'+folder_path+'-result.csv', 'w') as f:
        # create the csv writer
        writer = csv.writer(f)

        # write a row to the csv file
        writer.writerow(["client load", "measured load","median","95%", "99%", "99.9%"])
        writer.writerows(results)
'''