#!/usr/bin/env bash
#make
#i=1
#dirName=$(echo $1)
#serverQpn=$(echo $2)
#queues=$(echo $3)
#serverIPAddr=$(echo $4)
#ib_devname=$(echo $5)
#gid=$(echo $6)

dirName=$(echo $1)

: '
if [[ "$servTimeDist" == "FIXED" ]];
then
    servTimeDist=0
elif [[ "$servTimeDist" == "NORMAL" ]];
then
    servTimeDist=1
elif [[ "$servTimeDist" == "UNIFORM" ]];
then
    servTimeDist=2
elif [[ "$servTimeDist" == "EXPONENTIAL" ]];
then
    servTimeDist=3
elif [[ "$servTimeDist" == "BIMODAL" ]];
then
    servTimeDist=4
else
    echo "Unsupported request distribution mode! Pick one from: FIXED; NORMAL; UNIFORM; EXPONENTIAL."
fi
'
#if [ ! -d $dirName ];
#then
#    mkdir $dirName
#fi

#the mechanism is the dirName (i.e. LZCNT, AVX, SW, IDEAL SRQ)
#mkdir $dirName
: '
sudo /etc/init.d/openibd restart
sudo modprobe mlx_accel_tools
sudo mst start --with_fpga
sudo /./opt/Xilinx/Vivado/2016.2/bin/hw_serverpv
sudo mlx_fpga -d /dev/mst/mt4117_pciconf0_fpga_rdma load --factory
sudo mlx_fpga -d /dev/mst/mt4117_pciconf0_fpga_rdma query
sudo mcra /dev/mst/mt4117_pciconf0 0x5363c.12:1 0
sudo mcra /dev/mst/mt4117_pciconf0 0x5367c.12:1 0
sudo mcra /dev/mst/mt4117_pciconf0 0x53628.3:1 0
sudo mcra /dev/mst/mt4117_pciconf0 0x53668.3:1 0
sudo iptables -F; sudo iptables -t mangle -F
sudo iptables -D INPUT -m conntrack --ctstate INVALID -j DROP
sudo ethtool --set-priv-flags enp24s0 sniffer off
/root/disable-mellanox-shell-credits.sh
/home/hseyedro3/disable-mellanox-shell-credits.sh
sleep 20
'

echo performance > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
echo performance > /sys/devices/system/cpu/cpu1/cpufreq/scaling_governor
echo performance > /sys/devices/system/cpu/cpu2/cpufreq/scaling_governor
echo performance > /sys/devices/system/cpu/cpu3/cpufreq/scaling_governor
echo performance > /sys/devices/system/cpu/cpu4/cpufreq/scaling_governor
echo performance > /sys/devices/system/cpu/cpu5/cpufreq/scaling_governor
echo performance > /sys/devices/system/cpu/cpu6/cpufreq/scaling_governor
echo performance > /sys/devices/system/cpu/cpu7/cpufreq/scaling_governor
echo performance > /sys/devices/system/cpu/cpu8/cpufreq/scaling_governor
echo performance > /sys/devices/system/cpu/cpu9/cpufreq/scaling_governor
echo performance > /sys/devices/system/cpu/cpu10/cpufreq/scaling_governor
echo performance > /sys/devices/system/cpu/cpu11/cpufreq/scaling_governor
echo performance > /sys/devices/system/cpu/cpu12/cpufreq/scaling_governor
echo performance > /sys/devices/system/cpu/cpu13/cpufreq/scaling_governor
echo performance > /sys/devices/system/cpu/cpu14/cpufreq/scaling_governor
echo performance > /sys/devices/system/cpu/cpu15/cpufreq/scaling_governor
echo performance > /sys/devices/system/cpu/cpu16/cpufreq/scaling_governor
echo performance > /sys/devices/system/cpu/cpu17/cpufreq/scaling_governor
echo performance > /sys/devices/system/cpu/cpu18/cpufreq/scaling_governor
echo performance > /sys/devices/system/cpu/cpu19/cpufreq/scaling_governor
echo performance > /sys/devices/system/cpu/cpu20/cpufreq/scaling_governor
echo performance > /sys/devices/system/cpu/cpu21/cpufreq/scaling_governor
echo performance > /sys/devices/system/cpu/cpu22/cpufreq/scaling_governor
echo performance > /sys/devices/system/cpu/cpu23/cpufreq/scaling_governor
echo performance > /sys/devices/system/cpu/cpu24/cpufreq/scaling_governor
echo performance > /sys/devices/system/cpu/cpu25/cpufreq/scaling_governor
echo performance > /sys/devices/system/cpu/cpu26/cpufreq/scaling_governor
echo performance > /sys/devices/system/cpu/cpu27/cpufreq/scaling_governor
echo performance > /sys/devices/system/cpu/cpu28/cpufreq/scaling_governor
echo performance > /sys/devices/system/cpu/cpu29/cpufreq/scaling_governor
echo performance > /sys/devices/system/cpu/cpu30/cpufreq/scaling_governor
echo performance > /sys/devices/system/cpu/cpu31/cpufreq/scaling_governor
echo performance > /sys/devices/system/cpu/cpu32/cpufreq/scaling_governor
echo performance > /sys/devices/system/cpu/cpu33/cpufreq/scaling_governor
echo performance > /sys/devices/system/cpu/cpu34/cpufreq/scaling_governor
echo performance > /sys/devices/system/cpu/cpu35/cpufreq/scaling_governor
echo performance > /sys/devices/system/cpu/cpu36/cpufreq/scaling_governor
echo performance > /sys/devices/system/cpu/cpu37/cpufreq/scaling_governor
echo performance > /sys/devices/system/cpu/cpu38/cpufreq/scaling_governor
echo performance > /sys/devices/system/cpu/cpu39/cpufreq/scaling_governor
echo performance > /sys/devices/system/cpu/cpu40/cpufreq/scaling_governor
echo performance > /sys/devices/system/cpu/cpu41/cpufreq/scaling_governor
echo performance > /sys/devices/system/cpu/cpu42/cpufreq/scaling_governor
echo performance > /sys/devices/system/cpu/cpu43/cpufreq/scaling_governor
echo performance > /sys/devices/system/cpu/cpu44/cpufreq/scaling_governor
echo performance > /sys/devices/system/cpu/cpu45/cpufreq/scaling_governor
echo performance > /sys/devices/system/cpu/cpu46/cpufreq/scaling_governor
echo performance > /sys/devices/system/cpu/cpu47/cpufreq/scaling_governor

: '
for queues in 64;do #1 8 16 32 64 128 256;do
    #mkdir $dirName"/"$queues
    for dist in 6 7 9;do #5 7 9 6 8;do
        #mkdir $dirName"/"$queues"/"$dist
        for load in 50000 100000 200000 300000 400000 425000 450000 475000 500000 525000 550000 575000 600000 625000 650000 675000 700000 725000 750000 775000 800000;do
            #sudo tcpdump -i enp24s0 src 192.168.1.5 and dst port 4791 -w "fresh"$queues"/"$dirName"/"$load.pcap &
            #if doing sq/one priority change n to one otherwise $queues 
            #./UD_Client -w 1 -t 1 -d $dirName"/"$queues"/"$dist -v mlx5_0 -g 3 -m $dist -s 192.168.1.5 -l $load -q $serverQpn -n $queues
            ./UD_Server -s 192.168.1.20 -g 4 -v mlx5_0 -t 1 -q $queues -n $queues -b 100
            

            #sudo kill -15 $(pgrep "tcpdump")

            #./UD_Client -w $windowSize -t $threadNum -d $dirName"_r"$ratio"_p"$percent -v $ib_devname -g $gid -q $serverQpn -m $servTimeDist -s $serverIPAddr -r $ratio -p $percent >> runlog.txt
            #if [[ rps == 0 ]];then
            #	rps=$(tail -n 1 runlog.txt)
            #else
            #    rps_prev=$rps
            #    rps=$(tail -n 1 runlog.txt)
                #if [[ rps -lt rps_prev*97/100 ]];then
                    #break 2
                #fi
            #fi
        done
    done
done
'

#2 cores
: '
for dist in 5 6 7;do #5 7 9 6 8;do
    for queues in 1024;do #8 16 32 64 128 256;do
        for load in 200000 400000 500000 600000 700000 725000 750000 775000 800000 825000 850000 875000 900000 1000000 1100000 1200000 1300000 1400000 1500000 1600000 1700000 1800000 1900000 2000000;do            
            sudo mlx_fpga -d /dev/mst/mt4117_pciconf0_fpga_rdma load
            sleep 5
            #sudo mlx_fpga -d /dev/mst/mt4117_pciconf0_fpga_rdma load --factory
            ./RCC_Server -d mlx5_0 -s 20 -r 130 -g 4 -n $queues -t 2
        done
    done
done

for dist in 5 7;do #5 7 9 6 8;do
    for queues in 2;do #8 16 32 64 128 256;do
        for load in 200000 400000 500000 600000 700000 725000 750000 775000 800000 825000 850000 875000 900000 1000000 1100000 1200000 1300000 1400000 1500000 1600000 1700000 1800000 1900000 2000000;do            
            sudo mlx_fpga -d /dev/mst/mt4117_pciconf0_fpga_rdma load
            sleep 5
            #sudo mlx_fpga -d /dev/mst/mt4117_pciconf0_fpga_rdma load --factory
            ./RCC_Server -d mlx5_0 -s 20 -r 130 -g 4 -n $queues -t 2
        done
    done
done
'

#4 cores
: '
for dist in 5 6 14 15;do #5 7 9 6 8;do
    for queues in 1024;do #8 16 32 64 128 256;do
        for load in 200000 600000 1000000 1400000 1500000 1600000 1700000 1800000 1900000 2000000 2100000 2200000 2300000 2400000 2500000 2600000 2700000 2800000 2900000 3000000;do
            sleep 5
            #sudo mlx_fpga -d /dev/mst/mt4117_pciconf0_fpga_rdma load
            sudo mlx_fpga -d /dev/mst/mt4117_pciconf0_fpga_rdma load
            sleep 5
            ./RCC_Server -d mlx5_0 -s 20 -r 200 -g 4 -n $queues -t 4
        done
    done
done
'
: '
for dist in 14 15;do #5 7 9 6 8;do
    for queues in 1024;do #8 16 32 64 128 256;do
        for load in 100000 200000 300000 400000 500000 600000 700000 800000 900000 1000000 1100000 1200000 1300000 1400000 1500000 1600000 1700000 1800000 1900000 2000000;do
            #sudo mlx_fpga -d /dev/mst/mt4117_pciconf0_fpga_rdma load
            sleep 5
            sudo mlx_fpga -d /dev/mst/mt4117_pciconf0_fpga_rdma load --factory
            ./RCC_Server -d mlx5_0 -s 20 -r 200 -g 4 -n $queues -t 4
        done
    done
done
'

#8 cores
: '
for dist in 5 6;do #5 7 9 6 8;do
    for queues in 1024;do #8 16 32 64 128 256;do
        for load in 400000 1200000 2000000 2800000 3000000 3200000 3400000 3600000 3800000 4000000 4200000 4400000 4600000 4800000 5000000 5200000 5400000 5600000 5800000 6000000;do
            sleep 5
            #sudo mlx_fpga -d /dev/mst/mt4117_pciconf0_fpga_rdma load --factory
            sudo mlx_fpga -d /dev/mst/mt4117_pciconf0_fpga_rdma load
            sleep 5
            ./RCC_Server -d mlx5_0 -s 20 -r 64 -g 4 -n $queues -t 8
        done
    done
done
for dist in 14 15;do #5 7 9 6 8;do
    for queues in 1024;do #8 16 32 64 128 256;do
        #for load in 100000 200000 300000 400000 500000 600000 700000 800000 900000 1000000 1100000 1200000 1300000 1400000 1500000 1600000 1700000 1800000 1900000 2000000;do
        for load in 400000 1200000 2000000 2800000 3000000 3200000 3400000 3600000 3800000 4000000 4200000 4400000 4600000 4800000 5000000 5200000 5400000 5600000 5800000 6000000;do
            sleep 5
            #sudo mlx_fpga -d /dev/mst/mt4117_pciconf0_fpga_rdma load --factory
            sudo mlx_fpga -d /dev/mst/mt4117_pciconf0_fpga_rdma load
            sleep 5
            ./RCC_Server -d mlx5_0 -s 20 -r 64 -g 4 -n $queues -t 8
        done
    done
done
'


#12 cores
: '
for dist in 5;do #5 7 9 6 8;do
    for queues in 1024;do #8 16 32 64 128 256;do
        for load in 3000000 4200000 4500000;do
            sleep 5
            sudo mlx_fpga -d /dev/mst/mt4117_pciconf0_fpga_rdma load --factory
            #sudo mlx_fpga -d /dev/mst/mt4117_pciconf0_fpga_rdma load
            sleep 5
            ./RCC_Server -d mlx5_0 -s 20 -r 64 -g 4 -n $queues -t 12
        done
    done
done

for dist in 6;do #5 7 9 6 8;do
    for queues in 1024;do #8 16 32 64 128 256;do
        for load in 600000 1800000 3000000 4200000 4500000;do
            sleep 5
            sudo mlx_fpga -d /dev/mst/mt4117_pciconf0_fpga_rdma load --factory
            #sudo mlx_fpga -d /dev/mst/mt4117_pciconf0_fpga_rdma load
            sleep 5
            ./RCC_Server -d mlx5_0 -s 20 -r 64 -g 4 -n $queues -t 12
        done
    done
done
'
: '
for dist in 15;do #5 7 9 6 8;do
    for queues in 1024;do #8 16 32 64 128 256;do
        for load in 100000 200000 300000 400000 500000 600000 700000 800000 900000 1000000 1100000 1200000;do
        #for load in 600000 1800000 3000000 4200000 4500000 4800000 5100000 5400000 5700000 6000000 6300000 6600000 6900000 7200000 7500000 7800000 8100000 8400000 8700000 9000000;do
        #for load in 3900000 4200000 4500000 4800000 5100000 5400000 5700000 6000000;do
            sleep 5
            sudo mlx_fpga -d /dev/mst/mt4117_pciconf0_fpga_rdma load --factory
            #sudo mlx_fpga -d /dev/mst/mt4117_pciconf0_fpga_rdma load
            sleep 5
            ./RCC_Server -d mlx5_0 -s 20 -r 64 -g 4 -n $queues -t 12
        done
    done
done
'

#for dist in 19 20 21 22;do #19 20
for dist in 22;do #19 20
    for queues in 1024;do #8 16 32 64 128 256;do
        #50 PUT 50 GET
        #for load in 1000000 1500000 2000000 2500000 3000000 3500000 4000000 4400000 4800000 5200000 5600000 6000000 6200000 6400000 6600000 6800000;do 
        #for load in 200000 400000 600000 800000 1000000 1200000 1400000 1600000 1800000 2000000 2200000;do 
        
        for load in 1000000 1300000 1600000 1900000 2200000 2500000 2800000 3100000 3400000 3700000 4000000 4300000 4600000 4900000 5200000 5500000;do 


        #for load in 1000000 2000000 3000000 4000000 5000000 6000000 6400000 6800000 7200000 7600000 8000000 8400000 8800000 9200000;do 
        #for load in 1000000 2000000 3000000 4000000 4400000 4800000 5200000;do 

        #trimodal 33 GET 33 SCAN16 33 SCAN64
        #for load in 500000 1000000 1500000 1800000 2100000 2400000 2700000 3000000 3300000 3600000 3900000;do
        #for load in 2200000 2300000 2500000 2600000 2800000 2900000 3100000 3200000 3400000 3500000;do

        #for load in 300000 600000 900000 1200000 1500000 1800000 2100000 2400000 2700000 3000000 3300000;do
       
        #for load in 1800000 2400000 3000000 3600000 3900000 4200000 4350000 4500000 4800000 5100000 5400000 5700000 6000000;do
        #for load in 3600000 4500000 5400000 6300000 6600000 6900000 7200000 7500000 7800000 8100000;do

            #sleep 5
            #sudo mlx_fpga -d /dev/mst/mt4117_pciconf0_fpga_rdma load --factory
            sudo mlx_fpga -d /dev/mst/mt4117_pciconf0_fpga_rdma load
            #sleep 5
            #./RCC_Server -d mlx5_0 -s 20 -r 64 -g 4 -n $queues -t 12 >> "runlog_server_"$dist"_warmup_sharedcq_12conn_kernelST_w_QD_combinedfuncs_0counters.txt"
            #./RCC_Server -d mlx5_0 -s 20 -r 64 -g 4 -n $queues -t 12 -f $dirName"/"$dist"/"$queues"/"$load
            ./mtd -d mlx5_0 -r 64 -g 4 -n $queues -t 12 -f $dirName"/"$dist"/"$queues"/"$load
        done
    done
done

#done

